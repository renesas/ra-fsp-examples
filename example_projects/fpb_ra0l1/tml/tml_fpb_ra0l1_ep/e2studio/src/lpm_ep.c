/***********************************************************************************************************************
 * File Name    : lpm_ep.c
 * Description  : Contains data structures and functions used in lpm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lpm_ep.h"

/* Private variable */
static lpm_instance_t const * sp_lpm_instance [APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
    [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
};

/*******************************************************************************************************************//**
 * @brief     This function initializes the LPM module.
 * @param[in] lpm_mode                  LPM mode.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t lpm_init(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_LPM_Open(sp_lpm_instance[lpm_mode]->p_ctrl, sp_lpm_instance[lpm_mode]->p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enters LPM mode using WFI macro.
 * @param[in] lpm_mode                  LPM mode.
 * @retval    FSP_SUCCESS               Entered successfully.
 * @retval    Any other error codes     Failed to enter.
 **********************************************************************************************************************/
fsp_err_t lpm_mode_enter(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Wait for pending UART transmissions to complete */
    uart_console_flush();

    err = R_LPM_LowPowerModeEnter(sp_lpm_instance[lpm_mode]->p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened LPM mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lpm_deinit(void)
{
    if (g_lpm_sleep_ctrl.lpm_open)
    {
        R_LPM_Close(&g_lpm_sleep_ctrl);
    }

    if (g_lpm_sw_standby_ctrl.lpm_open)
    {
        R_LPM_Close(&g_lpm_sw_standby_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function set the period time for counter in LPM operation.
 * @param[in] p_time_cancel_lpm         The period time
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t tml_set_timer_cancel_lpm(uint16_t * p_time_cancel_lpm)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t period_counts = RESET_VALUE;
    timer_info_t info = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get the clock frequency of timer */
    err = tml_get_info(&g_timer_32bit_counter_lpm_ctrl, &info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Calculate the period counts */
    period_counts = (*p_time_cancel_lpm) * info.clock_frequency;

    /* The timer must be stopped before updating the period */
    err = tml_stop(APP_TML_32_BIT_COUNTER_LPM);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the period value */
    err = tml_set_period(&g_timer_32bit_counter_lpm_ctrl, period_counts);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs a procedure before entering LPM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
fsp_err_t lpm_enter_procedure(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Turn OFF the LPM LED in case of LPM mode */
    set_led(LED_LPM_STATE, LED_OFF);

    /* Software delay to print LPM mode name */
    R_BSP_SoftwareDelay(LPM_NAME_PRINT_TIME, LPM_TIME_TRANSITION_UNIT);

    /* Disable IO port before entering LPM mode */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start 32-bit counter operating in LPM mode */
    err = tml_start(APP_TML_32_BIT_COUNTER_LPM);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs a procedure after exiting LPM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
fsp_err_t lpm_exit_procedure(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Put IO port configuration back to user's selections */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Turn ON LPM state LED in case of normal mode */
    set_led(LED_LPM_STATE, LED_ON);

    /* Software delay before entering to the next LPM mode */
    R_BSP_SoftwareDelay(LPM_TIME_TRANSITION_VALUE, LPM_TIME_TRANSITION_UNIT);

    return err;
}
