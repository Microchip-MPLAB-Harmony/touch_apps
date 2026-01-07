/*******************************************************************************
  Analog-to-Digital Converter(ADC0) PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_adc0.c

  Summary
    ADC0 PLIB Implementation File.

  Description
    This file defines the interface to the ADC peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

  Remarks:
    None.

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/* This section lists the other files that are included in this file.
*/

#include "interrupts.h"
#include "plib_adc0.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: ADC0 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Initialize ADC module */
void ADC0_Initialize( void )
{
    /* Reset ADC */
    ADC0_REGS->ADC_CTRLA = (uint8_t)ADC_CTRLA_SWRST_Msk;

    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }

    /* Prescaler and timebase configuration */
    ADC0_REGS->ADC_CTRLB = (uint8_t)ADC_CTRLB_PRESCALER_DIV4 | ADC_CTRLB_TIMEBASE(1UL) ;

    /* Sampling length */
    ADC0_REGS->ADC_CTRLE = (uint8_t)ADC_CTRLE_SAMPLEN(3UL);

    /* Reference */
    ADC0_REGS->ADC_CTRLC = (uint8_t)ADC_CTRLC_REFSEL_VDD;

    /* Input pin */
    ADC0_REGS->ADC_INPUTCTRL = (uint16_t) ADC_POSINPUT_AIN2;

    /* Resolution & Scaling */
    ADC0_REGS->ADC_CTRLD = ADC_CTRLD_RESOLUTION_12BIT | ADC_CTRLD_SCALING(0x0UL) | ADC_CTRLD_FREERUN_Msk | ADC_CTRLD_VPD(0UL) ;

    /* Window Operation Mode */
    ADC0_REGS->ADC_WINCTRL = (uint32_t)ADC_WINCTRL_WINMODE(0x0UL);

    /* Clear all interrupt flags */
    ADC0_REGS->ADC_INTFLAG = (uint8_t)ADC_INTFLAG_Msk;



    /* Mode of opearation and Start type*/
    ADC0_REGS->ADC_COMMAND = (uint8_t)ADC_COMMAND_MODE(0UL) | (uint8_t)ADC_COMMAND_START(0UL) ;

    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Enable ADC module */
void ADC0_Enable( void )
{
    ADC0_REGS->ADC_CTRLA |= (uint8_t)ADC_CTRLA_ENABLE_Msk;
    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Disable ADC module */
void ADC0_Disable( void )
{
    ADC0_REGS->ADC_CTRLA &= (uint8_t)(~ADC_CTRLA_ENABLE_Msk);
    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Configure channel input */
void ADC0_ChannelSelect( ADC_POSINPUT positiveInput, ADC_NEGINPUT negativeInput )
{
    /* Configure pin scan mode and positive and negative input pins */
    ADC0_REGS->ADC_INPUTCTRL = (uint16_t)((uint16_t) positiveInput | (uint16_t) negativeInput);

    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Start the ADC conversion by SW */
void ADC0_ConversionStart( void )
{
    /* Start conversion */
    ADC0_REGS->ADC_COMMAND |= (uint32_t)ADC_COMMAND_START_IMMEDIATE;

    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Select ADC conversion start type */
void ADC0_ConversionStartModeSet( ADC_STARTMODE startMode )
{
    /* Start conversion */
    ADC0_REGS->ADC_COMMAND |= (uint32_t)startMode;

    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Check if adc conversion is on going conversion completed*/
bool ADC0_ConversionIsFinished( void )
{
    return ((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) == 0U);
}

/* Configure window comparison threshold values */
void ADC0_ComparisonWindowSet(uint16_t low_threshold, uint16_t high_threshold)
{
    ADC0_REGS->ADC_WINLT = low_threshold;
    ADC0_REGS->ADC_WINHT = high_threshold;
    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

void ADC0_WindowModeSet(ADC_WINMODE mode)
{
    ADC0_REGS->ADC_WINCTRL =  (ADC0_REGS->ADC_WINCTRL & (uint16_t)(~ADC_WINCTRL_WINMODE_Msk)) | (uint16_t)((uint32_t)mode);
    while((ADC0_REGS->ADC_STATUS & ADC_STATUS_ADCBUSY_Msk) != 0U)
    {
        /* Wait for Synchronization */
    }
}

/* Read the conversion result */
uint32_t ADC0_ConversionResultGet( void )
{
    return (uint32_t)ADC0_REGS->ADC_RESULT;
}

void ADC0_InterruptsClear(ADC_INTERRUPTS interruptMask)
{
    ADC0_REGS->ADC_INTFLAG = (uint8_t)interruptMask;
}

void ADC0_InterruptsEnable(ADC_INTERRUPTS interruptMask)
{
    ADC0_REGS->ADC_INTENSET = (uint8_t)interruptMask;
}

void ADC0_InterruptsDisable(ADC_INTERRUPTS interruptMask)
{
    ADC0_REGS->ADC_INTENCLR = (uint8_t)interruptMask;
}

/* Check whether result is ready */
bool ADC0_ResultReadyStatusGet( void )
{
    bool status;
    status =  (((ADC0_REGS->ADC_INTFLAG & ADC_INTFLAG_RESRDY_Msk) >> ADC_INTFLAG_RESRDY_Pos) != 0U);
    if (status == true)
    {
        ADC0_REGS->ADC_INTFLAG = (uint8_t)ADC_INTFLAG_RESRDY_Msk;
    }
    return status;
}
/* Check whether sample is ready */
bool ADC0_SampleReadyStatusGet( void )
{
    bool status;
    status =  (((ADC0_REGS->ADC_INTFLAG & ADC_INTENSET_SAMPRDY_Msk) >> ADC_INTENSET_SAMPRDY_Pos) != 0U);
    if (status == true)
    {
        ADC0_REGS->ADC_INTFLAG = (uint8_t)ADC_INTENSET_SAMPRDY_Msk;
    }
    return status;
}
