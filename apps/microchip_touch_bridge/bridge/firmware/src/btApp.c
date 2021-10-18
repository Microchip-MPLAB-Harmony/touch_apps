/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    btApp.c

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

#include "btApp.h"
#include "string.h"
#include "led.h"
#include "pcComm.h"

#define SEND_RN4678_DEBUG_DATA 0u

uintptr_t btUart;
uint8_t btRxData;
static volatile uint8_t txInProgress;

/**
 * @brief Bluetooth module sends some string data between %% to indicate that
 * whether comport is opened in PC. This vairable is ued to store received data
 * from PC and pocessed to know the status.
 * 
 */
uint8_t received_string[100];
uint8_t stringLen = 0;

/**
 * @brief Enumeration to know whether the connected command received from BT
 * 
 */
typedef enum {
    WAITING_FOR_HEADER,
    WAITING_FOR_FOOTER,
    COMMAND_RECEIVED
}btConnectStatus_t;

/**
 * @brief Variable to know whether the connected command received from BT
 * 
 */
btConnectStatus_t btConnectStatus;
/**
 * @brief Flag to know whether the comport is opened or not
 * 
 */
uint8_t btConnected;

/**
 * @brief String compare function to know whether the strings are same or not
 * 
 * @param a string a
 * @param b string b
 * @return uint8_t returns 1 if strings are same else return 0
 */
static uint8_t btCompareString(uint8_t *a, uint8_t *b)
{
   int c = 0;
 
   while (a[c] == b[c]) {
      if (a[c] == '\n' || b[c] == '\n')
         break;
      c++;
   }
   
   if (a[c] == '\n' && b[c] == '\n')
      return 1;
   else
      return 0;
}

typedef enum tag_btCheckName_t{
    BT_CMD_ENTER_CMD_MODE,
    BT_CMD_READ_BT_NAME,
    BT_CMD_CHANGE_BT_NAME,
    BT_CMD_CHANGE_ONLY_CLASSIC,
    BT_CMD_EXIT_CMD_MODE,
    BT_CMD_RESET,
    BT_CMD_WAIT_FOR_DONE,
    BT_CMD_DONE,
}btCheckName_t;
btCheckName_t btCheckNamestatus;

typedef enum tag_btCmdResponse_t{
    NO_RES,
    CMD,
    ERR,
    BTNAMERN4678,
    BTNAMETCH,
}btCmdResponse_t;

btCmdResponse_t btCmdResponse;
uint8_t btSerialNumber[4];

uint8_t rfclsoed[] = "RFCOMM_CLOSE\n";  /// indicates comport is closed
uint8_t rfopen[] = "RFCOMM_OPEN\n"; /// indicates comport is open
uint8_t rfreboot[] = "REBOOT\n"; // indicates a reset or reboot of RF Module
uint8_t name[] = "BTName=RN\n"; //"BTName=RN4678-3108"
uint8_t nametch[] = "BTName=TC\n"; //"BTName=TCH-BRIDGE"

SYS_TIME_HANDLE btDelaytimer = SYS_TIME_HANDLE_INVALID;

bool btDelayCompleted(void) {
    bool retvar = false;
    if(btDelaytimer == SYS_TIME_HANDLE_INVALID) {
        return true;
    } else {
        retvar = SYS_TIME_DelayIsComplete(btDelaytimer);
        if(retvar == true) {
            btDelaytimer = SYS_TIME_HANDLE_INVALID;
        }
    }    
    return retvar;
}

void btNonBlockDelayInit(uint32_t delay) {
    SYS_TIME_DelayMS(delay, &btDelaytimer);
}


/**
 * @brief Process the string to know whether it is OPEN or CLOSE commands.
 * 
 */
static void btProcessReceivedString(void) {
   
    // special case for connect - ignore serial from string "CONNECT,5CBAEF33E154"
    if(stringLen > 12) {
        stringLen = 9;
    }
    
    received_string[stringLen] = '\n';
    stringLen = 0;
    
    if(btCompareString(&received_string[0], &rfclsoed[0])){
        ledBlink(RED_LED, 1, 100, 1000);
        btConnected = 0;
    } else if(btCompareString(&received_string[0], &rfopen[0])){
        ledBlink(GREEN_LED, 1, 100, 1000);
        btConnected = 1;
    } else if(btCompareString(&received_string[0], &rfreboot[0])) {
        ledBlink(RED_LED, 1, 100, 1000);
        btConnected = 0;
    }  else if(btCompareString(&received_string[0], &name[0])) {
        btCmdResponse = BTNAMERN4678;
        btConnected = 0;
        /* Serial number of the kit */
        for (uint8_t cnt = 0 ;cnt < 4; cnt ++)
        {
            btSerialNumber[cnt] = received_string[14+cnt];
        }
    }   else if(btCompareString(&received_string[0], &nametch[0])) {
        ledBlink(YELLOW_LED, 2, 100, 1000);
        btCmdResponse = BTNAMETCH;
        btConnected = 0;
    }
}

/**
 * @brief Process the incoming data.
 * A string is received from BT to indicate the connection status.
 * Such string is embedded between % (like "%xxx%") to let application know about the status.
 * Process that separately.
 * @param data 
 */
static void btReceiveDataProcess(uint8_t data){
    switch(btConnectStatus) {
        case WAITING_FOR_HEADER:
            if(data == '%') {//} || data == '\n'){
                stringLen = 0;
                btConnectStatus = WAITING_FOR_FOOTER;
            }
            break;

        case WAITING_FOR_FOOTER:
            if(data == '%') {//}) || data == '\n') {
                btConnectStatus = WAITING_FOR_HEADER;
                btProcessReceivedString();
            } else if(data == '\n') {//}) || data == '\n') {
                btProcessReceivedString();
            } else {
                received_string[stringLen] = data;
                stringLen++;
                
                if(stringLen > 25) {
                    // error
                    stringLen = 0;
                    btConnectStatus = WAITING_FOR_HEADER;
                }
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Write callback to indicate that write is complete.
 * Initiate additional writes if more data is present
 * 
 * @param btUart context
 */
static void btUartWriteCallback(uintptr_t btUart ){
    
    uint8_t data = 0;

    ledBlink(RED_LED, 1, 10, 100);
    
    if(appConfig.pcInterface == BT_INTERFACE && btCheckNamestatus == BT_CMD_DONE) {
        if(pcLengthTxBuffer()){
            data = pcReadTxBuffer();
            SERCOM0_USART_Write((void *)&data, 1);
        } else{
            txInProgress = 0;
        }
    } else {
        txInProgress = 0;
    }
}

/**
 * @brief Read callback to indicate that read is complete.
 * Initiate additional read receive more data
 * 
 * @param btUart context
 */
static void btUartReadCallback(uintptr_t btUart) {
    
    ledBlink(GREEN_LED, 1, 10, 100);

    btReceiveDataProcess(btRxData);
    #if SEND_RN4678_DEBUG_DATA == 1u
        pcWriteRxBuffer(&btRxData, 1);
    #else
    if(appConfig.pcInterface == BT_INTERFACE) {
        if((btCheckNamestatus == BT_CMD_DONE) && (btConnectStatus == WAITING_FOR_HEADER)) {
            pcWriteRxBuffer(&btRxData, 1);
        }
    }
    #endif
    SERCOM0_USART_Read((void *)  &btRxData, 1);
}

uint8_t tempBuffer[15];
void btCheckNameSendCmd(uint8_t * ptr, uint8_t len) {
    for(uint8_t cnt = 0; cnt < len; cnt ++) {
        tempBuffer[cnt] = ptr[cnt];
    }
    tempBuffer[len] = '\r';
    SERCOM0_USART_Write((void *) &tempBuffer[0], len+1);
}


void btCheckName(void) {

    // if transmission is in progress or if configured delay is not lapsed
    if((btDelayCompleted() == false) || (txInProgress == 1)){ 
        return;
    }

    btNonBlockDelayInit(100); //msec
        
    switch(btCheckNamestatus) {
        case BT_CMD_ENTER_CMD_MODE:
            // btConnectStatus = WAITING_FOR_NAME_DATA;
            btConnectStatus = WAITING_FOR_FOOTER;
            txInProgress = 1;
            uint8_t data[] = "$$$";
            btCheckNameSendCmd(&data[0], sizeof(data));
            btCheckNamestatus = BT_CMD_READ_BT_NAME;
            break;
        case BT_CMD_READ_BT_NAME:
            txInProgress = 1;
            uint8_t data1[] = "D";
            btCheckNameSendCmd(&data1[0], sizeof(data1));
            btCheckNamestatus = BT_CMD_CHANGE_BT_NAME;
            break;
        case BT_CMD_CHANGE_BT_NAME:
            if(btCmdResponse == BTNAMERN4678) {
                txInProgress = 1;
                uint8_t data2[] = "SN,TCH-BRIDGE"; // change name
                uint8_t dataTemp[20], *ptr;
                ptr = &dataTemp[0];
                uint8_t cnt = 0;
                for(cnt = 0; cnt < sizeof(data2); cnt ++) {
                    *ptr++ = data2[cnt];
                }
                cnt = sizeof(data2);
                cnt= cnt + 4;
                dataTemp[13] = '-';
                
                dataTemp[14] = btSerialNumber[0];
                dataTemp[15] = btSerialNumber[1];
                dataTemp[16] = btSerialNumber[2];
                dataTemp[17] = btSerialNumber[3];
                
                btCheckNameSendCmd(&dataTemp[0], cnt);
                btCheckNamestatus = BT_CMD_CHANGE_ONLY_CLASSIC;
            } else if(btCmdResponse == BTNAMETCH){
                uint8_t data3[] = "---";
                btCheckNameSendCmd(&data3[0], sizeof(data3));
                btCheckNamestatus = BT_CMD_WAIT_FOR_DONE;
            }
            break;
        case BT_CMD_CHANGE_ONLY_CLASSIC:
                txInProgress = 1;
                uint8_t data4[] = "SG,2"; // only classic 
                btCheckNameSendCmd(&data4[0], sizeof(data4));
                btCheckNamestatus = BT_CMD_RESET;
            break;
        case BT_CMD_RESET:
                txInProgress = 1;
                uint8_t data5[] = "R,1"; // only classic 
                btCheckNameSendCmd(&data5[0], sizeof(data5));
                btCheckNamestatus = BT_CMD_WAIT_FOR_DONE;
            break;
        case BT_CMD_WAIT_FOR_DONE:
            btCheckNamestatus = BT_CMD_DONE;
            btConnectStatus = WAITING_FOR_HEADER;
            break;
        default:
            break;
    }
}

/**
 * @brief Processes data transaction related to Bluetooth.
 * This should be called in main while(1) loop.
 * 
 */
void btProcess(void){
    
    uint8_t data = 0;
    
        #if SEND_RN4678_DEBUG_DATA == 1u
        if(txInProgress == 0 ) {//} && btConnected == 1) {
            if(pcLengthTxBuffer()){
                txInProgress = 1;
                data = pcReadTxBuffer();
                SERCOM0_USART_Write((void *)&data, 1);
            }
        }
        #else
        if(appConfig.pcInterface == BT_INTERFACE) {
            if(txInProgress == 0 && btConnected == 1) {
                if(pcLengthTxBuffer()){
                    txInProgress = 1;
                    data = pcReadTxBuffer();
                    SERCOM0_USART_Write((void *)&data, 1);
                }
            }
        }
        #endif

    if(btCheckNamestatus != BT_CMD_DONE) {
        btCheckName();
    }

}

/**
 * @brief Initializes Bluethooth communications
 * 
 */
void btInit(void) {
    /* Reset Bluetooth module */
    BT_WAKEUP_Clear();
    BT_BTN_Clear();
    
    BT_RST_Clear();
    btNonBlockDelayInit(1); //msec
    while(btDelayCompleted() == false);
    BT_RST_Set();
    btNonBlockDelayInit(1); //msec
    while(btDelayCompleted() == false);
    
    BT_WAKEUP_Set();
    BT_BTN_Set();
    
    //bt sercom
    SERCOM0_USART_WriteCallbackRegister(btUartWriteCallback, btUart);
    SERCOM0_USART_ReadCallbackRegister(btUartReadCallback, btUart);

    /// configure to read data
    SERCOM0_USART_Read((void *) &btRxData,1);

    btNonBlockDelayInit(2000); //msec
    btCheckNamestatus = BT_CMD_ENTER_CMD_MODE;
}



/* *****************************************************************************
 End of File
 */
