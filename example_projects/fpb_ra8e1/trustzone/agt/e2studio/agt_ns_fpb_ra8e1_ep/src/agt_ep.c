/***********************************************************************************************************************
 * File Name    : agt_ep.c
 * Description  : Contains data structures and functions used in agt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "agt_ep.h"

/* Global variables */
volatile uint8_t g_periodic_timer_flag = RESET_FLAG;    /* Flag to check Timer1 is enabled or not */
volatile bool g_error_flag = false;                     /* Flag to capture error in ISR's */

/*******************************************************************************************************************//**
 * @brief     This function initializes AGT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t agt_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open Timer0 in One-Shot mode
     * Notes:
     * Arguments are passed in as NULL since the guard functions will ignore them.
     * The guard functions are generated in the secure project:
     * - src/g_timer_one_shot_guard.c
     * - src/g_timer_periodic_guard.c */

    err = g_timer_one_shot_open_guard(NULL, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open Timer1 in Periodic mode */
    err = g_timer_periodic_open_guard(NULL, NULL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts AGT Timer0 in One-Shot mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t agt_start_oneshot_timer(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start Timer0 in One-Shot mode */
    err = g_timer_one_shot_start_guard(NULL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for One-Shot mode AGT Timer0 and starts AGT Timer1 in Periodic mode.
 * @param[in] p_args                    Callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* Error status */
        fsp_err_t err = FSP_SUCCESS;

        timer_status_t periodic_timer_status;

        /* Retrieve the status of Timer1 running in Periodic mode */
        err = g_timer_periodic_status_get_guard(NULL, &periodic_timer_status);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = true;
        }
        else
        {
            if (TIMER_STATE_COUNTING != periodic_timer_status.state)
            {
                /* Start the Timer1 in Periodic mode only if the timer is in stopped state */
                err = g_timer_periodic_start_guard(NULL);
                if (FSP_SUCCESS != err)
                {
                    g_error_flag = true;
                }
                else
                {
                    /* Set the flag since Timer1 is started in Periodic mode */
                    g_periodic_timer_flag = SET_FLAG;
                }
            }
            else
            {
                g_periodic_timer_flag = ALREADY_RUNNING;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for Periodic timer and blinks LED on every 1 second.
 * @param[in] p_args                    Callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
#if defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
static volatile bsp_io_level_t led_level = BSP_IO_LEVEL_LOW;
#else
static volatile bsp_io_level_t led_level = BSP_IO_LEVEL_HIGH;
#endif /* Specific board */

    /* Change the state of the LED write value */
    led_level ^= BSP_IO_LEVEL_HIGH;

    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* Change LED state */
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[RESET_FLAG], led_level);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = true;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened AGT modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void agt_deinit(void)
{
    /* Close the opened AGT Timer0 */
    g_timer_one_shot_close_guard(NULL);

    /* Close the opened AGT Timer1 */
    g_timer_periodic_close_guard(NULL);
}
