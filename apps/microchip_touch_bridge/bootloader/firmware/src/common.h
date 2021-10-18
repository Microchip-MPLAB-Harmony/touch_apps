/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    hex_parser.h

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

#ifndef __COMMON_H_
#define __COMMON_H_

typedef enum tag_appMode_t {
    MODE_UART_BYPASS_USB_9600,
    MODE_UART_BYPASS_USB_19200,
    MODE_UART_BYPASS_USB_38400,
    MODE_UART_BYPASS_USB_115200,
    MODE_UART_BYPASS_BT_9600,
    MODE_UART_BYPASS_BT_19200,
    MODE_UART_BYPASS_BT_38400,
    MODE_UART_BYPASS_BT_115200,
    MODE_REESRVED1,
    MODE_REESRVED2,
    MODE_REESRVED3,
    MODE_REESRVED4,
    MODE_REESRVED5,
    MODE_TEST,
    MODE_CAP_BRIDGE,
    MODE_CAP_BOOTLOADER,
}appMode_t;

typedef struct tag_applicationConfig_t {
    uint8_t switch_positions;
    uint8_t pcInterface;
    appMode_t mode;
    uint8_t usb_mode;
}applicationConfig_t;

/**
 * @brief Starting address of application
 * 
 */
#define APP_START_ADDR	(0x10000)

/**
 * @brief common status is stored in this address location
 */
#define COMMON_STATUS_ADDR (APP_START_ADDR-STATUS_LEN)

/**
 * @brief Length of version in format "00.00.00"
 * 
 */
#define VERSION_LENGTH 8u
/**
 * @brief Length of status
 * 
 */
#define STATUS_LEN 64u
/**
 * @brief Bootloader version is stored here
 * 
 */
#define BT_VERSION_ADDR	(APP_START_ADDR-STATUS_LEN)

#define BOOTLOADER_VERSION "01.00.00" // must be 8 characters

typedef struct tag_commonStatus_t {
  uint8_t bl_version[VERSION_LENGTH];
}commonStatus_t;

extern commonStatus_t commonStatus;
extern applicationConfig_t appConfig;

#endif