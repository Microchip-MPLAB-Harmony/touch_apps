/*******************************************************************************
  Touch Library v3.17.0 Release

  Company:
    Microchip Technology Inc.

  File Name:
    touch.c

  Summary:
    QTouch Modular Library

  Description:
    Provides Initialization, Processing and ISR handler of touch library,
    Simple API functions to get/set the key touch parameters from/to the
    touch library data structures
*******************************************************************************/

/*******************************************************************************
Copyright (C) [2025], Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
************************************************************************************/



/*----------------------------------------------------------------------------
 *     include files
 *----------------------------------------------------------------------------*/
#include "definitions.h"
#include "../peripheral/rtc/plib_rtc.h"
#include "../interrupts.h"
#include "touch/touch.h"

/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/
/*! \brief configure keys, wheels and sliders.
 */
static touch_ret_t touch_sensors_config(void);

/*! \brief Touch measure complete callback function example prototype.
 */
static void qtm_measure_complete_callback(void);

/*! \brief Touch Error callback function prototype.
 */
static void qtm_error_callback(uint8_t error);


#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
/* low power processing function */
static void touch_process_lowpower(void);
static void touch_enable_lowpower_measurement(void);
static void touch_disable_lowpower_measurement(void);
/* configure pm, supc */
static void touch_configure_pm_supc(void);
/* low power touch detection callback */
static void touch_measure_wcomp_match(void);
#endif

/*----------------------------------------------------------------------------
 *     Global Variables
 *----------------------------------------------------------------------------*/
/* Flag to indicate time for touch measurement */
volatile uint8_t time_to_measure_touch_var = 0u;
/* post-process request flag */
static volatile uint8_t touch_postprocess_request = 0u;

/* Measurement Done Touch Flag  */
volatile uint8_t measurement_done_touch = 0u;

/* Error Handling */
uint8_t module_error_code = 0u;

/* Low-power measurement variables */
static uint16_t time_since_touch = 0u;
/* store the drift period for comparison */
static uint16_t measurement_period_store = DEF_TOUCH_MEASUREMENT_PERIOD_MS;
/* measurement mode; 0 - sequential, 1 - windowcomp*/

/* Acquisition module internal data - Size to largest acquisition set */
static uint16_t touch_acq_signals_raw[DEF_NUM_CHANNELS];
/* Acquisition set 1 - General settings */
static qtm_acq_node_group_config_t ptc_qtlib_acq_gen1
    = {DEF_NUM_CHANNELS, DEF_SENSOR_TYPE, DEF_PTC_CAL_AUTO_TUNE, (uint8_t)DEF_SEL_FREQ_INIT, DEF_PTC_INTERRUPT_PRIORITY};

/* Node status, signal, calibration values */
qtm_acq_node_data_t ptc_qtlib_node_stat1[DEF_NUM_CHANNELS];

/* Node configurations */
qtm_acq_saml10_node_config_t ptc_seq_node_cfg1[DEF_NUM_CHANNELS] = {NODE_0_PARAMS,NODE_1_PARAMS,NODE_2_PARAMS,NODE_3_PARAMS,NODE_4_PARAMS};

/* Container */
static qtm_acquisition_control_t qtlib_acq_set1 = {&ptc_qtlib_acq_gen1, &ptc_seq_node_cfg1[0], &ptc_qtlib_node_stat1[0]};

	/* Low-power autoscan related parameters */
static qtm_auto_scan_config_t auto_scan_setup 
        = {&qtlib_acq_set1, QTM_AUTOSCAN_NODE, QTM_AUTOSCAN_THRESHOLD, QTM_AUTOSCAN_TRIGGER_PERIOD};
/**********************************************************/
/*********** Frequency Hop Auto tune Module **********************/
/**********************************************************/

/* Buffer used with various noise filtering functions */
static uint16_t noise_filter_buffer[DEF_NUM_CHANNELS * NUM_FREQ_STEPS];
static uint8_t  freq_hop_delay_selection[NUM_FREQ_STEPS] = {DEF_MEDIAN_FILTER_FREQUENCIES};
static uint8_t  freq_hop_autotune_counters[NUM_FREQ_STEPS];

/* Configuration */
qtm_freq_hop_autotune_config_t qtm_freq_hop_autotune_config1 = {DEF_NUM_CHANNELS,
                                                                NUM_FREQ_STEPS,
                                                                &ptc_qtlib_acq_gen1.freq_option_select,
                                                                &freq_hop_delay_selection[0],
                                                                DEF_FREQ_AUTOTUNE_ENABLE,
                                                                FREQ_AUTOTUNE_MAX_VARIANCE,
                                                                FREQ_AUTOTUNE_COUNT_IN};

/* Data */
static qtm_freq_hop_autotune_data_t qtm_freq_hop_autotune_data1
    = {0, 0, &noise_filter_buffer[0], &ptc_qtlib_node_stat1[0], &freq_hop_autotune_counters[0]};

/* Container */
static qtm_freq_hop_autotune_control_t qtm_freq_hop_autotune_control1
    = {&qtm_freq_hop_autotune_data1, &qtm_freq_hop_autotune_config1};

/**********************************************************/
/*********************** Keys Module **********************/
/**********************************************************/

/* Keys set 1 - General settings */
qtm_touch_key_group_config_t qtlib_key_grp_config_set1 = {DEF_NUM_SENSORS,
                                                          DEF_TOUCH_DET_INT,
                                                          DEF_MAX_ON_DURATION,
                                                          DEF_ANTI_TCH_DET_INT,
                                                          DEF_ANTI_TCH_RECAL_THRSHLD,
                                                          DEF_TCH_DRIFT_RATE,
                                                          DEF_ANTI_TCH_DRIFT_RATE,
                                                          DEF_DRIFT_HOLD_TIME,
                                                          DEF_REBURST_MODE};

static qtm_touch_key_group_data_t qtlib_key_grp_data_set1;

/* Key data */
qtm_touch_key_data_t qtlib_key_data_set1[DEF_NUM_SENSORS];

/* Key Configurations */
qtm_touch_key_config_t qtlib_key_configs_set1[DEF_NUM_SENSORS] = { KEY_0_PARAMS, KEY_1_PARAMS, KEY_2_PARAMS, KEY_3_PARAMS,KEY_4_PARAMS}; 
/* Container */
static qtm_touch_key_control_t qtlib_key_set1
    = {&qtlib_key_grp_data_set1, &qtlib_key_grp_config_set1, &qtlib_key_data_set1[0], &qtlib_key_configs_set1[0]};

/**********************************************************/
/***************** Scroller Module ********************/
/**********************************************************/

/* Individual and Group Data */
qtm_scroller_data_t       qtm_scroller_data1[DEF_NUM_SCROLLERS];
static qtm_scroller_group_data_t qtm_scroller_group_data1 = {0};

/* Group Configuration */
static qtm_scroller_group_config_t qtm_scroller_group_config1 = {&qtlib_key_data_set1[0], DEF_NUM_SCROLLERS};

/* Scroller Configurations */
qtm_scroller_config_t qtm_scroller_config1[DEF_NUM_SCROLLERS] = {SCROLLER_0_PARAMS}; 

/* Container */
static qtm_scroller_control_t qtm_scroller_control1
    = {&qtm_scroller_group_data1, &qtm_scroller_group_config1, &qtm_scroller_data1[0], &qtm_scroller_config1[0]};



/*----------------------------------------------------------------------------
 *   function definitions
 *----------------------------------------------------------------------------*/


/*============================================================================
static touch_ret_t touch_sensors_config(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch key sensors
Input  : none
Output : none
Notes  :
============================================================================*/
/* Touch sensors config - assign nodes to buttons / wheels / sliders / surfaces / water level / etc */
static touch_ret_t touch_sensors_config(void)
{
    uint16_t    sensor_nodes;
    touch_ret_t touch_ret = TOUCH_SUCCESS;

    /* Init acquisition module */
    touch_ret = qtm_ptc_init_acquisition_module(&qtlib_acq_set1);
    touch_ret = qtm_ptc_qtlib_assign_signal_memory(&touch_acq_signals_raw[0]);

    /* Initialize sensor nodes */
    for (sensor_nodes = 0u; sensor_nodes < (uint16_t) DEF_NUM_CHANNELS; sensor_nodes++) {
        /* Enable each node for measurement and mark for calibration */
        touch_ret = qtm_enable_sensor_node(&qtlib_acq_set1, sensor_nodes);
        touch_ret = qtm_calibrate_sensor_node(&qtlib_acq_set1, sensor_nodes);
    }

    /* Enable sensor keys and assign nodes */
    for (sensor_nodes = 0u; sensor_nodes < (uint16_t)DEF_NUM_SENSORS; sensor_nodes++) {
			touch_ret=qtm_init_sensor_key(&qtlib_key_set1, (uint8_t) sensor_nodes, &ptc_qtlib_node_stat1[sensor_nodes]);
    }

	/* scroller init */
	touch_ret = qtm_init_scroller_module(&qtm_scroller_control1);


    return (touch_ret);
}

/*============================================================================
static void qtm_measure_complete_callback( void )
------------------------------------------------------------------------------
Purpose: Callback function called after the completion of
         measurement cycle. This function sets the post processing request
         flag to trigger the post processing.
Input  : none
Output : none
Notes  :
============================================================================*/
static void qtm_measure_complete_callback(void)
{
    touch_postprocess_request = 1u;
}

/*============================================================================
static void qtm_error_callback(uint8_t error)
------------------------------------------------------------------------------
Purpose: Callback function called after the completion of
         post processing. This function is called only when there is error.
Input  : error code
Output : decoded module error code
Notes  :
Derived Module_error_codes:
    Acquisition module error =1
    post processing module1 error = 2
    post processing module2 error = 3
    ... and so on

============================================================================*/
static void qtm_error_callback(uint8_t error)
{
	module_error_code = error + 1u;

}

/*============================================================================
void touch_init(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch library. PTC, timer, and
         datastreamer modules are initialized in this function.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_init(void)
{
	touch_timer_config();
#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
	/* configure voltage regulator to run in standby sleep mode */
	touch_configure_pm_supc();
	touch_disable_lowpower_measurement();
#endif

	/* Configure touch sensors with Application specific settings */
    (void)touch_sensors_config();

	
}

/*============================================================================
void touch_process(void)
------------------------------------------------------------------------------
Purpose: Main processing function of touch library. This function initiates the
         acquisition, calls post processing after the acquistion complete and
         sets the flag for next measurement based on the sensor status.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_process(void)
{
    touch_ret_t touch_ret;

    /* check the time_to_measure_touch for Touch Acquisition */
    if (time_to_measure_touch_var == 1u) {

        /* Do the acquisition */
         touch_ret = qtm_ptc_start_measurement_seq(&qtlib_acq_set1, qtm_measure_complete_callback);

        /* if the Acquistion request was successful then clear the request flag */
        if (TOUCH_SUCCESS == touch_ret) {
            /* Clear the Measure request flag */
			time_to_measure_touch_var = 0;
        }
    }
    /* check the flag for node level post processing */
    if (touch_postprocess_request == 1u){
        /* Reset the flags for node_level_post_processing */
        touch_postprocess_request = 0u;
        /* Run Acquisition module level post processing*/
        touch_ret = qtm_acquisition_process();
        /* Check the return value */
        if (TOUCH_SUCCESS == touch_ret) {
            /* Returned with success: Start module level post processing */

            touch_ret = qtm_freq_hop_autotune(&qtm_freq_hop_autotune_control1);
            if (TOUCH_SUCCESS != touch_ret) {
                qtm_error_callback(1);
        }
            touch_ret = qtm_key_sensors_process(&qtlib_key_set1);
            if (TOUCH_SUCCESS != touch_ret) {
                qtm_error_callback(2);
            }
            touch_ret = qtm_scroller_process(&qtm_scroller_control1);
            if (TOUCH_SUCCESS != touch_ret) {
                qtm_error_callback(3);
            }
         }else {
           /* Acq module Error Detected: Issue an Acq module common error code 0x80 */
            qtm_error_callback(0);
        }

        #if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
        if (0u != (qtlib_key_grp_data_set1.qtm_keys_status & QTM_KEY_DETECT)) {
            /* Something in detect */
            time_since_touch = 0u;
        }
        #endif


        if (0u != (qtlib_key_set1.qtm_touch_key_group_data->qtm_keys_status & QTM_KEY_REBURST)) {
            time_to_measure_touch_var = 1u;
        } else {
            measurement_done_touch =1u;
            #if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
            /* process lowpower touch measurement */
            touch_process_lowpower();
            #endif
        }
    }

}
#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
static void touch_configure_pm_supc(void)
{
    /* Configure PM */
    PM_REGS->PM_STDBYCFG = PM_STDBYCFG_BBIASHS_Msk| PM_STDBYCFG_VREGSMOD(0)| PM_STDBYCFG_DPGPDSW_Msk| PM_STDBYCFG_BBIASTR_Msk;

    while(PM_ConfigurePerformanceLevel(PLCFG_PLSEL0) != true) {

	}

    /* Configure VREG. Mask the values loaded from NVM during reset.*/
    SUPC_REGS->SUPC_VREG = SUPC_VREG_ENABLE_Msk | SUPC_VREG_SEL_BUCK | SUPC_VREG_RUNSTDBY_Msk | SUPC_VREG_VSVSTEP(0) | SUPC_VREG_VSPER(0) | SUPC_VREG_STDBYPL0_Msk;

}
#endif

#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
/*============================================================================
static void touch_disable_lowpower_measurement(void)
------------------------------------------------------------------------------
Purpose: 
Input  : none
Output : none
Notes  :
============================================================================*/
static void touch_disable_lowpower_measurement(void)
{
    /* Disable RTC to PTC event */
    EVSYS_REGS->EVSYS_USER[QTM_AUTOSCAN_STCONV_USER] = 0;
    EVSYS_REGS->CHANNEL[QTM_RTC_TO_PTC_EVSYS_CHANNEL].EVSYS_CHANNEL = EVSYS_CHANNEL_EVGEN(0) | EVSYS_CHANNEL_PATH(2) | EVSYS_CHANNEL_EDGSEL(0) \
                                        | EVSYS_CHANNEL_ONDEMAND_Msk | EVSYS_CHANNEL_RUNSTDBY(1);
    while((RTC_REGS->MODE0.RTC_SYNCBUSY & RTC_MODE0_SYNCBUSY_COUNT_Msk) == RTC_MODE0_SYNCBUSY_COUNT_Msk)
    {
        /* Wait for Synchronization after writing value to Count Register */
    }
    RTC_Timer32Stop();
    RTC_Timer32CounterSet(0);
    RTC_REGS->MODE0.RTC_EVCTRL = 0;
    RTC_Timer32CompareSet(DEF_TOUCH_MEASUREMENT_PERIOD_MS);
    RTC_Timer32Start();
	/* Store the measurement period */
	measurement_period_store = DEF_TOUCH_MEASUREMENT_PERIOD_MS;

}

/*============================================================================
static void touch_enable_lowpower_measurement(void)
------------------------------------------------------------------------------
Purpose: 
Input  : none
Output : none
Notes  :
============================================================================*/
static void touch_enable_lowpower_measurement(void)
{
    /* Enable event trigger */
    EVSYS_REGS->CHANNEL[QTM_RTC_TO_PTC_EVSYS_CHANNEL].EVSYS_CHANNEL = EVSYS_CHANNEL_EVGEN((uint32_t)QTM_AUTOSCAN_TRIGGER_GENERATOR) | EVSYS_CHANNEL_PATH(2) | EVSYS_CHANNEL_EDGSEL(0) \
                                        | EVSYS_CHANNEL_ONDEMAND_Msk | EVSYS_CHANNEL_RUNSTDBY(1);
    EVSYS_REGS->EVSYS_USER[QTM_AUTOSCAN_STCONV_USER] = QTM_RTC_TO_PTC_EVSYS_CHANNEL+1u;

    /* Set up timer with periodic event output and drift period */
    while((RTC_REGS->MODE0.RTC_SYNCBUSY & RTC_MODE0_SYNCBUSY_COUNT_Msk) == RTC_MODE0_SYNCBUSY_COUNT_Msk)
    {
        /* Wait for Synchronization after writing value to Count Register */
    }
    RTC_Timer32Stop();
    RTC_Timer32CounterSet(0);
    RTC_REGS->MODE0.RTC_EVCTRL = QTM_AUTOSCAN_TRIGGER_PERIOD_EVENT;
    RTC_Timer32CompareSet(DEF_TOUCH_DRIFT_PERIOD_MS);
    RTC_Timer32Start();
	/* Store the measurement period */
	measurement_period_store = DEF_TOUCH_DRIFT_PERIOD_MS;
}

/*============================================================================
static void touch_process_lowpower(void)
------------------------------------------------------------------------------
Purpose: Processing function for low-power touch measurment.
         Monitors the touch activity and if there is not touch for longer
         duration, low-power autoscan is enabled.
Input  : none
Output : none
Notes  :
============================================================================*/
static void touch_process_lowpower(void) {
       
	touch_ret_t touch_ret;
    if (time_since_touch >= DEF_TOUCH_TIMEOUT) {
    
		/* Start Autoscan */
		touch_ret = qtm_autoscan_sensor_node(&auto_scan_setup, touch_measure_wcomp_match);

        if ((touch_ret == TOUCH_SUCCESS) && (measurement_period_store != DEF_TOUCH_DRIFT_PERIOD_MS)){

            /* Enable Event System */
            touch_enable_lowpower_measurement();
        }
    } else if (measurement_period_store != DEF_TOUCH_MEASUREMENT_PERIOD_MS) {

        /* Cancel node auto scan */
        touch_ret = qtm_autoscan_node_cancel();

        /* disable event system low-power measurement */
        touch_disable_lowpower_measurement();
    }
		else
	{
		//doesn't reach
	}
}


/*============================================================================
static void touch_measure_wcomp_match(void)
------------------------------------------------------------------------------
Purpose: callback of autoscan function
Input  : none
Output : none
Notes  :
============================================================================*/
static void touch_measure_wcomp_match(void)
{
    if(measurement_period_store != DEF_TOUCH_MEASUREMENT_PERIOD_MS) {
        touch_disable_lowpower_measurement();
        time_to_measure_touch_var = 1u;	
        time_since_touch = 0u;
    }
}
#endif

/*============================================================================
void touch_timer_handler(void)
------------------------------------------------------------------------------
Purpose: This function updates the time elapsed to the touch key module to
         synchronize the internal time counts used by the module.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_timer_handler(void)
{
 
     
#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
    time_to_measure_touch_var = 1u;
        if (time_since_touch < (65535u - measurement_period_store)) {
            time_since_touch += measurement_period_store;
        } else {
            time_since_touch = 65535;
        }
        qtm_update_qtlib_timer(measurement_period_store);
#else
        qtm_update_qtlib_timer(DEF_TOUCH_MEASUREMENT_PERIOD_MS);
#endif

        }
 
void rtc_cb( RTC_TIMER32_INT_MASK intCause, uintptr_t context ); 
void rtc_cb( RTC_TIMER32_INT_MASK intCause, uintptr_t context )
{
    touch_timer_handler();
}
static uintptr_t rtc_context;
void touch_timer_config(void)
{  
    RTC_Timer32CallbackRegister(rtc_cb, rtc_context);

    while((RTC_REGS->MODE0.RTC_SYNCBUSY & RTC_MODE0_SYNCBUSY_COUNT_Msk) == RTC_MODE0_SYNCBUSY_COUNT_Msk)
    {
        
    }
    /* Wait for Synchronization after writing value to Count Register */
    RTC_Timer32Stop();
    RTC_Timer32CounterSet(0u);

    RTC_Timer32CompareSet((uint32_t) measurement_period_store);
    RTC_Timer32Start();  
}

uint16_t get_sensor_node_signal(uint16_t sensor_node)
{
    return (ptc_qtlib_node_stat1[sensor_node].node_acq_signals);
}

void update_sensor_node_signal(uint16_t sensor_node, uint16_t new_signal)
{
    ptc_qtlib_node_stat1[sensor_node].node_acq_signals = new_signal;
}

uint16_t get_sensor_node_reference(uint16_t sensor_node)
{
    return (qtlib_key_data_set1[sensor_node].channel_reference);
}

void update_sensor_node_reference(uint16_t sensor_node, uint16_t new_reference)
{
    qtlib_key_data_set1[sensor_node].channel_reference = new_reference;
}

uint16_t get_sensor_cc_val(uint16_t sensor_node)
{
    return (ptc_qtlib_node_stat1[sensor_node].node_comp_caps);
}

void update_sensor_cc_val(uint16_t sensor_node, uint16_t new_cc_value)
{
    ptc_qtlib_node_stat1[sensor_node].node_comp_caps = new_cc_value;
}

uint8_t get_sensor_state(uint16_t sensor_node)
{
    return (qtlib_key_set1.qtm_touch_key_data[sensor_node].sensor_state);
}

void update_sensor_state(uint16_t sensor_node, uint8_t new_state)
{
    qtlib_key_set1.qtm_touch_key_data[sensor_node].sensor_state = new_state;
}

void calibrate_node(uint16_t sensor_node)
{
	touch_ret_t touch_ret = TOUCH_SUCCESS;
    /* Calibrate Node */
	touch_ret = qtm_calibrate_sensor_node(&qtlib_acq_set1, sensor_node);
    if(touch_ret != TOUCH_SUCCESS) {
		/* Error condition */
	}
    /* Initialize key */
    touch_ret = qtm_init_sensor_key(&qtlib_key_set1, (uint8_t) sensor_node, &ptc_qtlib_node_stat1[sensor_node]);
    if(touch_ret != TOUCH_SUCCESS) {
		/* Error condition */
	}
}

uint8_t get_scroller_state(uint16_t sensor_node)
{
	return (qtm_scroller_control1.qtm_scroller_data[sensor_node].scroller_status);
}

uint16_t get_scroller_position(uint16_t sensor_node)
{
	return (qtm_scroller_control1.qtm_scroller_data[sensor_node].position);
}



/*============================================================================
void PTC_Handler_EOC(void)
------------------------------------------------------------------------------
Purpose: Interrupt service handler for PTC EOC interrupt
Input  : none
Output : none
Notes  : none
============================================================================*/
void PTC_Handler(void)
{
	qtm_ptc_clear_interrupt();
	qtm_saml10_ptc_handler_eoc();
}
