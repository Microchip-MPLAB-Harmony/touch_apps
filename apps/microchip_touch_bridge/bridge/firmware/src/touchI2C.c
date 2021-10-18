/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    touchI2C.c

  Summary:
    This file contains the source code related to I2C driver

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
 * @file touchI2C.c
 * @brief This file contains header for touch I2C communication functions
 * @copyright Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 * 
 */

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "touchI2C.h"

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static uintptr_t touchI2c;

volatile static uint8_t i2cWriteBuffer[CIRCULAR_BUFFER_LEN];
volatile static uint8_t i2cReadBuffer[CIRCULAR_BUFFER_LEN];
static uint8_t mode;
volatile static uint8_t slaveAddress = 0;
volatile static uint8_t rxInProgress;
volatile static uint8_t txInProgress;
volatile static uint8_t memAddress;

static callbackTx_T txCompleteCallback;
static callbackRx_T rxCompleteCallback;

static void touchI2cTxComplete(uintptr_t touchI2c){
    txInProgress = 0;
    if(txCompleteCallback != NULL){
        txCompleteCallback();
    }
}

static void touchI2cRxComplete(uintptr_t touchI2c){
    rxInProgress = 0;
    if(rxCompleteCallback != NULL){
        rxCompleteCallback(i2cReadBuffer[0]);
    }
}

static void touchI2cCompleteCallback(uintptr_t touchI2c){
    if(mode == 0){
        touchI2cTxComplete(touchI2c);
    } else {
        touchI2cRxComplete(touchI2c);
    }
}

/**
 * @brief initialization function to register uesr level callback and to configure
 * I2C settings.
 * 
 * @param txCallback user level callback function to indicate tx complete
 * @param rxCallback user level callback function to indicate rx complete
 */
void touchI2cInit(callbackTx_T txCallback, callbackRx_T rxCallback) {
    
    txCompleteCallback = txCallback;
    rxCompleteCallback = rxCallback;
    
    SERCOM2_I2C_CallbackRegister(touchI2cCompleteCallback, touchI2c);
}

/**
 * @brief Set the slave address from/to which the data must be read/write
 * 
 * @param slaveAddr I2C Slave address
 */
void touchI2cSetSlaveAddress(uint8_t slaveAddr){
    slaveAddress = slaveAddr;
}

/**
 * @brief Set the memroy address from/to which the data must be read/write
 * 
 * @param memAddr address of the memory
 */
void touchI2cSetMemoryAddrss(uint8_t memAddr){
    txInProgress = 1;
    mode = 0;
    memAddress = memAddr;
    SERCOM2_I2C_Write(slaveAddress, (uint8_t *) &memAddress, 1);
    while(txInProgress);
}

/**
 * @brief This function writes one byte of data. This is a blocking code.
 * Before calling this function, the memory address and slave address 
 * to which this data to be written must be set using touchI2cSetMemoryAddrss() and touchI2cSetSlaveAddress() 
 * for the very first byte.
 * The memory address is increased later on in this function.
 * 
 * Usage: \n 
 * touchI2cSetSlaveAddress(0x25); \n 
 * touchI2cSetMemoryAddrss(0x10); // Memory address is 0x10 \n 
 * for(uint8_t cnt = 0; cnt < 10; cnt ++) touchI2cWriteByte(buffer[cnt]);
 * 
 */
void touchI2cWriteByte(uint8_t data){
    txInProgress = 1;
    mode = 0;
    i2cWriteBuffer[0] = memAddress;
    i2cWriteBuffer[1] = data;
    memAddress++;
    SERCOM2_I2C_Write(slaveAddress, (uint8_t *) i2cWriteBuffer, 2);
    while(txInProgress);
}

/**
 * @brief This function reads one byte of data. This is a blocking code.
 * Before calling this function, the memory address and slave address 
 * to which this data to be written must be set using touchI2cSetMemoryAddrss() and touchI2cSetSlaveAddress() 
 * for the very first byte.
 * The memory address is increased later on in this function.
 * 
 * Usage: \n 
 * touchI2cSetSlaveAddress(0x25); \n 
 * touchI2cSetMemoryAddrss(0x10); // Memory address is 0x10 \n 
 * for(uint8_t cnt = 0; cnt < 10; cnt ++) buffer[cnt] = touchI2cReadByte();
 * 
 * @return uint8_t returns read data
 */
uint8_t touchI2cReadByte(void){
    rxInProgress = 1;
    mode = 1;
    SERCOM2_I2C_Read(slaveAddress, (uint8_t *) i2cReadBuffer, 1);
    while(rxInProgress);
    
    return i2cReadBuffer[0];
}

/**
 * @brief This function send data "len" number of data from address "memAddr".
 * It sends write command with first address being memAddr. 
 * This is a non-blocking code.
 * The completion is indicated by txCallback function registered in touchI2cInit 
 *
 * Usage: \n 
 * touchI2cSendDataToAddress(0x25, 0x10, &buffer[0], 10);
 * 
 * @param slaveAddr slave address
 * @param memAddr memory address to which data must be written
 * @param dataptr pointer from which data must be written
 * @param len length of data that must be written
 */
void touchI2cSendDataToAddress(uint8_t slaveAddr, uint8_t memAddr, uint8_t *dataptr, transferSize_t len){
    
    uint8_t *ptr = (uint8_t *) &i2cWriteBuffer[0];
    mode = 0;
    
    /// copy the data to local buffer. First data is memAddr
    i2cWriteBuffer[0] = memAddr;
    ptr = (uint8_t *) &i2cWriteBuffer[1];
    
    for(transferSize_t cnt = 0; cnt < len; cnt++){
        ptr[cnt] = dataptr[cnt];
    }
    
    SERCOM2_I2C_Write((uint16_t)slaveAddr, (uint8_t *) &i2cWriteBuffer[0], len+1);
}

/**
 * @brief This function receives data "len" number of data from address "memAddr".
 * First it sents a I2C write command with data as "memAddr"
 * Second it sends I2C read command for length defined by "len"
 * This is a non-blocking code.
 * The completion is indicated by rxCallback function registered in touchI2cInit 
 * 
 * Usage: \n 
 * touchI2cReceiveDataFromAddress(0x25, 0x10, &buffer[0], 10);
 * 
 * @param slaveAddr slave address
 * @param memAddr memory address from which data must be read
 * @param dataptr pointer to which data must be read
 * @param len length of data that must be read
 */
void touchI2cReceiveDataFromAddress(uint8_t slaveAddr, uint8_t memAddr, uint8_t *dataptr, transferSize_t len){
    
    mode = 1;
   
    i2cWriteBuffer[0] = memAddr;

    SERCOM2_I2C_WriteRead((uint16_t)slaveAddr, (uint8_t *) &i2cWriteBuffer[0], 1, dataptr, len);
}


/*******************************************************************************
 End of File
 */
