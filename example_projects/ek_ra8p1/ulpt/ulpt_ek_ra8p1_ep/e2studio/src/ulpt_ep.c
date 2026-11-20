/***********************************************************************************************************************
 * File Name    : ulpt_ep.c
 * Description  : Contains data structures and functions used in ulpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "ulpt_ep.h"
#include "console/uart_console.h"

/* Global variable */
volatile bool g_internal_reset_flag = false;

/* Private variables */
static volatile bool s_periodic_timer_flag = false;
static volatile bool s_one_shot_timer_flag = false;

/*******************************************************************************************************************//**
 * @brief     This function opens the LPM driver (Deep SW Standby mode).
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ulpt_lpm_open(void)
{
    return R_LPM_Open(&g_lpm_deep_sw_standby_ctrl, &g_lpm_deep_sw_standby_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function enters Deep SW Standby low power mode. The MCU is reset internally when ULPT0 (One-Shot
 *            timer) underflows while asleep.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Entered successfully.
 * @retval    Any other error codes     Failed to enter.
 **********************************************************************************************************************/
fsp_err_t ulpt_low_power_mode_enter(void)
{
    /* Delay to ensure data has already been transmitted before entering LPM mode */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);

    /* Wait for pending UART transmissions to complete */
    uart_console_flush();

    return R_LPM_LowPowerModeEnter(&g_lpm_deep_sw_standby_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function opens ULPT0 in One-Shot mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ulpt_one_shot_timer_open(void)
{
    return R_ULPT_Open(&g_timer_one_shot_ctrl, &g_timer_one_shot_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function opens ULPT1 in Periodic mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ulpt_periodic_timer_open(void)
{
    return R_ULPT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function gets the clock frequency for a ULPT timer and sets its period from a value in seconds.
 * @param[in] p_ctrl                    Pointer to the timer control block.
 * @param[in] period_in_second          Period value in seconds.
 * @retval    FSP_SUCCESS               Got and set successfully.
 * @retval    Any other error codes     Failed to get or set.
 **********************************************************************************************************************/
fsp_err_t ulpt_timer_period_set(timer_ctrl_t * p_ctrl, uint32_t period_in_second)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_info_t timer_info =
    {
        .clock_frequency = RESET_VALUE,
        .count_direction = (timer_direction_t) RESET_VALUE,
        .period_counts = RESET_VALUE,
    };

    err = R_ULPT_InfoGet(p_ctrl, &timer_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return R_ULPT_PeriodSet(p_ctrl, (uint32_t) (period_in_second * timer_info.clock_frequency));
}

/*******************************************************************************************************************//**
 * @brief     This function starts the One-Shot timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t ulpt_one_shot_start(void)
{
    return R_ULPT_Start(&g_timer_one_shot_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function starts the Periodic timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t ulpt_periodic_start(void)
{
    return R_ULPT_Start(&g_timer_periodic_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function stops the Periodic timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t ulpt_periodic_stop(void)
{
    return R_ULPT_Stop(&g_timer_periodic_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function consumes a pending Periodic timer underflow event (set from periodic_timer_callback()).
 * @param[in] None.
 * @retval    true                      An underflow was pending and has now been cleared.
 * @retval    false                     No underflow pending.
 **********************************************************************************************************************/
bool ulpt_periodic_underflow_consume(void)
{
    if (!s_periodic_timer_flag)
    {
        return false;
    }

    s_periodic_timer_flag = false;
    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function toggles the on-board LED used to visualize Periodic timer underflows, if one is available.
 * @param[in] None.
 * @retval    true                      An LED exists and was toggled.
 * @retval    false                     No LED available on this board (nothing done).
 **********************************************************************************************************************/
bool ulpt_periodic_led_toggle(void)
{
    led_power_t led_state = (led_power_t) RESET_VALUE;

    if (LED_USE >= g_bsp_leds.led_count)
    {
        return false;
    }

    R_IOPORT_PinRead(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_USE], (bsp_io_level_t *) &led_state);
    led_state ^= (led_power_t) BSP_IO_LEVEL_HIGH;
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_USE], (bsp_io_level_t) led_state);

    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function closes all opened hardware modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void hw_module_deinit(void)
{
    /* Close the opened ULPT0 module */
    if (g_timer_one_shot_ctrl.open)
    {
        (void) R_ULPT_Close(&g_timer_one_shot_ctrl);
    }

    /* Close the opened ULPT1 module */
    if (g_timer_periodic_ctrl.open)
    {
        (void) R_ULPT_Close(&g_timer_periodic_ctrl);
    }

    /* Close the opened LPM driver */
    if (g_lpm_deep_sw_standby_ctrl.lpm_open)
    {
        (void) R_LPM_Close(&g_lpm_deep_sw_standby_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for One-Shot ULPT timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        s_one_shot_timer_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for Periodic ULPT timer.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        s_periodic_timer_flag = true;
    }
}
