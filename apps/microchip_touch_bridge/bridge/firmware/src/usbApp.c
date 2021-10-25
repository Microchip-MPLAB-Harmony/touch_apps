/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    usbAPP.c

  Summary:
    This file contains the source code related to USB CDC and CAPBridge functionality

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
#include "led.h"
#include "usbApp.h"
#include "pcComm.h"
#include "capBridge.h"

uint8_t rxBuffer[8] USB_ALIGN;
uint8_t txBuffer[8] USB_ALIGN;

extern const USB_DEVICE_INIT usbDevInitDataCapBridge;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

uint8_t CACHE_ALIGN cdcRxBuffer[USB_APP_READ_BUFFER_SIZE];
uint8_t CACHE_ALIGN cdcTxBuffer[USB_APP_READ_BUFFER_SIZE];

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

USB_APP_DATA usbAppData;

static void usbSofEvent(void) {
    if(usbAppData.properConfiguration == false){
        return;
    }
}

static void usbConfigureRead(void) {
    
    if(usbAppData.isReadComplete == true) {
        
        usbAppData.isReadComplete = false;
        usbAppData.readTransferHandle =  USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        
        while(usbAppData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID) {
            USB_DEVICE_CDC_Read (USB_DEVICE_CDC_INDEX_0,
                    &usbAppData.readTransferHandle, usbAppData.cdcReadBuffer,
                    USB_APP_READ_BUFFER_SIZE);
        }
    }
}

/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/

USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler ( USB_DEVICE_CDC_INDEX index,
                                            USB_DEVICE_CDC_EVENT event, void * pData, uintptr_t userData ) {

    USB_APP_DATA * appDataObject;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE * eventDataRead;
    
    appDataObject = (USB_APP_DATA *)userData;

    switch(event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:

            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */

            USB_DEVICE_ControlSend(appDataObject->deviceHandle,
                    &appDataObject->getLineCodingData, sizeof(USB_CDC_LINE_CODING));
            break;

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:

            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */

            USB_DEVICE_ControlReceive(appDataObject->deviceHandle,
                    &appDataObject->setLineCodingData, sizeof(USB_CDC_LINE_CODING));
            
            break;

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:

            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */

            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            appDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            appDataObject->controlLineStateData.carrier = controlLineStateData->carrier;
            
            if(appDataObject->controlLineStateData.dtr) {
                
                if(appDataObject->properConfiguration == false) {
                    ledBlink(GREEN_LED, 1, 100, 1000);
                    appDataObject->properConfiguration = true;
                }
            } else {
                if( appDataObject->properConfiguration == true){
                    ledBlink(RED_LED, 1, 100, 1000);
                    appDataObject->properConfiguration = false;
                }
            }

            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:

            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */

            appDataObject->breakData = ((USB_DEVICE_CDC_EVENT_DATA_SEND_BREAK *)pData)->breakDuration;
            
            /* Complete the control transfer by sending a ZLP  */
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:

            /* This means that the host has sent some data*/
            eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *)pData;
            
            if(eventDataRead->status != USB_DEVICE_CDC_RESULT_ERROR)
            {
                appDataObject->isReadComplete = true;
                appDataObject->rxNewData = 1;
                
                appDataObject->numBytesRead = eventDataRead->length; 
            }
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:

            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */

            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:

            /* This means the GET LINE CODING function data is valid. We don't
             * do much with this data in this demo. */
            break;

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:

            /* This means that the data write got completed. We can schedule
             * the next read. */

            appDataObject->isWriteComplete = true;
            appDataObject->txInProgress = 0;
            break;
            
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_ABORTED:
            break;

        default:
            break;
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/***********************************************
 * Application USB Device Layer Event Handler.
 ***********************************************/
void APP_USBDeviceEventHandler ( USB_DEVICE_EVENT event, void * eventData, uintptr_t context ) {

    USB_DEVICE_EVENT_DATA_CONFIGURED *configuredEventData;

    switch(event) {
        case USB_DEVICE_EVENT_SOF:

            /* This event is used for switch debounce. This flag is reset
             * by the switch process routine. */
            usbAppData.sofEventHasOccurred = true;
            usbSofEvent();
            break;

        case USB_DEVICE_EVENT_RESET:

            /* Update LED to show reset state */
            usbAppData.isConfigured = false;

            break;

        case USB_DEVICE_EVENT_DECONFIGURED:
            usbAppData.isConfigured = false;
            usbAppData.properConfiguration = false;

            break;
            
        case USB_DEVICE_EVENT_CONFIGURED:

            /* Check the configuration. We only support configuration 1 */
            configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;
            
            if ( configuredEventData->configurationValue == 1)
            {
                /* Update LED to show configured state */
                
                /* Register the CDC Device application event handler here.
                 * Note how the usbAppData object pointer is passed as the
                 * user data */

                if(appConfig.usb_mode == USB_CDC) {
                    USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, (uintptr_t)&usbAppData);
                }
                
                /* Mark that the device is now configured */
                usbAppData.isConfigured = true;
            } else{
                usbAppData.isConfigured = false;
            }
            break;
        case USB_DEVICE_EVENT_ENDPOINT_READ_COMPLETE:
            usbAppData.isReadComplete = true;
            usbAppData.isWriteComplete = true;
            break;
        case USB_DEVICE_EVENT_ENDPOINT_WRITE_COMPLETE:
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(usbAppData.deviceHandle);
            
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:
            
            /* VBUS is not available. We can detach the device */
            USB_DEVICE_Detach(usbAppData.deviceHandle);
            
            usbAppData.isConfigured = false;
            usbAppData.properConfiguration = false;
                        
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
            /* Switch LED to show suspended state */
            
            break;

        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            
            break;
    }
}


/*****************************************************
 * This function is called in every step of the
 * application state machine.
 *****************************************************/

bool APP_StateReset(void)
{
    /* This function returns true if the device
     * was reset  */

    bool retVal;

    if(usbAppData.isConfigured == false)
    {
        usbAppData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
        usbAppData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        usbAppData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        usbAppData.isReadComplete = true;
        usbAppData.isWriteComplete = true;
        retVal = true;
    }
    else
    {
        retVal = false;
    }

    return(retVal);
}

/*******************************************************************************
  Function:
    void usbInit(void)

  Remarks:
    See prototype in app.h.
 */

void usbInit(void)
{
    
    if(appConfig.usb_mode == USB_CAPBRIDGE) {
        USB_DEVICE_Deinitialize(sysObj.usbDevObject0);
        
        sysObj.usbDevObject0 = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 , ( SYS_MODULE_INIT* ) & usbDevInitDataCapBridge);

        capBridgeInit();

    }

    
    /* Place the App state machine in its initial state. */
    usbAppData.state = APP_STATE_INIT;
    
    /* Device Layer Handle  */
    usbAppData.deviceHandle = USB_DEVICE_HANDLE_INVALID ;

    /* Device configured status */
    usbAppData.isConfigured = false;

    /* Initial get line coding state */
    usbAppData.getLineCodingData.dwDTERate = 115200;
    usbAppData.getLineCodingData.bParityType = 0;
    usbAppData.getLineCodingData.bParityType = 0;
    usbAppData.getLineCodingData.bDataBits = 8;

    /* Read Transfer Handle */
    usbAppData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Write Transfer Handle */
    usbAppData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;

    /* Initialize the read complete flag */
    usbAppData.isReadComplete = true;

    /*Initialize the write complete flag*/
    usbAppData.isWriteComplete = true;

    usbAppData.endpointRx = (1 | USB_EP_DIRECTION_OUT);
    usbAppData.endpointTx = (1 | USB_EP_DIRECTION_IN);
    /* Reset other flags */
    usbAppData.sofEventHasOccurred = false;
    
    /* To know status of Switch */
    usbAppData.isSwitchPressed = false;

    /* Set up the read buffer */
    usbAppData.cdcReadBuffer = &cdcRxBuffer[0];

    /* Set up the read buffer */
    usbAppData.cdcWriteBuffer = &cdcTxBuffer[0];       
    
    usbAppData.properConfiguration = false;
}

void usbProcess(void) {
	/* USB Device layer tasks routine */ 
    USB_DEVICE_Tasks(sysObj.usbDevObject0);
    /* USB FS Driver Task Routine */ 
    DRV_USBFSV1_Tasks(sysObj.drvUSBFSV1Object);
    
    switch(usbAppData.state)
    {
        case APP_STATE_INIT:

            /* Open the device layer */
            usbAppData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );

            if(usbAppData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(usbAppData.deviceHandle, APP_USBDeviceEventHandler, 0);

                usbAppData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }

            break;

        case APP_STATE_WAIT_FOR_CONFIGURATION:

            if(usbAppData.isConfigured)
            {
                if(appConfig.usb_mode == USB_CDC) {
                    pcCommResetPtrs();
                } else {
                    USB_DEVICE_EndpointEnable(usbAppData.deviceHandle, 0, usbAppData.endpointRx,
                                USB_TRANSFER_TYPE_INTERRUPT, 8);
                    USB_DEVICE_EndpointEnable(usbAppData.deviceHandle, 0, usbAppData.endpointTx,
                                USB_TRANSFER_TYPE_INTERRUPT, 8);
                    usbAppData.isReadComplete = false;
                    usbAppData.isWriteComplete = false;
                    USB_DEVICE_EndpointRead(usbAppData.deviceHandle, &usbAppData.readTransferHandle,  usbAppData.endpointRx, rxBuffer, 8);
                    ledBlink(GREEN_LED, 1, 100, 1000);
                }
                /* If the device is configured then lets start reading */
                usbAppData.state = APP_STATE_CONFIRMED;
            }
           
            break;
        
        case APP_STATE_CONFIRMED:
            if(appConfig.usb_mode == USB_CAPBRIDGE) {
                if(usbAppData.isWriteComplete == true) {
                    usbAppData.isWriteComplete = false;
                    if(rxBuffer[0] != 0) {
                        capBridgeProcessRxData(rxBuffer, txBuffer);
                    }
                    USB_DEVICE_EndpointWrite(usbAppData.deviceHandle, &usbAppData.writeTransferHandle,  usbAppData.endpointTx, txBuffer, 8, USB_DEVICE_TRANSFER_FLAGS_MORE_DATA_PENDING);
                }
                if(usbAppData.isReadComplete == true) {
                    usbAppData.isReadComplete = false;
                    ledBlink(GREEN_LED, 1, 100, 1000);
                    USB_DEVICE_EndpointRead(usbAppData.deviceHandle, &usbAppData.readTransferHandle,  usbAppData.endpointRx, rxBuffer, 8);
                }
            }
            break;

        case APP_STATE_ERROR:
        default:
            
            break;
    }
    if(appConfig.usb_mode == USB_CDC) {

        if(usbAppData.properConfiguration) {
            /* set buffer for read data */
            usbConfigureRead();
        }

        if(usbAppData.txInProgress == 0 && usbAppData.properConfiguration == true) {

            usbAppData.numBytesWrite = 0;
            while(pcLengthTxBuffer()){
                ledBlink(RED_LED, 1, 10, 100);
                usbAppData.cdcWriteBuffer[usbAppData.numBytesWrite] = pcReadTxBuffer();
                usbAppData.numBytesWrite++;
            }

            if(usbAppData.numBytesWrite != 0){
                usbAppData.txInProgress = 1;
                USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0,
                        &usbAppData.writeTransferHandle,
                        usbAppData.cdcWriteBuffer, usbAppData.numBytesWrite,
                        USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            }
        }

        if(usbAppData.rxNewData){
            usbAppData.rxNewData = 0;
            ledBlink(GREEN_LED, 1, 10, 100);
            pcWriteRxBuffer(usbAppData.cdcReadBuffer, usbAppData.numBytesRead);
        }
    }
}


/*******************************************************************************
 End of File
 */

