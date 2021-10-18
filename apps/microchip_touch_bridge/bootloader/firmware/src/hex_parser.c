/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    hex_parser.c

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

#include <inttypes.h>
#include "hex_parser.h"
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#define APP_START_ADDRESS 0x10000

#define HEX_PARSER_RECORDTYPE_DATA                    (0)
#define HEX_PARSER_RECORDTYPE_EOF                     (1)
#define HEX_PARSER_RECORDTYPE_EXTENDED_SEGMENT        (2)
#define HEX_PARSER_RECORDTYPE_START_SEGMENT_ADDRESS   (3) /* Not supported */
#define HEX_PARSER_RECORDTYPE_EXTENDED_LINEAR_ADDRESS (4)
#define HEX_PARSER_RECORDTYPE_START_LINEAR_ADDRESS    (5) /* Not supported */
//hex_parser_callback_t destination_buffer_callback = NULL;

static void process_byte(char byte);
static uint8_t test_newline(char byte);
static uint8_t test_nibble(char byte);
static uint8_t parse_nibble (char byte);

uint32_t hexfile_address;
uint32_t device_address;
uint16_t bytes;
uint8_t *pdata;

typedef enum {
    HEX_PARSER_STATE_NEW = 0,
    HEX_PARSER_STATE_LENGTH0,
    HEX_PARSER_STATE_LENGTH1,
    HEX_PARSER_STATE_ADDRESS0,
    HEX_PARSER_STATE_ADDRESS1,
    HEX_PARSER_STATE_ADDRESS2,
    HEX_PARSER_STATE_ADDRESS3,
    HEX_PARSER_STATE_RECORD0,
    HEX_PARSER_STATE_RECORD1,
    HEX_PARSER_STATE_DATA,
    HEX_PARSER_STATE_CHECKSUM0,
    HEX_PARSER_STATE_CHECKSUM1,
    HEX_PARSER_STATE_NEWLINE
} hex_parser_state_t;

static hex_parser_state_t parser_state = HEX_PARSER_STATE_NEW;
static hex_parser_status_t parser_status = HEX_PARSER_EOF;
static uint8_t parser_counter, parser_nibble;
static uint8_t parser_length, parser_recordtype, parser_checksum;
static uint32_t parser_address, parser_address_segment;
static uint8_t parser_data[HEX_PARSER_MAX_SEGMENT_LENGTH];

void hex_parser_init (void)
{
    device_address = 0;
    
    parser_address_segment = 0;
    parser_state = HEX_PARSER_STATE_NEW;
}

hex_parser_status_t hex_parser_process (const uint8_t *hex_source, uint32_t source_bytes)
{
    parser_status = HEX_PARSER_BUSY;
    while (source_bytes-- && (parser_status == HEX_PARSER_BUSY)){
        process_byte((char)*hex_source++);
    }
    if (parser_status == HEX_PARSER_ERROR) {
    }
    return parser_status;
}

#define repeat_file_as_is 0u
#define print_process_data 0u
#define switch_to_app 1u
#define processs_hex_data 1u
void programBuffer(uint8_t *buffer, uint8_t length, uint32_t address);
void (*funcpointer) (void);
extern uint32_t hexFileSizeExpected, hexFileSizeReceived;


static char getAscci (uint8_t byte)
{
    if ((byte >= 0) && (byte <= 9)) {
        return byte + '0';
    } else if ( byte < 16){
        return byte + 'A' - 10;
    } else {
        return 0;
    }
}

void print8( uint8_t data ) {
    SYS_CONSOLE_Print(sysObj.sysConsole0, "%c%c", getAscci(data>>4), getAscci(data & 0x0f));
}

void print16( uint16_t data ) {
    print8(data>>8);
    print8(data & 0xff);
}

void print32( uint32_t data ) {
    print16(data>>16);
    print16(data & 0xffff);
}

static void process_byte(char byte)
{
    #if repeat_file_as_is == 1u
    SYS_CONSOLE_Print(sysObj.sysConsole0, "%c",byte);
    #endif
    switch (parser_state){
    case HEX_PARSER_STATE_NEW:
        if (test_newline(byte)) {break;} /* skip if newline*/
        if (byte == ':'){
            parser_state  = HEX_PARSER_STATE_LENGTH0;
            parser_checksum = 0;
        } else {    
            parser_status = HEX_PARSER_ERROR;
            break;
        }
    break;
    case HEX_PARSER_STATE_LENGTH0:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }

        parser_length = parse_nibble(byte) * 0x10;
        parser_state = HEX_PARSER_STATE_LENGTH1;

    break;
    case HEX_PARSER_STATE_LENGTH1:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }

        parser_length += parse_nibble(byte);

        parser_checksum += parser_length;
        parser_state = HEX_PARSER_STATE_ADDRESS0;

        if (parser_length > HEX_PARSER_MAX_SEGMENT_LENGTH) {
            parser_status = HEX_PARSER_ERROR;
            break;
        }

    break;
    case HEX_PARSER_STATE_ADDRESS0:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_address = (uint16_t)parse_nibble(byte) * 0x1000;

        parser_state = HEX_PARSER_STATE_ADDRESS1;

    break;
    case HEX_PARSER_STATE_ADDRESS1:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_address += (uint16_t)parse_nibble(byte) * 0x0100;

        parser_state = HEX_PARSER_STATE_ADDRESS2;

    break;
    case HEX_PARSER_STATE_ADDRESS2:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_address += (uint16_t)parse_nibble(byte) * 0x0010;

        parser_state = HEX_PARSER_STATE_ADDRESS3;

    break;
    case HEX_PARSER_STATE_ADDRESS3:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_address += (uint16_t)parse_nibble(byte);

        parser_checksum += (parser_address & 0xFF) + (parser_address >> 8);

        parser_address += parser_address_segment;

        parser_state = HEX_PARSER_STATE_RECORD0;

    break;
    case HEX_PARSER_STATE_RECORD0:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_recordtype = parse_nibble(byte) * 0x10;

        parser_state = HEX_PARSER_STATE_RECORD1;

    break;
    case HEX_PARSER_STATE_RECORD1:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_recordtype += parse_nibble(byte);
        parser_nibble = 0;
        parser_counter = 0;


        parser_checksum += parser_recordtype;

        switch (parser_recordtype) {
        case HEX_PARSER_RECORDTYPE_DATA:
        {
            parser_state = HEX_PARSER_STATE_DATA;
        }
        break;
        case HEX_PARSER_RECORDTYPE_EOF:
            if (parser_length != 0) { parser_status = HEX_PARSER_ERROR; break; }
            parser_state = HEX_PARSER_STATE_CHECKSUM0;
        break;
        case HEX_PARSER_RECORDTYPE_EXTENDED_SEGMENT:
        case HEX_PARSER_RECORDTYPE_EXTENDED_LINEAR_ADDRESS:
            if (parser_length != 2) { parser_status = HEX_PARSER_ERROR; break; }
            parser_state = HEX_PARSER_STATE_DATA;
        break;

        default:
            parser_status = HEX_PARSER_ERROR;
            break;
        }

    break;
    case HEX_PARSER_STATE_DATA:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }

        if (!parser_nibble){
            parser_data[parser_counter] = parse_nibble(byte)*0x10;
            parser_nibble = 1;
        } else {
            parser_data[parser_counter] += parse_nibble(byte);
            parser_checksum += parser_data[parser_counter];

            parser_nibble = 0;
            parser_counter++;
        }

        if (parser_counter >= parser_length){
            parser_state = HEX_PARSER_STATE_CHECKSUM0;
        }
    break;
    case HEX_PARSER_STATE_CHECKSUM0:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }
        parser_checksum += parse_nibble(byte) * 0x10;
        parser_state = HEX_PARSER_STATE_CHECKSUM1;

    break;
    case HEX_PARSER_STATE_CHECKSUM1:
        if (!test_nibble(byte)) { parser_status = HEX_PARSER_ERROR; break; }

        parser_checksum += parse_nibble(byte);
        if (parser_checksum != 0) { parser_status = HEX_PARSER_ERROR;   break; }

        parser_state = HEX_PARSER_STATE_NEWLINE;

    break;
    case HEX_PARSER_STATE_NEWLINE:
        if (test_newline(byte)) {
            /*check recordtype, do the thing.*/

            switch (parser_recordtype) {
            case HEX_PARSER_RECORDTYPE_DATA:
            #if print_process_data ==1u
                print8(parser_length);
                print16(parser_address);
                print8(0);
                for (parser_counter = 0; parser_counter < parser_length; parser_counter++)
                {
                    print8(parser_data[parser_counter]);
                }
                SYS_CONSOLE_Print(sysObj.sysConsole0, "\n");
                print32(parser_address_segment);
                SYS_CONSOLE_Print(sysObj.sysConsole0, "\n");
            #endif
            #if processs_hex_data == 1u
                programBuffer(parser_data, parser_length, parser_address);
            #endif
            break;
            case HEX_PARSER_RECORDTYPE_EOF:
            {
                parser_status = HEX_PARSER_EOF;
            }
            break;
            case HEX_PARSER_RECORDTYPE_EXTENDED_SEGMENT:
                parser_address_segment = (((uint32_t)parser_data[0])*0x100+((uint32_t)parser_data[1])) * 0x10;
            break;
            case HEX_PARSER_RECORDTYPE_EXTENDED_LINEAR_ADDRESS:
                parser_address_segment = (((uint32_t)parser_data[0])*0x100+((uint32_t)parser_data[1])) * 0x10000;
            break;
            default:
                break;
            }

            parser_state = HEX_PARSER_STATE_NEW;
        } else {
             parser_status = HEX_PARSER_ERROR;
        }
    break;
    default:
        break;
    }
}

uint8_t pageBufferCnt = 0;
uint8_t pageBuffer[NVMCTRL_PAGE_SIZE]; /* page size*/
uint32_t flashAddr = 0;

uint32_t currentRowErraseAddr = 0;
extern uint8_t fileProcessNoError;
static void programToFlash(uint32_t addr) {

    if(fileProcessNoError == 0 ){
        return;
    }

    if(addr < APP_START_ADDRESS || addr > 0x00040000) {
        SYS_CONSOLE_Print(sysObj.sysConsole0, "ERR Addr\t%06x\n", addr);
        fileProcessNoError = 0;
        return;
    }

    if (currentRowErraseAddr != (addr & 0xFFFFFF00)) {
        currentRowErraseAddr = addr;
        DRV_NVMCTRL_SectorErase(sysObj.drvMemory0,addr);
        while(NVMCTRL_IsBusy());
    }

    DRV_NVMCTRL_PageWrite(sysObj.drvMemory0, (void *) pageBuffer, addr);
    while(NVMCTRL_IsBusy());
}


void programBuffer(uint8_t *bufferin, uint8_t length, uint32_t address) {

    uint8_t bufferCapacity = 0;
    uint8_t residual = 0;

    if(pageBufferCnt == 0) {
        /* do no increase flash address until we receive page size (64) data */
        residual = (uint8_t) (address & 0x0f);
        flashAddr = address & 0xFFFFFFF0;  /* ignore residual: 0x3ff2 -> 0x3ff0*/
        pageBufferCnt = residual;
        memset(pageBuffer, 0, NVMCTRL_PAGE_SIZE);
        memcpy(&pageBuffer[pageBufferCnt], bufferin, length);
        pageBufferCnt = pageBufferCnt + length;
    } else if(address <= (flashAddr + NVMCTRL_PAGE_SIZE)) {
        /* address is within frame buffer range */

        /* check for discontinuty in address */
        uint8_t differenceInAddr = address - (flashAddr+pageBufferCnt);
        if(differenceInAddr) {
            /* some data is not there between two addresses */
            pageBufferCnt += differenceInAddr;
        }

        if((pageBufferCnt+length) > NVMCTRL_PAGE_SIZE) {
            /* incoming data is more than page buffer size. */
            residual = (pageBufferCnt + length) - NVMCTRL_PAGE_SIZE;
            bufferCapacity = NVMCTRL_PAGE_SIZE-pageBufferCnt;
            memcpy(&pageBuffer[pageBufferCnt], bufferin, bufferCapacity);
            pageBufferCnt = NVMCTRL_PAGE_SIZE;
        } else {
            memcpy(&pageBuffer[pageBufferCnt], bufferin, length);
            pageBufferCnt = pageBufferCnt + length;
        }

        if(pageBufferCnt >= NVMCTRL_PAGE_SIZE) {
            /* buffer count reached the limit - copy the buffer data to Flash Memory */
            programToFlash(flashAddr);
            /*reset buffer memory and count */
            memset(pageBuffer, 0, NVMCTRL_PAGE_SIZE);
            pageBufferCnt = 0;

            /* copy residual data */
            if(residual != 0) {
                flashAddr += NVMCTRL_PAGE_SIZE;
                uint8_t *pptr = &bufferin[bufferCapacity];
                memcpy(&pageBuffer[0], pptr, residual);
                pageBufferCnt = residual;
            }
        }
    } else {
        /* there is a break in address - write to flash */

        /* when the received address is more ha page buffer area, copy the exiting content to flash */
        programToFlash(flashAddr);
        memset(pageBuffer, 0, NVMCTRL_PAGE_SIZE);
        pageBufferCnt = 0;

        /* fresh start */
        residual = (uint8_t) (address & 0x0f);
        flashAddr = address & 0xFFFFFFF0;  /* ignore residual: 0x3ff2 -> 0x3ff0*/
        pageBufferCnt = residual;
        memset(pageBuffer, 0, NVMCTRL_PAGE_SIZE);
        memcpy(&pageBuffer[pageBufferCnt], bufferin, length);
        pageBufferCnt = pageBufferCnt + length;
    }
}


static uint8_t parse_nibble (char byte)
{
    if ((byte >= '0') && (byte <= '9')) {
        return byte -'0';
    } else if ((byte >= 'A') && (byte <= 'F')){
        return byte - 'A' + 10;
    } else if ((byte >= 'a') && (byte <= 'f')){
        return byte - 'a' + 10;
    } else {
        return 0;
    }
}

static uint8_t test_newline(char byte)
{
    if ((byte == '\r')||(byte == '\n')) {
        return 1;
    } else {
        return 0;
    }
}

static uint8_t test_nibble(char byte)
{
    if (((byte >= '0') && (byte <= '9')) || ((byte >= 'A') && (byte <= 'F')) || ((byte >= 'a') && (byte <= 'f'))) {
        return 1;
    } else {
        return 0;
    }
}



void callApp(void) {
    uint32_t msp            = *(uint32_t *)(APP_START_ADDRESS);
    uint32_t reset_vector   = *(uint32_t *)(APP_START_ADDRESS + 4);

    if (msp == 0xffffffff)
    {
        return;
    }

    /* Disable the USART before configurations */
    SERCOM0_REGS->USART_INT.SERCOM_CTRLA &= ~SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    /* Wait for sync */
    while((SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }

#if switch_to_app == 1u
    __set_MSP(msp);
    asm("bx %0"::"r" (reset_vector));
#endif
}
