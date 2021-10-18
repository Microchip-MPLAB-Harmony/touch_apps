/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    pcComm.c

  Summary:
    This file contains the source code related to PC communications.

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
 * @file pcComm.c
 * @brief This file contains buffer declaration, function defenition for PC Side communication 
 * @copyright Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 * 
 */

#include "pcComm.h"


/**
 * \brief MCU to PC Circular buffer. The data which needs to be transmitted to
 *  PC through USB/BLE is written to this buffer.
 * use pcWriteTxBufferByte() or pcWriteTxBuffer() to write to this buffer
 * use pcReadTxBuffer() to read data from this buffer
 */
static volatile uint8_t pcCommTxBuffer[CIRCULAR_BUFFER_LEN];
/**
 * \brief MCU to PC Circular buffer Read pointer
 * 
 */
static volatile transferSize_t pcCommTxBufferReadPtr;
/**
 * \brief MCU to PC Circular buffer write pointer
 * 
 */
static volatile transferSize_t pcCommTxBufferWritePtr;
/**
 * \brief MCU to PC transmit stauts flag
 * 
 */
static volatile uint8_t pcCommTxInProgress;

/**
 * \brief PC to MCU Circular buffer. The data which is received from PC trhough
 * USB/BLE is written to this buffer.
 * use pcWriteRxBuffer() to write to this buffer
 * use pcReadTxBuffer() to read from this buffer
 */
static volatile uint8_t pcCommRxBuffer[CIRCULAR_BUFFER_LEN];
/**
 * \brief PC to MCU Circular buffer Read pointer
 * 
 */
static volatile transferSize_t pcCommRxBufferReadPtr;
/**
 * \brief PC to MCU Circular buffer write pointer
 * 
 */
static volatile transferSize_t pcCommRxBufferWritePtr;
/**
 * \brief PC to MCU receive stauts flag
 * 
 */
static volatile uint8_t pcCommDataReceived;

/**
 * @brief This function fills the pcCommRxBuffer.
 * This is used by USB/BLE to fill data from PC to pcCommRxBuffer buffer.
 * 
 * @param buffer buffer to copy the data
 * @param len length of data to be copied
 */
void pcWriteRxBuffer(uint8_t *buffer, transferSize_t len){
    for(transferSize_t cnt = 0; cnt < len; cnt++){
        pcCommRxBuffer[pcCommRxBufferWritePtr] = buffer[cnt];
        pcCommRxBufferWritePtr++;
        if(pcCommRxBufferWritePtr == CIRCULAR_BUFFER_LEN){
            pcCommRxBufferWritePtr = 0;
        }
    }
}

/**
 * @brief This function returns the number of NEW data available in pcCommRxBuffer.
 * This is used by application to get data from PC.
 * 
 * @return transferSize_t returns the number of data available to read
 */
transferSize_t pcLengthRxBuffer(void){
    transferSize_t length = 0;
    if(pcCommRxBufferWritePtr >= pcCommRxBufferReadPtr){
        length = pcCommRxBufferWritePtr - pcCommRxBufferReadPtr;
    }else{
        length = CIRCULAR_BUFFER_LEN-pcCommRxBufferReadPtr+pcCommRxBufferWritePtr;
    }
    return length;
}

/**
 * @brief This funciton returns one byte of data from the pcCommRxBuffer buffer.
 * This is used by application to get the data from PC.
 * 
 * @return uint8_t data
 */
uint8_t pcReadRxBuffer(void) {
    uint8_t data = 0;
    if(pcCommRxBufferReadPtr != pcCommRxBufferWritePtr){
        data = pcCommRxBuffer[pcCommRxBufferReadPtr];
        pcCommRxBufferReadPtr++;
        if(pcCommRxBufferReadPtr == CIRCULAR_BUFFER_LEN){
            pcCommRxBufferReadPtr = 0;
        }
    }
    return data;
}

/**
 * @brief This function returns the number of NEW data available in pcCommTxBuffer.
 * This is used by USB/BLE process to get data to PC.
 * 
 * @return transferSize_t returns the number of data available to send to PC
 */
transferSize_t pcLengthTxBuffer(void){
    transferSize_t length = 0;
    if(pcCommTxBufferWritePtr >= pcCommTxBufferReadPtr){
        length = pcCommTxBufferWritePtr - pcCommTxBufferReadPtr;
    }else{
        length = CIRCULAR_BUFFER_LEN-pcCommTxBufferReadPtr+pcCommTxBufferWritePtr;
    }
    return length;
}

/**
 * @brief This funciton returns one byte of data from the pcCommTxBuffer.
 * This is used by USB/BLE process to get the data from PC.
 * 
 * @return uint8_t data
 */
uint8_t pcReadTxBuffer(void){
    uint8_t data = 0;
    if(pcCommTxBufferReadPtr != pcCommTxBufferWritePtr){
        data = pcCommTxBuffer[pcCommTxBufferReadPtr];
        pcCommTxBufferReadPtr++;
        if(pcCommTxBufferReadPtr == CIRCULAR_BUFFER_LEN){
            pcCommTxBufferReadPtr = 0;
        }
    }
    return data;
}

/**
 * @brief This function fills the pcCommTxBuffer.
 * If application wants to send any data to PC then this function is used.
 * Alternate function pcWriteTxBufferByte()
 * 
 * @param buffer buffer to copy the data
 * @param len length of data to be copied
 */
void pcWriteTxBuffer(uint8_t *buffer, transferSize_t len){
    
    for(transferSize_t cnt = 0; cnt < len; cnt++){
        pcCommTxBuffer[pcCommTxBufferWritePtr] = buffer[cnt];
        pcCommTxBufferWritePtr++;
        if(pcCommTxBufferWritePtr == CIRCULAR_BUFFER_LEN){
            pcCommTxBufferWritePtr = 0;
        }
    }
}

/**
 * @brief This function fills the pcCommTxBuffer.
 * If application wants to send any data to PC then this function is used.
 * Alternate function pcWriteTxBuffer()
 * 
 * @param data data which needs to be copied to data buffer
 */
void pcWriteTxBufferByte(uint8_t data) {
    pcCommTxBuffer[pcCommTxBufferWritePtr] = data;
    pcCommTxBufferWritePtr++;
    if(pcCommTxBufferWritePtr == CIRCULAR_BUFFER_LEN){
        pcCommTxBufferWritePtr = 0;
    }
}

void pcCommResetPtrs(void){
    pcCommTxBufferWritePtr = 0;
    pcCommTxBufferReadPtr = 0;
    pcCommRxBufferWritePtr = 0;
    pcCommRxBufferReadPtr = 0;
}

void pcCommInit(void) {

}

void pcCommProcess(void) {
    
}

/*******************************************************************************
 End of File
 */

