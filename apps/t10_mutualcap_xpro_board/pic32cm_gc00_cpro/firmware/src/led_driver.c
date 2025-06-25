/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/*----------------------------------------------------------------------------
 *     include files
 *----------------------------------------------------------------------------*/
#include <stddef.h>      // Defines NULL
#include <stdbool.h>     // Defines true
#include <stdlib.h>      // Defines EXIT_FAILURE
#include "definitions.h" // SYS function prototypes
// #include <string.h>
#include "led_driver.h"
// #include "strings.h"
//  *****************************************************************************
//  Section: Global Variables
//  *****************************************************************************

static uint8_t testTxData[APP_TRANSMIT_DATA_LENGTH] = {0x00, 0x00};
extern volatile uint8_t measurement_done_touch;
uint8_t ackData = 0;
uint8_t button_mask = 0;
uint8_t slider_mask = 0;
uint8_t scroller_pos = 0;
bool buttons_updated = false;
bool slider_updated = false;
bool mcp23017_PortA_configured = false;
bool mcp23017_PortB_configured = false;
APP_STATES button_state;
volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;

// *****************************************************************************
// Function Prototypes
// *****************************************************************************

void APP_I2CCallback(uintptr_t context);
void Update_LED_Display(void);

// *****************************************************************************
// Update_LED_Display()
// State driven LED DRIVE using I2C
// *****************************************************************************

void Update_LED_Display()
{

    button_state = APP_STATE_TOUCH_COMPLETE;

    while (button_state != APP_STATE_DISPLAY_UPDATE_COMPLETE)
    {
        switch (button_state)
        {
        case APP_STATE_TOUCH_COMPLETE:
            /*check the buttons and slider*/
            slider_mask = 0;
            button_mask = 0;
            buttons_updated = false;
            slider_updated = false;
            testTxData[0] = 0;
            testTxData[1] = 0;
            for (uint8_t cnt = 0; cnt < 4; cnt++)
            {
                if (get_sensor_state(cnt) & 0x80)
                {
                    button_mask |= (1 << (3 - cnt));
                }
            }
            if (get_scroller_state(0) & 0x80)
            {
                scroller_pos = get_scroller_position(0);
                scroller_pos = scroller_pos >> 5;
                slider_mask = (1 << 7);
                while (scroller_pos > 0)
                {
                    slider_mask |= (1 << (7 - (scroller_pos)));
                    scroller_pos--;
                }
            }
            button_mask = button_mask << 4;
            button_state = APP_STATE_MCP23017_STATUS_VERIFY;
            break;

        case APP_STATE_MCP23017_STATUS_VERIFY:
            /* Register the TWIHS Callback with transfer status as context */
            SERCOM1_I2C_CallbackRegister(APP_I2CCallback, (uintptr_t)&transferStatus);
            /* Verify if EEPROM is ready to accept new requests */
            transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
            SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
            button_state = APP_STATE_MCP23017_WRITE;
            break;

        case APP_STATE_MCP23017_WRITE:
            if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
            {
                if (mcp23017_PortA_configured == false)
                {
                    /*setup MCP23017 PORTA as outputs*/
                    testTxData[1] = 0x00;
                    testTxData[0] = MCP23017_IODIRA_ADDR;
                    SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &testTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    mcp23017_PortA_configured = true;
                    button_state = APP_STATE_MCP23017_WAIT_WRITE_COMPLETE;
                }
                else if (mcp23017_PortB_configured == false)
                {
                    /*setup MCP23017 PORTB as outputs*/
                    testTxData[1] = 0x00;
                    testTxData[0] = MCP23017_IODIRB_ADDR;
                    SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &testTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    mcp23017_PortB_configured = true;
                    button_state = APP_STATE_MCP23017_WAIT_WRITE_COMPLETE;
                }
                else if (buttons_updated == false)
                {
                    /* Write button data to MCP23017 */
                    testTxData[1] = button_mask;
                    testTxData[0] = MCP23017_OLATA_ADDR;
                    SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &testTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    buttons_updated = true;
                    button_state = APP_STATE_MCP23017_WAIT_WRITE_COMPLETE;
                }
                else
                {
                    /* Write slider data to MCP23017 */
                    testTxData[1] = slider_mask;
                    testTxData[0] = MCP23017_OLATB_ADDR;
                    SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &testTxData[0], APP_TRANSMIT_DATA_LENGTH);
                    slider_updated = true;
                    button_state = APP_STATE_MCP23017_WAIT_WRITE_COMPLETE;
                }
            }
            else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
            {
                /* Device is not ready to accept new requests.
                 * Keep checking until the EEPROM becomes ready. */
                button_state = APP_STATE_MCP23017_STATUS_VERIFY;
            }
            break;
        case APP_STATE_MCP23017_WAIT_WRITE_COMPLETE:
            if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
            {
                /* Read the status of internal write cycle */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                button_state = APP_STATE_MCP23017_CHECK_INTERNAL_WRITE_STATUS;
            }
            else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
            {
                button_state = APP_STATE_XFER_ERROR;
            }
            break;

        case APP_STATE_MCP23017_CHECK_INTERNAL_WRITE_STATUS:
            if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
            {
                if (slider_updated == true)
                {
                    button_state = APP_STATE_DISPLAY_UPDATE_COMPLETE;
                }
                else
                {
                    button_state = APP_STATE_MCP23017_STATUS_VERIFY;
                }
            }
            else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
            {
                /* EEPROM's internal write cycle is not complete. Keep checking. */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                /* Register the TWIHS Callback with transfer status as context */
                SERCOM1_I2C_CallbackRegister(APP_I2CCallback, (uintptr_t)&transferStatus);
                SERCOM1_I2C_Write(APP_MCP23017_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
            }
            break;

        case APP_STATE_XFER_ERROR:
        {
            button_state = APP_STATE_MCP23017_STATUS_VERIFY;
            break;
        }

        default:
            break;
        }
    }
}

// *****************************************************************************
// I2C Callback
// *****************************************************************************

void APP_I2CCallback(uintptr_t context)
{
    volatile APP_TRANSFER_STATUS *tferStatus = (APP_TRANSFER_STATUS *)context;

    if (SERCOM1_I2C_ErrorGet() == SERCOM_I2C_ERROR_NONE)
    {
        if (tferStatus)
        {
            *tferStatus = APP_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (tferStatus)
        {
            *tferStatus = APP_TRANSFER_STATUS_ERROR;
        }
    }
}
/*******************************************************************************
 End of File
 */