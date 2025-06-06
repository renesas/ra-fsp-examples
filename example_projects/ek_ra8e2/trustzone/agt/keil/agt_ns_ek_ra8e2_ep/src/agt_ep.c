/***********************************************************************************************************************
 * File Name    : agt_ep.c
 * Description  : Contains data structures and functions used in agt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "agt_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup agt_ns_ep
 * @{
 **********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;
extern volatile uint8_t g_periodic_timer_flag ;
extern volatile uint32_t g_error_flag ;
/*******************************************************************************************************************//**
 * @brief       This functions initializes AGT module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of AGT module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t agt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /*
       Open Timer0 in One Shot Mode
       Notes:
       Arguments are passed in as NULL since the guard functions will ignore them.
       The guard functions are generated in the secure project agt_s_ek_ra6m4_ep:
        - src/g_timer_one_shot_guard.c
        - src/g_timer_one_shot_guard.c
    */

    err = g_timer_one_shot_open_guard(NULL, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nAGT0 timer open failed.\r\nRestart the Application\r\n");
        return err;
    }

    /* Open Timer1 in Periodic mode */
    err = g_timer_periodic_open_guard(NULL, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Close Timer0 in One Shot Mode */
        if ((FSP_SUCCESS != g_timer_one_shot_close_guard(NULL)))
        {
            APP_ERR_PRINT ("\r\nOne shot timer close failed.\r\nRestart the Application\r\n");
        }
        APP_ERR_PRINT ("\r\nAGT1 timer open failed.\r\nRestart the Application\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions starts AGT0 in one-shot mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Timer started successfully
 * @retval      Any Other Error code apart from FSP_SUCCESS  
 **********************************************************************************************************************/
fsp_err_t agt_start_oneshot_timer(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start Timer0 in one-shot mode */
    err = g_timer_one_shot_start_guard(NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n AGT0 timer start failed.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is callback for one-shot mode timer and starts AGT1 timer in Periodic mode.
 *
 * @param[in] (timer_callback_args_t *)
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        fsp_err_t err = FSP_SUCCESS;
        timer_status_t periodic_timer_status;

        /* Retrieve the status of timer running in periodic mode */
        err = g_timer_periodic_status_get_guard(NULL, &periodic_timer_status);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = SET_FLAG;
        }
        else
        {
            if (TIMER_STATE_COUNTING != periodic_timer_status.state)
            {
                /* Start the timer in periodic mode only if the timer is in stooped state */
                err = g_timer_periodic_start_guard(NULL);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    g_error_flag = SET_FLAG;
                    APP_ERR_PRINT("\r\nAGT1 Cannot be started in Periodic mode");
                }
                else
                {
                    g_periodic_timer_flag = SET_FLAG; /* Set the flag since timer1 is started in periodic mode */
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
 * @brief This function is callback for periodic timer and blinks LED on every 1 Second.
 *
 * @param[in] (timer_callback_args_t *)
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
    static volatile bsp_io_level_t led_level = BSP_IO_LEVEL_HIGH;

    /* If this board has no LEDs then trap here */
    if (LED_COUNT_ZERO == g_bsp_leds.led_count)
    {
        g_error_flag = SET_FLAG;
        APP_ERR_PRINT ("\r\nError: No Led's Found on the board");
        return;
    }

    /* Change the state of the LED write value */
    led_level ^= BSP_IO_LEVEL_HIGH;

    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* Change LED state */
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[RESET_FLAG], led_level);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            g_error_flag = SET_FLAG;
            APP_ERR_PRINT("\r\nLED_PIN state cannot be toggled");
            return;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function closes opened AGT module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void agt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close AGT0 module */
    err = g_timer_one_shot_close_guard(NULL);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT0 Close failure message */
        APP_ERR_PRINT("** R_AGT_Close API for channel 0 failed **  \r\n");
    }

    /* Close AGT1 module */
    err = g_timer_periodic_start_guard(NULL);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT1 Close failure message */
        APP_ERR_PRINT("** R_AGT_Close API for channel 1 failed **  \r\n");
    }

}

/*******************************************************************************************************************//**
 * @} (end addtogroup agt_ns_ep)
 **********************************************************************************************************************/