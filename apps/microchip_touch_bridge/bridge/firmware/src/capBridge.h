/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    capBridge.h

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

#ifndef _CAPBRIDGE_H    /* Guard against multiple inclusion */
#define _CAPBRIDGE_H

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "touchI2C.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define CMD_VERSION              15
#define CMD_ARA_WRITE            14
#define CMD_ARA_READ             13
#define CMD_SMB_READ_BYTE        1
#define CMD_SMB_WRITE_BYTE       2

typedef struct __attribute__((packed)) {
  uint8_t  command;       // Request type
  uint8_t  slaveAddress;
  uint8_t  regOffsetL;
  uint8_t  regOffsetH;
  uint8_t  regValueL;
  uint8_t  regValueH;
  uint8_t  checksum;
  uint8_t  error;
}commandDataStruct_t;

typedef union tag_capBridgeCommands_t {
  commandDataStruct_t reg;
  uint8_t bytes[8];
}capBrdigeComamnt_t;

void capBridgeInit(void);
void capBridgeProcessRxData(uint8_t *rxBuffer, uint8_t *txBuffer);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _CAPBRIDGE_H */

/* *****************************************************************************
 End of File
 */
