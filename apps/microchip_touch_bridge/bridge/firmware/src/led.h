/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    led.h

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


//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

/**
 * @file led.h
 * @brief This file contains header for led module
 * @copyright Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 * 
 */

#ifndef _LED_H
#define _LED_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "config_summary.h"

/**
 * @brief LEDs status
 * 
 */
typedef enum {
    LED_NO_ACTION,
    LED_BLINK,
    LED_OFF,
    LED_ON
} ledStatus_t;

/**
 * @brief Maximum number of LEDs used in this application
 * 
 */
typedef enum {
    YELLOW_LED,
    RED_LED,
    GREEN_LED,
    MAX_LED
} numOfLeds_t;

/**
 * @brief maintains status and cnt needed for blink functionality
 * 
 */
typedef struct tagLeds_t {
    ledStatus_t status;
    uint8_t currentTime;
    uint8_t blink_cnt;
    uint16_t blink_time;
    uint16_t blinkOffTime;
    uint16_t blinkOnTime;
} leds_t;

/**
 * @brief Updated LED module's time reference
 * 
 */
void ledUpdateTime(void);

/**
 * @brief Configures a particular LED for blink.
 * 
 * @param which_led Which led needs to be configured for blinking
 * @param blink_cnt how many times bliking should happen
 * @param onTime On time of the blink
 * @param offTime OFF time of the blink
 */
void ledBlink(numOfLeds_t which_led, uint8_t blink_cnt, uint16_t onTime, uint16_t offTime);
/**
 * @brief LEDs process function. Should be called in main while(1) loop
 * 
 */
void ledProcess(void);

#endif /* _LED_H */
/*******************************************************************************
 End of File
 */
