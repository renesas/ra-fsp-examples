/***********************************************************************************************************************
 * File Name    : uarta_example_app.c
 * Description  : Example application layer for the UARTA EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: uarta_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "uarta_ep.h"
#include "uarta_example_app.h"

/* Function declarations */
static fsp_err_t uarta_operation(void);
static void handle_error(fsp_err_t err);

/***********************************************************************************************************************
 * Function Name: uarta_example_app_run.
 * Description  : Single entry-point for the UARTA example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void uarta_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char message[BUFFER_LEN] = {RESET_VALUE};

    /* Initialize the UARTA module at its default configured baud rate */
    err = uarta_init();
    handle_error(err);

    /* Get UARTA baud rate from user input */
    err = uarta_get_baud_rate();
    handle_error(err);

    /* Notify the upcoming baud rate change, and guide to run EP after select baud rate */
    snprintf(message, sizeof(message), EP_GUIDE1, g_baud_rate);
    err = uarta_send((uint8_t *)message);
    handle_error(err);

    /* Calculate and apply the selected baud rate for the UARTA module */
    err = uarta_calculate_baud_rate();
    handle_error(err);

    /* Wait for the first message after reconnect */
    while (true != g_rx_flag)
    {
        /* Do nothing */
    }
    g_rx_flag = false;

    while (true)
    {
        /* Print guide to the terminal application */
        err = uarta_send((uint8_t *)EP_GUIDE2);
        handle_error(err);

        /* Perform the UARTA operation */
        err = uarta_operation();
        handle_error(err);

        /* Trap on a UARTA receive error raised from the callback if any */
        if (FSP_SUCCESS != g_uarta_rx_error)
        {
            err = g_uarta_rx_error;
            g_uarta_rx_error = FSP_SUCCESS;
            handle_error(err);
        }
    }
}
/***********************************************************************************************************************
* End of function uarta_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: uarta_operation.
 * Description  : This function performs the UARTA operation.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Performed successfully.
 *                Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static fsp_err_t uarta_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    size_t tx_buffer_len = RESET_VALUE;

    /* Wait for the user to send a full message */
    while (true != g_rx_flag)
    {
        /* Do nothing */
    }
    g_rx_flag = false;

    /* Copy the received data frame to the transmit buffer */
    memcpy(g_tx_buffer, g_rx_buffer, g_rx_len);

    /* Clean the receive buffer */
    memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

    tx_buffer_len = strlen((char *)g_tx_buffer);

    /* Move the transmit buffer to add the echo message */
    memmove((char *)g_tx_buffer + ECHO_MESSAGE_LEN, (char *)g_tx_buffer, tx_buffer_len + 1);

    /* Add the echo message to the transmit buffer */
    memcpy((char *)g_tx_buffer, ECHO_MESSAGE, ECHO_MESSAGE_LEN);

    /* Add a newline character to the transmit buffer */
    strcat((char *)g_tx_buffer, "\r\n");

    /* Send the received data back to the terminal */
    err = uarta_send(g_tx_buffer);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Clean the transmit buffer */
    memset(g_tx_buffer, NULL_CHAR, sizeof(g_tx_buffer));

    return err;
}
/***********************************************************************************************************************
* End of function uarta_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by de-initializing UARTA, and entering a trap state.
 * Arguments    : err                       Error code.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err)
{
    if (FSP_SUCCESS != err)
    {
        uarta_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
