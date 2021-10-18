/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    led.c

  Summary:
    This file contains the source code related LED functionality.

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


/**
 * @file led.c
 * @brief This file contains logic for glowing/blinking LEDs
 * @copyright Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 * 
 */

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
#include "led.h"

/**
 * @brief contains the LED stauts for MAX_LED counts
 * 
 */
static leds_t leds[MAX_LED];

/**
 * @brief update the GPIO of hte corresponding led
 * 
 * @param which_led which led needs to be updated
 * @param status ON or OFF
 */
static void ledUpdate(uint8_t which_led, uint8_t status) {
    switch(which_led) {
        case RED_LED:
            if(status){
                LED_RED_Clear();
            }else{
                LED_RED_Set();
            }
            break;
        case YELLOW_LED:
            if(status){
                LED_YELLOW_Clear();
            }else{
                LED_YELLOW_Set();
            }
            break;
        case GREEN_LED:
            if(status){
                LED_GREEN_Clear();
            }else{
                LED_GREEN_Set();
            }
            break;
        default:
            break;
    }
}

void ledUpdateTime(void)
{
    for(uint8_t cnt = 0; cnt < MAX_LED; cnt++){
        leds[cnt].blink_time += SYS_TIME_RESOLUTION_MSEC;
    }
}

void ledProcess(void) {
    /* cycle through all LEDs */
    for(uint8_t cnt = 0; cnt < MAX_LED; cnt++) {

        /* check if LED is configured for Blink */
        if (leds[cnt].status == LED_BLINK) {
            if(leds[cnt].blink_time > leds[cnt].blinkOffTime) {
            /* if time is more than OFF time and if blink count
                still more than 0, turn ON led*/
                leds[cnt].blink_time = 0;
                if(leds[cnt].blink_cnt > 0) {
                    leds[cnt].blink_cnt --;
                    ledUpdate(cnt, 1);
                }
            } else if(leds[cnt].blink_time > leds[cnt].blinkOnTime) {
            /* if time is more than ON time turn OFF the LED*/
                ledUpdate(cnt, 0);
                /* if blink count is zero, then reset LED status */
                if(leds[cnt].blink_cnt == 0) {
                    leds[cnt].blink_time = 0;
                    leds[cnt].status = LED_NO_ACTION;
                }
            }
        }
    }
}

void ledBlink(numOfLeds_t which_led, uint8_t blink_cnt, uint16_t onTime, uint16_t offTime)
{
	if(blink_cnt != 0 && leds[which_led].status == LED_NO_ACTION) {
        ledUpdate(which_led, 1);
		leds[which_led].status = LED_BLINK;
		leds[which_led].blink_cnt = blink_cnt-1;
		leds[which_led].blink_time = 0;
        leds[which_led].blinkOffTime = offTime;
        leds[which_led].blinkOnTime = onTime;
	}
}