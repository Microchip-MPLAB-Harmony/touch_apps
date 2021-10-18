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
#include "config_summary.h"
#include "btApp.h"
#include "usbApp.h"
#include "pcComm.h"
#include "led.h"
#include "common.h"

applicationConfig_t appConfig;
commonStatus_t commonStatus;

uint8_t appver[VERSION_LENGTH] = APPLICATION_VERSION;

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

#define CONCAT(A, B) A ## B

USB_DEVICE_HANDLE usbHandle;
uintptr_t touchUart, touchSpi, touchI2c, timer;

void TimerInterrupt ( uintptr_t context ){
    ledUpdateTime();
    timeToRead = 1;
}

void readInputSwitchStatus(void) {
    if(SW1_Get()) {
        appConfig.switch_positions |= 1;
    }
    if(SW2_Get()) {
        appConfig.switch_positions |= 2;
    }
    if(SW3_Get()) {
        appConfig.switch_positions |= 4;
    }
    if(SW4_Get()) {
        appConfig.switch_positions |= 8;
    }
    
    appConfig.mode = appConfig.switch_positions;
    
    if(appConfig.mode >= MODE_UART_BYPASS_USB_9600 && appConfig.mode <= MODE_UART_BYPASS_USB_115200) {
        appConfig.pcInterface = USB_INTERFACE;
        appConfig.usb_mode = USB_CDC;
    } else if(appConfig.mode == MODE_CAP_BRIDGE) {
        appConfig.pcInterface = USB_INTERFACE;
        appConfig.usb_mode = USB_CAPBRIDGE;
    } else if(appConfig.mode >= MODE_UART_BYPASS_BT_9600 && appConfig.mode <= MODE_UART_BYPASS_BT_115200) {
        appConfig.pcInterface = BT_INTERFACE;
    } else if(appConfig.mode == MODE_TEST) {
        appConfig.pcInterface = NO_INTERFACE;
    }
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    SYS_TIME_CallbackRegisterMS(TimerInterrupt,timer,SYS_TIME_RESOLUTION_MSEC,SYS_TIME_PERIODIC);

    readInputSwitchStatus();

    if(appConfig.mode == MODE_TEST) {
        LED_RED_Clear();
        LED_GREEN_Clear();
        LED_YELLOW_Clear();
        while(1);
    }

    if(appConfig.mode >= MODE_UART_BYPASS_USB_9600 && appConfig.mode <= MODE_UART_BYPASS_BT_115200) {
        touchDeviceInit();
    }

    if(appConfig.pcInterface == USB_INTERFACE) {
        usbInit();
    }
    btInit();

    while ( true )
    {
        if(appConfig.pcInterface == USB_INTERFACE) {
            usbProcess();
        }
        btProcess();

        if(appConfig.mode >= MODE_UART_BYPASS_USB_9600 && appConfig.mode <= MODE_UART_BYPASS_BT_115200) {
            touchDeviceProcess();
        }
        
        ledProcess();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

