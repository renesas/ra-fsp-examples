/***********************************************************************************************************************
 * File Name    : uart_control.c
 * Description  : This source file implements the functions declared in uart_control.h
 *                for configuring and operating a UART peripheral to communicate with
 *                a serial terminal.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "uart_control.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern share_mem_t *share_mem;

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
volatile uint8_t g_rx_complete_flag = RESET_FLAG;
char g_rx_buffer[BUFFER_SIZE_DOWN] = {RESET_VALUE};

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
volatile static uint16_t rx_count = RESET_VALUE;
volatile static uart_event_t g_uart_event = (uart_event_t) RESET_VALUE;

/*******************************************************************************************************************//**
 *  @brief      This function is used to initialize the UARTA module to transfer the EP log to the host PC
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize R_SCI_B_UART channel with baud rate 115200 */
    err = R_SCI_B_UART_Open(&g_uart8_ctrl, &g_uart8_cfg);
    return err;
}


/*******************************************************************************************************************//**
 *  @brief      This function is used to de-initialize the UART module
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void uart_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize UARTA if it opened */
    if (MODULE_CLOSE != g_uart8_ctrl.open)
    {
        err = R_SCI_B_UART_Close(&g_uart8_ctrl);
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
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t uart_write_message (char * format, ...)
{
    /* Initialize variables */
    fsp_err_t    err  = FSP_SUCCESS;
    uint32_t   timeout = RESET_VALUE;
    uint32_t   len     = RESET_VALUE;
    int     ret     = RESET_VALUE;
    unsigned char g_tx_buffer[BUFFER_SIZE_UP] = {RESET_VALUE};
    va_list aptr;
    if (CORE_0_MESSAGE == format)
    {
        FSP_PARAMETER_NOT_USED(format);

        /* copy data and length from share memory to transfer buffer */
        memcpy(g_tx_buffer, share_mem->buf_out,sizeof(g_tx_buffer));
        len = share_mem->length;
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
    timeout = len * UART_TIMEOUT_ONE_BYTE;

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

    /* Ensure the previous print request from CPU0 completes before handling a new message */
    if (CORE_0_MESSAGE == format)
    {
        R_IPC_EventGenerate(&g_ipc0_ctrl, IPC_GENERATE_EVENT_IRQ3);
    }
    return err;
}
/*****************************************************************************************************************
 *  @brief      uart callback
 *  @param[in]  uart_callback_args_t  * p_args
 *  @retval     None
 ****************************************************************************************************************/
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
                        g_rx_complete_flag = SET_FLAG;
                        rx_count = RESET_VALUE;
                    }
                    break;
                }
                case BACKSPACE:
                {
                    if (rx_count > 0)
                    {
                        rx_count--;
                        g_rx_buffer[rx_count] = '\0';     // clear the last char
                        /* Update the terminal */
                        uart_write_message(" \b \b");
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
                        APP_PRINT_INFO(LOG_POS,"[INFO]The receiver buffer overflowed.Please press enter key and input again\r\n");
                    }
                    break;
                }

            }
        }
    }
}

