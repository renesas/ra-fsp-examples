/***********************************************************************************************************************
 * File Name    : gpt_timer.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_timer.h"
#include "led.h"

uint32_t app_value;

void gpt_timer(void)
{
    fsp_err_t err                           = FSP_SUCCESS;
    uint32_t gpt_desired_period_ms          = RESET_VALUE;
    uint64_t period_counts                  = RESET_VALUE;
    uint32_t pclkd_freq_hz                  = RESET_VALUE;
    gpt_desired_period_ms =  1000 / app_value;
    /* Get the source clock frequency (in Hz) */
    pclkd_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
    /* Convert period to PCLK counts so it can be set in hardware. */
    period_counts = (uint64_t)((gpt_desired_period_ms  * (pclkd_freq_hz * CLOCK_TYPE_SPECIFIER))  / TIMER_UNITS_MILLISECONDS);

    /*Initialize Periodic Timer */
    err = init_gpt_timer(&g_timer_periodic_ctrl, &g_timer_periodic_cfg, PERIODIC_MODE_TIMER);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** INIT GPT TIMER FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Start Periodic Timer*/
    err = start_gpt_timer(&g_timer_periodic_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** START GPT TIMER FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Period Set API set the desired period counts on the on-board LED */
    err = R_GPT_PeriodSet(&g_timer_periodic_ctrl, (uint32_t)period_counts);
    if(FSP_SUCCESS != err)
    {
       APP_ERR_PRINT("** R_GPT_PeriodSet FAILED ** \r\n");
       APP_ERR_TRAP(err);
    }
}

/*****************************************************************************************************************
 * @brief       Initialize GPT timer.
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @param[in]   p_timer_cfg     Timer instance Configuration structure
 * @param[in]   timer_mode      Mode of GPT Timer
 * @retval      FSP_SUCCESS     Upon successful open of timer.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful open .
 ****************************************************************************************************************/
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg, uint8_t timer_mode)
{
    FSP_PARAMETER_NOT_USED (timer_mode);
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT Timer */
    err = R_GPT_Open(p_timer_ctl, p_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_GPT_TimerOpen FAILED ** \r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Start GPT timers in periodic.
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @retval      FSP_SUCCESS     Upon successful start of timer.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful start .
 ****************************************************************************************************************/
fsp_err_t start_gpt_timer (timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Starts GPT timer */
    err = R_GPT_Start(p_timer_ctl);
    if (FSP_SUCCESS != err)
    {
        /* In case of GPT_open is successful and start fails, requires a immediate cleanup.
         * Since, cleanup for GPT open is done in start_gpt_timer,Hence cleanup is not required */
        APP_ERR_PRINT ("\r\n ** R_GPT_Start API failed ** \r\n");
    }
    return err;
}

fsp_err_t stop_gpt_timer (timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Stop GPT timer */
    err = R_GPT_Stop(p_timer_ctl);
    if (FSP_SUCCESS != err)
    {
        /* In case of GPT_open is successful and start fails, requires a immediate cleanup.
         * Since, cleanup for GPT open is done in start_gpt_timer,Hence cleanup is not required */
        APP_ERR_PRINT ("\r\n ** R_GPT_Stop API failed ** \r\n");
    }
    return err;
}

void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Timer Close API call*/
    err = R_GPT_Close(p_timer_ctl);
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT ("\r\n ** R_GPT_Close FAILED ** \r\n");
    }

}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_gpt_ep)
 **********************************************************************************************************************/
