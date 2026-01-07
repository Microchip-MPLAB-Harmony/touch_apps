/*
 * led_driver.h
 *
 * Created: 18-09-2017 11:45:02
 *  Author: I41681
 */

#ifndef LED_DRIVER_H_
#define LED_DRIVER_H_

#include <stdint.h>
#include "config/pl10_qt8/touch/touch.h"
#include "config/pl10_qt8/peripheral/sercom/i2c_master/plib_sercom0_i2c_master.h"

// MCP23017 definitions
#define MCP23017_ADDR   0x20
#define MCP23017_IODIRA 0x00
#define MCP23017_IODIRB 0x01
#define MCP23017_GPIOA  0x12
#define MCP23017_GPIOB  0x13
#define MCP23017_OLATA  0x14
#define MCP23017_OLATB  0x15

#define LED_HOR 0
#define LED_VER 1


#define LED_RIGHT 1
#define LED_LEFT 0
#define LED_UP 1
#define LED_DOWN 0

#define LED_SCROLL 0
#define LED_BLINK 1
#define LED_NO_ACTION 10

#define MODE_POS 0u
#define MODE_GES 1u
#define LED_BLINK_DUR 200
#define LED_BLINK_ON 100
#define LED_SCROLL_PERIOD 50


// --- Public APIs ---

// Initialize MCP23017 + LED states
void LED_Init(void);

void led_gpio_update(uint8_t data, uint8_t ver_or_hor);

void led_decode_position(void);

void led_reset(void);

void led_blink_scroll(void);

void led_init_pwm(void);

void led_process(void);

void led_scroll(uint8_t hor, uint8_t hor_dir, uint8_t ver, uint8_t ver_dir);

void led_decode_gesture(void);

void led_scroll_callback(void);

void led_update_time(void);

void led_blink(uint8_t);

void led_process_position_mode(void);
void led_realtime_feedback(void);
#define led_turn_on_dual_led() led_dual_touch = 1
#define led_turn_off_dual_led() led_dual_touch = 0
#endif // LED_DRIVER_H



/* LED_DRIVER_H_ */
