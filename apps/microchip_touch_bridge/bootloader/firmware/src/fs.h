/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    fs.h

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

/**
 * @file fs.h
 * @brief This file contains the vitual fat file system
 * @copyright Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 * 
 */

#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "definitions.h"

/**
 * @brief kB to bytes convertor
 * 
 */
#define kB(x) (x*1024u)

/**
 * @brief Macro too keep a particular variable in a particular address
 * 
 */
#define KEEP(x)              __attribute__((keep)) __attribute__((address(x)))

/**
 * @brief Position in KITINFO.TXT file to update BT Version info
 * 
 */
#define BT_VERSION_POS 55u
/**
 * @brief Position in KITINFO.TXT file to update APP Version info
 * 
 */
#define APP_VERSION_POS 80u
/**
 * @brief Position in KITINFO.TXT file to update APP Status
 * 
 */
#define APP_STATUS_POS 94u
/**
 * @brief Length of application status "YES"/"NO "
 * 
 */
#define APP_STATUS_LENGTH 3u
/**
 * @brief Postion in KITINFO.TXT file to update dip switch status.
 * 
 */
#define SWITCH_STATUS_POS 110u
/**
 * @brief Length of application status "YES"/"NO "
 * 
 */
#define SWITCH_STATUS_LENGTH 4u

DRV_HANDLE fsOpen( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent );

/**
 * @brief 
 * 
 * @param handle 
 */
void fsClose( const DRV_HANDLE handle );

SYS_STATUS fsStatus( const SYS_MODULE_INDEX drvIndex );

/**
 * @brief This function does not do any action.
 * 
 * @param handle Memory handle
 * @param address address of sector which needs to be erased
 * @return Always returns true
 */
bool fsSectorErase( const DRV_HANDLE handle, uint32_t address );

/**
 * @brief This function sends FAT data to USB Host.
 * 
 * @param handle Memory handle
 * @param rx_data incoming data buffer pointer
 * @param rx_data_length length of requested data
 * @param address address from which the data must be sent.
 * @return Always return true.
 */
bool fsRead( const DRV_HANDLE handle, void *rx_data, uint32_t rx_data_length, uint32_t address );

/**
 * @brief Write data to flash memory. Receives data and pass the data to hex parser.
 * 
 * @param handle Memory handle
 * @param tx_data incoming data buffer pointer
 * @param address address to which the data must be written. This address is used only to determine whether the host accesses FAT region or not.
 * @return Always return true.
 */
bool fsPageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address );

/**
 * @brief Returns the status of the transfer
 * 
 * @param handle Memory Handle
 * @return MEMORY_DEVICE_TRANSFER_STATUS It always returns "MEMORY_DEVICE_TRANSFER_COMPLETED"
 */
MEMORY_DEVICE_TRANSFER_STATUS fsTransferStatusGet( const DRV_HANDLE handle );

/**
 * @brief Returns the geometry of the memory.
 * This returns the geometry as 1MB eventhrough SAMD21 does not have 1MB.
 * This is is hanlded in fsPageWrite()
 * 
 * @param handle Memory Handle
 * @param geometry pointer to update geometry
 * @return alwyas returns true
 */
bool fsGeometryGet( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry );

/**
 * @brief The data received in fsPageWrite is processed in this function.
 * This function monitors the incoming data and identifies whether the data is for .hex file.
 * If yes, then the data is sent to hexparser code. Also, this function checks for completion of the .hex file reception.
 * 
 * @param data data buffer pointer
 * @param addrIn address input (as per FAT defenition)
 * @param secnum Sector number to which the data is inteded for
 */
void fsWriteProcess(uint8_t *data, uint32_t addrIn, uint16_t secnum);

/**
 * @brief Size of the sector
 * 
 */
#define SECTOR_SIZE 512u

/**
 * @brief return size of the array
 * 
 */
#define elements(a) (sizeof(a)/sizeof(*a))


#define COMPILER_PRAGMA(arg)          _Pragma(#arg)
#define COMPILER_PACK_SET(alignment)  COMPILER_PRAGMA(pack(alignment))
#define COMPILER_PACK_RESET()         COMPILER_PRAGMA(pack())

COMPILER_PACK_SET(1);
/**
 * @brief FAT File Entry format in Root Director
 * 
 */
typedef union FatDirectoryEntry {
    uint8_t data[32];
    struct {
        uint8_t filename[11];
        uint8_t attributes;
        uint8_t reserved;
        uint8_t creation_time_ms;
        uint16_t creation_time;
        uint16_t creation_date;
        uint16_t accessed_date;
        uint16_t first_cluster_high_16;
        uint16_t modification_time;
        uint16_t modification_date;
        uint16_t first_cluster_low_16;
        uint32_t filesize;
    } ;
} FatDirectoryEntry_t;
COMPILER_PACK_RESET();

#ifdef __cplusplus
}
#endif

#endif
