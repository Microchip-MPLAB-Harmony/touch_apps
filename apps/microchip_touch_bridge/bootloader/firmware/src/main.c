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
#include "common.h"
#include "hex_parser.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

//uint8_t buffer[NVMCTRL_FLASH_PAGESIZE];
uint8_t buffer[NVMCTRL_FLASH_PAGESIZE];// __attribute__((aligned(NVMCTRL_FLASH_ROWSIZE),keep,externally_visible,space(prog)))= {0};

volatile uint32_t swtichToAppDelay = 0;
volatile uint32_t waitingForHexFileDelay = 0;

applicationConfig_t appConfig;
commonStatus_t commonStatus;

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

}

void readVersionInformation(void) {
  memcpy(&commonStatus, (uint8_t *) COMMON_STATUS_ADDR, sizeof(commonStatus_t));
}


void resetBt(void) {
    BT_WAKEUP_Set();
    BT_BTN_Set();
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    readInputSwitchStatus();

    readVersionInformation();

    LED_YELLOW_OutputEnable();
    LED_YELLOW_Set();
    LED_RED_OutputEnable();
    LED_RED_Set();
    LED_GREEN_OutputEnable();
    LED_GREEN_Set();

    resetBt();

    // appConfig.mode = MODE_CAP_BOOTLOADER;

    if(appConfig.mode != MODE_CAP_BOOTLOADER) {
      callApp();
      SYS_CONSOLE_Print(sysObj.sysConsole0, "No Valid App Found\n");
    }else {
    }
    SYS_CONSOLE_Print(sysObj.sysConsole0, "Entering Bootloader Mode...\n");

    LED_YELLOW_Clear();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

        if(swtichToAppDelay != 0) {

          while(SYS_CONSOLE_WriteCountGet(sysObj.sysConsole0)) {
            SYS_CONSOLE_Tasks(sysObj.sysConsole0);
          }
          
          swtichToAppDelay--;
          
          if(swtichToAppDelay == 1) {

            USB_DEVICE_Deinitialize(sysObj.usbDevObject0);

            NVIC_DisableIRQ(USB_IRQn);
            NVIC_DisableIRQ(SERCOM0_IRQn);

            WDT_REGS->WDT_CTRL = WDT_CTRL_ENABLE_Msk;
            while((WDT_REGS->WDT_STATUS & WDT_STATUS_SYNCBUSY_Msk) != WDT_STATUS_SYNCBUSY_Msk);

            /* wait for watchdog reset */

            while(1);

            // callApp();
          }
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

