/***********************************************************************************************************************
 * File Name    : gpt_ep.c
 * Description  : Contains functions used in gpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "gpt_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes GPT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t timer_init(void)
{
    return R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function starts measurement using GPT timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t timer_start_measure(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Reset timer counter */
    err = R_GPT_Reset(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return R_GPT_Start(&g_timer_ctrl);
}

/*******************************************************************************************************************//**
 * @brief      This function measures the timing info by reading the timer.
 * @param[out] p_time                    Pointer to store the OSPI operation execution time.
 * @retval     FSP_SUCCESS               Measured successfully.
 * @retval     Any other error codes     Failed to measure.
 **********************************************************************************************************************/
fsp_err_t timer_get_measure(uint32_t * p_time)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t timer_status =
    {
     .counter = RESET_VALUE,
     .state = RESET_VALUE
    };
    timer_info_t timer_info =
    {
     .clock_frequency = RESET_VALUE,
     .count_direction = RESET_VALUE,
     .period_counts = RESET_VALUE
    };

    /* Get status of timer */
    err = R_GPT_StatusGet(&g_timer_ctrl, &timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get info of timer */
    err = R_GPT_InfoGet(&g_timer_ctrl, &timer_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Stop timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Convert count value to nanoseconds unit */
    *p_time = (timer_status.counter * 100) / (timer_info.clock_frequency / 10000000);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes opened GPT module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void timer_deinit(void)
{
    if (g_timer_ctrl.open)
    {
        /* Close the opened GPT timer */
        R_GPT_Close(&g_timer_ctrl);
    }
}
