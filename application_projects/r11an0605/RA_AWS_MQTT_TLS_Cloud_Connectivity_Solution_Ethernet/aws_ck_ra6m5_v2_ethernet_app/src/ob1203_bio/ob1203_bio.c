/***********************************************************************************************************************
 * File Name    : ob1203_bio.c
 * Description  : Contains macros, data structures and functions for OB1203
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#include "ob1203_bio.h"
#include "hal_data.h"
#include "common_utils.h"

#define OB1203_BIO_TIMEOUT (10000) // 10s
#define OB1203_BIO_MAX_COUNTS (10)
#define OB1203_BIO_IR_LED_MAX_CURRENT (0x03FF)
#define OB1203_BIO_RED_LED_MAX_CURRENT (0x01FF)
#define OB1203_BIO_DEFAULT_TARGET_COUNTS (0x30000)
#define OB1203_BIO_PPG_RED_LED_DATA_THRESHOLD (8000)
#define OB1203_BIO_DEFAULT_WARM_UP_COUNTS (10)
#define OB1203_BIO_MAX_NUMBER_OF_SAMPLES_20 (20)
#define OB1203_BIO_OLDEST_IR_LED_POS (0)
#define OB1203_BIO_OLDEST_RED_LED_POS (1)
#define OB1203_BIO_MAX_TARGET_COUNTS (0x38000UL) // 220k
#define OB1203_BIO_MIN_TARGET_COUNTS (0xC000UL)  // 48k
#define OB1203_BIO_TOLERANCE_DEFAULT (6000)
#define OB1203_BIO_TOLERANCE_LOWEST_RANGE (0x5800UL)
#define OB1203_BIO_STEP (8)
#define OB1203_BIO_TOLERANCE_PERSIST (0x7530) // 30k counts ( < +/-2^15) -- so we can represent values as 18 bit data minus setpoint in int16_t
#define OB1203_BIO_IN_RANGE_PERSIST (4)
#define OB1203_BIO_SPO2_MAX_VALUE   (100)
#define OB1203_BIO_LOW_SIGNAL_THRESHOLD (0x4000) // Set to MIN_TARGET-2^16. Minimum biosensor ADC count. Below this sensor exits BIO mode and reverts to proximity sensor "wait for finger" mode
#define OB1203_BIO_DEFAULT_MAX_LOW_SAMPLES (20)
#define OB1203_BIO_PROX_PERSIST (0x02)

typedef enum e_ob1203_bio_callback_status
{
    OB1203_BIO_CALLBACK_STATUS_WAIT = (0),
    OB1203_BIO_CALLBACK_STATUS_SUCCESS,
    OB1203_BIO_CALLBACK_STATUS_REPEAT,
} ob1203_bio_callback_status_t;

static bool ob1203_bio_each_led_gain_current_control(ob1203_bio_t *const p_bio,
                                                     rm_ob1203_ppg_prox_gain_t *const p_gain,
                                                     uint16_t *const p_current,
                                                     uint32_t const ppg_data,
                                                     ob1203_led_target_counts_t *const p_target_counts,
                                                     ob1203_bio_led_type_t const led_type,
                                                     bool *const p_update);
static void ob1203_bio_i2c_callback_flag_clear(void);
static bool ob1203_bio_i2c_communication_success(void);
static void ob1203_bio_i2c_callback_wait(uint32_t delay_ms);
#if OB1203_BIO_IRQ_ENABLE
static void ob1203_bio_irq_callback_flag_clear(void);
static void ob1203_bio_irq_callback_wait(void);
#endif
static volatile ob1203_bio_callback_status_t gs_i2c_callback_status = OB1203_BIO_CALLBACK_STATUS_WAIT;
#if OB1203_BIO_IRQ_ENABLE
static volatile ob1203_bio_callback_status_t gs_irq_callback_status = OB1203_BIO_CALLBACK_STATUS_WAIT;
#endif
static const uint16_t gs_max_currents[OB1203_BIO_LED_MAX_NUM] =
{
        OB1203_BIO_IR_LED_MAX_CURRENT,
        OB1203_BIO_RED_LED_MAX_CURRENT,
};

/*******************************************************************************************************************//**
 * @brief               I2C bus of OB1203 sensor callback
 * @param[in]   p_args  OB1203 Callback function
 * @retval              None
 ***********************************************************************************************************************/
void ob1203_comms_i2c_callback(rm_ob1203_callback_args_t *p_args)
{
    if (RM_OB1203_EVENT_SUCCESS == p_args->event)
    {
        gs_i2c_callback_status = OB1203_BIO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        gs_i2c_callback_status = OB1203_BIO_CALLBACK_STATUS_REPEAT;
    }
}

/*******************************************************************************************************************//**
 * @brief               OB1203 sensor callback
 * @param[in]   p_args  OB1203 Callback function
 * @retval              None
 ***********************************************************************************************************************/
void ob1203_irq_callback(rm_ob1203_callback_args_t *p_args)
{
#if OB1203_BIO_IRQ_ENABLE
    FSP_PARAMETER_NOT_USED(p_args);

    gs_irq_callback_status = OB1203_BIO_CALLBACK_STATUS_SUCCESS;
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}

/*******************************************************************************************************************//**
 * Open OB1203 sensor.
 *
 * @param      p_bio                  Ob1203_bio structure
 * @param[in]  p_prox_instance        Instance of prox mode
 * @param[in]  p_ppg_instance         Instance of ppg mode
 * @param[in]  p_spo2                 SPO2 structure
 * @retval     True                   Upon successful open OB1203 sensor
 * @retval     False                  Upon unsuccessful open OB1203 sensors
 **********************************************************************************************************************/
bool ob1203_bio_open(ob1203_bio_t *const p_bio,
                     rm_ob1203_instance_t *const p_prox_instance,
                     rm_ob1203_instance_t *const p_ppg_instance,
                     spo2_t *const p_spo2)
{
    /* Clear callback flags */
    ob1203_bio_i2c_callback_flag_clear();
#if OB1203_BIO_IRQ_ENABLE
    ob1203_bio_irq_callback_flag_clear();
#endif

    /* Set parameters */
    p_bio->p_prox_instance = p_prox_instance;
    p_bio->p_ppg_instance = p_ppg_instance;
    p_bio->mode = RM_OB1203_OPERATION_MODE_PROXIMITY;
    p_bio->mode_change = false;
    p_bio->low_signal_counts = OB1203_BIO_DEFAULT_MAX_LOW_SAMPLES;
    p_bio->p_spo2 = p_spo2;
    p_bio->data_check.threshold = OB1203_BIO_PPG_RED_LED_DATA_THRESHOLD;
    p_bio->data_check.warm_up = OB1203_BIO_DEFAULT_WARM_UP_COUNTS;
    p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].target_counts = OB1203_BIO_DEFAULT_TARGET_COUNTS;
    p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].target_counts = OB1203_BIO_DEFAULT_TARGET_COUNTS;
    p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].in_range = false;
    p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].in_range = false;
    p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].in_range_persist = 0;
    p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].in_range_persist = 0;
    p_bio->interrupt_cfg.prox_type = RM_OB1203_PROX_INTERRUPT_TYPE_NORMAL;
    p_bio->interrupt_cfg.persist = OB1203_BIO_PROX_PERSIST;
    p_bio->interrupt_cfg.ppg_type = RM_OB1203_PPG_INTERRUPT_TYPE_DATA;
    p_bio->prev_in_range = false;
    p_bio->target_counts_change = true;
    p_bio->algorithm_ready = false;

    /* Set the first instance to run */
    switch (p_bio->mode)
    {
        case RM_OB1203_OPERATION_MODE_PROXIMITY :
        {
            p_bio->p_instance = p_bio->p_prox_instance;
        }
        break;

        case RM_OB1203_OPERATION_MODE_PPG :
        {
            p_bio->p_instance = p_bio->p_ppg_instance;
        }
        break;

        default :
            return false;
    }

    /* Initialize the algorithm */
    spo2_init(p_bio->p_spo2);

    return true;
}

/*******************************************************************************************************************//**
 * Init OB1203 sensor in bio mode.
 *
 * @param      p_bio                  Ob1203_bio structure
 * @retval     True                   Upon successful Init OB1203 sensor's operation mode.
 * @retval     False                  Upon unsuccessful Init OB1203 sensor's operation mode.
 **********************************************************************************************************************/
bool ob1203_bio_operation_mode_init(ob1203_bio_t *const p_bio)
{
    fsp_err_t err;
    uint8_t counter;

    if (false != p_bio->mode_change)
    {
        /* Clear the flag */
        p_bio->mode_change = false;

        /* Close a previous instance */
        RM_OB1203_Close(p_bio->p_instance->p_ctrl);
    }

    /* Set the instance to run */
    switch (p_bio->mode)
    {
        case RM_OB1203_OPERATION_MODE_PROXIMITY :
        {
            p_bio->p_instance = p_bio->p_prox_instance;
        }
        break;

        case RM_OB1203_OPERATION_MODE_PPG :
        {
            p_bio->p_instance = p_bio->p_ppg_instance;
        }
        break;

        default :
            return false;
    }

    /* Open an instance */
    counter = OB1203_BIO_MAX_COUNTS;
    do
    {
        err = RM_OB1203_Open(p_bio->p_instance->p_ctrl, p_bio->p_instance->p_cfg);
        if (FSP_SUCCESS != err)
        {
        	APP_PRINT ("\r\nOB1203 sensor open fail \r\n");
            RM_OB1203_Close(p_bio->p_instance->p_ctrl);
        }
        /* Decrement */
        counter--;
    }
    while((FSP_SUCCESS != err) && (0 != counter));

    if (0 == counter)
    {
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Start measurement for OB1203 sensor in bio mode.
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @retval     True                   Upon successful start measuring with OB1203 sensor
 * @retval     False                  Upon unsuccessful start measuring withOB1203 sensor
 **********************************************************************************************************************/
bool ob1203_bio_measurement_start(ob1203_bio_t *const p_bio)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t counter;

#if OB1203_BIO_IRQ_ENABLE
    ob1203_bio_irq_callback_flag_clear();

    /* Set device interrupt configurations */
    counter = OB1203_BIO_MAX_COUNTS;
    do
    {
        ob1203_bio_i2c_callback_flag_clear();
        err = RM_OB1203_DeviceInterruptCfgSet(p_bio->p_instance->p_ctrl, p_bio->interrupt_cfg);
        if (FSP_SUCCESS != err)
        {
            return false;
        }
        ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

        /* Decrement */
        counter--;
    } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

    if (0 == counter)
    {
        return false;
    }
#endif

    /* Start a measurement */
    counter = OB1203_BIO_MAX_COUNTS;
    do
    {
        ob1203_bio_i2c_callback_flag_clear();
        err = RM_OB1203_MeasurementStart(p_bio->p_instance->p_ctrl);
        if (FSP_SUCCESS != err)
        {
            return false;
        }
        ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

        /* Decrement */
        counter--;
    } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

    if (0 == counter)
    {
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Stop measurement for OB1203 sensor in bio mode.
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @retval     True                   Upon successful stop measuring OB1203 sensor
 * @retval     False                  Upon unsuccessful stop measuring OB1203 sensor
 **********************************************************************************************************************/
bool ob1203_bio_measurement_stop(ob1203_bio_t *const p_bio)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t counter;

    /* Stop a measurement */
    counter = OB1203_BIO_MAX_COUNTS;
    do
    {
        ob1203_bio_i2c_callback_flag_clear();
        RM_OB1203_MeasurementStop(p_bio->p_instance->p_ctrl);
        if (FSP_SUCCESS != err)
        {
            return false;
        }
        ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

        /* Decrement */
        counter--;
    } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

    if (0 == counter)
    {
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Wait for measurement periodically for OB1203 sensor.
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @retval     True                   Upon successful waiting OB1203 sensor
 * @retval     False                  Upon unsuccessful waiting OB1203 sensor
 **********************************************************************************************************************/
bool ob1203_bio_measurement_period_wait(ob1203_bio_t *const p_bio)
{
#if OB1203_BIO_IRQ_ENABLE
    FSP_PARAMETER_NOT_USED(p_bio);
#else
    fsp_err_t err = FSP_SUCCESS;
    uint8_t counter;
    rm_ob1203_device_status_t device_status;
    bool * p_flag;
#endif

#if OB1203_BIO_IRQ_ENABLE

    /* Wait IRQ callback */
    ob1203_bio_irq_callback_wait();

    /* Clear the IRQ callback flag */
    ob1203_bio_irq_callback_flag_clear();
#else
    /* Wait until the measurement is completed */
    switch (p_bio->mode)
    {
        case RM_OB1203_OPERATION_MODE_PROXIMITY :
        {
            p_flag = &(device_status.prox_interrupt_occur);
        }
        break;

        case RM_OB1203_OPERATION_MODE_PPG :
        {
            if (RM_OB1203_PPG_INTERRUPT_TYPE_FIFO_AFULL == p_bio->interrupt_cfg.ppg_type)
            {
                p_flag = &(device_status.fifo_afull_interrupt_occur);
            }
            else
            {
                p_flag = &(device_status.ppg_measurement_complete);
            }
        }
        break;

        default :
            return false;
    }

    do
    {
        /* Get Device status */
        counter = OB1203_BIO_MAX_COUNTS;
        do
        {
            ob1203_bio_i2c_callback_flag_clear();
            err = RM_OB1203_DeviceStatusGet(p_bio->p_instance->p_ctrl, &device_status);
            if (FSP_SUCCESS != err)
            {
                return false;
            }
            ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

            /* Decrement */
            counter--;
        } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

        if (0 == counter)
        {
            return false;
        }
    }
    while(false == *p_flag);
#endif

    return true;
}

/*******************************************************************************************************************//**
 * Read raw data of OB1203 sensor.
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param      p_raw_data             OB1203 raw data structure
 * @retval     True                   Upon successful reading of raw data with OB1203 sensor
 * @retval     False                  Upon unsuccessful reading of raw data with OB1203 sensor
 **********************************************************************************************************************/
bool ob1203_bio_ppg_raw_data_read(ob1203_bio_t *const p_bio, rm_ob1203_raw_data_t *const p_raw_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t counter;
    rm_ob1203_fifo_info_t fifo_info;

    if (RM_OB1203_PPG_INTERRUPT_TYPE_FIFO_AFULL == p_bio->interrupt_cfg.ppg_type)
    {
        /* Get FIFO info */
        counter = OB1203_BIO_MAX_COUNTS;
        do
        {
            ob1203_bio_i2c_callback_flag_clear();
            err = RM_OB1203_FifoInfoGet(p_bio->p_instance->p_ctrl, &fifo_info);
            if (FSP_SUCCESS != err)
            {
                return false;
            }
            ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

            /* Decrement */
            counter--;
        } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

        if (0 == counter)
        {
            return false;
        }

        /* Set overflow counter */
        p_bio->overflow = fifo_info.overflow_counter;

        p_bio->number_of_samples = OB1203_BIO_MAX_NUMBER_OF_SAMPLES_20;
    }
    else
    {
        /* Set overflow counter */
        p_bio->overflow = 0;

        /* Set number of samples */
        p_bio->number_of_samples = 2;
    }

    if (0 != p_bio->number_of_samples)
    {
        /* Read raw data */
        counter = OB1203_BIO_MAX_COUNTS;
        do
        {
            ob1203_bio_i2c_callback_flag_clear();
            err = RM_OB1203_PpgRead(p_bio->p_instance->p_ctrl, p_raw_data, p_bio->number_of_samples);
            if (FSP_SUCCESS != err)
            {
                return false;
            }
            ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

            /* Decrement */
            counter--;
        } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

        if (0 == counter)
        {
            return false;
        }
    }

    return true;
}

/*******************************************************************************************************************//**
 * Calculate perfusion index with OB1203 sensor.
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param[in]  p_raw_data             OB1203 raw data structure
 * @param      p_ppg_data             OB1203 PPG data structure
 * @param      p_valid                Check OB1203 sensor data valid
 * @retval     True                   Upon successful perfusion index calculation with OB1203 sensor
 * @retval     False                  Upon unsuccessful perfusion index calculation with OB1203 sensor
 **********************************************************************************************************************/
bool ob1203_bio_ppg_data_calculate(ob1203_bio_t *const p_bio,
                                   rm_ob1203_raw_data_t *const p_raw_data,
                                   rm_ob1203_ppg_data_t *const p_ppg_data,
                                   bool *const p_valid)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Reset flag */
    *p_valid = false;

    if (0 != p_bio->number_of_samples)
    {
        /* Calculate PPG data from raw data */
        err = RM_OB1203_PpgDataCalculate(p_bio->p_instance->p_ctrl, p_raw_data, p_ppg_data);
        if (FSP_SUCCESS != err)
        {
            return false;
        }

        /* Set IR LED data for checking perfusion index (PI) */
        p_bio->ppg_data_for_pi = p_ppg_data->ppg_data[OB1203_BIO_OLDEST_IR_LED_POS];

        /* Check if the PPG value are above the threshold */
        if (p_bio->data_check.threshold <= p_ppg_data->ppg_data[OB1203_BIO_OLDEST_RED_LED_POS])
        {
            /* Check if warm up is completed */
            if (0 == p_bio->data_check.warm_up)
            {
                *p_valid = true;
            }
            else
            {
                p_bio->data_check.warm_up--;
            }
        }
        else
        {
            p_bio->data_check.warm_up = OB1203_BIO_DEFAULT_WARM_UP_COUNTS;
        }
    }

    /* Check if low signal */
    if (OB1203_BIO_LOW_SIGNAL_THRESHOLD > p_ppg_data->ppg_data[p_bio->number_of_samples - 2])
    {
        p_bio->low_signal_counts--;
        if (0 == p_bio->low_signal_counts)
        {
            p_bio->low_signal_counts = OB1203_BIO_DEFAULT_MAX_LOW_SAMPLES;
            p_bio->mode_change = true;
            *p_valid = false;
        }
    }
    else
    {
        p_bio->low_signal_counts = OB1203_BIO_DEFAULT_MAX_LOW_SAMPLES;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Auto gain and currents control for OB1203 sensor
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param[in]  p_ppg_data             OB1203 PPG data structure
 * @param      p_gain_currents        ob1203_bio_gain_currents structure
 * @param      p_update               Flag to check the update
 * @retval     True                   Auto gain and currents control successfully
 * @retval     False                  Auto gain and currents control unsuccessfully
 **********************************************************************************************************************/
bool ob1203_bio_auto_gain_currents_control(ob1203_bio_t *const p_bio,
                                           rm_ob1203_ppg_data_t *const p_ppg_data,
                                           ob1203_bio_gain_currents_t *const p_gain_currents,
                                           bool *const p_update)
{
    bool result = true;
    bool ir_update = false;
    bool red_update = false;

    /* Reset flag */
    *p_update = false;

    /* Check if samples have one pair or more */
    if (2 <= p_bio->number_of_samples)
    {
        /* IR LED */
        result = ob1203_bio_each_led_gain_current_control(p_bio,
                                                          NULL,
                                                          &p_gain_currents->currents.ir_led,
                                                          p_ppg_data->ppg_data[p_bio->number_of_samples - 2],
                                                          &p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR],
                                                          OB1203_BIO_LED_TYPE_IR,
                                                          &ir_update);
        if (false == result)
        {
            return false;
        }

        /* Red LED */
        result = ob1203_bio_each_led_gain_current_control(p_bio,
                                                          &p_gain_currents->gain.ppg_prox,
                                                          &p_gain_currents->currents.red_led,
                                                          p_ppg_data->ppg_data[p_bio->number_of_samples - 1],
                                                          &p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED],
                                                          OB1203_BIO_LED_TYPE_RED,
                                                          &red_update);
        if (false == result)
        {
            return false;
        }

        if ((false != ir_update) || (false != red_update))
        {
            /* Set the flag */
            *p_update = true;
        }
    }
    else
    {
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Control led gain current of OB1203 sensor
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param[in]  p_ppg_data             OB1203 PPG data structure
 * @param[in]  led_type
 * @param      p_gain                 PPG and proximity gain range        
 * @param      p_current   
 * @param      p_target_counts           
 * @param      p_update               Flag to check the update
 * @retval     True                   Control led gain current of OB1203 sensor successfully
 **********************************************************************************************************************/
static bool ob1203_bio_each_led_gain_current_control(ob1203_bio_t *const p_bio,
                                                     rm_ob1203_ppg_prox_gain_t *const p_gain,
                                                     uint16_t *const p_current,
                                                     uint32_t const ppg_data,
                                                     ob1203_led_target_counts_t *const p_target_counts,
                                                     ob1203_bio_led_type_t const led_type,
                                                     bool *const p_update)
{
    uint32_t tolerance = OB1203_BIO_TOLERANCE_DEFAULT;
    uint32_t tolerance_tmp;
    uint32_t step_change; // How much the counts will change with one step of the current
    uint32_t tmp_u32;
    uint16_t tmp_u16;

    /* Clear flag */
    *p_update = false;

    /* Set tolerance */
    if (OB1203_BIO_MIN_TARGET_COUNTS == p_target_counts->target_counts)
    {
        /* Allow the signal to be +/=22k from from target counts in the lowest range */
        tolerance = OB1203_BIO_TOLERANCE_LOWEST_RANGE;
    }
    else
    {
        /* Set the tolerance to be the step size at the current target counts */
        step_change = (uint32_t)(OB1203_BIO_STEP * p_target_counts->target_counts) / (uint32_t)*p_current;

        if (OB1203_BIO_TOLERANCE_DEFAULT > step_change)
        {
            tolerance = OB1203_BIO_TOLERANCE_DEFAULT;
        }
        else if (OB1203_BIO_TOLERANCE_LOWEST_RANGE < step_change)
        {
            tolerance = OB1203_BIO_TOLERANCE_LOWEST_RANGE; // Constrain
        }
        else
        {
            tolerance = step_change;
        }
    }

    /* When target counts persist in range */
    if (OB1203_BIO_IN_RANGE_PERSIST < p_target_counts->in_range_persist)
    {
        tolerance_tmp = OB1203_BIO_TOLERANCE_PERSIST;
    }
    else
    {
        tolerance_tmp = tolerance;
    }

    if (ppg_data < (p_target_counts->target_counts - tolerance_tmp)) // Too low case
    {
        if (OB1203_BIO_TOLERANCE_PERSIST == tolerance_tmp)
        {
            /* Out of range */
            p_target_counts->in_range_persist = 0;
        }

        if (gs_max_currents[led_type] > *p_current + OB1203_BIO_STEP)
        {
            *p_current += OB1203_BIO_STEP;
            *p_update = true;
        }
        else // Current is close to max.
        {
            tmp_u32 = (p_target_counts->target_counts * 9) / 10;
            if (OB1203_BIO_MIN_TARGET_COUNTS < tmp_u32)
            {
                /* Decrease target counts */
                p_target_counts->target_counts = tmp_u32;
                p_bio->target_counts_change = true;
            }
            else if (OB1203_BIO_MIN_TARGET_COUNTS == p_target_counts->target_counts)
            {
                /* Increase gain */
                if ((NULL != p_gain) && (OB1203_BIO_LED_TYPE_RED == led_type))
                {
                    switch (*p_gain)
                    {
                    case RM_OB1203_PPG_PROX_GAIN_1:
                    {
                        *p_gain = RM_OB1203_PPG_PROX_GAIN_1P5;
                        *p_update = true;
                    }
                    break;

                    case RM_OB1203_PPG_PROX_GAIN_1P5:
                    {
                        *p_gain = RM_OB1203_PPG_PROX_GAIN_2;
                        *p_update = true;
                    }
                    break;

                    case RM_OB1203_PPG_PROX_GAIN_2:
                    {
                        *p_gain = RM_OB1203_PPG_PROX_GAIN_4;
                        *p_update = true;
                    }
                    break;

                    case RM_OB1203_PPG_PROX_GAIN_4:
                    default:
                    {
                    }
                    break;
                    }
                }
            }
            else
            {
                p_target_counts->target_counts = OB1203_BIO_MIN_TARGET_COUNTS;
                p_bio->target_counts_change = true;
            }

            p_target_counts->in_range_persist = 0;
        }
    }
    else if (ppg_data > (p_target_counts->target_counts + tolerance_tmp)) // Too high case
    {
        if (OB1203_BIO_TOLERANCE_PERSIST == tolerance_tmp)
        {
            /* Out of range */
            p_target_counts->in_range_persist = 0;
        }

        if ((NULL != p_gain) && (OB1203_BIO_LED_TYPE_RED == led_type))
        {
            /* Decrease gain */
            switch (*p_gain)
            {
            case RM_OB1203_PPG_PROX_GAIN_1P5:
            {
                tmp_u32 = (OB1203_BIO_MIN_TARGET_COUNTS * 15) / 8;
                if (ppg_data > tmp_u32)
                {
                    *p_gain = RM_OB1203_PPG_PROX_GAIN_1;
                    *p_update = true;
                    p_target_counts->in_range_persist = 0;
                }
            }
            break;

            case RM_OB1203_PPG_PROX_GAIN_2:
            {
                tmp_u32 = (OB1203_BIO_MIN_TARGET_COUNTS * 3) / 2;
                if (ppg_data > tmp_u32)
                {
                    *p_gain = RM_OB1203_PPG_PROX_GAIN_1P5;
                    *p_update = true;
                    p_target_counts->in_range_persist = 0;
                }
            }
            break;

            case RM_OB1203_PPG_PROX_GAIN_4:
            {
                tmp_u32 = (OB1203_BIO_MIN_TARGET_COUNTS * 5) / 2;
                if (ppg_data > tmp_u32)
                {
                    *p_gain = RM_OB1203_PPG_PROX_GAIN_1P5;
                    *p_update = true;
                    p_target_counts->in_range_persist = 0;
                }
            }
            break;

            case RM_OB1203_PPG_PROX_GAIN_1:
            default:
            {
            }
            break;
            }

            if (false == *p_update) // Gain cannot be changed
            {
                /* Change target counts */
                tmp_u16 = (uint16_t)((*p_current * 9) / 8);
                tmp_u32 = (p_target_counts->target_counts * 10) / 9;
                if ((gs_max_currents[led_type] > tmp_u16) && (OB1203_BIO_MAX_TARGET_COUNTS > tmp_u32))
                {
                    p_target_counts->target_counts = tmp_u32;
                    p_bio->target_counts_change = true;
                }
                else
                {
                    /* Decrease current value */
                    if ((2 * OB1203_BIO_STEP) < *p_current)
                    {
                        *p_current -= OB1203_BIO_STEP;
                        *p_update = true;
                    }
                }
            }
        }
        else
        {
            /* Increase target counts */
            tmp_u16 = (uint16_t)((*p_current * 9) / 8);
            tmp_u32 = (p_target_counts->target_counts * 10) / 9;
            if ((gs_max_currents[led_type] > tmp_u16) && (OB1203_BIO_MAX_TARGET_COUNTS > tmp_u32))
            {
                p_target_counts->target_counts = tmp_u32;
                p_bio->target_counts_change = true;
            }
            else
            {
                /* Decrease current value */
                if ((2 * OB1203_BIO_STEP) < *p_current)
                {
                    *p_current -= OB1203_BIO_STEP;
                    *p_update = true;
                }
            }
        }
    }
    else // Just right
    {
        if ((ppg_data > (p_target_counts->target_counts - tolerance)) &&
            (ppg_data < (p_target_counts->target_counts + tolerance)))
        {
            if (OB1203_BIO_IN_RANGE_PERSIST >= p_target_counts->in_range_persist)
            {
                p_target_counts->in_range_persist++;
            }
        }
    }

    /* Check if target counts is in range */
    if (OB1203_BIO_IN_RANGE_PERSIST < p_target_counts->in_range_persist)
    {
        p_target_counts->in_range = true;
    }
    else
    {
        p_target_counts->in_range = false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Reconfigure gain currents
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param      p_gain_currents        ob1203_bio_gain_currents structure
 * @retval     True                   Reconfigure gain currents successfully
 * @retval     False                  Reconfigure gain currents unsuccessfully
 **********************************************************************************************************************/
bool ob1203_bio_gain_currents_reconfigure(ob1203_bio_t *const p_bio, ob1203_bio_gain_currents_t *const p_gain_currents)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t counter;

    /* Set Gain */
    counter = OB1203_BIO_MAX_COUNTS;
    do
    {
        ob1203_bio_i2c_callback_flag_clear();
        err = RM_OB1203_GainSet(p_bio->p_instance->p_ctrl, p_gain_currents->gain);
        if (FSP_SUCCESS != err)
        {
            return false;
        }
        ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

        /* Decrement */
        counter--;
    } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

    if (0 == counter)
    {
        return false;
    }

    /* Set Currents */
    counter = OB1203_BIO_MAX_COUNTS;
    do
    {
        ob1203_bio_i2c_callback_flag_clear();
        err = RM_OB1203_LedCurrentSet(p_bio->p_instance->p_ctrl, p_gain_currents->currents);
        if (FSP_SUCCESS != err)
        {
            return false;
        }
        ob1203_bio_i2c_callback_wait(OB1203_BIO_TIMEOUT);

        /* Decrement */
        counter--;
    } while ((false == ob1203_bio_i2c_communication_success()) && (0 != counter));

    if (0 == counter)
    {
        return false;
    }

    /* Set Device interrupt configurations */
    p_bio->interrupt_cfg.ppg_type = RM_OB1203_PPG_INTERRUPT_TYPE_DATA;

    return true;
}

/*******************************************************************************************************************//**
 * Check status of the preparation for the algorithm
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param      p_ready                status of the preparation for the algorithm
 * @retval     True                   Check perfusion index successfully
 **********************************************************************************************************************/
bool ob1203_bio_algorithm_preparation_check(ob1203_bio_t *const p_bio, bool *const p_ready)
{
    /* Clear flag */
    *p_ready = false;

    /* Check if previous counts were in range */
    if (false == p_bio->prev_in_range)
    {
        /* Check if both counts are in range */
        if ((false != p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].in_range) &&
            (false != p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].in_range))
        {
            /* Check device interrupt type */
            if (RM_OB1203_PPG_INTERRUPT_TYPE_DATA == p_bio->interrupt_cfg.ppg_type)
            {
                /* Set Device interrupt configurations */
                p_bio->interrupt_cfg.ppg_type = RM_OB1203_PPG_INTERRUPT_TYPE_FIFO_AFULL;
            }
            else
            {
                /* Check if target counts change */
                if (false != p_bio->target_counts_change)
                {
                    p_bio->target_counts_change = false;
                }
                else
                {
                    p_bio->prev_in_range = true;
                    *p_ready = true;
                }
            }
        }
    }
    else
    {
        if ((false == p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].in_range) ||
            (false == p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].in_range))
        {
            /* Set Device interrupt configurations */
            p_bio->interrupt_cfg.ppg_type = RM_OB1203_PPG_INTERRUPT_TYPE_DATA;
            p_bio->prev_in_range = false;
        }
        else
        {
            *p_ready = true;
        }
    }

    return true;
}

/*******************************************************************************************************************//**
 * Check if an operation mode needs to be changed
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param      p_change               Assign to true if the operation mode needs to be changed
 * @retval     True                   Check successfully
 * @retval     False                  Check unsuccessfully
 **********************************************************************************************************************/
bool ob1203_bio_mode_change_check(ob1203_bio_t *const p_bio, bool *const p_change)
{
    /* Clear flag */
    *p_change = false;

    switch (p_bio->mode)
    {
        case RM_OB1203_OPERATION_MODE_PROXIMITY :
        {
            p_bio->mode = RM_OB1203_OPERATION_MODE_PPG;
            p_bio->interrupt_cfg.ppg_type = RM_OB1203_PPG_INTERRUPT_TYPE_DATA;
            p_bio->mode_change = true;
            *p_change = true;
        }
        break;

        case RM_OB1203_OPERATION_MODE_PPG :
        {
            if (false != p_bio->mode_change)
            {
                p_bio->mode = RM_OB1203_OPERATION_MODE_PROXIMITY;
                p_bio->interrupt_cfg.prox_type = RM_OB1203_PROX_INTERRUPT_TYPE_NORMAL;
                p_bio->interrupt_cfg.persist = OB1203_BIO_PROX_PERSIST;
                *p_change = true;
            }
        }
        break;

        default :
            return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * Add ppg samples
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param[in]  p_ppg_data             OB1203 PPG data structure
 * @retval     True                   Add ppg samples successfully
 **********************************************************************************************************************/
bool ob1203_bio_samples_add(ob1203_bio_t *const p_bio, rm_ob1203_ppg_data_t *const p_ppg_data)
{
    spo2_target_counts_t target_counts;
    uint32_t i;
    uint32_t length;

    /* Set target counts */
    target_counts.ir_led  = p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].target_counts;
    target_counts.red_led = p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].target_counts;

    /* Add oldest samples to deal with missing (overwritten) samples */
    length = (uint32_t)(p_bio->overflow >> 1);
    for (i = 0; i < length; i++)
    {
        add_sample(p_bio->p_spo2,
                   target_counts,
                   p_ppg_data->ppg_data[OB1203_BIO_OLDEST_IR_LED_POS],
                   p_ppg_data->ppg_data[OB1203_BIO_OLDEST_RED_LED_POS]);

        /* Increment missed counts */
        missed_sample_counts_increment(p_bio->p_spo2);
    }

    /* Add samples */
    length = (uint32_t)(p_bio->number_of_samples >> 1);
    for (i = 0; i < length; i++)
    {
        add_sample(p_bio->p_spo2,
                   target_counts,
                   p_ppg_data->ppg_data[2 * i],
                   p_ppg_data->ppg_data[2 * i + 1]);

        /* Increment read counts */
        read_sample_counts_increment(p_bio->p_spo2);
    }

    /* Set a flag for the algorithm */
    p_bio->algorithm_ready = true;

    return true;
}

/*******************************************************************************************************************//**
 * Clear ppg samples
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @retval     True                   Clear ppg samples successfully
 **********************************************************************************************************************/
bool ob1203_bio_samples_clear(ob1203_bio_t *const p_bio)
{
    /* Clear counts of samples */
    counts_of_samples_clear(p_bio->p_spo2);

    /* Clear a flag for the algorithm */
    p_bio->algorithm_ready = false;

    return true;
}

/*******************************************************************************************************************//**
 * Reset the algorithm
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @retval     True                   Reset the algorithm successfully
 **********************************************************************************************************************/
bool ob1203_bio_algorithm_reset(ob1203_bio_t *const p_bio)
{
    /* Reset the algorithm */
    spo2_reset(p_bio->p_spo2);

    return true;
}

/*******************************************************************************************************************//**
 * Calculate heart rate and SpO2 values
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param[in]  p_bio_data             Ob1203_bio_data structure
 * @retval     True                   Calculate heart rate and SpO2 values successfully
 * @retval     False                  Calculate heart rate and SpO2 values unsuccessfully
 **********************************************************************************************************************/
bool ob1203_bio_hr_spo2_calculate(ob1203_bio_t *const p_bio, ob1203_bio_data_t *const p_bio_data)
{
    bool result = true;
    spo2_target_counts_t target_counts;

    if (false != p_bio->algorithm_ready)
    {
        /* Set target counts */
        target_counts.ir_led  = p_bio->led_target_counts[OB1203_BIO_LED_TYPE_IR].target_counts;
        target_counts.red_led = p_bio->led_target_counts[OB1203_BIO_LED_TYPE_RED].target_counts;

        /* Calculate heart rate and SpO2 by the algorithm */
        hr_and_spo2_calculate(p_bio->p_spo2,
                              &p_bio_data->heart_rate,
                              &p_bio_data->spo2,
                              &p_bio_data->perfusion_index,
                              target_counts);
        if (false == result)
        {
            return false;
        }
    }

    return true;
}

/*******************************************************************************************************************//**
 * Calculate a respiration rate value
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param[in]  p_bio_data             Ob1203_bio_data structure
 * @retval     True                   Calculate heart rate and SpO2 values successfully
 **********************************************************************************************************************/
bool ob1203_bio_rr_calculate(ob1203_bio_t *const p_bio, ob1203_bio_data_t *const p_bio_data)
{
    if (false != p_bio->algorithm_ready)
    {
        /* Calculate respiration rate by the algorithm */
        rr_calculate(p_bio->p_spo2, &p_bio_data->respiration_rate);
    }

    return true;
}

/*******************************************************************************************************************//**
 * Check perfusion index
 *
 * @param[in]  p_bio                  Ob1203_bio structure
 * @param      p_valid                Check OB1203 sensor data valid
 * @retval     True                   Check perfusion index successfully
 **********************************************************************************************************************/
bool ob1203_bio_perfusion_index_check(ob1203_bio_t *const p_bio, bool * const p_valid)
{
    if (false != p_bio->algorithm_ready)
    {
        /* Check perfusion index (PI) */
        check_pi(p_bio->p_spo2, p_bio->ppg_data_for_pi, p_valid);
    }

    return true;
}

/*******************************************************************************************************************//**
 * Clear i2c callback flags
 *
 * @param      None
 * @retval     None
 **********************************************************************************************************************/
static void ob1203_bio_i2c_callback_flag_clear(void)
{
    /* Clear status */
    gs_i2c_callback_status = OB1203_BIO_CALLBACK_STATUS_WAIT;
}


/*******************************************************************************************************************//**
 * Check I2C communication of OB1203 sensor
 *
 * @param      None
 * @retval     True         I2C Communication successfully
 * @retval     False        I2C Communication unsuccessfully
 **********************************************************************************************************************/
static bool ob1203_bio_i2c_communication_success(void)
{
    if (OB1203_BIO_CALLBACK_STATUS_SUCCESS == gs_i2c_callback_status)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************************************************//**
 * ob1203_bio_i2c_callback_wait function
 *
 * @param[in]  timeout_ms   
 * @retval     None
 **********************************************************************************************************************/
static void ob1203_bio_i2c_callback_wait(uint32_t const timeout_ms)
{
    uint32_t counter = timeout_ms;

    while (OB1203_BIO_CALLBACK_STATUS_WAIT == gs_i2c_callback_status)
    {
        if (0 == counter)
        {
            break;
        }

        /* Decrement */
        counter--;
    }
}

#if OB1203_BIO_IRQ_ENABLE
/*******************************************************************************************************************//**
 * Clear the IRQ callback flag
 *
 * @param      None 
 * @retval     None
 **********************************************************************************************************************/
static void ob1203_bio_irq_callback_flag_clear(void)
{
    /* Clear status */
    gs_irq_callback_status = OB1203_BIO_CALLBACK_STATUS_WAIT;
}

/*******************************************************************************************************************//**
 * ob1203_bio_irq_callback_wait function
 *
 * @param      None 
 * @retval     None
 **********************************************************************************************************************/
static void ob1203_bio_irq_callback_wait()
{
    while (OB1203_BIO_CALLBACK_STATUS_WAIT == gs_irq_callback_status)
    {
        vTaskDelay(1);
    }
}
#endif
