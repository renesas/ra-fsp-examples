/***********************************************************************************************************************
 * File Name    : tml_ep.c
 * Description  : Contains data structures and functions used in tml_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "tml_ep.h"

/* Global variables */
volatile bool g_timer_flag[APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT] = false,
    [APP_TML_16_BIT] = false,
    [APP_TML_32_BIT] = false,
    [APP_TML_16_BIT_CAPTURE] = false,
};
volatile uint16_t g_number_cycles = RESET_VALUE;
volatile uint32_t g_captured_time = RESET_VALUE;

/* Private variables */
static timer_instance_t const * sp_tml_instance[APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT] = &g_timer_8bit,
    [APP_TML_16_BIT] = &g_timer_16bit,
    [APP_TML_32_BIT] = &g_timer_32bit,
    [APP_TML_16_BIT_CAPTURE] = &g_timer_16bit_capture,
    [APP_TML_32_BIT_COUNTER_LPM] = &g_timer_32bit_counter_lpm,
};

static tml_instance_ctrl_t * sp_tml_instance_ctrl[APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT] = &g_timer_8bit_ctrl,
    [APP_TML_16_BIT] = &g_timer_16bit_ctrl,
    [APP_TML_32_BIT] = &g_timer_32bit_ctrl,
    [APP_TML_16_BIT_CAPTURE] = &g_timer_16bit_capture_ctrl,
    [APP_TML_32_BIT_COUNTER_LPM] = &g_timer_32bit_counter_lpm_ctrl,
};

static uint16_t s_capture_overflows = RESET_VALUE;

/***********************************************************************************************************************
 * @brief     This function initializes the TML module.
 * @param[in] tml_mode                  TML mode.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t tml_init(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open timer */
    err = R_TML_Open(sp_tml_instance[tml_mode]->p_ctrl, sp_tml_instance[tml_mode]->p_cfg);

#if (!TML_CFG_PARAM_CHECKING_ENABLE)
    if (FSP_SUCCESS == err)
    {
        sp_tml_instance_ctrl[tml_mode]->open = TML_OPEN;
    }
#endif /* !TML_CFG_PARAM_CHECKING_ENABLE */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts TML module.
 * @param[in] tml_mode                  TML mode.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t tml_start(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start timer */
    err = R_TML_Start(sp_tml_instance[tml_mode]->p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the TML module.
 * @param[in] tml_mode                  TML mode.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t tml_stop(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t timer_status;

    /* Get status of count timer */
    err = R_TML_StatusGet(sp_tml_instance[tml_mode]->p_ctrl, &timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (TIMER_STATE_STOPPED != timer_status.state)
    {
        /* Stop timer */
        err = R_TML_Stop(sp_tml_instance[tml_mode]->p_ctrl);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets timer configuration information.
 * @param[in]  p_ctrl                   Pointer to the timer control block.
 * @param[out] p_info                   Pointer to the timer configuration structure.
 * @retval     FSP_SUCCESS              Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
fsp_err_t tml_get_info(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get the clock frequency of timer */
    err = R_TML_InfoGet(p_ctrl, p_info);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the provided period value.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @param[in] period_counts             Period value.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t tml_set_period(timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set the period for the timer */
    err = R_TML_PeriodSet(p_ctrl, period_counts);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened TML module.
 * @param[in] tml_mode                  TML mode.
 * @retval    None.
 **********************************************************************************************************************/
void tml_close(app_tml_mode_t tml_mode)
{
    /* Close the opened TML module */
    if (sp_tml_instance_ctrl[tml_mode]->open)
    {
        R_TML_Close(sp_tml_instance[tml_mode]->p_ctrl);
#if (!TML_CFG_PARAM_CHECKING_ENABLE)
        sp_tml_instance_ctrl[tml_mode]->open = MODULE_CLOSED;
#endif /* !TML_CFG_PARAM_CHECKING_ENABLE */
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened TML modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tml_deinit(void)
{
   tml_close(APP_TML_8_BIT);
   tml_close(APP_TML_16_BIT);
   tml_close(APP_TML_32_BIT);
   tml_close(APP_TML_16_BIT_CAPTURE);
   tml_close(APP_TML_32_BIT_COUNTER_LPM);
}

/*******************************************************************************************************************//**
 * @brief     This function turns the on-board LED ON or OFF.
 * @param[in] led_num                   LED is set state.
 * @param[in] b_value                   LED state.
 * @retval    None.
 **********************************************************************************************************************/
void set_led(int led_num, bsp_io_level_t b_value)
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[led_num], b_value);
}

/*******************************************************************************************************************//**
 * @brief     This function toggles on-board LED1.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void toggle_led(void)
{
    bsp_io_level_t led_state = (bsp_io_level_t) RESET_VALUE;

    R_IOPORT_PinRead(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_COUNTER_MODE], &led_state);
    led_state ^= LED_ON;
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_COUNTER_MODE], (bsp_io_level_t)led_state);
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for 8-bit counter TML.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void timer_8bit_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_timer_flag[APP_TML_8_BIT] = true;
        g_number_cycles++;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for 16-bit counter TML.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void timer_16bit_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_timer_flag[APP_TML_16_BIT] = true;
        g_number_cycles++;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for 32-bit counter TML.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void timer_32bit_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_timer_flag[APP_TML_32_BIT] = true;
        g_number_cycles++;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for 16-bit capture TML.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void timer_16bit_capture_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        s_capture_overflows++;
    }
    if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
    {
        g_captured_time = (g_timer_16bit_capture_cfg.period_counts * s_capture_overflows) + p_args->capture;
        g_timer_flag[APP_TML_16_BIT_CAPTURE] = true;
    }
}
