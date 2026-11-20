/***********************************************************************************************************************
 * File Name    : gpt_timer.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdbool.h>
#include "gpt_timer.h"

/* Boolean flag to determine One-Shot mode timer is expired or not */
bool volatile g_one_shot_expired = false;

gpt_instance_ctrl_t * gp_active_timer_ctrl = NULL;

/***********************************************************************************************************************
 * @brief     This function initializes GPT timer.
 * @param[in] p_timer_ctl               Pointer to the control structure.
 * @param[in] p_timer_cfg               Pointer to a configuration structure.
 * @param[in] timer_mode                Mode of GPT Timer.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    gp_active_timer_ctrl = (gpt_instance_ctrl_t *) p_timer_ctl;

    if (gp_active_timer_ctrl->open == MODULE_CLOSE)
    {
        /* Initialize GPT Timer */
        err = R_GPT_Open(p_timer_ctl, p_timer_cfg);
    }

    return err;
}

/***********************************************************************************************************************
 * @brief     This function starts GPT timer in Periodic, One-Shot, PWM modes.
 * @param[in] p_timer_ctl               Pointer to the timer control structure.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t start_gpt_timer(timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Starts GPT timer */
    err = R_GPT_Start(p_timer_ctl);

    return err;
}

/***********************************************************************************************************************
 * @brief     This function sets the duty cycle of PWM timer.
 * @param[in] duty_cycle_percent        Duty cycle percent.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t duty_cycle_counts = RESET_VALUE;
    uint32_t current_period_counts = RESET_VALUE;
    timer_info_t info = {(timer_direction_t)RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get the current period setting */
    err = R_GPT_InfoGet(&g_timer_pwm_ctrl, &info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    else
    {
        /* Update period counts locally */
        current_period_counts = info.period_counts;

        /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
         * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
         * not required for 16-bit timers. */
        duty_cycle_counts = (uint32_t) ((uint64_t) (current_period_counts * duty_cycle_percent) / GPT_MAX_PERCENT);

#if defined(BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA6T2_MCK)\
    || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA8T2_MCK)
        duty_cycle_counts = (current_period_counts - duty_cycle_counts);
#endif

        /* Duty cycle set API set the desired intensity on the on-board LED */
        err = R_GPT_DutyCycleSet(&g_timer_pwm_ctrl, duty_cycle_counts, TIMER_PIN);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    return err;
}

/***********************************************************************************************************************
 * @brief     This function sets period value of Periodic timer.
 * @param[in] period_counts             Period value.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t set_period_gpt_timer(timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set period value */
    err = R_GPT_PeriodSet(p_ctrl, period_counts);

    return err;
}

/***********************************************************************************************************************
 * @brief     This function closes opened GPT module.
 * @param[in] p_timer_ctl               Pointer to the timer control structure.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_timer_ctl;

    if (p_instance_ctrl->open != MODULE_CLOSE)
    {
        /* Close the opened GPT timer */
        R_GPT_Close(p_timer_ctl);
    }
}

/***********************************************************************************************************************
 * @brief      This function is callback for a GPT timer in One-Shot mode.
 * @param[in]  p_args                   Pointer to the callback function parameter data.
 * @retval     None.
 **********************************************************************************************************************/
void user_gpt_one_shot_callback(timer_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        if (TIMER_EVENT_CYCLE_END == p_args->event)
        {
            /* Set boolean flag on One-Shot mode timer expired */
            g_one_shot_expired = true;
        }
    }
}
