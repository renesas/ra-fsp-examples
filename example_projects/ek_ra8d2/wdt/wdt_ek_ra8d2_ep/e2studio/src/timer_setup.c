/***********************************************************************************************************************
 * File Name    : timer_setup.c
 * Description  : Contains data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "timer_setup.h"
#include "wdt_ep.h"

/* Global variable */
extern bsp_leds_t g_bsp_leds;

/* Variable to capture timer underflow counts and print WDT status */
volatile uint32_t g_timer_underflow_counter = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT timer in Periodic mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_gpt_module(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the GPT timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the GPT timer.
 * @param[in] None
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t timer_stop(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Stop GPT timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the GPT timer.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_gpt_module(void)
{
    /* Close the opened GPT timer */
    if (g_timer_ctrl.open)
    {
        R_GPT_Close(&g_timer_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     The callback function refreshes WDT counter and toggles LED when GPT timer's counter wrapped around.
 * @param[in] p_args   Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = LED_ON;

    /* Refresh WDT, if the user has not pressed the push button */
    err = R_WDT_Refresh(&g_wdt_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], LED_ON);
    }
    else
    {
        /* Counter is used to count the number of times GPT callback triggered */
        g_timer_underflow_counter++;

        /* Toggle LED */
        level_led ^= LED_ON;
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], level_led);
    }
}
