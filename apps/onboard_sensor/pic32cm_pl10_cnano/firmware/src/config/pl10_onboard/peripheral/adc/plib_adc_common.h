/*******************************************************************************
  Analog-to-Digital Converter(ADC) Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_adc_common.h

  Summary
    ADC Peripheral Library Interface Header File.

  Description
    This file defines the common types for the ADC peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_ADC_COMMON_H    // Guards against multiple inclusion
#define PLIB_ADC_COMMON_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/* This section lists the other files that are included in this file.
*/

#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Preprocessor macros
// *****************************************************************************
// *****************************************************************************
#define ADC_STATUS_NONE 0U
#define ADC_STATUS_RESRDY ADC_INTFLAG_RESRDY_Msk
#define ADC_STATUS_WINMON ADC_INTFLAG_WINMON_Msk
#define ADC_STATUS_OVERRUN ADC_INTFLAG_OVERRUN_Msk
#define ADC_STATUS_MASK (ADC_STATUS_RESRDY | ADC_STATUS_OVERRUN | ADC_STATUS_WINMON)    
#define ADC_STATUS_INVALID 0xFFFFFFFFU

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/* The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/

typedef enum
{
    ADC_POSINPUT_AIN2 = ADC_INPUTCTRL_MUXPOS_AIN2,
    ADC_POSINPUT_AIN3 = ADC_INPUTCTRL_MUXPOS_AIN3,
    ADC_POSINPUT_AIN4 = ADC_INPUTCTRL_MUXPOS_AIN4,
    ADC_POSINPUT_AIN5 = ADC_INPUTCTRL_MUXPOS_AIN5,
    ADC_POSINPUT_AIN6 = ADC_INPUTCTRL_MUXPOS_AIN6,
    ADC_POSINPUT_AIN7 = ADC_INPUTCTRL_MUXPOS_AIN7,
    ADC_POSINPUT_AIN16 = ADC_INPUTCTRL_MUXPOS_AIN16,
    ADC_POSINPUT_AIN17 = ADC_INPUTCTRL_MUXPOS_AIN17,
    ADC_POSINPUT_AIN18 = ADC_INPUTCTRL_MUXPOS_AIN18,
    ADC_POSINPUT_AIN19 = ADC_INPUTCTRL_MUXPOS_AIN19,
    ADC_POSINPUT_AIN20 = ADC_INPUTCTRL_MUXPOS_AIN20,
    ADC_POSINPUT_AIN21 = ADC_INPUTCTRL_MUXPOS_AIN21,
    ADC_POSINPUT_AIN22 = ADC_INPUTCTRL_MUXPOS_AIN22,
    ADC_POSINPUT_AIN23 = ADC_INPUTCTRL_MUXPOS_AIN23,
    ADC_POSINPUT_AIN24 = ADC_INPUTCTRL_MUXPOS_AIN24,
    ADC_POSINPUT_AIN25 = ADC_INPUTCTRL_MUXPOS_AIN25,
    ADC_POSINPUT_AIN26 = ADC_INPUTCTRL_MUXPOS_AIN26,
    ADC_POSINPUT_AIN27 = ADC_INPUTCTRL_MUXPOS_AIN27,
    ADC_POSINPUT_AIN28 = ADC_INPUTCTRL_MUXPOS_AIN28,
    ADC_POSINPUT_AIN29 = ADC_INPUTCTRL_MUXPOS_AIN29,
    ADC_POSINPUT_AIN31 = ADC_INPUTCTRL_MUXPOS_AIN31,
    ADC_POSINPUT_AIN40 = ADC_INPUTCTRL_MUXPOS_AIN40,
    ADC_POSINPUT_AIN41 = ADC_INPUTCTRL_MUXPOS_AIN41,
    ADC_POSINPUT_AIN42 = ADC_INPUTCTRL_MUXPOS_AIN42,
    ADC_POSINPUT_AIN43 = ADC_INPUTCTRL_MUXPOS_AIN43,
    ADC_POSINPUT_GND = ADC_INPUTCTRL_MUXPOS_GND,
    ADC_POSINPUT_VDDCOREDIV4 = ADC_INPUTCTRL_MUXPOS_VDDCOREDIV4,
    ADC_POSINPUT_TEMPSENSE = ADC_INPUTCTRL_MUXPOS_TEMPSENSE,
    ADC_POSINPUT_VDDIODIV10 = ADC_INPUTCTRL_MUXPOS_VDDIODIV10,
    ADC_POSINPUT_VDDIO2DIV10 = ADC_INPUTCTRL_MUXPOS_VDDIO2DIV10,
    ADC_POSINPUT_ACVREFSCALE0 = ADC_INPUTCTRL_MUXPOS_ACVREFSCALE0,
    ADC_POSINPUT_ACVREFSCALE1 = ADC_INPUTCTRL_MUXPOS_ACVREFSCALE1,
    ADC_POSINPUT_TESTBUF = ADC_INPUTCTRL_MUXPOS_TESTBUF,
    ADC_POSINPUT_PTC = ADC_INPUTCTRL_MUXPOS_PTC,
}ADC_POSINPUT;

// *****************************************************************************

typedef enum
{
    ADC_NEGINPUT_AIN2 = ADC_INPUTCTRL_MUXNEG_AIN2,
    ADC_NEGINPUT_AIN3 = ADC_INPUTCTRL_MUXNEG_AIN3,
    ADC_NEGINPUT_AIN4 = ADC_INPUTCTRL_MUXNEG_AIN4,
    ADC_NEGINPUT_AIN5 = ADC_INPUTCTRL_MUXNEG_AIN5,
    ADC_NEGINPUT_AIN6 = ADC_INPUTCTRL_MUXNEG_AIN6,
    ADC_NEGINPUT_AIN7 = ADC_INPUTCTRL_MUXNEG_AIN7,
    ADC_NEGINPUT_AIN16 = ADC_INPUTCTRL_MUXNEG_AIN16,
    ADC_NEGINPUT_AIN17 = ADC_INPUTCTRL_MUXNEG_AIN17,
    ADC_NEGINPUT_AIN18 = ADC_INPUTCTRL_MUXNEG_AIN18,
    ADC_NEGINPUT_AIN19 = ADC_INPUTCTRL_MUXNEG_AIN19,
    ADC_NEGINPUT_AIN20 = ADC_INPUTCTRL_MUXNEG_AIN20,
    ADC_NEGINPUT_AIN21 = ADC_INPUTCTRL_MUXNEG_AIN21,
    ADC_NEGINPUT_AIN22 = ADC_INPUTCTRL_MUXNEG_AIN22,
    ADC_NEGINPUT_AIN23 = ADC_INPUTCTRL_MUXNEG_AIN23,
    ADC_NEGINPUT_AIN24 = ADC_INPUTCTRL_MUXNEG_AIN24,
    ADC_NEGINPUT_AIN25 = ADC_INPUTCTRL_MUXNEG_AIN25,
    ADC_NEGINPUT_AIN26 = ADC_INPUTCTRL_MUXNEG_AIN26,
    ADC_NEGINPUT_AIN27 = ADC_INPUTCTRL_MUXNEG_AIN27,
    ADC_NEGINPUT_AIN28 = ADC_INPUTCTRL_MUXNEG_AIN28,
    ADC_NEGINPUT_AIN29 = ADC_INPUTCTRL_MUXNEG_AIN29,
    ADC_NEGINPUT_AIN31 = ADC_INPUTCTRL_MUXNEG_AIN31,
    ADC_NEGINPUT_AIN40 = ADC_INPUTCTRL_MUXNEG_AIN40,
    ADC_NEGINPUT_AIN41 = ADC_INPUTCTRL_MUXNEG_AIN41,
    ADC_NEGINPUT_AIN42 = ADC_INPUTCTRL_MUXNEG_AIN42,
    ADC_NEGINPUT_AIN43 = ADC_INPUTCTRL_MUXNEG_AIN43,
    ADC_NEGINPUT_GND = ADC_INPUTCTRL_MUXNEG_GND,
    ADC_NEGINPUT_VREFP = ADC_INPUTCTRL_MUXNEG_VREFP,
}ADC_NEGINPUT;

typedef uint32_t ADC_STATUS;
typedef enum
{
    ADC_WINMODE_DISABLED                   = ADC_WINCTRL_WINMODE_NONE,
    ADC_WINMODE_LESS_THAN_WINLT           = ADC_WINCTRL_WINMODE_BELOW,
    ADC_WINMODE_GREATER_THAN_WINHT        = ADC_WINCTRL_WINMODE_ABOVE,
    ADC_WINMODE_BETWEEN_WINLT_AND_WINHT   = ADC_WINCTRL_WINMODE_INSIDE,
    ADC_WINMODE_OUTSIDE_WINLT_AND_WINHT   = ADC_WINCTRL_WINMODE_OUTSIDE
} ADC_WINMODE;

typedef enum
{
    ADC_STARTMODE_STOP       = ADC_COMMAND_START_STOP, // Stop an ongoing conversion
    ADC_STARTMODE_IMMEDIATE  = ADC_COMMAND_START_IMMEDIATE, // Start conversion immediately
    ADC_STARTMODE_INPUT      = ADC_COMMAND_START_INPUT, // Start conversion on INPUTCTRL register write
    ADC_STARTMODE_EVENT      = ADC_COMMAND_START_EVENT  // Start conversion on event trigger (EVCTRL.STARTEI must be set)
} ADC_STARTMODE;

typedef enum
{
    ADC_INTERRUPT_RESRDY   = ADC_INTFLAG_RESRDY_Msk,  // Bit 0: Result Ready
    ADC_INTERRUPT_SAMPRDY  = ADC_INTFLAG_SAMPRDY_Msk,  // Bit 1: Sample Ready  
    ADC_INTERRUPT_WCMP     = ADC_INTFLAG_WCMP_Msk,  // Bit 2: Window Comparator
    ADC_INTERRUPT_RESOVR   = ADC_INTFLAG_RESOVR_Msk,  // Bit 3: Result Overwrite
    ADC_INTERRUPT_SAMPOVR  = ADC_INTFLAG_SAMPOVR_Msk,  // Bit 4: Sample Overwrite
    ADC_INTERRUPT_TRIGOVR  = ADC_INTFLAG_TRIGOVR_Msk,  // Bit 5: Trigger Overrun
    // Bits 6-31: Reserved (0)
} ADC_INTERRUPTS;

// *****************************************************************************


typedef void (*ADC_CALLBACK)(ADC_STATUS status, uintptr_t context);


typedef struct
{
    ADC_CALLBACK callback;

    uintptr_t context;

} ADC_CALLBACK_OBJ;



// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif /* PLIB_ADC_COMMON_H*/
