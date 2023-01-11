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


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
void touch_status_display1(void);

uint8_t key_status1 = 0;

extern volatile uint8_t measurement_done_touch;

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
     touch_process();   
        if(measurement_done_touch == 1)
    {
        measurement_done_touch = 0;
        // process touch data
        touch_status_display1();
    }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

void touch_status_display1(void)
{
    key_status1 = get_sensor_state(0) & KEY_TOUCHED_MASK;
    if (0u != key_status1) {
        //Touch detect
        LED_1_Clear();
        
    } else {
        //Touch No detect
        LED_1_Set();
    }

    key_status1 = get_sensor_state(1) & KEY_TOUCHED_MASK;
    if (0u != key_status1) {
        //Touch detect
        LED_2_Clear();
    } else {
        //Touch No detect
        LED_2_Set();
    }

    key_status1 = get_sensor_state(2) & KEY_TOUCHED_MASK;
    if (0u != key_status1) {
        //Touch detect
        LED_4_Clear();
    } else {
        //Touch No detect
        LED_4_Set();
    }

    key_status1 = get_sensor_state(3) & KEY_TOUCHED_MASK;
    if (0u != key_status1) 
    {
        //Touch detect
        LED_3_Clear();
    } else {
        //Touch No detect
        LED_3_Set();
    }
}
/*******************************************************************************
 End of File
*/

