/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    uartbypass.c

  Summary:
    This file contains the source code related to bypass mode.

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

#include "uartbypass.h"

#if TOUCH_DEVICE == UART_BYPASS
#include "touchUart.h"
#include "pcComm.h"

volatile uint8_t touchDeviceDataReceived;
volatile uint8_t touchDeviceTransmitInProgress;
volatile uint8_t timeToRead;

void touchDeviceRxCompleteCallback(uint8_t data){
    touchDeviceDataReceived = 1;
    pcWriteTxBufferByte(data);
}

void touchDeviceTxCompleteCallback(void){
    touchDeviceTransmitInProgress = 0;
}

void touchDeviceInit(void) {

    USART_SERIAL_SETUP serailsetup;

    if(appConfig.mode == MODE_UART_BYPASS_USB_115200 || appConfig.mode == MODE_UART_BYPASS_BT_115200) {
      serailsetup.baudRate = 115200;
    } else if(appConfig.mode == MODE_UART_BYPASS_USB_38400 || appConfig.mode == MODE_UART_BYPASS_BT_38400) {
      serailsetup.baudRate = 38400;
    } else if(appConfig.mode == MODE_UART_BYPASS_USB_19200 || appConfig.mode == MODE_UART_BYPASS_BT_19200) {
      serailsetup.baudRate = 19200;
    } else if(appConfig.mode == MODE_UART_BYPASS_USB_9600 || appConfig.mode == MODE_UART_BYPASS_BT_9600) {
      serailsetup.baudRate = 9600;
    } else {
      serailsetup.baudRate = 115200;
    }
    // serailsetup.baudRate = 115200;
    serailsetup.dataWidth = USART_DATA_8_BIT;
    serailsetup.parity = USART_PARITY_NONE;
    serailsetup.stopBits = USART_STOP_1_BIT;

    touchUartInit(touchDeviceTxCompleteCallback, touchDeviceRxCompleteCallback, &serailsetup);

}

void touchDeviceProcess(void) {
    uint8_t data;
    if(touchDeviceTransmitInProgress == 0){
        /* length of data received from PC is more than 1*/
        if(pcLengthRxBuffer()){
            touchDeviceTransmitInProgress = 1;
            data = pcReadRxBuffer();
            touchUartSendData(&data, 1);
        }
    }
}

#endif