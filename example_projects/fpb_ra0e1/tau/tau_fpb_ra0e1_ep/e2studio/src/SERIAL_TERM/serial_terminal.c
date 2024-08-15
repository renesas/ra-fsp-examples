/***********************************************************************************************************************
 * File Name    : serial_terminal.c
 * Description  : Contains declarations of data structures and functions used tau.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#include "common_utils.h"
#include <stdarg.h>

/*******************************************************************************************************************//**
 * @addtogroup serial_terminal
 * @{
 **********************************************************************************************************************/

/* Public global variables declarations */
volatile uart_event_t g_uart_event = (uart_event_t) RESET_VALUE;
volatile _Bool g_rx_complete_flag = false;
extern char g_rx_buffer [BUFFER_SIZE_DOWN];

/*******************************************************************************************************************//**
 *  @brief      This function is used to initialize the UARTA module to transfer the EP log to the host PC
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UARTA channel with baud rate 115200 */
    err = R_UARTA_Open(&g_uart_ctrl, &g_uart_cfg);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function is used to de-initialize the UARTA module
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void uart_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize UARTA if it opened */
    if (MODULE_CLOSE != g_uart_ctrl.open)
    {
        err = R_UARTA_Close(&g_uart_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "UARTA Close API failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function is used to transfer a message to the host PC
 *  @param[in]  format   pointer to message buffer
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
uint8_t serial_write_message (char * format, ...)
{
    uint8_t    status  = FSP_SUCCESS;
    uint32_t   timeout = RESET_VALUE;
    size_t    len     = RESET_VALUE;
    int     ret     = RESET_VALUE;
    unsigned char g_tx_buffer[BUFFER_SIZE_UP] = {RESET_VALUE};
    va_list aptr;

    va_start(aptr, format);
    ret = vsprintf((char *)g_tx_buffer, format, aptr);
    va_end(aptr);

    /* Check VSPRINTF function return code */
    if (RESET_VALUE > ret)
    {
        return (uint8_t)FSP_ERR_INVALID_ARGUMENT;
    }

    /* Calculate length of message */
    len = strlen((char *)g_tx_buffer);
    if (RESET_VALUE == len)
    {
        return (uint8_t)FSP_ERR_INVALID_SIZE;
    }

    /* Set the waiting time value according to the message length */
    timeout = (uint32_t)len * UART_TIMEOUT_ONE_BYTE;

    /* Reset UARTA callback variable */
    g_uart_event = RESET_VALUE;

    /* Write message to UART interface */
    status = (uint8_t)R_UARTA_Write(&g_uart_ctrl, g_tx_buffer, len);
    if ((uint8_t)FSP_SUCCESS != status)
    {
        return status;
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

    return status;
}
/*****************************************************************************************************************
 *  @brief      Serial user callback
 *  @param[in]  p_args  Callback
 *  @retval     None
 ****************************************************************************************************************/
void serial_callback(uart_callback_args_t *p_args)
{
    static volatile uint16_t rx_count = RESET_VALUE;
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
                case ENTER_KEY:
                    /* Set flag if at least one character has been received */
                    if (RESET_VALUE != rx_count)
                    {
                        g_rx_complete_flag = true;
                        rx_count = RESET_VALUE;
                    }
                break;

                    /* Read all data provided by user until enter button is pressed */
                default:
                    /* Check the number of characters received to ensure there is no buffer overflow */
                    if (BUFFER_SIZE_DOWN > rx_count)
                    {
                    	g_rx_buffer[rx_count++] = (char) p_args->data;
                    }
                    else
                    {
                    	APP_PRINT("\r\nThe receiver buffer overflowed. Please press enter key and input again\r\n");
                    }
            }
        }
    }
}
#endif /* USE_VIRTUAL_COM */

/*******************************************************************************************************************//**
 * @} (end addtogroup serial_terminal)
 **********************************************************************************************************************/
