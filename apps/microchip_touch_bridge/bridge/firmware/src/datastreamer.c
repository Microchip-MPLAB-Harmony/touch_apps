/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
#include "datastreamer.h"
#include "pcComm.h"

#if TOUCH_DEVICE == AT42QT2120

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************




qt_debug_read_registers qt_register_val;

uint8_t datastreamer_id[] = {
0x5F, 0xB4, 0x00, 0x86, 0x4A, 0x03, 0xEB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x55, 0x01, 0x6E, 0xA0};

void datastreamerOutput(void) {

	static uint8_t    sequence = 0u;
	uint8_t           send_header;
	volatile uint16_t i;

	send_header = sequence & (0x0f);
	if (send_header == 0) {
		for (i = 0; i < sizeof(datastreamer_id); i++) {
			pcWriteTxBufferByte(datastreamer_id[i]);
		}
	}

	// Start token
	pcWriteTxBufferByte(0x55);

	// Frame Start
	pcWriteTxBufferByte(sequence); 
	
	pcWriteTxBufferByte(qt_register_val.chip_id);
	pcWriteTxBufferByte(qt_register_val.firmware_version);
	pcWriteTxBufferByte(qt_register_val.detection_status);
	pcWriteTxBufferByte(qt_register_val.slider_position);
	
	for(uint8_t cnt = 0; cnt < NO_OF_KEYS; cnt++) {
		uint16_t tempdata = qt_register_val.key_signal[cnt];
		pcWriteTxBufferByte(tempdata);
		pcWriteTxBufferByte(tempdata>>8);
		tempdata = qt_register_val.ref_data[cnt];
		pcWriteTxBufferByte(tempdata);
		pcWriteTxBufferByte(tempdata>>8);
		
		int16_t delta = qt_register_val.key_signal[cnt];
		delta -= qt_register_val.ref_data[cnt];
		pcWriteTxBufferByte(delta);
		pcWriteTxBufferByte(delta>>8);
		
		pcWriteTxBufferByte((uint8_t)((qt_register_val.key_status & (1<<cnt))>>cnt));
	}
    qt_register_val.key_status++;


	/* Frame End */
	pcWriteTxBufferByte(sequence++);

	/* End token */
	pcWriteTxBufferByte(~0x55);
}

/*******************************************************************************
 End of File
 */


#endif
