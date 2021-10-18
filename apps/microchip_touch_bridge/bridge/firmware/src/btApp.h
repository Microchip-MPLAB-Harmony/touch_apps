/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    btApp.h

  Summary:
    This file contains the source code for Bluetooth module related functions.

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

#ifndef _BTAPP_H    /* Guard against multiple inclusion */
#define _BTAPP_H


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

    
#include "config_summary.h"

/**
 * @brief Initializes Bluethooth communications
 * 
 */
void btInit(void);
/**
 * @brief Processes data transaction related to Bluetooth.
 * This should be called in main while(1) loop.
 * 
 */
void btProcess(void);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BT_APP_H */

/* *****************************************************************************
 End of File
 */
