/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common initialization function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_init.h"
#include "board_cfg.h"

#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))
#define NUM_SWITCH            (sizeof(irq_pins) / sizeof(irq_pins[0]))

uint32_t g_curr_led_freq = BLINK_FREQ_1HZ;

static const struct
{
    const external_irq_instance_t * const p_irq;
} irq_pins[] =
{
        {&g_external_irq12},
        {&g_external_irq13},
};

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} pwm_pins[] =
{
    {&g_gpt_blue, GPT_IO_PIN_GTIOCB},
    {&g_gpt_red_and_green, GPT_IO_PIN_GTIOCA_AND_GTIOCB}
};

static uint32_t cur_dc = 0, cur_rate = 0;

int curr_led_freq = 0;
uint32_t pwm_dcs[3] = {LED_INTENSITY_90, LED_INTENSITY_50, LED_INTENSITY_10};
uint32_t pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};

void button_irq13_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    if (++cur_rate > (NUM_RATES - 1))
        cur_rate = 0;

    R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[cur_rate]);

    g_curr_led_freq = (uint32_t)pwm_rates[cur_rate];
}

void button_irq12_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    if (++cur_dc > (NUM_DCS - 1))
        cur_dc = 0;

    for (uint32_t j = 0; j < (sizeof(pwm_pins) / sizeof(pwm_pins[0])); j++)
    {
        R_GPT_DutyCycleSet(pwm_pins[j].p_timer->p_ctrl, pwm_dcs[cur_dc], pwm_pins[j].pin);
    }
}

static fsp_err_t ICU_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++)
    {
        fsp_err = R_ICU_ExternalIrqOpen(irq_pins[i].p_irq->p_ctrl, irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != fsp_err)
            return fsp_err;

        fsp_err = R_ICU_ExternalIrqEnable(irq_pins[i].p_irq->p_ctrl);
        if (FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    return fsp_err;
}

void gpt_blink_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(pwm_pins[0].p_timer->p_ctrl, &status);

        for (uint32_t i = 0; i < (sizeof(pwm_pins) / sizeof(pwm_pins[0])); i++)
        {
            if (TIMER_STATE_COUNTING == status.state)
            {
                R_GPT_Stop(pwm_pins[i].p_timer->p_ctrl);
            }
            else
            {
                R_GPT_Start(pwm_pins[i].p_timer->p_ctrl);
            }
        }
    }
}

static fsp_err_t GPT_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < (sizeof(pwm_pins) / sizeof(pwm_pins[0])); i++)
    {
        fsp_err = R_GPT_Open(pwm_pins[i].p_timer->p_ctrl, pwm_pins[i].p_timer->p_cfg);
        if (FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if (FSP_SUCCESS != fsp_err)
            return fsp_err;
    }
    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[cur_rate]);
    {
        if (FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        /* Turn ON red LED to indicate fatal error */
        TURN_RED_ON;

        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}

static fsp_err_t ADC_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    fsp_err = R_ADC_B_Open(&g_adc_b_ctrl, &g_adc_b_cfg);
#else
    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    fsp_err = R_ADC_B_ScanCfg(&g_adc_b_ctrl, &g_adc_b_scan_cfg);
#else
    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    fsp_err = adc_start_calibration();
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    fsp_err = R_ADC_B_ScanStart(&g_adc_b_ctrl);
#else
    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function initiates the calibration.
 * @param[in]   None.
 * @retval      FSP_SUCCESS                  Upon successful of ADC calibration.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful calibration failure.
 ***********************************************************************************************************************/
fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    adc_status_t adc_status;         /* To get ADC status */

    /* Initiate ADC calibration */
    err = R_ADC_B_Calibrate(&g_adc_b_ctrl, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    do
    {
        /* To get the ADC status */
        err = R_ADC_B_StatusGet(&g_adc_b_ctrl, &adc_status);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    } while (ADC_STATE_IDLE != adc_status.state); /* Wait here till the calibration.
                                                   * It takes 24 milliseconds to 780 milliseconds based on clock */

    return err;
}
#endif

fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = GPT_Initialize();
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = ICU_Initialize();
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = ADC_Initialize();
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}
