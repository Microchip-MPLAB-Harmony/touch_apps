/*******************************************************************************
  Touch Library v3.17.0 Release

  Company:
    Microchip Technology Inc.

  File Name:
    touch.h

  Summary:
    QTouch Modular Library

  Description:
    Configuration macros for touch library

*******************************************************************************/

/*******************************************************************************
Copyright (c) 2025 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/
#ifndef TOUCH_H
#define TOUCH_H
#include "device.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END
//SAML10

/*----------------------------------------------------------------------------
 *     include files
 *----------------------------------------------------------------------------*/

#include "touch_api_ptc.h"

/**********************************************************/
/******************* Acquisition controls *****************/
/**********************************************************/
/* Defines the Measurement Time in milli seconds.
 * Range: 1 to 255.
 * Default value: 20.
 */
#define DEF_TOUCH_MEASUREMENT_PERIOD_MS 20u

/* Defines the Type of sensor
 * Default value: NODE_MUTUAL.
 */
#define DEF_SENSOR_TYPE NODE_SELFCAP_SHIELD

/* Set sensor calibration mode for charge share delay ,Prescaler or series resistor.
 * Range: CAL_AUTO_TUNE_NONE / CAL_AUTO_TUNE_RSEL / CAL_AUTO_TUNE_PRSC / CAL_AUTO_TUNE_CSD
 * Default value: CAL_AUTO_TUNE_NONE.
 */

#define DEF_PTC_CAL_OPTION   CAL_AUTO_TUNE_NONE

/* Calibration option to ensure full charge transfer */
/* Bits 7:0 = XX | TT SELECT_TAU | X | CAL_OPTION */
#define DEF_PTC_TAU_TARGET CAL_CHRG_5TAU
#define DEF_PTC_CAL_AUTO_TUNE (uint8_t)((DEF_PTC_TAU_TARGET << CAL_CHRG_TIME_POS) | DEF_PTC_CAL_OPTION)

/* Defines the interrupt priority for the PTC. Set low priority to PTC interrupt for applications having interrupt time
 * constraints.
 */
#define DEF_PTC_INTERRUPT_PRIORITY 2u

/* Set default bootup acquisition frequency.
 * Range: FREQ_SEL_0 - FREQ_SEL_15 , FREQ_SEL_SPREAD
 * Default value: FREQ_SEL_0.
 */
#define DEF_SEL_FREQ_INIT FREQ_SEL_0


/*----------------------------------------------------------------------------
 *     defines
 *----------------------------------------------------------------------------*/

/**********************************************************/
/***************** Node Params   ******************/
/**********************************************************/
/* Acquisition Set 1 */
/* Defines the number of sensor nodes in the acquisition set
 * Range: 1 to 65535.
 * Default value: 1
 */
#define DEF_NUM_CHANNELS (5u)


/* Defines node parameter setting
 * {X-line, Y-line, Charge Share Delay (CSD), NODE_RSEL_PRSC(series resistor, prescaler), NODE_G(Analog Gain , Digital Gain),
 * filter level}
 */


#define NODE_0_PARAMS                                                                                               \
{                                                                                                                  \
   X_NONE, Y(3), 0,(uint8_t)PRSC_DIV_SEL_4, NODE_GAIN(GAIN_1, GAIN_1), (uint8_t)FILTER_LEVEL_16                   \
}
#define NODE_1_PARAMS                                                                                               \
{                                                                                                                  \
   X_NONE, Y(13), 0,(uint8_t)PRSC_DIV_SEL_4, NODE_GAIN(GAIN_1, GAIN_1), (uint8_t)FILTER_LEVEL_16                   \
}
#define NODE_2_PARAMS                                                                                               \
{                                                                                                                  \
   X_NONE, Y(14), 0,(uint8_t)PRSC_DIV_SEL_4, NODE_GAIN(GAIN_1, GAIN_1), (uint8_t)FILTER_LEVEL_16                   \
}
#define NODE_3_PARAMS                                                                                               \
{                                                                                                                  \
   X_NONE, Y(17), 0,(uint8_t)PRSC_DIV_SEL_4, NODE_GAIN(GAIN_1, GAIN_1), (uint8_t)FILTER_LEVEL_16                   \
}
#define NODE_4_PARAMS                                                                                               \
{                                                                                                                  \
   X_NONE, Y(16), 0,(uint8_t)PRSC_DIV_SEL_4, NODE_GAIN(GAIN_1, GAIN_1), (uint8_t)FILTER_LEVEL_16                   \
}

/**********************************************************/
/***************** Key Params   ******************/
/**********************************************************/
/* Defines the number of key sensors
 * Range: 1 to 65535.
 * Default value: 1
 */
#define DEF_NUM_SENSORS (5u)

/* Defines Key Sensor setting
 * {Sensor Threshold, Sensor Hysterisis, Sensor AKS}
 */

#define KEY_0_PARAMS                                                                                            \
{                                                                                                              \
    20u, (uint8_t)HYST_25, (uint8_t)NO_AKS_GROUP                       \
}


#define KEY_1_PARAMS                                                                                            \
{                                                                                                              \
    20u, (uint8_t)HYST_25, (uint8_t)NO_AKS_GROUP                       \
}


#define KEY_2_PARAMS                                                                                            \
{                                                                                                              \
    20u, (uint8_t)HYST_25, (uint8_t)AKS_GROUP_1                       \
}


#define KEY_3_PARAMS                                                                                            \
{                                                                                                              \
    20u, (uint8_t)HYST_25, (uint8_t)AKS_GROUP_1                       \
}


#define KEY_4_PARAMS                                                                                            \
{                                                                                                              \
    20u, (uint8_t)HYST_25, (uint8_t)AKS_GROUP_1                       \
}


/* De-bounce counter for additional measurements to confirm touch detection
 * Range: 0 to 255.
 * Default value: 4.
 */
#define DEF_TOUCH_DET_INT 4u

/* De-bounce counter for additional measurements to confirm away from touch signal
 * to initiate Away from touch re-calibration.
 * Range: 0 to 255.
 * Default value: 5.
 */
#define DEF_ANTI_TCH_DET_INT 5u

/* Threshold beyond with automatic sensor recalibration is initiated.
 * Range: RECAL_100/ RECAL_50 / RECAL_25 / RECAL_12_5 / RECAL_6_25 / MAX_RECAL
 * Default value: RECAL_100.
 */
#define DEF_ANTI_TCH_RECAL_THRSHLD (uint8_t)RECAL_100

/* Rate at which sensor reference value is adjusted towards sensor signal value
 * when signal value is greater than reference.
 * Units: 200ms
 * Range: 0-255
 * Default value: 20u = 4 seconds.
 */
#define DEF_TCH_DRIFT_RATE 20u

/* Rate at which sensor reference value is adjusted towards sensor signal value
 * when signal value is less than reference.
 * Units: 200ms
 * Range: 0-255
 * Default value: 5u = 1 second.
 */
#define DEF_ANTI_TCH_DRIFT_RATE 5u

/* Time to restrict drift on all sensor when one or more sensors are activated.
 * Units: 200ms
 * Range: 0-255
 * Default value: 20u = 4 seconds.
 */
#define DEF_DRIFT_HOLD_TIME 20u

/* Set mode for additional sensor measurements based on touch activity.
 * Range: REBURST_NONE / REBURST_UNRESOLVED / REBURST_ALL
 * Default value: REBURST_UNRESOLVED
 */
#define DEF_REBURST_MODE (uint8_t)REBURST_UNRESOLVED

/* Sensor maximum ON duration upon touch.
 * Range: 0-255
 * Default value: 0
 */
#define DEF_MAX_ON_DURATION 0u

/**********************************************************/
/***************** Slider/Wheel Parameters ****************/
/**********************************************************/
/* Defines the number of scrollers (sliders or wheels)
 */
#define DEF_NUM_SCROLLERS 1u

/* Defines scroller parameter setting
 * {touch_scroller_type, touch_start_key, touch_scroller_size,
 * SCROLLER_RESOL_DEADBAND(touch_scroller_resolution,touch_scroller_deadband), touch_scroller_hysterisis,
 * touch_scr_detect_threshold}
 * Configuring scr_detect_threshold: By default, scr_detect_threshold parameter should be
 * set equal to threshold value of the underlying keys. Then the parameter has to be tuned based on the actual contact
 * size of the touch when moved over the scroller. The contact size of the moving touch can be observed from
 * "contact_size" parameter on scroller runtime data structure.
 */
#define SCROLLER_0_PARAMS                                                                                       \
{                                                                                                              \
    (uint8_t)SCROLLER_TYPE_SLIDER, 2u, 3u,                            \
		SCROLLER_RESOL_DEADBAND((uint8_t)SCR_RESOL_8_BIT, (uint8_t)SCR_DB_10_PERCENT),(uint8_t)8,20\
}



/**********************************************************/
/********* Frequency Hop Module ****************/
/**********************************************************/

/* sets the frequency steps for hop.
 * Range: 3 to 7.
 * Default value: 3
 */
#define NUM_FREQ_STEPS 3u

/* PTC Sampling Delay Selection - 0 to 15 PTC CLK cycles */

#define DEF_MEDIAN_FILTER_FREQUENCIES (uint8_t)FREQ_SEL_0,(uint8_t)FREQ_SEL_1,(uint8_t)FREQ_SEL_2

/* Enable / Disable the frequency hop auto tune
 * Range: 0 / 1
 * Default value: 1
 */
#define DEF_FREQ_AUTOTUNE_ENABLE 1u

/* sets the maximum variance for Frequency Hop Auto tune.
 * Range: 1 to 255.
 * Default value: 15
 */
#define FREQ_AUTOTUNE_MAX_VARIANCE 25u

/* sets the Tune in count for Frequency Hop Auto tune.
 * Range: 1 to 255.
 * Default value: 6
 */
#define FREQ_AUTOTUNE_COUNT_IN 6u

/* Auto scan trigger Periods */
#define NODE_SCAN_8MS		0u
#define NODE_SCAN_16MS		1u
#define NODE_SCAN_32MS		2u
#define NODE_SCAN_64MS		3u
#define NODE_SCAN_128MS		4u
#define NODE_SCAN_256MS		5u
#define NODE_SCAN_512MS		6u
#define NODE_SCAN_1024MS	7u

/* Event system parameters */
#define QTM_AUTOSCAN_TRIGGER_GENERATOR			((uint32_t)QTM_AUTOSCAN_TRIGGER_PERIOD+4UL)
#define QTM_AUTOSCAN_STCONV_USER				19u
#define QTM_RTC_TO_PTC_EVSYS_CHANNEL			0u
#define QTM_AUTOSCAN_TRIGGER_PERIOD_EVENT		(1u << QTM_AUTOSCAN_TRIGGER_PERIOD)

/**********************************************************/
/******************* Low-power parameters *****************/
/**********************************************************/
/* Enable or disable low-power scan 
* Range: 0 or 1
* Default value: 1
*/
#define DEF_TOUCH_LOWPOWER_ENABLE 1u

/* Node selection for Low-power scan. 
* Range: 0 to (DEF_NUM_CHANNELS-1).
* Default value: 0
*/
#define QTM_AUTOSCAN_NODE			 0

/* Touch detection threshold for Low-power node. 
* Range: 10 to 255
* Default value: 10
*/
#define QTM_AUTOSCAN_THRESHOLD		 10u

/* Defines the Auto scan trigger period.
 * The Low-power measurement period determine the interval between low-power touch measurement.
 * Range: NODE_SCAN_4MS to NODE_SCAN_512MS 
 * Check API file to get the actual range. For certain devices, range is NODE_SCAN_8MS to NODE_SCAN_1024MS 
 * Default value: NODE_SCAN_64MS
*/
#define QTM_AUTOSCAN_TRIGGER_PERIOD	 NODE_SCAN_512MS

/* Waiting time (in millisecond) for the application to switch to low-power measurement after the last touch.
* Range: 1 to 65535
* Default value: 5000
*/
#define DEF_TOUCH_TIMEOUT                       5000u

/* Defines drift measurement period
* During low-power measurement, it is recommended to perform periodic active measurement to perform drifting.
* This parameter defines the measurement interval to perform drifting.
* Range: 1 to 65535 ( should be more than QTM_AUTOSCAN_TRIGGER_PERIOD)
* Default value: 2000
*/
#define DEF_TOUCH_DRIFT_PERIOD_MS               2000u
/**********************************************************/
/***************** Communication - Data Streamer ******************/
/**********************************************************/
#define DEF_TOUCH_DATA_STREAMER_ENABLE 0u



/**********************************************************/



/* Acquisition variables */
extern qtm_acq_node_data_t ptc_qtlib_node_stat1[DEF_NUM_CHANNELS];
extern qtm_acq_saml10_node_config_t ptc_seq_node_cfg1[DEF_NUM_CHANNELS];


/* Keys variables */
extern qtm_touch_key_group_config_t qtlib_key_grp_config_set1;
extern qtm_touch_key_data_t qtlib_key_data_set1[DEF_NUM_SENSORS];
extern qtm_touch_key_config_t qtlib_key_configs_set1[DEF_NUM_SENSORS];
/* Scroller variables */
extern qtm_scroller_config_t qtm_scroller_config1[DEF_NUM_SCROLLERS];
extern qtm_scroller_data_t qtm_scroller_data1[DEF_NUM_SCROLLERS];
/* Frequency Hop Autotune variables */
extern qtm_freq_hop_autotune_config_t qtm_freq_hop_autotune_config1;
extern uint8_t module_error_code;


#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
extern volatile uint8_t time_to_measure_touch_var;
#endif
extern volatile uint8_t measurement_done_touch;


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // TOUCH_H
