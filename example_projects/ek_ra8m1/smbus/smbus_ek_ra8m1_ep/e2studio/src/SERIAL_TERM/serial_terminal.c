/***********************************************************************************************************************
 * File Name    : serial_terminal.c
 * Description  : Contains declarations of data structures and functions used tau.c
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

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
char g_rx_buffer [BUFFER_SIZE_DOWN] = {RESET_VALUE};

/***********************************************************************************************************************
 *  Function Name: uart_init
 *  Description  : This function is used to initialize the SCI UART module to transfer the EP log to the host PC.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize SCI UART channel with baud rate 115200 */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    err = R_SCI_UART_Open(&g_uart_ctrl, &g_uart_cfg);
#else
    err = R_SCI_B_UART_Open(&g_uart_ctrl, &g_uart_cfg);
#endif

    return err;
}
/***********************************************************************************************************************
* End of function uart_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: uart_deinit
 *  Description  : This function is used to de-initialize the SCI UART module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void uart_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize SCI UART if it opened */
    if (MODULE_CLOSE != g_uart_ctrl.open)
    {
#if (BSP_FEATURE_SCI_VERSION == 1U)
        err = R_SCI_UART_Close(&g_uart_ctrl);
#else
        err = R_SCI_B_UART_Close(&g_uart_ctrl);
#endif
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "SCI UART Close API failed\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function uart_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_write_message
 *  Description  : This function is used to transfer a message to the host PC.
 *  Arguments[in]: format   pointer to message buffer
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
uint8_t serial_write_message (char * format, ...)
{
    uint8_t    err  = FSP_SUCCESS;
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
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Calculate length of message */
    len = strlen((char *)g_tx_buffer);
    if (RESET_VALUE == len)
    {
        return FSP_ERR_INVALID_SIZE;
    }

    /* Set the waiting time value according to the message length */
    timeout = (uint32_t)len * UART_TIMEOUT_ONE_BYTE;

    /* Reset SCI UART callback variable */
    g_uart_event = RESET_VALUE;

    /* Write message to UART interface */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    err = (uint8_t) R_SCI_UART_Write(&g_uart_ctrl, g_tx_buffer, len);
#else
    err = (uint8_t) R_SCI_B_UART_Write(&g_uart_ctrl, g_tx_buffer, len);
#endif
    if (FSP_SUCCESS != err)
    {
        return err;
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
/***********************************************************************************************************************
* End of function serial_write_message
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: comms_smbus_callback
 *  Description  : This function gets user input via UART.
 *  Arguments[in]: p_args         Callback
 *  Return Value : None
 **********************************************************************************************************************/
void serial_callback(uart_callback_args_t *p_args)
{
    static volatile uint16_t rx_count = RESET_VALUE;
    if (NULL != p_args)
    {
        /* Get SCI UART callback event */
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
/***********************************************************************************************************************
* End of function serial_callback
***********************************************************************************************************************/

#endif /* USE_VIRTUAL_COM */

/*******************************************************************************************************************//**
 * @} (end addtogroup serial_terminal)
 **********************************************************************************************************************/
