/***********************************************************************************************************************
 * File Name    : backlight_thread_entry.c
 * Description  : This thread is responsible for initializing the General Purpose Timer (GPT) module and configuring it
 *                for PWM output. The PWM signal is used to control the LCD backlight brightness. After initialization,
 *                the thread remains active to handle backlight updates such as brightness adjustment.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "backlight_thread.h"
#include "Application.h"

static fsp_err_t gpt_timer_PWM_setup(void);
static void set_backlight_pwm(uint8_t pwm_duty_cycle);

/* BL Thread entry function */
/* pvParameters contains TaskHandle_t */
void backlight_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    uint32_t  pwm_duty_percent  = 0;

    /* Initialize and start the GPT timer*/
    gpt_timer_PWM_setup();

    while (1)
    {
        /* Wait until request control backlight from emWin Thread */
        if(xTaskNotifyWait(0, 0, &pwm_duty_percent, portMAX_DELAY) == pdTRUE)
        {
            set_backlight_pwm((uint8_t)pwm_duty_percent);     // update PWM period
        }
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @brief This function is setting up GPT/PWM timer
 * @param[in] None
 * @retval    fsp_err_t
 **********************************************************************************************************************/
static fsp_err_t gpt_timer_PWM_setup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open GPT */
    err = R_GPT_Open(&g_timer_PWM_ctrl, &g_timer_PWM_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }
    /* Enable GPT Timer */
    err = R_GPT_Enable(&g_timer_PWM_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_PWM_ctrl);
    if(FSP_SUCCESS != err)
    {
        return err;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief * This function updates the PWM duty cycle value to control the brightness level of the backlight.
 *          The input duty cycle is expected to be in the range 0 (0% brightness) to 100 (100% brightness).
 * @param[in] pwm_duty_cycle    Duty cycle percentage for backlight brightness.
 *                              Valid range: 0 to 100.
 * @retval    none
 **********************************************************************************************************************/
static void set_backlight_pwm(uint8_t pwm_duty_cycle)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_info_t info = {0};
    uint32_t current_period_count = 0;
    uint32_t duty_cycle_count     = 0;

    /* Get the current period setting. */
    if(R_GPT_InfoGet(&g_timer_PWM_ctrl, &info))
    {
        APP_ERR_TRAP(err);
    }
    current_period_count = info.period_counts;

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
     * not required for 16-bit timers. */
    duty_cycle_count = (uint32_t) (((uint64_t) current_period_count * pwm_duty_cycle)/GPT_PWM_MAX_PERCENT);
    R_GPT_DutyCycleSet(&g_timer_PWM_ctrl, duty_cycle_count, GPT_IO_PIN_GTIOCB);
}

