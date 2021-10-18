/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    touchSPI.c

  Summary:
    This file contains the source code related to SPI driver

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
#include "touchSPI.h"

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

uintptr_t touchSPI;

callbackTx_T txCompleteCallback;
callbackRx_T rxCompleteCallback;

static volatile uint8_t rxData;

void touchSPITransferComplete(uintptr_t touchSPI){
    if(txCompleteCallback != NULL){
        txCompleteCallback();
    }
}

void touchSPIRxComplete(uintptr_t touchSPI){
    if(rxCompleteCallback != NULL){
        rxCompleteCallback(rxData);
    }
    SERCOM3_SPI_Read((void *) &rxData,1);
}

void touchSPIInit(callbackTx_T txCallback, callbackRx_T rxCallback) {
    
    txCompleteCallback = txCallback;
    rxCompleteCallback = rxCallback;
    
    SERCOM3_SPI_CallbackRegister(touchSPITransferComplete, touchSPI);
}

void touchSPISendData(uint8_t *buffer, transferSize_t len){
    SERCOM3_SPI_Write((void *) buffer, len);
}

void touchSPIReadWrite(uint8_t *txBuffer, transferSize_t txSize, uint8_t* rxBuffer, transferSize_t rxSize){
    SERCOM3_SPI_WriteRead(txBuffer, txSize, rxBuffer, rxSize);
}

void touchSPIActivateSS(void) {
    SPI_SS_Clear();
}

void touchSPIDeactivateSS(void) {
    SPI_SS_Set();
}

uint8_t touchSPIExchangeData(uint8_t txdata) {
    uint8_t rxdata;
    SERCOM3_SPI_WriteRead(&txdata, 1, &rxdata, 1);
    while(SERCOM3_SPI_IsBusy());
    return rxdata;
}

void touchSPIDelayUsec(uint32_t delay) {

  SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;
  SYS_TIME_RESULT result = SYS_TIME_ERROR;
  result = SYS_TIME_DelayUS(delay, &timer);

  if (result != SYS_TIME_ERROR)
  {
        // Handle error
    if (SYS_TIME_DelayIsComplete(timer) != true){
        while (SYS_TIME_DelayIsComplete(timer) == false);
    }
  }
    
//   else if (SYS_TIME_DelayIsComplete(timer) != true)
//   {
//       // Wait till the delay has not expired
//       while (SYS_TIME_DelayIsComplete(timer) == false);
//   }
}
/*******************************************************************************
 End of File
 */

