/***********************************************************************************************************************
 * File Name    : sau_uart_ep.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stddef.h>
#include "sau_uart_ep.h"

/* Global variables */
uint8_t g_rx_buffer[BUFFER_LEN] = {RESET_VALUE};
uint8_t g_tx_buffer[BUFFER_LEN] = {RESET_VALUE};
volatile uint16_t g_rx_len = RESET_VALUE;
volatile bool g_rx_flag = false;

/* Private variable */
static volatile uart_event_t g_sau_uart_event = (uart_event_t)RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function initializes the SAU UART module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t sau_uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open SAU UART module */
    err = R_SAU_UART_Open(&g_sau_uart_ctrl, &g_sau_uart_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to the terminal application on the host PC.
 * @param[in] p_data                    Pointer to data buffer.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t sau_uart_send(uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = RESET_VALUE;

    /* Set a timeout value according to the length of the input data */
    timeout = (uint32_t)strlen((char *)p_data) * TIME_OUT_FOR_ONE_BYTE;

    /* Reset SAU UART callback event */
    g_sau_uart_event = (uart_event_t)RESET_VALUE;

    /* Perform SAU UART write operation */
    err = R_SAU_UART_Write(&g_sau_uart_ctrl, p_data, strlen((char *)p_data));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for UART_EVENT_TX_COMPLETE event */
    while (UART_EVENT_TX_COMPLETE != g_sau_uart_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Decrease timeout value then software delay */
        timeout--;
        R_BSP_SoftwareDelay(TIME_OUT_VALUE, TIME_OUT_UNIT);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened SAU UART module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sau_uart_deinit(void)
{
    /* Close the opened SAU UART module */
    if (MODULE_CLOSE != g_sau_uart_ctrl.open)
    {
        R_SAU_UART_Close(&g_sau_uart_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     UART user callback.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void sau_uart_callback(uart_callback_args_t *p_args)
{
    static volatile uint16_t rx_count = RESET_VALUE;

    if (NULL != p_args)
    {
        /* Get SAU UART callback event */
        g_sau_uart_event = p_args->event;

        /* Handle the character reception event */
        if (UART_EVENT_RX_CHAR == p_args->event)
        {
            switch (p_args->data)
            {
                /* If Enter is pressed by the user, set flag to process the data */
                case CR_ASCII:
                case LF_ASCII:
                {
                    /* Set flag if at least one character has been received */
                    if (RESET_VALUE != rx_count)
                    {
                        g_rx_len = rx_count;
                        g_rx_flag = true;
                        rx_count = RESET_VALUE;
                    }
                    break;
                }

                /* Read all data provided by the user until Enter button is pressed */
                default:
                {
                    /* Check the number of characters received to ensure there is no buffer overflow */
                    if (BUFFER_LEN > rx_count)
                    {
                        g_rx_buffer[rx_count++] = (uint8_t)p_args->data;
                    }
                    break;
                }
            }
        }
    }
}
