/***********************************************************************************************************************
 * File Name    : timer_setup.c
 * Description  : Contains data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_setup.h"

/*******************************************************************************************************************//**
 * @addtogroup r_wdt_ep
 * @{
 **********************************************************************************************************************/

/* External global variables */

/* LED structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;

/* Global variables */

/* Variable to capture timer underflow counts and print WDT status */
volatile uint32_t g_timer_underflow_counter = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       Initializes the GPT timer in Periodic mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful open.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t init_gpt_module(void)
{
    /* Variable to track error and return value */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print error on RTT console */
        APP_ERR_PRINT("\r\n** R_GPT_Open API failed **\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       Starts the GPT HAL timer and returns the error to the application.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Timer driver was started successfully.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful to start timer.
 **********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* Variable to track error and return value */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print error on RTT console */
        APP_ERR_PRINT("\r\n** R_GPT_Start API failed **\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       Closes the GPT HAL driver and prints an error message if the close operation fails.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_gpt_module(void)
{
    /* Variable to track error and return value */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module */
    err=  R_GPT_Close(&g_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Print error on RTT console */
        APP_ERR_PRINT("\r\n** R_GPT_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is called when GPT timer's counter wrapped around.
 *              Refresh WDT counter and toggle LED state.
 * @param[IN]   p_args   Callback function parameter data.
 * @retval      None
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    /* Variable to track error and return value */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
    || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
    static bsp_io_level_t level_led = BSP_IO_LEVEL_LOW;
#else
    static bsp_io_level_t level_led = BSP_IO_LEVEL_HIGH;
#endif

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh WDT, if the user has not pressed the push button */
    err = R_WDT_Refresh(&g_wdt_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error, along with output on RTT Viewer */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
    || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
#else
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
#endif
        /* Print error on RTT console */
        APP_ERR_PRINT("\r\n** R_WDT_Refresh API failed **\r\n");
    }
    else
    {
        /* Counter is used to count the number of times GPT callback triggered */
        g_timer_underflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], level_led);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_wdt_ep)
 **********************************************************************************************************************/
