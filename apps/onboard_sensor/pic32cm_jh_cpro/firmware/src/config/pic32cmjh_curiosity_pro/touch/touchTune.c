/*******************************************************************************
  Touch Library v3.14.0 Release

  Company:
    Microchip Technology Inc.

  File Name:
    touchTune.c

  Summary:
    QTouch Modular Library

  Description:
    Configuration macros for touch library

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2023 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
 
#include "touchTune.h"

void touchUartTxComplete(uintptr_t lTouchUart);
void touchUartRxComplete(uintptr_t lTouchUart);

#if DEF_TOUCH_TUNE_ENABLE == 1U

#if ((DEF_SENSOR_TYPE == NODE_SELFCAP) || (DEF_SENSOR_TYPE == NODE_SELFCAP_SHIELD) )
#define TECH SELF_CAP
#elif (DEF_SENSOR_TYPE == NODE_MUTUAL)
#define TECH MUTUAL_CAP
#endif


#define SCROLLER_MODULE_OUTPUT 0u


#define FREQ_HOP_AUTO_MODULE_OUTPUT 1u




/*******************************************************************************
 * Acqusition Parameters
*******************************************************************************/
typedef struct __attribute__((packed)) {
	uint16_t node_xmask;
	uint16_t node_ymask;
	uint8_t csd;
	uint8_t  prsc_res;
	uint8_t  gain;
	uint8_t  node_oversampling;
}channel_acq_param;
typedef struct __attribute__((packed)) {
	uint16_t acq_signal;
	uint16_t reference;
	int16_t delta;
	uint8_t sensor_state;
	uint16_t ccvalue;
}sensorData_t;
void copy_channel_config_data(uint8_t id, uint8_t channel);
void update_acq_config(uint8_t channel);
void copy_acq_config(uint8_t channel);

/*******************************************************************************
 * Keys' Module Parameters
*******************************************************************************/


/*******************************************************************************
 * Frequency Hop Autotune Parameters
*******************************************************************************/
#if FREQ_HOP_AUTO_MODULE_OUTPUT == 1u
typedef struct  __attribute__((packed)) {
	uint8_t numFreq;
	uint8_t freqList[16];
}tuneFreqData_t;
#define FREQ_HOP_AUTOTUNE_PARAM_LEN		(3U)
#define DEBUG_DATA_FREQ_HOP_LEN (sizeof(tuneFreqData_t))
static tuneFreqData_t runtime_freq_hop_auto_data_arr;
void copy_freq_hop_auto_runtime_data(uint8_t channel_num);
#endif



/*******************************************************************************
 * UART Related function prototypes
*******************************************************************************/
void uart_send_frame_header(uint8_t trans_type, uint8_t frame,uint16_t frame_len);
void uart_recv_frame_data(uint8_t frame_id,uint16_t len);
void uart_send_data(uint8_t con_or_debug, uint8_t *data_ptr,  uint16_t data_len);
void copy_run_time_data(uint8_t channel_num);
void uart_send_data_wait(uint8_t data);
void UART_Write(uint8_t data);
void uart_get_string(uint8_t *data_recv, uint16_t len);
uint8_t uart_get_char(void);

typedef struct tag_uart_command_info_t {
	uint8_t transaction_type;
	uint8_t frame_id;
	uint16_t num_of_bytes;
	uint8_t header_status;
} uart_command_info_t;
static uart_command_info_t uart_command_info;

static uint16_t tx_data_len = 0;
static uint8_t *tx_data_ptr ;
static volatile uint8_t  current_debug_data;
static volatile uint8_t  uart_tx_in_progress = 0;
static volatile uint8_t  uart_frame_header_flag = 1;
static volatile uint8_t  config_or_debug = 0;
static volatile uint8_t  write_buf_channel_num;
static volatile uint8_t  write_buf_read_ptr;
static volatile uint16_t command_flags = 0x0000;
static uint16_t max_channels_or_scrollers;

#if UART_RX_BUF_LENGTH <= 255 
typedef uint8_t rx_buff_ptr_t;
#else 
typedef uint16_t rx_buff_ptr_t;
#endif
static volatile rx_buff_ptr_t read_buf_read_ptr;
static volatile rx_buff_ptr_t read_buf_write_ptr;

static uint8_t rxData;
static uintptr_t touchUart;
rx_buff_ptr_t uart_min_num_bytes_received(void);
uint8_t getDebugIndex(uint8_t debugFramId);
uint8_t getConfigIndex(uint8_t frameid);

static uint8_t read_buffer[UART_RX_BUF_LENGTH];  

static sensorData_t runtime_data_arr;

/*******************************************************************************
 * Configuration Macros Parameters
*******************************************************************************/
#define NO_OF_CONFIG_FRAME_ID	 (5u)

#define PROJECT_CONFIG_DATA_LEN	 (10u)
#define CONFIG_0_PTR ((uint8_t*) &proj_config[0])
#define CONFIG_0_LEN ((uint16_t)  PROJECT_CONFIG_DATA_LEN)

#define CONFIG_1_PTR ((uint8_t*) (&ptc_seq_node_cfg1[0].node_xmask))
#define CONFIG_1_LEN ((uint16_t)  (sizeof(channel_acq_param) * DEF_NUM_CHANNELS))

#define CONFIG_2_PTR ((uint8_t*) (&(qtlib_key_configs_set1[0].channel_threshold)))
#define CONFIG_2_LEN ((uint16_t)  (sizeof(qtm_touch_key_config_t) * DEF_NUM_CHANNELS))

#define CONFIG_3_PTR ((uint8_t*) (&qtlib_key_grp_config_set1.sensor_touch_di))
#define CONFIG_3_LEN ((uint16_t)  (sizeof(qtm_touch_key_group_config_t))-(2U))

#if FREQ_HOP_AUTO_MODULE_OUTPUT == 1u
#define CONFIG_4_PTR ((uint8_t*) (&qtm_freq_hop_autotune_config1.enable_freq_autotune))
#define CONFIG_4_LEN ((uint16_t)  FREQ_HOP_AUTOTUNE_PARAM_LEN)
#endif

/*******************************************************************************
 * Debug Data Configuration
*******************************************************************************/
#define OUTPUT_MODULE_CNT 2u

#define DATA_0_PTR 			((uint8_t*)&runtime_data_arr.acq_signal)
#define DATA_0_ID 			(DEBUG_MASK + KEY_DEBUG_DATA_ID)
#define DATA_0_LEN			(uint16_t) sizeof(sensorData_t)
#define DATA_0_REPEAT 		DEF_NUM_CHANNELS
#define DATA_0_FRAME_LEN 	(uint16_t) (DATA_0_LEN * DATA_0_REPEAT)

#if FREQ_HOP_AUTO_MODULE_OUTPUT == 1u
#define DATA_1_PTR 			((uint8_t*)&runtime_freq_hop_auto_data_arr.numFreq)
#define DATA_1_ID 			(DEBUG_MASK + FREQ_HOP_AUTO_TUNE_DATA_ID)
#define DATA_1_LEN			(uint16_t) (sizeof(tuneFreqData_t))
#define DATA_1_REPEAT 		1u
#define DATA_1_FRAME_LEN 	(uint16_t) (DATA_1_LEN * DATA_1_REPEAT)
#endif

/* configuration details */
static uint8_t proj_config[PROJECT_CONFIG_DATA_LEN] = {PROTOCOL_VERSION, (uint8_t) samc2x, (uint8_t) TECH, (DEF_NUM_CHANNELS),
									(uint8_t) (SENSOR_NODE_CONFIG_MASK|SENSOR_KEY_CONFIG_MASK|COMMON_SENSOR_CONFIG_MASK|FREQ_HOPPING_AUTO_TUNE_MASK), (0u), (0u),
									(uint8_t) (KEY_DEBUG_MASK|FREQ_HOP_AUTO_TUNE_DEBUG_MASK), (0u),(0u)};

static uint16_t frame_len_lookup[NO_OF_CONFIG_FRAME_ID]  = {CONFIG_0_LEN,CONFIG_1_LEN,CONFIG_2_LEN,CONFIG_3_LEN,CONFIG_4_LEN};
static uint8_t *configPointerArray[NO_OF_CONFIG_FRAME_ID]	= {CONFIG_0_PTR,CONFIG_1_PTR,CONFIG_2_PTR,CONFIG_3_PTR,CONFIG_4_PTR};

/* output data details */
static uint8_t *debug_frame_PointerArray[OUTPUT_MODULE_CNT]  = {DATA_0_PTR,DATA_1_PTR};
static uint8_t debug_frame_id[OUTPUT_MODULE_CNT]		  = {DATA_0_ID,DATA_1_ID};
static uint16_t debug_frame_data_len[OUTPUT_MODULE_CNT]  = {DATA_0_LEN,DATA_1_LEN};
static uint16_t debug_frame_total_len[OUTPUT_MODULE_CNT] = {DATA_0_FRAME_LEN,DATA_1_FRAME_LEN};
static uint8_t debug_num_ch_scroller[OUTPUT_MODULE_CNT] = {DATA_0_REPEAT,DATA_1_REPEAT};
static void (*debug_func_ptr[OUTPUT_MODULE_CNT])(uint8_t ch) = {copy_run_time_data,copy_freq_hop_auto_runtime_data};

static uint8_t frameIdToConfigID[NO_OF_CONFIG_FRAME_ID] = {PROJECT_CONFIG_ID,SENSOR_NODE_CONFIG_ID,SENSOR_KEY_CONFIG_ID,COMMON_SENSOR_CONFIG_ID,FREQ_HOPPING_AUTO_TUNE_ID};

uint8_t getDebugIndex(uint8_t debugFramId) {
	uint8_t arrayIndex = 0u;
	for(uint8_t cnt = 0u; cnt < OUTPUT_MODULE_CNT; cnt++) {
		if(debug_frame_id[cnt] == debugFramId){
			arrayIndex = cnt;
			break;
		}
	}
	return arrayIndex;
}

uint8_t getConfigIndex(uint8_t frameid) {
	uint8_t arrayIndex = 0u;
	for(uint8_t cnt = 0u; cnt < NO_OF_CONFIG_FRAME_ID; cnt++) {
		if(frameIdToConfigID[cnt] == frameid){
			arrayIndex = cnt;
			break;
		}
	}
	return arrayIndex;
}

#if FREQ_HOP_AUTO_MODULE_OUTPUT == 1u
void copy_freq_hop_auto_runtime_data(uint8_t channel_num)
{
	runtime_freq_hop_auto_data_arr.numFreq = (qtm_freq_hop_autotune_config1.num_freqs);	
	for (uint8_t count = 0u; count < (uint8_t) NUM_FREQ_STEPS; count++) {
		/* Frequencies */
		runtime_freq_hop_auto_data_arr.freqList[count] = qtm_freq_hop_autotune_config1.median_filter_freq[count];
	}
}
#endif




static channel_acq_param acq_data;
void copy_acq_config(uint8_t channel)
{
qtm_acq_pic32cmjh_node_config_t *ptr = &ptc_seq_node_cfg1[channel];
    max_channels_or_scrollers = DEF_NUM_CHANNELS;
    acq_data.csd	= ptr->node_csd;
	acq_data.prsc_res	= ptr->node_rsel_prsc;
	acq_data.gain	= ptr->node_gain;
	acq_data.node_oversampling	= ptr->node_oversampling;
	if(channel == 0u) {
		uart_send_frame_header((uint8_t)MCU_RESPOND_CONFIG_DATA_TO_PC, uart_command_info.frame_id,((uint16_t) sizeof(channel_acq_param) * (uint8_t) DEF_NUM_CHANNELS));
		uart_send_data(STREAMING_CONFIG_DATA,(uint8_t *) &acq_data.node_xmask, (uint16_t) sizeof(channel_acq_param));
	} else {
		tx_data_ptr	= (uint8_t *) &acq_data.node_xmask;
		tx_data_len = (uint16_t) sizeof(channel_acq_param);
	}
}

void update_acq_config(uint8_t channel) {
	ptc_seq_node_cfg1[channel].node_rsel_prsc = acq_data.prsc_res;
	ptc_seq_node_cfg1[channel].node_gain = acq_data.gain;
    ptc_seq_node_cfg1[channel].node_csd	= acq_data.csd;
	ptc_seq_node_cfg1[channel].node_oversampling = acq_data.node_oversampling;
    calibrate_node(channel);
}


void copy_channel_config_data(uint8_t id, uint8_t channel) {
	uint8_t arrayIndex = getConfigIndex(id);
    switch(id) {
		case SENSOR_NODE_CONFIG_ID:
		copy_acq_config(channel);
		break;
		default:
		max_channels_or_scrollers = 1;
		uart_send_frame_header((uint8_t)MCU_RESPOND_CONFIG_DATA_TO_PC, uart_command_info.frame_id,frame_len_lookup[arrayIndex]);
		uart_send_data(STREAMING_CONFIG_DATA,configPointerArray[arrayIndex],frame_len_lookup[arrayIndex]);
		break;
	}
}


void copy_run_time_data(uint8_t channel_num)
{
	int16_t delta_temp ;	
	runtime_data_arr.acq_signal = ptc_qtlib_node_stat1[channel_num].node_acq_signals;
	runtime_data_arr.reference = qtlib_key_data_set1[channel_num].channel_reference;
	
	delta_temp = (int16_t) (ptc_qtlib_node_stat1[channel_num].node_acq_signals);
	delta_temp -= (int16_t) qtlib_key_data_set1[channel_num].channel_reference;
	runtime_data_arr.delta = delta_temp;

	runtime_data_arr.sensor_state = (uint8_t) ((qtlib_key_data_set1[channel_num].sensor_state & 0x80u) >> 7);
	runtime_data_arr.ccvalue = ptc_qtlib_node_stat1[channel_num].node_comp_caps;

}

uint8_t uart_get_char(void)
{
	uint8_t data = read_buffer[read_buf_read_ptr];
	read_buf_read_ptr++;
	if (read_buf_read_ptr == UART_RX_BUF_LENGTH) {
		read_buf_read_ptr = 0u;
	}
	return data;
}

void uart_get_string(uint8_t *data_recv, uint16_t len)
{
	for(uint16_t idx = 0; idx < len; idx++)
	{
		*data_recv = uart_get_char();
		data_recv++;		
	}
}


void touchTuneNewDataAvailable(void) {
	command_flags |= SEND_DEBUG_DATA;
}

void UART_Write(uint8_t data) {
	static uint8_t txData;
	txData = data;
	if (SERCOM4_USART_Write(&txData, 1)) {

	}
}

void uart_send_data_wait(uint8_t data)
{
	uart_tx_in_progress = 1u;
	UART_Write(data);
	while (uart_tx_in_progress == 1u) {

	}
}

void uart_send_data(uint8_t con_or_debug, uint8_t *data_ptr,  uint16_t data_len) {
	if (uart_tx_in_progress == 0u) {
		config_or_debug           = con_or_debug;
		uart_tx_in_progress       = 1u;
		write_buf_channel_num	  = 1u;
		write_buf_read_ptr        = 1u;
		tx_data_ptr			      = data_ptr;
		tx_data_len			      = data_len;
		UART_Write(tx_data_ptr[0]);
	}
}

rx_buff_ptr_t uart_min_num_bytes_received(void)
{
	int16_t retvar = (int16_t) read_buf_write_ptr;
	retvar -= (int16_t) read_buf_read_ptr;
	if (retvar < 0)
	{
		retvar = retvar + (int16_t) UART_RX_BUF_LENGTH;
	}
	return (rx_buff_ptr_t)(retvar);
}

void uart_send_frame_header(uint8_t trans_type, uint8_t frame,uint16_t frame_len)
{
	uart_frame_header_flag = 0;
	uart_send_data_wait(DV_HEADER);
 	uart_send_data_wait(trans_type);
 	uart_send_data_wait(frame);
	uart_send_data_wait((uint8_t)(frame_len));
	uart_send_data_wait((uint8_t)(frame_len>>8));
	uart_frame_header_flag = 1;
}

void uart_recv_frame_data(uint8_t frame_id, uint16_t len)
{
    static uint8_t ch_num;
    uint16_t num_data;
	uint8_t tempData;
	uint8_t arrayIndex = getConfigIndex(frame_id);
    num_data = uart_min_num_bytes_received();
    switch(frame_id)
    {
		case 0u:
		break;
        case SENSOR_NODE_CONFIG_ID:
            while(num_data > (uint16_t) sizeof(channel_acq_param)) {

                uint8_t *ptr = (uint8_t *) &acq_data.node_xmask;
                for(uint16_t cnt = 0u; cnt < (uint16_t) sizeof(channel_acq_param); cnt++) {
                    ptr[cnt] = uart_get_char();
                }
                update_acq_config(ch_num);
                ch_num++;
                num_data -= (uint16_t) sizeof(channel_acq_param);

                if(ch_num == (uint8_t) DEF_NUM_CHANNELS) {
                    ch_num = 0u;
                    uart_command_info.header_status = HEADER_AWAITING;
                    command_flags = command_flags & (uint16_t) (~((uint16_t)1<<uart_command_info.frame_id));
                    tempData = uart_get_char();
					if(tempData != DV_FOOTER) {
						/* Error condition */
					}
                    break;
                }
            }
        break;
        
        case SENSOR_KEY_CONFIG_ID:
            while(num_data > (uint16_t) sizeof(qtm_touch_key_config_t)) {

                uint8_t *ptr = (uint8_t *) &qtlib_key_configs_set1[ch_num].channel_threshold;
                for(uint16_t cnt = 0u; cnt < (uint16_t) sizeof(qtm_touch_key_config_t); cnt++) {
                    ptr[cnt] = uart_get_char();
                }
                ch_num++;
                num_data -= (uint16_t) sizeof(qtm_touch_key_config_t);

                if(ch_num == (uint8_t) DEF_NUM_CHANNELS) {
                    ch_num = 0u;
                    uart_command_info.header_status = HEADER_AWAITING;
                    command_flags = command_flags & (uint16_t) (~((uint16_t)1<<uart_command_info.frame_id));
                    tempData = uart_get_char();
					if(tempData != DV_FOOTER) {
						/* Error condition */
					}
                    break;
                }
            }
        break;


        default:
            uart_get_string(configPointerArray[arrayIndex],uart_command_info.num_of_bytes);
            tempData = uart_get_char();
        break;
    }
}


void touchTuneInit(void) {

    SERCOM4_USART_WriteCallbackRegister(touchUartTxComplete, touchUart);
    SERCOM4_USART_ReadCallbackRegister(touchUartRxComplete, touchUart);

    if(SERCOM4_USART_Read((void *) &rxData, 1)) {
	
	}
}


void touchTuneProcess(void)
{
	static uint8_t debug_index = 0;

	switch (uart_command_info.header_status) {
		case HEADER_AWAITING:
			if (uart_min_num_bytes_received() > 5u)
			{
				if (uart_get_char() == DV_HEADER)
				{
					uart_get_string((uint8_t *) &uart_command_info, 4);
					uart_command_info.header_status = DATA_AWAITING;
				}
			} else {

			}
			break;
		case DATA_AWAITING:
			if(uart_command_info.transaction_type == (uint8_t) PC_SEND_CONFIG_DATA_TO_MCU)
			{
                if(uart_command_info.num_of_bytes >= UART_RX_BUF_LENGTH) {
                    uart_recv_frame_data(uart_command_info.frame_id,uart_command_info.num_of_bytes);
                }else if (uart_min_num_bytes_received() > uart_command_info.num_of_bytes)
				{
					command_flags = command_flags | (uint16_t) ((uint16_t)1 << (uart_command_info.frame_id ));
					uart_command_info.header_status	= DATA_RECEIVED;
				} else {
					/* not enough data to process */
				}
			}
			else if (uart_command_info.transaction_type == (uint8_t) PC_REQUEST_CONFIG_DATA_FROM_MCU)
			{
				if(uart_min_num_bytes_received() > 1u)
				{
					uint8_t data1 = uart_get_char();
					uint8_t data2 = uart_get_char();
					if((data1 == ZERO) && (data2 == DV_FOOTER)) {
						command_flags = command_flags | (uint16_t) ((uint16_t)1 << (uart_command_info.frame_id ));
						uart_command_info.header_status	= DATA_RECEIVED;
					}
				}
			} else {
				/* error condition */
			}
		break;
		case DATA_RECEIVED:
			if(uart_tx_in_progress == 0u) {
				if((command_flags & 0x0FFFu) != 0u) {
					if (uart_command_info.transaction_type == (uint8_t) PC_REQUEST_CONFIG_DATA_FROM_MCU)
					{
						copy_channel_config_data(uart_command_info.frame_id, 0);
						uart_command_info.header_status = HEADER_AWAITING;
					}
					else if(uart_command_info.transaction_type == (uint8_t) PC_SEND_CONFIG_DATA_TO_MCU)
					{
						uart_recv_frame_data(uart_command_info.frame_id,uart_command_info.num_of_bytes);
						uart_command_info.header_status = HEADER_AWAITING;
						command_flags = command_flags & (uint16_t) (~((uint16_t)1<<uart_command_info.frame_id));
					} else {
						/* error condition */
					}
				} else {
					/* Data received - but command flag not set - error condition */
				}
			} else {
				/* transmission is in progress */
			}
		break;
		default:
			uart_command_info.header_status = HEADER_AWAITING;
		break;
	}

	if(uart_tx_in_progress == 0u) {
	 	/* to send periodic data */
		if((command_flags & SEND_DEBUG_DATA) == SEND_DEBUG_DATA) {

			current_debug_data = debug_frame_id[debug_index];
			
			uart_send_frame_header((uint8_t)MCU_SEND_TUNE_DATA_TO_PC, current_debug_data, debug_frame_total_len[debug_index]);
									
			(debug_func_ptr[debug_index])(0);
			
			max_channels_or_scrollers = debug_num_ch_scroller[debug_index];
			
			uart_send_data(STREAMING_DEBUG_DATA, (uint8_t *)debug_frame_PointerArray[debug_index], debug_frame_data_len[debug_index]);
			
			debug_index++;
			
			if(debug_index == OUTPUT_MODULE_CNT) {
				debug_index = 0;
			}
		}
	}
}

#endif

void touchUartTxComplete(uintptr_t lTouchUart)
{
	#if (DEF_TOUCH_TUNE_ENABLE == 1u)

	uint8_t arrayIndex = 0u;

	if (uart_frame_header_flag != 1u)
	{
		uart_tx_in_progress = 0u;
	} 
	else 
	{
		if (write_buf_read_ptr < tx_data_len )
		{
			UART_Write(tx_data_ptr[write_buf_read_ptr]);
			write_buf_read_ptr++;
		} else {
			if(config_or_debug == STREAMING_CONFIG_DATA) {
				/* per channel data are sent channel by channel to reduce RAM requirements */
				if (write_buf_channel_num < max_channels_or_scrollers)
				{
                    copy_channel_config_data(uart_command_info.frame_id, write_buf_channel_num);
					write_buf_read_ptr = 1;
					write_buf_channel_num++;
					UART_Write(tx_data_ptr[0]);
				}
				else if(write_buf_channel_num == max_channels_or_scrollers)
				{
					write_buf_channel_num++;
					command_flags = command_flags & (uint16_t) (~((uint16_t)1<<uart_command_info.frame_id));
					UART_Write(DV_FOOTER);
				}
				else
				{
					uart_tx_in_progress = 0;
				}
			} else if(config_or_debug == STREAMING_DEBUG_DATA) {
				/* per channel data are sent channel by channel to reduce RAM requirements */
				if (write_buf_channel_num < max_channels_or_scrollers)
				{
					arrayIndex = getDebugIndex(current_debug_data);
					(*debug_func_ptr[arrayIndex])(write_buf_channel_num);  
					write_buf_read_ptr = 1;
					write_buf_channel_num++;
					UART_Write(tx_data_ptr[0]);
				}
				else if(write_buf_channel_num == max_channels_or_scrollers)
				{
					write_buf_channel_num++;
					command_flags = command_flags & (uint16_t) (~SEND_DEBUG_DATA);
					UART_Write(DV_FOOTER);
				}
				else
				{
					uart_tx_in_progress = 0;
				}
			} else {
				/* error condition - control should not reach here*/
			}
		}
	}
	#endif
}

void touchUartRxComplete(uintptr_t lTouchUart)
{
	#if (DEF_TOUCH_TUNE_ENABLE == 1u)
	read_buffer[read_buf_write_ptr] = rxData;
	read_buf_write_ptr++;
	if (read_buf_write_ptr == UART_RX_BUF_LENGTH) {
		read_buf_write_ptr = 0u;
	}
	if(SERCOM4_USART_Read((void *) &rxData,1)) {
	}
	#endif
}
