/***********************************************************************************************************************
 * File Name    : uart_ep.c
 * Description  : Contains data structures and functions used in uart_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "uart_ep.h"

/* Global variables */
volatile bool g_b_uart_rx_flag = false;
volatile bool g_b_uart_tx_flag = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes UART module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t uart_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART channel with baud rate 115200 */
    err = R_UART_OPEN(&g_uart_ctrl, &g_uart_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function transmits user specified number of bytes from the source buffer pointer.
 * @param[in] p_src                     Pointer to the source buffer.
 * @param[in] bytes                     The number of bytes to transfer.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
fsp_err_t uart_write(uint8_t const * const p_src, uint32_t const bytes)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit data */
    err = R_UART_WRITE(&g_uart_ctrl, p_src, bytes);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function receives user specified number of bytes into destination buffer pointer.
 * @param[in]  bytes                    The number of bytes to transfer.
 * @param[out] p_dest                   Pointer to the destination buffer.
 * @retval     FSP_SUCCESS              Received successfully.
 * @retval     Any other error codes    Failed to receive.
 **********************************************************************************************************************/
fsp_err_t uart_read(uint8_t * const p_dest, uint32_t const bytes)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Receive data */
    err = R_UART_READ(&g_uart_ctrl, p_dest, bytes);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened UART module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void uart_deinit(void)
{
    /* Close the opened UART module */
    if (g_uart_ctrl.open)
    {
        R_UART_CLOSE(&g_uart_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     The callback function for UART module.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void uart_cb(uart_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        switch (p_args->event)
        {
            case UART_EVENT_RX_COMPLETE:
            {
                g_b_uart_rx_flag = true;
                break;
            }

            case UART_EVENT_TX_COMPLETE:
            {
                g_b_uart_tx_flag = true;
                break;
            }

            default:
            {
                break;
            }
        }
    }
}
