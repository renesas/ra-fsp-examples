/***********************************************************************************************************************
 * File Name    : tau_ep.c
 * Description  : Contains data structures and functions used in tau_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "tau_ep.h"

/* TAU callback flags */
volatile bool g_led1_status_flag_cur = false;
volatile bool g_input_capture_detect_flag = false;
volatile bool g_external_counter_flag = false;
volatile bool g_measure_pulse_width_flag = false;
volatile bool g_delay_timer_end_flag = false;

/* Capture timer count value */
volatile uint32_t g_capture_value = RESET_VALUE;

/* Interrupt event counter */
volatile uint32_t g_interrupt_counter = RESET_VALUE;

/* Timer information */
timer_info_t g_periodic_info =
{
    .clock_frequency = RESET_VALUE,
    .count_direction = (timer_direction_t) RESET_VALUE,
    .period_counts = RESET_VALUE,
};

/*******************************************************************************************************************//**
 * @brief     This function initializes TAU timer.
 * @param[in] p_ctrl                    Pointer to the TAU timer control block.
 * @param[in] p_cfg                     Pointer to the TAU timer configuration structure.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t tau_init(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_TAU_Open(p_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts TAU timer.
 * @param[in] p_ctrl                    Pointer to the TAU timer control block.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t tau_start(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_TAU_Start(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables external event triggers that start, stop, clear, or capture the counter.
 * @param[in] p_ctrl                    Pointer to the TAU timer control block.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t tau_enable(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_TAU_Enable(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets timer information.
 * @param[in]  p_ctrl                   Pointer to the TAU timer control block.
 * @param[out] p_info                   Pointer to the timer information structure.
 * @retval     FSP_SUCCESS              Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
fsp_err_t tau_info_get(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_TAU_InfoGet(p_ctrl, p_info);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function resets TAU timer.
 * @param[in] p_ctrl                    Pointer to the TAU timer control block.
 * @retval    FSP_SUCCESS               Reset successfully.
 * @retval    Any other error codes     Failed to reset.
 **********************************************************************************************************************/
fsp_err_t tau_reset(timer_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_TAU_Reset(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets period value for TAU timer.
 * @param[in] p_ctrl                    Pointer to the TAU timer control block.
 * @param[in] period_counts             Period value.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t tau_period_set(timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_TAU_PeriodSet(p_ctrl, period_counts);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened TAU timer.
 * @param[in] p_ctrl                    Pointer to the TAU timer control block.
 * @retval    None.
 **********************************************************************************************************************/
void tau_close(timer_ctrl_t * const p_ctrl)
{
    tau_instance_ctrl_t * p_instance_ctrl = (tau_instance_ctrl_t *) p_ctrl;

    if (p_instance_ctrl->open != 0U)
    {
        R_TAU_Close(p_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes all opened TAU timer instances.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tau_deinit(void)
{
    /* Close opened TAU interval timer instance */
    tau_close(&g_interval_timer_ctrl);

    /* Close opened TAU delay timer instance */
    tau_close(&g_delay_timer_ctrl);

    /* Close opened TAU square wave output sequence 2 timer instance */
    tau_close(&g_square_wave_output_sequence2_ctrl);

    /* Close opened TAU divider timer instance */
    tau_close(&g_divider_timer_ctrl);

    /* Close opened TAU input capture timer instance */
    tau_close(&g_input_capture_ctrl);

    /* Close opened TAU external event counter timer instance */
    tau_close(&g_external_event_counter_timer_ctrl);

    /* Close opened TAU square wave output sequence 3 timer instance */
    tau_close(&g_square_wave_output_sequence3_ctrl);

    /* Close opened TAU measure low level pulse width timer instance */
    tau_close(&g_measure_low_level_pulse_width_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for interval timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @return    None.
 **********************************************************************************************************************/
void tau_interval_timer_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_led1_status_flag_cur = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for delay timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void tau_delay_timer_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_delay_timer_end_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for input capture timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void tau_input_capture_timer_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
    {
        g_input_capture_detect_flag = true;
        g_capture_value = p_args->capture;
        g_interrupt_counter++;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for external event counter timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void tau_external_event_counter_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_external_counter_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for measure low level pulse width timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void tau_measure_low_level_pulse_width_timer_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
    {
        g_measure_pulse_width_flag = true;
        g_capture_value = p_args->capture;
        g_interrupt_counter++;
    }
}
