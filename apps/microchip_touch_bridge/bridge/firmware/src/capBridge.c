/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    capBridge.c

  Summary:
    This file contains the source code for CAP GUI related functions.

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

#include "capBridge.h"

capBrdigeComamnt_t rxData;
capBrdigeComamnt_t txData;

void capBridgeInit(void){ 
    touchI2cInit(NULL, NULL);
}

void capBridgeProcessRxData(uint8_t *rxBuffer, uint8_t *txBuffer) {

    capBrdigeComamnt_t *rxdata;
    capBrdigeComamnt_t *txdata;
    rxdata = (capBrdigeComamnt_t *) rxBuffer;
    txdata = (capBrdigeComamnt_t *) txBuffer;

    for(uint8_t ii = 0; ii < 8; ii++){
        txBuffer[ii] = rxBuffer[ii];
    }

    switch(rxdata->reg.command)
    {
        case CMD_VERSION:
        {
            txdata->reg.slaveAddress = 0x00 & 0x70; // the board ID
            txdata->reg.regOffsetL = 0;
            txdata->reg.regOffsetH = 1;             //Major version
            txdata->reg.regValueL = 2;
            txdata->reg.regValueH = 5;               //Minor version
        break;
        }
        case CMD_SMB_READ_BYTE:  
        {
            touchI2cSetSlaveAddress(rxdata->reg.slaveAddress>>1);
            touchI2cSetMemoryAddrss(rxdata->reg.regOffsetL);
            txdata->reg.regValueL = touchI2cReadByte();
        break;
        }
        case CMD_SMB_WRITE_BYTE:
        {
            touchI2cSetSlaveAddress(rxdata->reg.slaveAddress>>1);
            touchI2cSetMemoryAddrss(rxdata->reg.regOffsetL);
            touchI2cWriteByte(rxdata->reg.regValueL);
        break;
        }
        case CMD_ARA_WRITE:
        {
            break;
        }
        case  CMD_ARA_READ:
        {
            break;
        }
        default:
        break;
    }
}
