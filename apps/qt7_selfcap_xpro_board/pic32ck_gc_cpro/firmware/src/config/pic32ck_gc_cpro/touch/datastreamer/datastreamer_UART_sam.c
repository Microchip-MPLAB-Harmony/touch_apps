/*******************************************************************************
  Touch Library v3.17.0 Release

  Company:
    Microchip Technology Inc.

  File Name:
    datastreamer_UART_sam.c

  Summary:
    QTouch Modular Library

  Description:
    Provides the datastreamer protocol implementation, transmission of
          module data to data visualizer software using UART port.
	
*******************************************************************************/

/*******************************************************************************
Copyright (C) [2025], Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
************************************************************************************/

/*----------------------------------------------------------------------------
  include files
----------------------------------------------------------------------------*/
#include "touch/datastreamer/datastreamer.h"
#include "definitions.h"

#if (DEF_TOUCH_DATA_STREAMER_ENABLE == 1u)

/*----------------------------------------------------------------------------
 *     defines
 *----------------------------------------------------------------------------*/
#define ACQ_MODULE_AUTOTUNE_OUTPUT 0
#define FREQ_HOP_AUTO_MODULE_OUTPUT 1
#define SCROLLER_MODULE_OUTPUT 1
#define SURFACE_MODULE_OUTPUT 0

/*----------------------------------------------------------------------------
  global variables
----------------------------------------------------------------------------*/
static uint8_t data[] = {
    0x5F, 0xB4, 0x00, 0x86, 0x4A, 0x03, 0xEB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x55, 0x01, 0x6E, 0xA0};

/*----------------------------------------------------------------------------
  prototypes
----------------------------------------------------------------------------*/
void datastreamer_transmit(uint8_t data_byte);

/*----------------------------------------------------------------------------
 *   function definitions
 *----------------------------------------------------------------------------*/

/*============================================================================
void datastreamer_init(void)
------------------------------------------------------------------------------
Purpose: Initialization for datastreamer module
Input  : none
Output : none
Notes  :
============================================================================*/
void datastreamer_init(void)
{
}

/*============================================================================
void datastreamer_transmit(uint8_t data_byte)
------------------------------------------------------------------------------
Purpose: Transmits the single byte through the configured UART port.
Input  : Byte to be transmitted
Output : none
Notes  :
============================================================================*/
void datastreamer_transmit(uint8_t data_byte)
{
	/* Write the data bye */
   if(SERCOM5_USART_Write(&data_byte, 1)) {

   }
}

/*============================================================================
void datastreamer_output(void)
------------------------------------------------------------------------------
Purpose: Forms the datastreamer frame based on the configured modules, Tranmits
         the frame as single packet through UART port.
Input  : none
Output : none
Notes  :
============================================================================*/
void datastreamer_output(void)
{
	int16_t           temp_int_calc;
	static uint8_t    sequence = 0u;
	uint16_t          u16temp_output;
	uint8_t           u8temp_output, send_header;
	uint16_t count_bytes_out;

	send_header = sequence & (0x0fu);
	if (send_header == 0u) {
		for (count_bytes_out = 0u; count_bytes_out < (uint16_t) sizeof(data); count_bytes_out++) {
			datastreamer_transmit(data[count_bytes_out]);
		}
	}

	// Start token
	datastreamer_transmit(0x55u);

	// Frame Start
	datastreamer_transmit(sequence);

	for (count_bytes_out = 0u; count_bytes_out < (uint16_t) DEF_NUM_SENSORS; count_bytes_out++) {
		/* Signals */
		u16temp_output = get_sensor_node_signal(count_bytes_out);
		datastreamer_transmit((uint8_t)u16temp_output);
		datastreamer_transmit((uint8_t)(u16temp_output >> 8u));

		/* Reference */
		u16temp_output = get_sensor_node_reference(count_bytes_out);
		datastreamer_transmit((uint8_t)u16temp_output);
		datastreamer_transmit((uint8_t)(u16temp_output >> 8u));

		/* Touch delta */
		temp_int_calc = (int16_t) get_sensor_node_signal(count_bytes_out);
		temp_int_calc -= (int16_t) get_sensor_node_reference(count_bytes_out);
		u16temp_output = (uint16_t)(temp_int_calc);
		datastreamer_transmit((uint8_t)u16temp_output);
		datastreamer_transmit((uint8_t)(u16temp_output >> 8u));

		/* Comp Caps */
		u16temp_output = get_sensor_cc_val(count_bytes_out);
		datastreamer_transmit((uint8_t)u16temp_output);
		datastreamer_transmit((uint8_t)(u16temp_output >> 8u));

#if (ACQ_MODULE_AUTOTUNE_OUTPUT == 1)

#if (DEF_PTC_CAL_OPTION == CAL_AUTO_TUNE_CSD)
		/* CSD */
		u8temp_output = ptc_seq_node_cfg1[count_bytes_out].node_csd;
		datastreamer_transmit(u8temp_output);
#else
		/* Prescalar */
		u8temp_output = NODE_PRSC(ptc_seq_node_cfg1[count_bytes_out].node_rsel_prsc);
		datastreamer_transmit(u8temp_output);
#endif

#endif
		/* State */
		u8temp_output = get_sensor_state(count_bytes_out);
		if (0u != (u8temp_output & 0x80u)) {
			datastreamer_transmit(0x01u);
		} else {
			datastreamer_transmit(0x00u);
		}

		/* Threshold */
		datastreamer_transmit(qtlib_key_configs_set1[count_bytes_out].channel_threshold);
	}

#if (SCROLLER_MODULE_OUTPUT == 1)

	for (count_bytes_out = 0u; count_bytes_out < DEF_NUM_SCROLLERS; count_bytes_out++) {

		/* State */
		u8temp_output = qtm_scroller_data1[count_bytes_out].scroller_status;
		if (0u != (u8temp_output & 0x01u)) {
			datastreamer_transmit(0x01u);
		} else {
			datastreamer_transmit(0x00u);
		}

		/* Delta */
		u16temp_output = qtm_scroller_data1[count_bytes_out].contact_size;
		datastreamer_transmit((uint8_t)u16temp_output);
		datastreamer_transmit((uint8_t)(u16temp_output >> 8u));

		/* Threshold */
		u16temp_output = qtm_scroller_config1[count_bytes_out].contact_min_threshold;
		datastreamer_transmit((uint8_t)u16temp_output);
		datastreamer_transmit((uint8_t)(u16temp_output >> 8u));

		/* filtered position */
		u16temp_output = qtm_scroller_data1[count_bytes_out].position;
		datastreamer_transmit((uint8_t)(u16temp_output & 0x00FFu));
		datastreamer_transmit((uint8_t)((u16temp_output & 0xFF00u) >> 8u));
	}

#endif

#if (FREQ_HOP_AUTO_MODULE_OUTPUT == 1)

	/* Frequency selection - from acq module */
	datastreamer_transmit(*qtm_freq_hop_autotune_config1.freq_option_select);

	for (count_bytes_out = 0u; count_bytes_out < NUM_FREQ_STEPS; count_bytes_out++) {
		/* Frequencies */
		datastreamer_transmit(qtm_freq_hop_autotune_config1.median_filter_freq[count_bytes_out]);
	}
#endif

	/* Other Debug Parameters */
	datastreamer_transmit(module_error_code);

	/* Frame End */
	datastreamer_transmit(sequence++);

	/* End token */
	datastreamer_transmit((uint8_t)(~(0x55u)));
}

#endif
