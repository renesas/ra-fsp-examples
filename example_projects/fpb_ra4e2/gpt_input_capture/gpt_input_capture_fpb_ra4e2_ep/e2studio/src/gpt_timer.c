/***********************************************************************************************************************
 * File Name    : gpt_timer.c
 * Description  : Contains data structures and functions used in gpt_timer.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "gpt_timer.h"

/* Global variables */
volatile bool g_b_start_measurement = false;
uint32_t g_capture_overflow = RESET_VALUE;
uint64_t g_capture_count = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function initializes GPT timer.
 * @param[in] p_ctrl                    Pointer to the control structure.
 * @param[in] p_cfg                     Pointer to a configuration structure.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_gpt(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT Timer */
    err = R_GPT_Open(p_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables external event triggers that start, stop, clear, or capture the counter.
 * @param[in] p_ctrl                    Pointer to the control structure.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t enable_gpt(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable GPT timer */
    err = R_GPT_Enable(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts GPT timer.
 * @param[in] p_ctrl                    Pointer to the timer control structure.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t start_gpt(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets GPT timer information.
 * @param[in]  p_ctrl                   Pointer to the control structure.
 * @param[out] p_info                   Pointer to the store various information for a timer resource.
 * @retval     FSP_SUCCESS              Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
fsp_err_t info_get_gpt(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get GPT timer information */
    err = R_GPT_InfoGet(p_ctrl, p_info);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened GPT module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_gpt(void)
{
    /* Close the opened periodic GPT timer */
    if (g_timer_ctrl.open)
    {
        R_GPT_Close(&g_timer_ctrl);
    }

    /* Close the opened input capture GPT timer  */
    if (g_input_capture_ctrl.open)
    {
        R_GPT_Close(&g_input_capture_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is callback for a GPT input capture.
 * @param[in] p_args                  Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void input_capture_user_callback(timer_callback_args_t *p_args)
{
    /* Check for the event */
    switch (p_args->event)
    {
        case TIMER_EVENT_CAPTURE_A:
        {
            /* Capture the count in a variable */
            g_capture_count = p_args->capture;

            /* Set start measurement */
            g_b_start_measurement = true;
            break;
        }
        case TIMER_EVENT_CYCLE_END:
        {
            /* An overflow occurred during capture */
            g_capture_overflow++;
            break;
        }
        default:
        {
            break;
        }
    }
}
