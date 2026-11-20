/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Contains data structures and functions used in the common_init.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "common_init.h"
#include "usb_pcdc_board_cfg.h"

/* Global variable */
volatile uint32_t g_curr_led_freq = BLINK_FREQ_1HZ;

volatile static uint32_t s_cur_dc = 0;
volatile static uint32_t s_cur_rate = 0;
static const uint32_t s_pwm_dcs[3] = {LED_INTENSITY_90, LED_INTENSITY_50, LED_INTENSITY_10};
static const uint32_t s_pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};

/* Private functions */
static fsp_err_t icu_init(void);
static fsp_err_t gpt_init(void);
static fsp_err_t adc_init(void);

/*******************************************************************************************************************//**
 * @brief     This function is a callback of the user button SW2 (IRQ13) and changes the LED blinking frequency.
 * @param[in] p_args                        Pointer to the callback arguments (unused).
 * @retval    None.
 **********************************************************************************************************************/
void button_irq13_callback(external_irq_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    s_cur_rate++;
    if (s_cur_rate > (NUM_RATES - 1))
    {
        s_cur_rate = 0;
    }

    R_GPT_PeriodSet(g_blinker.p_ctrl, s_pwm_rates[s_cur_rate]);

    g_curr_led_freq = s_pwm_rates[s_cur_rate];
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback of the user button SW1 (IRQ12) and changes the LED intensity.
 * @param[in] p_args                        Pointer to the callback arguments (unused).
 * @retval    None.
 **********************************************************************************************************************/
void button_irq12_callback(external_irq_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    s_cur_dc++;
    if (s_cur_dc > (NUM_DCS - 1))
    {
        s_cur_dc = 0;
    }

    for (uint32_t j = 0; j < (sizeof(s_pwm_pins) / sizeof(s_pwm_pins[0])); j++)
    {
        R_GPT_DutyCycleSet(s_pwm_pins[j].p_timer->p_ctrl, s_pwm_dcs[s_cur_dc], s_pwm_pins[j].pin);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback of the blinking timer and starts or stops the PWM timers of the LEDs.
 * @param[in] p_args                        Pointer to the callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_blink_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(s_pwm_pins[0].p_timer->p_ctrl, &status);

        for (uint32_t i = 0; i < (sizeof(s_pwm_pins) / sizeof(s_pwm_pins[0])); i++)
        {
            if (TIMER_STATE_COUNTING == status.state)
            {
                R_GPT_Stop(s_pwm_pins[i].p_timer->p_ctrl);
            }
            else
            {
                R_GPT_Start(s_pwm_pins[i].p_timer->p_ctrl);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT, ICU, and ADC modules.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Initialized successfully.
 * @retval    Any other error codes         Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = gpt_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = icu_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = adc_init();

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads the MCU die temperature from the ADC.
 * @param[out] p_adc_data                   Pointer to store the raw ADC value of the temperature channel.
 * @retval     FSP_SUCCESS                  Read successfully.
 * @retval     Any other error codes        Failed to read.
 **********************************************************************************************************************/
fsp_err_t adc_temperature_get(uint16_t * p_adc_data)
{
    fsp_err_t err = FSP_SUCCESS;

#if !(BSP_PERIPHERAL_ADC_B_PRESENT)
    /* Start the ADC scan again in single scan mode */
    if (ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode)
    {
        err = ADC_SCANSTART(&g_adc_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

    err = ADC_READ(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, p_adc_data);

    return err;
}

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function initiates the ADC calibration and waits until the calibration is finished.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Calibrated successfully.
 * @retval    Any other error codes         Failed to calibrate.
 **********************************************************************************************************************/
fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status;

    /* Initiate the ADC calibration */
    err = ADC_CALIBRATE(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    do
    {
        /* Get the ADC status */
        err = ADC_STATUSGET(&g_adc_ctrl, &adc_status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait here until the calibration is finished, it takes 24 milliseconds to 780 milliseconds based on
         * the clock */
    } while (ADC_STATE_IDLE != adc_status.state);

    return err;
}
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief     This function opens and enables the external IRQ of the user buttons.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Initialized successfully.
 * @retval    Any other error codes         Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++)
    {
        err = R_ICU_ExternalIrqOpen(s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = R_ICU_ExternalIrqEnable(s_irq_pins[i].p_irq->p_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens the GPT timers of the LEDs and starts the blinking timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Initialized successfully.
 * @retval    Any other error codes         Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t gpt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    for (uint32_t i = 0; i < (sizeof(s_pwm_pins) / sizeof(s_pwm_pins[0])); i++)
    {
        err = R_GPT_Open(s_pwm_pins[i].p_timer->p_ctrl, s_pwm_pins[i].p_timer->p_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_GPT_PeriodSet(g_blinker.p_ctrl, s_pwm_rates[s_cur_rate]);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_GPT_Start(g_blinker.p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens, configures, and starts the ADC.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Initialized successfully.
 * @retval    Any other error codes         Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = ADC_OPEN(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = ADC_SCANCFG(&g_adc_ctrl, &ADC_CHANNEL_CONFIG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

    err = ADC_SCANSTART(&g_adc_ctrl);

    return err;
}
