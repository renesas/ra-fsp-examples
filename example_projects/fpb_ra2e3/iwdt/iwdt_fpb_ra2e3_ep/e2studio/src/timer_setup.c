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
#include "iwdt_ep.h"

/* External global variable */
/* LED Structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;

/* Global variable */
/* Variable to capture timer underflow counts and print IWDT refresh status */
volatile uint32_t g_timer_underflow_counter = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function initializes the timer module used to periodically refresh the IWDT.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized timer module successfully.
 * @retval    Any other error codes     Failed to initialize timer module.
 **********************************************************************************************************************/
fsp_err_t init_timer_module(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_TIMER_OPEN(&g_timer_ctrl, &g_timer_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the timer that periodically refreshes the IWDT.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started timer successfully.
 * @retval    Any other error codes     Failed to start timer.
 **********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_TIMER_START(&g_timer_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the timer that periodically refreshes the IWDT.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Stopped timer successfully.
 * @retval    Any other error codes     Failed to stop timer.
 **********************************************************************************************************************/
fsp_err_t timer_stop(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_TIMER_STOP(&g_timer_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened timer module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_timer_module(void)
{
    /* Close the opened timer module */
    if (g_timer_ctrl.open)
    {
        R_TIMER_CLOSE(&g_timer_ctrl);
    }
}

#if (BSP_PERIPHERAL_TAU_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function is called when the TAU timer counter wraps around.
 *            Refreshes the IWDT counter and toggles the LED state.
 * @param[in] p_args                    Pointer to the callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void tau_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = LED_OFF;

    /* Refresh IWDT */
    err = iwdt_refresh();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], LED_ON);
    }
    else
    {
        /* Counter is used to count the number of times TAU callback triggered */
        g_timer_underflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], level_led);
    }
}
#else
/*******************************************************************************************************************//**
 * @brief     This function is called when the GPT timer counter wraps around.
 *            Refreshes the IWDT counter and toggles the LED state.
 * @param[in] p_args                    Pointer to the callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = LED_OFF;

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh IWDT */
    err = iwdt_refresh();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], LED_ON);
    }
    else
    {
        /* Counter is used to count the number of times GPT callback triggered */
        g_timer_underflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], level_led);
    }
}
#endif /* BSP_PERIPHERAL_TAU_PRESENT */
