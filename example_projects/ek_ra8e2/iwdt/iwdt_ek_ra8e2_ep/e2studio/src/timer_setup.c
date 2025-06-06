/***********************************************************************************************************************
 * File Name    : timer_setup.c
 * Description  : Contains data structures and functions used in timer_setup.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_setup.h"

/*******************************************************************************************************************//**
 * @addtogroup r_iwdt_ep
 * @{
 **********************************************************************************************************************/

/* External Global Variables */
/* LED Structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;

/* Global Variables */
/* Variable to capture timer overflow counts and print IWDT status */
volatile uint32_t g_timer_overflow_counter = RESET_VALUE;

#if (BSP_PERIPHERAL_TAU_PRESENT == 1)
/*******************************************************************************************************************//**
 * @brief     Initialize the TAU timer in Interval mode.
 * @param[IN]   None
 * @retval FSP_SUCCESS        Upon successful open
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t init_tau_module(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Open TAU module */
    err = R_TAU_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_TAU_Open API failed ** \r\n");
    }
    return err;
}
/*******************************************************************************************************************//**
 * @brief    Start the TAU HAL timer and return the error to the Application.
 * @param[IN]   None
 * @retval FSP_SUCCESS                 Timer started successfully.
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful to start timer
 **********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Start TAU timer */
    err = R_TAU_Start(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_TAU_Start API failed ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief    Close the TAU HAL driver and Handle the return closing API error, to the Application.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_tau_module(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the TAU module */
    err=  R_TAU_Close(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_TAU_Close API failed ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * This function is called when tau timer's counter wrapped around.
 * @brief    Refresh IWDT counter and toggle LED state.
 * @param[IN]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void tau_callback(timer_callback_args_t *p_args)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = BSP_IO_LEVEL_HIGH;

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh IWDT */
    err = R_IWDT_Refresh(&g_iwdt_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error, along with output on RTT */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);

        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_IWDT_Refresh API failed ** \r\n");
    }
    else
    {
        /* Counter is used to count the number of times TAU callback triggered */
        g_timer_overflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;

        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], level_led);
    }
}
#else
/*******************************************************************************************************************//**
 * @brief     Initialize the GPT timer in Periodic mode.
 * @param[IN]   None
 * @retval FSP_SUCCESS        Upon successful open
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t init_gpt_module(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Open API failed ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief    Start the GPT HAL timer and return the error to the Application.
 * @param[IN]   None
 * @retval FSP_SUCCESS                 Timer started successfully.
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful to start timer
 **********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* Variable to track error and return values */
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
 **********************************************************************************************************************/
void deinit_gpt_module(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module */
    err=  R_GPT_Close(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Close API failed ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * This function is called when gpt timer's counter wrapped around.
 * @brief    Refresh IWDT counter and toggle LED state.
 * @param[IN]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = BSP_IO_LEVEL_HIGH;

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh IWDT */
    err = R_IWDT_Refresh(&g_iwdt_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error, along with output on RTT */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || (BOARD_RA4T1_MCK) ||\
    defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK)
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
#else
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
#endif
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_IWDT_Refresh API failed ** \r\n");
    }
    else
    {
        /* Counter is used to count the number of times GPT callback triggered. */
        g_timer_overflow_counter++;

        /* Toggle LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || (BOARD_RA4T1_MCK) ||\
    defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK)
        level_led ^= BSP_IO_LEVEL_LOW;
#else
        level_led ^= BSP_IO_LEVEL_HIGH;
#endif
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], level_led);
    }
}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iwdt_ep)
 **********************************************************************************************************************/
