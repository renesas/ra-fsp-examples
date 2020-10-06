/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_init.h"
#include "board_cfg.h"

#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))
#define NUM_SWITCH            (sizeof(irq_pins) / sizeof(irq_pins[0]))

int g_curr_led_freq = BLINK_FREQ_1HZ;
adc_info_t  g_adc_info_rtn;

static const struct
{
    const external_irq_instance_t * const p_irq;
} irq_pins[] =
{
        {&g_external_irq10},
        {&g_external_irq11},
};

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} pwm_pins[] =
{
    {&g_gpt_blue, GPT_IO_PIN_GTIOCA},
};

/**********************************************************************************************************************
 * Function Name: ADC_Initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t ADC_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    fsp_err = R_ADC_InfoGet(&g_adc_ctrl, &g_adc_info_rtn);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function ADC_Initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ICU_Initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t ICU_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen(irq_pins[i].p_irq->p_ctrl, irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable(irq_pins[i].p_irq->p_ctrl);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }
    return fsp_err;
}
/**********************************************************************************************************************
 End of function ICU_Initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: GPT_Initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t GPT_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < 1; i++ )
    {
        fsp_err = R_GPT_Open(pwm_pins[i].p_timer->p_ctrl, pwm_pins[i].p_timer->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[g_board_status.led_frequency]);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR

        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function GPT_Initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = ADC_Initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = ICU_Initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = GPT_Initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    led_duty_cycle_update();
    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_duty_cycle_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void led_duty_cycle_update(void)
{
    R_GPT_DutyCycleSet(pwm_pins[0].p_timer->p_ctrl, pwm_dcs[g_board_status.led_intensity], pwm_pins[0].pin);
}
/**********************************************************************************************************************
 End of function led_duty_cycle_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_blinker_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void gpt_blinker_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(pwm_pins[0].p_timer->p_ctrl, &status);

        if (TIMER_STATE_COUNTING == status.state)
        {
            R_GPT_Stop(pwm_pins[0].p_timer->p_ctrl);
        }
        else
        {
            R_GPT_Start(pwm_pins[0].p_timer->p_ctrl);
        }
    }
}
/**********************************************************************************************************************
 End of function gpt_blinker_callback
 *********************************************************************************************************************/

