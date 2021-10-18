/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    hex_parser.h

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

#ifndef __HEX_PARSER_INCLUDED_
#define __HEX_PARSER_INCLUDED_

#define HEX_PARSER_MAX_SEGMENT_LENGTH 32

typedef enum {
    HEX_PARSER_EOF = 0,
    HEX_PARSER_ERROR,
    HEX_PARSER_BUSY,
    HEX_PARSER_WRITE_ERROR,
    HEX_PARSER_ADDRESS_ERROR,
} hex_parser_status_t;

void hex_parser_init (void);
hex_parser_status_t hex_parser_process (const uint8_t *hex_source, uint32_t source_bytes);
void callApp(void);
void print8( uint8_t data );
void print16( uint16_t data );
void print32( uint32_t data );

#endif