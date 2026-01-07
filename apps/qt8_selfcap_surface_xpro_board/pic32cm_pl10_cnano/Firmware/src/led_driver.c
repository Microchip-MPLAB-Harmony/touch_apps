#include "led_driver.h"
#include "config/pic32cm_pl10_qt8/peripheral/sercom/i2c_master/plib_sercom0_i2c_master.h"
#include "config/pic32cm_pl10_qt8/touch/qtm_gestures_2d_0x0023_api.h"
#include "config/pic32cm_pl10_qt8/touch/qtm_surface_cs2t_0x0025_api.h"

extern qtm_surface_cs2t_control_t qtm_surface_cs_control1;
extern qtm_gestures_2d_control_t  qtm_gestures_2d_control1;
extern uint8_t interrupt_cnt;

typedef struct tag_led_scroll_t 
{
	uint8_t value;
	uint8_t dir;
} led_scroll_t;



led_scroll_t led_scroll_data[2];

uint8_t gesture_or_pos_mode = MODE_POS;

volatile uint8_t led_1msec_interrupt_cnt;
uint8_t          i2c_write_buf[3];
uint8_t          led_current_action     = LED_NO_ACTION;
uint8_t          scroll_in_progress     = 0;
uint8_t          pinch_zoom_in_progress = 0;
int8_t           previous_gest_info;
uint8_t          zoom_pinch_position = 0;
uint8_t          led_blink_cnt;
uint16_t         led_blink_time_cnt;
uint8_t          led_time_track;
uint8_t          led_dual_touch = 0;

uint8_t i2c_app_status = 1;

// --- Low level MCP23017 write ---
static void MCP23017_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t data[2];
    data[0] = reg;
    data[1] = value;
    SERCOM0_I2C_Write(MCP23017_ADDR, data, 2);
    while (SERCOM0_I2C_IsBusy());
}

void LED_Init(void)
{
   // Set all pins as outputs
    MCP23017_WriteReg(MCP23017_IODIRA, 0x00);
    MCP23017_WriteReg(MCP23017_IODIRB, 0x00);
    // Set all outputs low
    MCP23017_WriteReg(MCP23017_OLATA, 0x00);
    MCP23017_WriteReg(MCP23017_OLATB, 0x00);
}

void led_gpio_update(uint8_t data, uint8_t ver_or_hor)
{
    if (ver_or_hor == LED_HOR) 
    {
        uint8_t porta = data & 0x1F;   // only GPA0..4

        // handle dual-touch LED on GPA5
        if (led_dual_touch)
            porta |= (1 << 5);  // set GPA5
        else
            porta &= ~(1 << 5); // clear GPA5

        MCP23017_WriteReg(MCP23017_OLATA, porta);
    } 
    else // LED_VER
    {
        uint8_t portb = data & 0x1F;   // only GPB0..4
        MCP23017_WriteReg(MCP23017_OLATB, portb);
    }
}

/* update the LED timing variables */
void led_update_time(void)
{
	if (led_time_track != interrupt_cnt) {
		if (interrupt_cnt > led_time_track) {
			led_1msec_interrupt_cnt += (interrupt_cnt - led_time_track);
		} else {
			led_1msec_interrupt_cnt += (interrupt_cnt + DEF_TOUCH_MEASUREMENT_PERIOD_MS - led_time_track);
		}
		led_time_track = interrupt_cnt;
	}
}



/* this function process the LED blinking and scrolling action */
void led_blink_scroll(void)
{
	if (led_current_action == LED_SCROLL) {
		if (led_1msec_interrupt_cnt > LED_SCROLL_PERIOD) {
			led_1msec_interrupt_cnt = 0;

			if ((led_scroll_data[0].value == 0) && (led_scroll_data[1].value == 0)) {
				led_current_action = LED_NO_ACTION;
			}

			for (uint8_t cnt = 0; cnt < 2; cnt++) {
				led_gpio_update(led_scroll_data[cnt].value, cnt);
				if (led_scroll_data[cnt].dir == 1) {
					led_scroll_data[cnt].value >>= 1;
				} else {
					led_scroll_data[cnt].value <<= 1;
				}
			}
		}
	} else if (led_current_action == LED_BLINK) {
		led_blink_time_cnt += led_1msec_interrupt_cnt;
		led_1msec_interrupt_cnt = 0;

		if (led_blink_time_cnt > LED_BLINK_DUR) {
			led_blink_time_cnt = 0;
			if (led_blink_cnt > 0) {
				led_blink_cnt--;
				led_gpio_update(0x1f, LED_HOR);
				led_gpio_update(0x1f, LED_VER);
			}
		} else if (led_blink_time_cnt > LED_BLINK_ON) {
			led_reset();
			if (led_blink_cnt == 0) {
				led_blink_time_cnt = 0;
				led_current_action = LED_NO_ACTION;
			}
		}
	}
}

/* process the blinking and scrolling */
void led_process(void)
{
	/* check for time update */
	led_update_time();
	/* perform scrolling and blink actions */
	led_blink_scroll();
}

/* this function sets the LED bliking action */
void led_blink(uint8_t blink_cnt)
{
	if (blink_cnt != 0 && led_current_action == LED_NO_ACTION && pinch_zoom_in_progress == 0) {
		led_current_action      = LED_BLINK;
		led_blink_cnt           = blink_cnt - 1;
		led_blink_time_cnt      = 0;
		led_1msec_interrupt_cnt = 0;
		led_gpio_update(0x1f, LED_HOR);
		led_gpio_update(0x1f, LED_VER);
	}
}

/* this function sets the LED scrolling action */
void led_scroll(uint8_t hor, uint8_t hor_dir, uint8_t ver, uint8_t ver_dir)
{
	/* update the scrolling only if currently there is no action */
	if (led_current_action == LED_NO_ACTION && pinch_zoom_in_progress == 0) {
		led_current_action = LED_SCROLL;
		scroll_in_progress = 1;

		if (hor) {
			led_scroll_data[LED_HOR].dir = hor_dir;
			if (hor_dir == LED_RIGHT) {
				led_scroll_data[LED_HOR].value = (1 << 4);
			} else {
				led_scroll_data[LED_HOR].value = (1);
			}
		}

		if (ver) {
			led_scroll_data[LED_VER].dir = ver_dir;
			if (ver_dir == LED_UP) {
				led_scroll_data[LED_VER].value = (1 << 4);
			} else {
				led_scroll_data[LED_VER].value = (1);
			}
		}
	}
}

/* decode the position for pinch and zoom */
void led_decode_position_pinch_zoom(uint8_t v_pos, uint8_t h_pos)
{
	uint8_t temp_data = 0;
	led_reset();

	for (uint8_t cnt = 0; cnt < 5; cnt++) {
		if (cnt < h_pos) {
			temp_data |= (1 << (4 - cnt));
		}
	}
	led_gpio_update(temp_data, LED_HOR);

	temp_data = 0;
	for (uint8_t cnt = 0; cnt < 5; cnt++) {
		if (cnt < v_pos) {
			temp_data |= (1 << (4 - cnt));
		}
	}
	led_gpio_update(temp_data, LED_VER);
}

/* decode the gesture and set the LED action accordingly */
void led_decode_gesture(void)
{
	uint8_t gesture_info_l = 0;

	gesture_info_l = qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture; // & 0xf3;

	if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xf0) == RIGHT_SWIPE) 
    {
		led_scroll(1, LED_RIGHT, 0, 0);
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xf0) == LEFT_SWIPE) 
    {
		led_scroll(1, LED_LEFT, 0, 0);                                                                     
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xf0) == UP_SWIPE) 
    {
		led_scroll(0, 0, 1, LED_UP);
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xf0) == DOWN_SWIPE) 
    {
		led_scroll(0, 0, 1, LED_DOWN);
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xf1) == CW_WHEEL) 
    {
		led_scroll(1, LED_RIGHT, 1, LED_UP);
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xf1) == CCW_WHEEL) 
    {
		led_scroll(1, LED_LEFT, 1, LED_DOWN);
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0xF0)
	           == TAP) { // tap - lsb contains tap count
		if (zoom_pinch_position == 0)
			led_blink(gesture_info_l & 0x0f);
	} 
    else if ((qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & TAP_DUAL) == TAP_DUAL) 
    {
		if (zoom_pinch_position == 0) 
        {
			// double tap - lsb contains tap count
			led_turn_on_dual_led();
			led_blink(qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0x0f);
		}
	} 
    else if (qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture == PINCH) 
    {
		if (qtm_gestures_2d_control1.qtm_gestures_data->gestures_info > 127) 
        {
			gesture_info_l = -qtm_gestures_2d_control1.qtm_gestures_data->gestures_info;
		} else {
			gesture_info_l = 0;
		}

		if (zoom_pinch_position == 0) {
			if (pinch_zoom_in_progress != 6) 
            {
				pinch_zoom_in_progress = 6;
			}
		}

		if (previous_gest_info > (int8_t)qtm_gestures_2d_control1.qtm_gestures_data->gestures_info) 
        {
			previous_gest_info = (int8_t)qtm_gestures_2d_control1.qtm_gestures_data->gestures_info;
			if (pinch_zoom_in_progress > 0)
				pinch_zoom_in_progress--;
		}

		led_decode_position_pinch_zoom(pinch_zoom_in_progress, pinch_zoom_in_progress);
		zoom_pinch_position = PINCH;
		led_turn_on_dual_led();
	} 
    else if (qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture == ZOOM) 
    {
		if (qtm_gestures_2d_control1.qtm_gestures_data->gestures_info > 127) {
			gesture_info_l = -qtm_gestures_2d_control1.qtm_gestures_data->gestures_info;
		} 
        else 
        {
			gesture_info_l = qtm_gestures_2d_control1.qtm_gestures_data->gestures_info;
		}

		if (previous_gest_info < (int8_t)qtm_gestures_2d_control1.qtm_gestures_data->gestures_info) 
        {
			previous_gest_info = (int8_t)qtm_gestures_2d_control1.qtm_gestures_data->gestures_info;
			pinch_zoom_in_progress++;
		}

		if (pinch_zoom_in_progress > 6) 
        {
			pinch_zoom_in_progress = 6;
		}
		led_decode_position_pinch_zoom(pinch_zoom_in_progress, pinch_zoom_in_progress);
		zoom_pinch_position = ZOOM;
		led_turn_on_dual_led();
	}
	// else
	{
		if ((qtm_surface_cs_control1.qtm_surface_contact_data[0].qt_contact_status == 0)
		    && (qtm_surface_cs_control1.qtm_surface_contact_data[1].qt_contact_status == 0)) 
        {
			previous_gest_info     = 0;
			pinch_zoom_in_progress = 0;
			if (zoom_pinch_position != 0) 
            {
				qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture = 0;
				led_reset();
			}
			zoom_pinch_position = 0;
		}
	}
	/* glow the dual finger gesture indication led when
	    - dual finger is touched and either wheel/swipe gesture is in progress
	    - ignore this when the mode is in position (not gesture) */
	if (led_current_action != LED_NO_ACTION || zoom_pinch_position != 0) 
    {
		if (qtm_gestures_2d_control1.qtm_gestures_data->gestures_which_gesture & 0x0c) 
        {
			if ((gesture_info_l & 0xF0) != TAP) {
				led_turn_on_dual_led();
			}
		}
	} else {
		led_turn_off_dual_led();
		led_reset();
	}
}

void led_reset(void)
{
	MCP23017_WriteReg(MCP23017_OLATA, 0x00);
    MCP23017_WriteReg(MCP23017_OLATB, 0x00);
}

void led_decode_position(void)
{
	uint8_t h_pos = 0, v_pos = 0;
	led_reset();

	if (qtm_surface_cs_control1.qtm_surface_contact_data->qt_contact_status) {
		h_pos = (qtm_surface_cs_control1.qtm_surface_contact_data->h_position / 43);
		h_pos = (1 << (5 - h_pos));
		led_gpio_update(h_pos, LED_HOR);

		v_pos = (qtm_surface_cs_control1.qtm_surface_contact_data->v_position / 52);
		v_pos = (1 << (4 - v_pos));
		led_gpio_update(v_pos, LED_VER);
	}
}
