/***********************************************************************************************************************
* File Name    : serial.c
* Description  : Contains macros, data structures, and common functions used for the terminal operations.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1U)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <hal_data.h>
#include "serial.h"
#if BSP_PERIPHERAL_SCI_PRESENT
  #include "r_sci_uart/r_sci_uart_instance_cfg.h"
#elif BSP_PERIPHERAL_SCI_B_PRESENT
  #include "r_sci_b_uart/r_sci_b_uart_instance_cfg.h"
#elif BSP_PERIPHERAL_UARTA_PRESENT
  #include "r_uarta/r_uarta_instance_cfg.h"
#endif

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static char g_serial_tx_buffer [SERIAL_TX_MAX_SIZE];
static char g_serial_rx_buffer [SERIAL_RX_MAX_SIZE];

static volatile uart_event_t g_serial_event = (uart_event_t)SERIAL_DATA_ZERO;
static volatile uint32_t g_serial_rx_count = SERIAL_DATA_ZERO;
static volatile bool g_serial_rx_enable = false;

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_callback
 *  Description  : SCI UART callback function for handling UART events.
 *                 This function processes UART events such as transmit complete, receive character, and receive
 *                 complete. It manages storing received data into the buffer and handles special characters like Enter
 *                 and Backspace. 
 *  Arguments[in]: p_args  Pointer to UART callback arguments.
 *  Return Value : None
 **********************************************************************************************************************/
void serial_callback(uart_callback_args_t *p_args)
{
    /* Verify that the UART channel is correct. */
    if (g_serial_cfg.channel == p_args->channel)
    {
        /* Handle the UART event */
        switch (p_args->event)
        {
            case UART_EVENT_TX_COMPLETE:
                g_serial_event |= UART_EVENT_TX_COMPLETE;
                break;

            case UART_EVENT_RX_CHAR:
                /* If disabled, do not store received data into the buffer. */
                if (false == g_serial_rx_enable)
                {
                    break;
                }

                if (SERIAL_RX_MAX_SIZE > g_serial_rx_count)
                {
                    g_serial_rx_buffer[g_serial_rx_count ++] = (char)p_args->data;
                    g_serial_event |= UART_EVENT_RX_CHAR;
                }

                /* Store received data if enabled. */
                switch ((char)p_args->data)
                {
                    /* Handle Enter character and process the received data. */
                    case SERIAL_CHAR_LF:
                    case SERIAL_CHAR_CR:
                        if (SERIAL_DATA_ZERO != g_serial_rx_count)
                        {
                            g_serial_event |= UART_EVENT_RX_COMPLETE;
                        }
                        break;

                    /* Handle Backspace character and remove the last character from the buffer. */
                    case SERIAL_CHAR_BS:
                        if (SERIAL_DATA_ZERO < g_serial_rx_count)
                        {
                            g_serial_rx_count --;
                        }
                        break;

                    /* Store received data bytes into the buffer. */
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}
/***********************************************************************************************************************
* End of function serial_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_init
 *  Description  : This function initializes the UART module, sets up the transmit/receive pins, configures the 
 *                 baud rate, and enables receiving data into the buffer.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
uint32_t serial_init(void)
{
    uint32_t status = FSP_SUCCESS;

    /* Initialize the UART pins. */
    R_BSP_PinAccessEnable();
    R_BSP_PinCfg(SERIAL_RX_PIN, SERIAL_PIN_RX_CFG);
    R_BSP_PinCfg(SERIAL_TX_PIN, SERIAL_PIN_TX_CFG);
    R_BSP_PinAccessDisable();

    /* Initialize the UART module. */
#if BSP_PERIPHERAL_SCI_PRESENT
    status = R_SCI_UART_Open(&g_serial_ctrl, &g_serial_cfg);
#elif BSP_PERIPHERAL_SCI_B_PRESENT
    status = R_SCI_B_UART_Open(&g_serial_ctrl, &g_serial_cfg);
#elif BSP_PERIPHERAL_UARTA_PRESENT
    status = R_UARTA_Open(&g_serial_ctrl, &g_serial_cfg);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Calculate baud rate settings based on the user-configured baud rate. */
#if BSP_PERIPHERAL_SCI_PRESENT
    status = R_SCI_UART_BaudCalculate(SERIAL_BAUD_RATE, SERIAL_MODULATION, SERIAL_ERR_X1000, g_serial_cfg_extend.p_baud_setting);
#elif BSP_PERIPHERAL_SCI_B_PRESENT
    status = R_SCI_B_UART_BaudCalculate(SERIAL_BAUD_RATE, SERIAL_MODULATION, SERIAL_ERR_X1000, g_serial_cfg_extend.p_baud_setting);
#elif BSP_PERIPHERAL_UARTA_PRESENT
    status = R_UARTA_BaudCalculate(SERIAL_BAUD_RATE, SERIAL_ERR_X1000, UARTA_CLOCK_SOURCE_HOCO, g_serial_cfg_extend.p_baud_setting);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Set the baud rate settings. */
#if BSP_PERIPHERAL_SCI_PRESENT
    status = R_SCI_UART_BaudSet(&g_serial_ctrl, g_serial_cfg_extend.p_baud_setting);
#elif BSP_PERIPHERAL_SCI_B_PRESENT
    status = R_SCI_B_UART_BaudSet(&g_serial_ctrl, g_serial_cfg_extend.p_baud_setting);
#elif BSP_PERIPHERAL_UARTA_PRESENT
    status = R_UARTA_BaudSet(&g_serial_ctrl, g_serial_cfg_extend.p_baud_setting);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Enable storing received data into the buffer. */
    g_serial_rx_enable = true;

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function serial_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_deinit
 *  Description  : This function de-initializes the UART module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void serial_deinit(void)
{
    /* De-initialize the opened UART module. */
    if (MODULE_CLOSE != g_serial_ctrl.open)
    {
#if BSP_PERIPHERAL_SCI_PRESENT
        R_SCI_UART_Close(&g_serial_ctrl);
#elif BSP_PERIPHERAL_SCI_B_PRESENT
        R_SCI_B_UART_Close(&g_serial_ctrl);
#elif BSP_PERIPHERAL_UARTA_PRESENT
        R_UARTA_Close(&g_serial_ctrl);
#endif
    }
}
/***********************************************************************************************************************
* End of function serial_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_printf
 *  Description  : This function formats a string using a variable argument list and sends it to the UART interface.
 *                 It waits for the transmission to complete and returns the status of the operation.
 *  Arguments[in]: p_format  Pointer to the formatted string.
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
uint32_t serial_printf(char * p_format, ...)
{
    uint32_t status = FSP_SUCCESS;
    va_list aptr;
    uint32_t timeout = RESET_VALUE;

    /* Initialize the argument list. */
    va_start(aptr, p_format);

    /* Format the string into the transmit buffer. */
    int32_t ret = vsprintf(g_serial_tx_buffer, p_format, aptr);

    /* Clean up the argument list. */
    va_end(aptr);

    /* Check vsprintf function return code. */
    if (SERIAL_DATA_ZERO > ret)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Get the message length of the transmit buffer. */
    uint32_t length = strlen(g_serial_tx_buffer);
    if (SERIAL_DATA_ZERO == length)
    {
        return FSP_ERR_INVALID_SIZE;
    }

    /* Set the timeout value for serial transfer based on the message length. */
    timeout = SAFETY_FACTOR * length * (SECOND_TO_MICROSECOND / SERIAL_BAUD_RATE);

    /* Clear the serial transmit complete event. */
    g_serial_event &= ~UART_EVENT_TX_COMPLETE;

    /* Send the transmit buffer to the UART interface. */
#if BSP_PERIPHERAL_SCI_PRESENT
    status = R_SCI_UART_Write(&g_serial_ctrl, (uint8_t *)g_serial_tx_buffer, length);
#elif BSP_PERIPHERAL_SCI_B_PRESENT
    status = R_SCI_B_UART_Write(&g_serial_ctrl, (uint8_t *)g_serial_tx_buffer, length);
#elif BSP_PERIPHERAL_UARTA_PRESENT
    status = R_UARTA_Write(&g_serial_ctrl, (uint8_t *)g_serial_tx_buffer, length);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Wait for the transmission to complete. */
    while (UART_EVENT_TX_COMPLETE != (UART_EVENT_TX_COMPLETE & g_serial_event))
    {
        timeout --;
        R_BSP_SoftwareDelay(SERIAL_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);

        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
    }

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function serial_printf
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_read
 *  Description  : This function reads data from the UART receive buffer.
 *                 It reads the received data from the UART buffer and returns the number of bytes read.
 *                 It supports reading a single character or the entire buffer that ends with an Enter character.
 *  Arguments[in]: p_format  Pointer to the formatted string.
 *  Return Value : Number of bytes read from the receive buffer.
 **********************************************************************************************************************/
uint32_t serial_read(void * const p_buffer, uint32_t buffer_size)
{
    uint32_t read_len = SERIAL_DATA_ZERO;

    /* Disable storing received data into the buffer. */
    g_serial_rx_enable = false;

    /* Read only the newest character if buffer size is 1. */
    if ((1U == buffer_size) && (SERIAL_DATA_ZERO != g_serial_rx_count))
    {
        /* Remove the Enter character. */
        g_serial_rx_count --;
        *(char *)p_buffer = g_serial_rx_buffer[g_serial_rx_count];
        return 1U;
    }

    /* Read the entire buffer that ends with the Enter character. */
    else
    {
        read_len = (g_serial_rx_count >= buffer_size) ? buffer_size: g_serial_rx_count;
        memcpy(p_buffer, g_serial_rx_buffer, read_len);
    }

    /* Clear all control variables before receiving data. */
    g_serial_event &= ~(UART_EVENT_RX_COMPLETE | UART_EVENT_RX_CHAR);
    g_serial_rx_count = SERIAL_DATA_ZERO;

    /* Enable storing received data into the buffer. */
    g_serial_rx_enable = true;

    return read_len;
}
/***********************************************************************************************************************
* End of function serial_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_has_data
 *  Description  : This function checks if there is complete data in the UART receive buffer.
 *                 It checks if a complete message (ending with Enter) has been received.
 *  Arguments    : None.
 *  Return Value : SERIAL_TRUE if has a complete data, otherwise SERIAL_FALSE.
 **********************************************************************************************************************/
uint32_t serial_has_data(void)
{
    if (UART_EVENT_RX_COMPLETE & g_serial_event)
    {
        return SERIAL_TRUE;
    }
    return SERIAL_FALSE;
}
/***********************************************************************************************************************
* End of function serial_has_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: serial_has_key
 *  Description  : This function checks if there is any character in the UART receive buffer.
 *                 It checks if a character has been received but not yet processed.
 *  Arguments    : None.
 *  Return Value : SERIAL_TRUE if has a complete data, otherwise SERIAL_FALSE.
 **********************************************************************************************************************/
uint32_t serial_has_key(void)
{
    if (UART_EVENT_RX_CHAR & g_serial_event)
    {
        return SERIAL_TRUE;
    }
    return SERIAL_FALSE;
}
/***********************************************************************************************************************
* End of function serial_has_key
***********************************************************************************************************************/

#endif /* USE_VIRTUAL_COM */
