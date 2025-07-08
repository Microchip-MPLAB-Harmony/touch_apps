/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


uint8_t key_status1 = 0;
uint8_t scroller_status1   = 0;
uint16_t scroller_position1 = 0;


extern volatile uint8_t measurement_done_touch;

void touch_status_display1(void);

/* typedef for non-secure callback functions */
typedef void (*funcptr_void) (void) __attribute__((cmse_nonsecure_call));

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    uint32_t msp_ns = *((uint32_t *)(TZ_START_NS));
    volatile funcptr_void NonSecure_ResetHandler;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    if (msp_ns != 0xFFFFFFFF)
    {
        /* Set non-secure main stack (MSP_NS) */
        __TZ_set_MSP_NS(msp_ns);

        /* Get non-secure reset handler */
        NonSecure_ResetHandler = (funcptr_void)(*((uint32_t *)((TZ_START_NS) + 4U)));

        /* Start non-secure state software application */
        NonSecure_ResetHandler();
    }

    while ( true )
    {
        touch_process();

	if (measurement_done_touch == 1) 
	   {
	     touch_status_display1();
	   }
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}

void touch_status_display1(void)
{

	key_status1 = get_sensor_state(0) & 0x80;
	if (0u != key_status1) {
		LED_BUTTON_0_Clear();
	} else 
    {
		LED_BUTTON_0_Set();
	}

	scroller_status1   = get_scroller_state(0);
	scroller_position1 = get_scroller_position(0);
	
    LED_SLIDER_0_Set();
    LED_SLIDER_1_Set();
    LED_SLIDER_2_Set();
    LED_SLIDER_3_Set();
    LED_SLIDER_4_Set();
    LED_SLIDER_5_Set();

	if (0u != scroller_status1) {

		LED_SLIDER_0_Clear();

		if (scroller_position1 > 43) {
			LED_SLIDER_1_Clear();
		}
		if (scroller_position1 > 85) {
			LED_SLIDER_2_Clear();
		}
		if (scroller_position1 > 120) {
			LED_SLIDER_3_Clear();
		}
		if (scroller_position1 > 165) {
			LED_SLIDER_4_Clear();
		}
		if (scroller_position1 > 213) {
            LED_SLIDER_5_Clear();
		}
	}
}
/*******************************************************************************
 End of File
*/

