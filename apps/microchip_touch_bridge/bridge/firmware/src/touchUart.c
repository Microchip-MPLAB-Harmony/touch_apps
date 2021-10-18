/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    touchUART.c

  Summary:
    This file contains the source code related to UART driver

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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "touchUart.h"
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

uintptr_t touchUart;
/**
 * @brief UART Tx complete call back function
 * 
 */
callbackTx_T txCompleteCallback;
/**
 * @brief UART Rx complete call back function
 * 
 */
callbackRx_T rxCompleteCallback;

/**
 * @brief Rx Data
 * 
 */
static volatile uint8_t rxData;

/**
 * @brief UART Tx complete call back function.
 * If user callback is not NULL, call the user registered callback function
 * 
 */
void touchUartTxComplete(uintptr_t touchUart) {
    if(txCompleteCallback != NULL){
        txCompleteCallback();
    }
}

/**
 * @brief UART Rx complete call back function.
 * If user callback is not NULL, call the user registered callback function
 * 
 */
void touchUartRxComplete(uintptr_t touchUart){
    if(rxCompleteCallback != NULL){
        rxCompleteCallback(rxData);
    }
    SERCOM5_USART_Read((void *) &rxData,1);
}

/**
 * @brief initialization function to register uesr level callback and to configure
 * UART serttings.
 * 
 * @param txCallback user level callback function to indicate tx complete
 * @param rxCallback user level callback function to indicate rx complete
 */
void touchUartInit(callbackTx_T txCallback, callbackRx_T rxCallback, USART_SERIAL_SETUP *serailsetup) {
    
    txCompleteCallback = txCallback;
    rxCompleteCallback = rxCallback;

    SERCOM5_USART_SerialSetup(serailsetup, SERCOM5_USART_FrequencyGet());
    
    
    SERCOM5_USART_ReadCallbackRegister(touchUartRxComplete, touchUart);
    SERCOM5_USART_WriteCallbackRegister(touchUartTxComplete, touchUart);
    
    SERCOM5_USART_Read((void *) &rxData,1);
}

/**
 * @brief Transmit data through UART
 * 
 * @param buffer data buffer pointer
 * @param len length of data buffer
 */
void touchUartSendData(uint8_t *buffer, transferSize_t len){
    
    SERCOM5_USART_Write((void *) buffer, len);
}

/*******************************************************************************
 End of File
 */

