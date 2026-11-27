/***********************************************************************************************************************
 * File Name    : sau_uart_example_app.c
 * Description  : Example application layer for the SAU UART EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sau_uart_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sau_uart_ep.h"
#include "sau_uart_example_app.h"

/* Private function declarations */
static fsp_err_t sau_uart_operation(void);
static void handle_error(fsp_err_t err);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SAU UART example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sau_uart_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the SAU UART module */
    err = sau_uart_init();
    handle_error(err);

    while (true)
    {
        /* Perform the SAU UART operation */
        err = sau_uart_operation();
        handle_error(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs SAU UART operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t sau_uart_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = sau_uart_send((uint8_t *) EP_GUIDE);
    handle_error(err);

    while (true != g_rx_flag)
    {
        /* Do nothing */
    }

    /* Clear the SAU UART receive flag to detect new frames */
    g_rx_flag = false;

    /* Copy the received data frame to transmit buffer */
    memcpy(g_tx_buffer, g_rx_buffer, g_rx_len);

    /* Clean the receive buffer */
    memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

    size_t tx_buffer_len = strlen((char*)g_tx_buffer);

    /* Move the transmit buffer to add echo message */
    memmove((char*)g_tx_buffer + ECHO_MESSAGE_LEN, (char*)g_tx_buffer, tx_buffer_len + 1);

    /* Add echo message for the transmit buffer */
    memcpy((char*)g_tx_buffer, ECHO_MESSAGE, ECHO_MESSAGE_LEN);

    /* Add a newline character for the transmit buffer */
    strcat((char*)g_tx_buffer, "\r\n");

    /* Send received data back to the terminal */
    err = sau_uart_send(g_tx_buffer);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Clean the SAU UART transmit buffer */
    memset(g_tx_buffer, NULL_CHAR, sizeof(g_tx_buffer));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by de-initializing the related modules, and entering a trap state.
 * @param[in] err                       Error code.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err)
{
    if (FSP_SUCCESS != err)
    {
        sau_uart_deinit();
        __BKPT(0);
    }
}
