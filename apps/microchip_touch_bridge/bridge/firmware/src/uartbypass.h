/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    uartbypass.h

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

#ifndef _UARTBYPASS_H
#define _UARTBYPASS_H

#include "config_summary.h"

#if TOUCH_DEVICE == UART_BYPASS

extern volatile uint8_t timeToRead;

void touchDeviceInit(void);
void touchDeviceProcess(void);
void touchDeviceRxCompleteCallback(uint8_t data);
void touchDeviceTxCompleteCallback(void);
void touchDeviceSendData(uint8_t *buffer, transferSize_t len);
#endif

#endif