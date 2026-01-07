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


/*----------------------------------------------------------------------------
 *   Extern variables
 *----------------------------------------------------------------------------*/
extern volatile uint8_t measurement_done_touch;

/*----------------------------------------------------------------------------
 *   Global variables
 *----------------------------------------------------------------------------*/
int16_t touchDelta[DEF_NUM_CHANNELS] = {0};

/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/
void processTouchStatus(void);

bool touchDetectMultiTouch(uint8_t active_sensor);

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main(void)
{
    /* Initialize all modules */
    SYS_Initialize(NULL);

    while (true)
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks();
        touch_process();

        if (measurement_done_touch == 1)
        {
            measurement_done_touch = 0;
            processTouchStatus();
            // process touch data
        }
    }

    /* Execution should not come here during normal operation */

    return (EXIT_FAILURE);
}
/*============================================================================
void processTouchStatus(void)
------------------------------------------------------------------------------
Purpose: Sample code snippet to demonstrate how to check the status of the
         sensors
Input  : none
Output : none
Notes  : none
============================================================================*/
void processTouchStatus(void)
{
    uint8_t key_status1 = 0;
    /* Read touchDelta of all channels */
    for (uint8_t chCnt = 0; chCnt < DEF_NUM_CHANNELS; chCnt++)
    {
        touchDelta[chCnt] = (int16_t)get_sensor_node_signal(chCnt) - (int16_t)get_sensor_node_reference(chCnt);
    }

    key_status1 = get_sensor_state(0) & KEY_TOUCHED_MASK;
    if (0u != key_status1)
    {
        if (touchDetectMultiTouch(0))
        {
            // Touch No detect
            LED_1_Set();
        }
        else
        {
            // Touch detect
            LED_1_Clear();
        }
    }
    else
    {
        // Touch No detect
        LED_1_Set();
    }

    key_status1 = get_sensor_state(1) & KEY_TOUCHED_MASK;
    if (0u != key_status1)
    {
        if (touchDetectMultiTouch(1))
        {
            // Touch No detect
            LED_2_Set();
        }
        else
        {
            // Touch detect
            LED_2_Clear();
        }
    }
    else
    {
        // Touch No detect
        LED_2_Set();
    }

    key_status1 = get_sensor_state(2) & KEY_TOUCHED_MASK;
    if (0u != key_status1)
    {
        if (touchDetectMultiTouch(2))
        {
            // Touch No detect
            LED_3_Set();
        }
        else
        {
            // Touch detect
            LED_3_Clear();
        }
    }
    else
    {
        // Touch No detect
        LED_3_Set();
    }

    key_status1 = get_sensor_state(3) & KEY_TOUCHED_MASK;
    if (0u != key_status1)
    {
        if (touchDetectMultiTouch(3))
        {
            // Touch No detect
            LED_4_Set();
        }
        else
        {
            // Touch detect
            LED_4_Clear();
        }
    }
    else
    {
        // Touch No detect
        LED_4_Set();
    }
}

/*============================================================================
bool touchDetectMultiTouch (uint16_t active_sensor)
------------------------------------------------------------------------------
Purpose: Check touchDelta of adjacent sensors and alert if any are greater than the
         cut-off threshold which is 33% of sensor in touch
Input  : active_sensor
Output : bool (true if another sensor delta exceeded the 33% of sensor in detect)
Notes  : none
============================================================================*/
bool touchDetectMultiTouch(uint8_t active_sensor)
{
    // 33% of delta is the threshold for the multi-touch_detected function
    int16_t threshold = touchDelta[active_sensor] / 3;

    for (uint8_t chCnt = 0; chCnt < DEF_NUM_CHANNELS; chCnt++)
    {
        if (chCnt != active_sensor)
        {
            if (touchDelta[chCnt] > threshold)
            {
                return true;
            }
        }
    }
    return false;
}

/*******************************************************************************
 End of File
*/

