/***********************************************************************************************************************
 * File Name    : uart_contol.c
 * Description  : This source file implements the functions declared in uart_control.h
 *                for configuring and operating a UART peripheral to communicate with
 *                a serial terminal.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <stdarg.h>
#include "uart_control.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
volatile bool g_rx_complete_flag = false;
uint8_t g_rx_buffer [BUFFER_SIZE_DOWN];

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
volatile static uint16_t rx_count = RESET_VALUE;
volatile static uart_event_t g_uart_event = (uart_event_t) RESET_VALUE;

/*******************************************************************************************************************//**
 *  @brief      This function is used to initialize the UARTA module to transfer the EP log to the host PC
 *  @param      uart_instance_t *   Pointer to UART module instance.
 *  @retval     FSP_SUCCESS    UART initialization was successful
 *  @retval     A failure occurred; return value indicates the error code.
 **********************************************************************************************************************/
fsp_err_t uart_init(const uart_instance_t *uart_instance)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize R_SCI_B_UART channel with baud rate 115200 */
    err = R_SCI_B_UART_Open(uart_instance->p_ctrl, uart_instance->p_cfg);
    return err;
}


/*******************************************************************************************************************//**
 * @brief      This function is used to de-initialize the UART module
 * This function closes the provided UART instance and releases any associated hardware resources.
 * It should be called when UART communication is no longer required to ensure proper cleanup.
 * @param[in]  uart_instance   Pointer to UART module instance.
 * @retval     None
 **********************************************************************************************************************/
void uart_deinit(const uart_instance_t *uart_instance)
{
    fsp_err_t err = FSP_SUCCESS;
    sci_b_uart_instance_ctrl_t * p_ctrl = (sci_b_uart_instance_ctrl_t*) uart_instance->p_ctrl;

    /* De-initialize UARTA if it opened */
    if (MODULE_CLOSE != p_ctrl->open)
    {
        err = R_SCI_B_UART_Close(p_ctrl);
        if(FSP_SUCCESS != err)
        {
            /* Error trap */
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function is used to transfer a message to the host PC
 *  @param[in]  format   pointer to message buffer
 *  @retval     FSP_SUCCESS    UART write message was successful
 *  @retval     A failure occurred; return value indicates the error code.
 **********************************************************************************************************************/
fsp_err_t uart_write_message (unsigned char * buffer, uint32_t length, char * format, ...)
{
    fsp_err_t   err   = FSP_SUCCESS;
    uint32_t  timeout = RESET_VALUE;
    size_t    len     = RESET_VALUE;
    int       ret     = RESET_VALUE;
    unsigned char g_tx_buffer[MAX_MESSAGE_LENGTH] = {RESET_VALUE};
    va_list aptr;
    if (NULL != buffer && 0 != length)
    {
        len = (size_t) length;
        memcpy(g_tx_buffer, buffer, length);
    }
    else
    {
        va_start(aptr, format);
        ret = vsprintf((char *)g_tx_buffer, format, aptr);
        va_end(aptr);

        /* Check VSPRINTF function return code */
        if (RESET_VALUE > ret)
        {
            return FSP_ERR_INVALID_ARGUMENT;
        }

        /* Calculate length of message */
        len = strlen((char *)g_tx_buffer);
        if (RESET_VALUE == len)
        {
            return FSP_ERR_INVALID_SIZE;
        }
    }
    /* Set the waiting time value according to the message length */
    timeout = (uint32_t)len * UART_TIMEOUT_ONE_BYTE;

    /* Reset UARTA callback variable */
    g_uart_event = RESET_VALUE;

    /* Write message to UART interface */
    err = R_SCI_B_UART_Write(&g_uart8_ctrl, g_tx_buffer, len);
    if (FSP_SUCCESS != err)
    {
        /* Error trap */
        APP_ERR_TRAP(err);
    }

    /* Check for event transfer complete */
    while (UART_EVENT_TX_COMPLETE != g_uart_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Decrease timeout value, then delay the transfer time by one character */
        timeout --;
        R_BSP_SoftwareDelay(UART_CHAR_TIMEOUT_VALUE, UART_CHAR_TIMEOUT_UNIT);
    }
    return err;
}
/*****************************************************************************************************************
 * @brief      UART callback function.
 * This function is invoked by the UART driver in response to UART events such as transmission complete,
 * reception complete, error conditions, etc. It handles these events as required by the application logic.
 * @param[in]  p_args   Pointer to the structure containing callback event information.
 * @retval     None
 **********************************************************************************************************************/
void uart_callback(uart_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Get UARTA callback event */
        g_uart_event = p_args->event;
        /* Handle the character reception event */

        if (UART_EVENT_RX_CHAR == g_uart_event)
        {
            switch (p_args->data)
            {
                /* If Enter Key is pressed by user, set flag to process the data */
                case CR:
                case LF:
                {
                    /* Set flag if at least one character has been received */
                    if (RESET_VALUE != rx_count)
                    {
                        g_rx_complete_flag = true;
                        rx_count = RESET_VALUE;
                    }
                    break;
                }
                    /* Read all data provided by user until enter button is pressed */
                default:
                {
                    /* Check the number of characters received to ensure there is no buffer overflow */
                    if (BUFFER_SIZE_DOWN > rx_count)
                    {
                        g_rx_buffer[rx_count++] = (char) p_args->data;
                    }
                    else
                    {
                        APP_PRINT("\r\nThe receiver buffer overflowed. Please press enter key and input again\r\n");
                    }
                    break;
                }

            }
        }
    }
}

