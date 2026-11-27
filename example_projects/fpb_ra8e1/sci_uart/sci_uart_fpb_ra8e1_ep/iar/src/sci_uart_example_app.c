/***********************************************************************************************************************
 * File Name    : sci_uart_example_app.c
 * Description  : Example application layer for the SCI UART EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sci_uart_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sci_uart_example_app.h"
#include "timer_pwm.h"
#include "uart_ep.h"

/* Private function */
static void handle_error(fsp_err_t err);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SCI UART example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sci_uart_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART module */
    err = uart_init();
    handle_error(err);

    /* Initialize GPT in PWM mode */
    err = gpt_init();
    handle_error(err);

    /* Start GPT */
    err = gpt_start();
    handle_error(err);

    /* UART operation */
    err = uart_operation();
    handle_error(err);
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by de-initializing the related modules, and entering a trap state.
 * @param[in] err       Error code.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err)
{
    if (FSP_SUCCESS != err)
    {
        uart_deinit();
        gpt_deinit();
        __BKPT(0);
    }
}
