/*******************************************************************************
  Touch Library v3.19.0 Release

  Company:
    Microchip Technology Inc.

  File Name:
    driven_shield.c

  Summary:
    QTouch Modular Library

  Description:
    Provides Initialization, Processing and ISR handler of touch library,
    Simple API functions to get/set the key touch parameters from/to the
    touch library data structures
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

#include "definitions.h"
#include "driven_shield.h"
#include "touch.h"


#if (DEF_ENABLE_DRIVEN_SHIELD == 1u)




/*============================================================================
void drivenshield_port_mux_config()
------------------------------------------------------------------------------
Purpose: configures pin mux to switch between timer and PTC
Input  : pin and mux position
Output : None
============================================================================*/
static void drivenshield_port_mux_config(uint8_t pin, uint8_t mux);

/*============================================================================
static void drivenshield_port_mux_config(uint8_t pin, uint8_t mux)
------------------------------------------------------------------------------
Purpose: configures pin mux to switch between timer and PTC
Input  : pin and mux position
Output : None
============================================================================*/
static void drivenshield_port_mux_config(uint8_t pin, uint8_t mux)
{
	uint8_t temp_pin =(uint8_t) pin%32u;
	uint8_t port = (uint8_t)pin>>5u; /* div by 32 */

	if(mux == 0u)
	{
		PORT_REGS->GROUP[port].PORT_PINCFG[temp_pin] = 0;
	}
	else
	{
		PORT_REGS->GROUP[port].PORT_PINCFG[temp_pin] = 0x01;

		if(temp_pin%2u !=0u)
		{
			/* odd */
			PORT_REGS->GROUP[port].PORT_PMUX[temp_pin>>1u] &= ~(uint8_t)0xf0;
			PORT_REGS->GROUP[port].PORT_PMUX[temp_pin>>1u] |= (mux << 4u);
		}
		else
		{
			/* even */
			PORT_REGS->GROUP[port].PORT_PMUX[temp_pin>>1u] &= ~(uint8_t)0x0f;
			PORT_REGS->GROUP[port].PORT_PMUX[temp_pin>>1u] |= (uint8_t)(mux);
		}
	}
}


qtm_drivenshield_config_t qtm_drivenshield_config;

static const uint8_t offset_vs_prescaler[4] = { 1, 1, 1, 1 };
/*============================================================================
void drivenshield_configure(void)
------------------------------------------------------------------------------
Purpose: Sets up the qtm_drivenshield_config_t qtm_drivenshield_config object
Input  : Users application / configuration parameters
Output : None
Notes  : This setup is very product dependent,
         users can setup the delays between the SW_Trigger event and
         PWM2 and PTC start, Select Two or Three level Shield mode
         Users also use this function to configure GPIO pins and Enable
         GCLKs and APBClocks for the peripherals associated with the shield
============================================================================*/
void drivenshield_configure(void)
{
	touch_ret_t touch_ret = TOUCH_SUCCESS;
	/* Shield configuration */
	touch_ret = qtm_drivenshield_setup(&qtm_drivenshield_config);
	if (touch_ret != TOUCH_SUCCESS) {
		while (true){}
			;
	}
	touch_ret = qtm_drivenshield_register_start_callback(&drivenshield_start);
	if (touch_ret != TOUCH_SUCCESS) {
		while (true){}
			;
	}
	

	/* Dedicated Shield Timer pin mux setting */
	drivenshield_port_mux_config((uint8_t)PIN_PB08E_TC4_WO0, (uint8_t)MUX_PB08E_TC4_WO0);

	/* stop all the timers */
	drivenshield_stop();
}

/*============================================================================
void drivenshield_start(uint8_t csd, uint8_t sds, uint8_t prescaler, uint8_t volatile *dst_addr, uint8_t value)
------------------------------------------------------------------------------
Purpose: user call back from the SAMD21 Acquisition engine
Input  : Charge Share Delay (CSD) setting from PTC Acq. engine, (Set to 0 in SAMD21)
         Sample Delay Selection (SDS) setting from PTC Acq. engine this is the Frequency Hop Value for this cycle
         Prescaler setting from the PTC Acq. Engine
Output : None
Notes  : This function uses the EVSYS to start the PTC to acquire touch
============================================================================*/
void drivenshield_start(uint8_t csd, uint8_t sds, uint8_t prescaler, uint8_t volatile *dst_addr, uint8_t value)
{
	static uint8_t  filter_level = 0;
	static uint32_t addr;
	uint16_t        period = 0, count = 0, cc = 0;

    bool check;
	addr = (uint32_t)dst_addr;
	filter_level = value;

	/* Configure DMA transfer */
	check = DMAC_ChannelTransfer((DMAC_CHANNEL)0, &filter_level, (uint32_t *) addr, 3u);
	if (check != true) {
		/* error condition. During normal operation control shouldn't come here */
	}

	/* TC/TCC period value */
	period = (uint16_t) ((uint16_t)csd + 1u);
	period = (uint16_t) (period << 2);
	period = period + sds;
	period = (uint16_t) (period << 1);
	period = (uint16_t) (period - 1u);

	/* TC/TCC compare value */
	cc = (uint16_t) ((uint16_t)csd + 1u);
	cc = (uint16_t) (cc << 1);
	cc = (uint16_t) (cc + sds);
	cc = (uint16_t) (cc << 1);

	/* TC/TCC count value - initial offset */
	count = (uint16_t) ((uint16_t)csd + 1u);
	count = (uint16_t) (count << 1);
	if (prescaler <= 3u) {
		count = count - offset_vs_prescaler[prescaler];
	} else {
		/* Using Prescaler value greater than PRSC_DIV_SEL_8
		is not recommended with Driven Shield */
	}
	while (period > 255u) {
		prescaler = prescaler + 1u;
		period    = period >> 1u;
		cc        = cc >> 1u;
		count     = count >> 1u;
	}


	/* Dedicated Shield */
	TC4_REGS->COUNT8.TC_PER = (uint8_t)period;
	TC4_REGS->COUNT8.TC_COUNT = (uint8_t)count;
	TC4_REGS->COUNT8.TC_CC[0] = (uint8_t)cc;
	TC4_REGS->COUNT8.TC_CTRLA = TC_CTRLA_MODE_COUNT8 | TC_CTRLA_PRESCALER(prescaler);
	TC4_CompareStart();
}

/*============================================================================
void drivenshield_stop(void)
------------------------------------------------------------------------------
Purpose: Stops the softshield timers
Input  : none
Output : none
Notes  : This function is called from the PTC EOC handler in the users application in touch.c
============================================================================*/
void drivenshield_stop(void)
{
	/* Dedicated Shield */
	TC4_CompareStop();
}
#endif