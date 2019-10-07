/***********************************************************************************************************************
 * File Name    : timer_setup.c
 * Description  : Contains data structures and functions used in timer_setup.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_setup.h"

/*******************************************************************************************************************//**
 * @addtogroup r_wdt_ep
 * @{
 **********************************************************************************************************************/

/*
 * External Global Variables
 */
/* LED Structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;

/*
 * Global Variables
 */
/* Variable to capture timer overflow counts and print WDT status */
volatile uint32_t g_timer_overflow_counter = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     Initialize the GPT timer in Periodic mode.
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful open
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t init_gpt_module(void)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_TimerOpen API failed ** \r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief    Start the GPT HAL timer and return the error to the Application.
 * @param[IN]   None
 * @retval FSP_SUCCESS                 Timer driver closed successfully.
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful to start timer
 ***********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Start API failed ** \r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief    Close the GPT HAL driver and Handle the return closing API error, to the Application.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
void deinit_gpt_module(void)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module */
    err=  R_GPT_Close(&g_timer_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Close API failed ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * This function is called when gpt timer's counter wrapped around.
 * @brief    Refresh WDT counter and toggle LED state.
 * @param[IN]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* variable to toggle LED */
    static bsp_io_level_t level_led = BSP_IO_LEVEL_HIGH;

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh WDT, if user has not pressed the push button */
    err = R_WDT_Refresh(&g_wdt_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error, along with output on RTT*/
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);

        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_WDT_Refresh API failed ** \r\n");
    }
    else
    {
        /* Counter is used to count the number of times GPT callback triggered. */
        g_timer_overflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], level_led);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_wdt_ep)
 **********************************************************************************************************************/
