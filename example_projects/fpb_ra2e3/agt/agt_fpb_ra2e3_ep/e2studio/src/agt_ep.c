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

    /* Open Timer0 in One-Shot mode */
    err = R_AGT_Open(&g_timer_one_shot_ctrl, &g_timer_one_shot_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open Timer1 in Periodic mode */
    err = R_AGT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts AGT timer.
 * @param[in] p_ctrl                    Pointer to the timer control structure.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t agt_start(timer_ctrl_t * const p_ctrl)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Start timer */
    err = R_AGT_Start(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function gets AGT timer information.
 * @param[in] p_ctrl                    Pointer to the timer control structure.
 * @param[in] p_info                    Pointer to the timer information structure.
 * @retval    FSP_SUCCESS               Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
fsp_err_t agt_info_get(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Get timer information */
    err = R_AGT_InfoGet(p_ctrl, p_info);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function updates period for AGT timer.
 * @param[in] p_ctrl                    Pointer to the timer control structure.
 * @param[in] period_counts             Period value.
 * @retval    FSP_SUCCESS               Updated successfully.
 * @retval    Any other error codes     Failed to update.
 **********************************************************************************************************************/
fsp_err_t agt_period_set(timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Update period */
    err = R_AGT_PeriodSet(p_ctrl, period_counts);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function retrieves the current state and counter value for AGT timer.
 * @param[in] p_ctrl                    Pointer to the timer control structure.
 * @param[in] p_status                  Pointer to the current timer status.
 * @retval    FSP_SUCCESS               Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
fsp_err_t agt_status_get(timer_ctrl_t * const p_ctrl, timer_status_t * const p_status)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Retrieve the current state and counter value */
    err = R_AGT_StatusGet(p_ctrl, p_status);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the AGT timer.
 * @param[in] p_ctrl                    Pointer to the timer control structure.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t agt_stop(timer_ctrl_t * const p_ctrl)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Stop timer */
    err = R_AGT_Stop(p_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened AGT modules.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void agt_deinit(void)
{
    /* Close opened Timer0 */
    if (g_timer_one_shot_ctrl.open)
    {
        R_AGT_Close(&g_timer_one_shot_ctrl);
    }

    /* Close opened Timer1 */
    if (g_timer_periodic_ctrl.open)
    {
        R_AGT_Close(&g_timer_periodic_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is callback for One-Shot mode timer and starts AGT1 timer in Periodic mode.
 * @param[in] p_args    Pointer to the callback function parameter data.
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
        err = agt_status_get(&g_timer_periodic_ctrl, &periodic_timer_status);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = SET_FLAG;
        }
        else
        {
            if (TIMER_STATE_COUNTING != periodic_timer_status.state)
            {
                /* Start the Timer1 in Periodic mode only if the timer is in stopped state */
                err = agt_start(&g_timer_periodic_ctrl);
                if (FSP_SUCCESS != err)
                {
                    g_error_flag = SET_FLAG;
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
 * @brief     This function is callback for periodic timer and blinks LED.
 * @param[in] p_args    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8T1_MCK)\
	|| defined (BOARD_RA8T2_MCK)
    static volatile bsp_io_level_t led_level = BSP_IO_LEVEL_LOW;
#else
    static volatile bsp_io_level_t led_level = BSP_IO_LEVEL_HIGH;
#endif /* BOARD_RA4W1_EK, BOARD_RA6T1_RSSK, BOARD_RA6T2_MCK, BOARD_RA8T1_MCK, BOARD_RA8T2_MCK */

    /* Change the state of the LED write value */
    led_level ^= BSP_IO_LEVEL_HIGH;

    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* Change LED state */
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[RESET_FLAG], led_level);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = SET_FLAG;
        }
    }
}
