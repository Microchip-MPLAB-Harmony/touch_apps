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
#include"config/pic32ck_gc_cpro/touch/touch_example.h"


/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/
void touch_status_display1(void);
/*----------------------------------------------------------------------------
 *   Extern variables
 *----------------------------------------------------------------------------*/
extern volatile uint8_t measurement_done_touch;

/*----------------------------------------------------------------------------
 *   Global variables
 *----------------------------------------------------------------------------*/
uint8_t  scroller_status1   = 0;
uint16_t scroller_position1 = 0;

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        touch_process();
        if (measurement_done_touch == 1) 
        {
			touch_status_display1();
		}
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*============================================================================
void touch_status_display(void)
------------------------------------------------------------------------------
Purpose: Sample code snippet to demonstrate how to check the status of the
         sensors
Input  : none
Output : none
Notes  : none
============================================================================*/
void touch_status_display1(void)
{

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

