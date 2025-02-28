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
#if (BSP_FEATURE_SCI_VERSION == 1U)
  #include "r_sci_uart/r_sci_uart_instance_cfg.h"
#else
  #include "r_sci_b_uart/r_sci_b_uart_instance_cfg.h"
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
static volatile uint32_t g_serial_rx_length = SERIAL_DATA_ZERO;
static volatile bool g_serial_rx_enable = false;

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief SCI UART callback function for handling UART events.
 *
 * This function processes UART events such as transmit complete, receive character, and receive complete.
 * It manages storing received data into the buffer and handles special characters like Enter and Backspace.
 *
 * @param[in] p_args  Pointer to UART callback arguments.
 * @retval None
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

                /* Store received data if enabled. */
                switch ((char)p_args->data)
                {
                    /* Handle Enter character and process the received data. */
                    case SERIAL_CHAR_LF:
                    case SERIAL_CHAR_CR:
                        if (SERIAL_DATA_ZERO != g_serial_rx_count)
                        {
                            g_serial_rx_length = g_serial_rx_count;
                            g_serial_event |= UART_EVENT_RX_COMPLETE;
                        }
                        break;

                    /* Handle Backspace character and remove the last character from the buffer. */
                    case SERIAL_CHAR_BS:
                        if (SERIAL_DATA_ZERO != g_serial_rx_count)
                        {
                            g_serial_rx_count --;
                        }
                        break;

                    /* Store received data bytes into the buffer. */
                    default:
                        if (SERIAL_RX_MAX_SIZE > g_serial_rx_count)
                        {
                            g_serial_rx_buffer[g_serial_rx_count ++] = (char)p_args->data;
                            g_serial_event |= UART_EVENT_RX_CHAR;
                        }
                        break;
                }
                break;

            default:
                break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief Initializes the SCI UART module and configures the baud rate.
 *
 * This function initializes the UART module, sets up the transmit/receive pins, configures the baud rate,
 * and enables receiving data into the buffer.
 *
 * @retval FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t serial_init(void)
{
    uint32_t status = FSP_SUCCESS;

    /* Initialize the UART pins. */
    R_BSP_PinAccessEnable();
    R_BSP_PinCfg(SERIAL_RX_PIN, SERIAL_PIN_CFG);
    R_BSP_PinCfg(SERIAL_TX_PIN, SERIAL_PIN_CFG);
    R_BSP_PinAccessDisable();

    /* Initialize the UART module. */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    status = R_SCI_UART_Open(&g_serial_ctrl, &g_serial_cfg);
#else
    status = R_SCI_B_UART_Open(&g_serial_ctrl, &g_serial_cfg);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Calculate baud rate settings based on the user-configured baud rate. */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    status = R_SCI_UART_BaudCalculate(SERIAL_BAUD_RATE, SERIAL_MODULATION, SERIAL_ERR_X1000, g_serial_cfg_extend.p_baud_setting);
#else
    status = R_SCI_B_UART_BaudCalculate(SERIAL_BAUD_RATE, SERIAL_MODULATION, SERIAL_ERR_X1000, g_serial_cfg_extend.p_baud_setting);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Set the baud rate settings. */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    status = R_SCI_UART_BaudSet(&g_serial_ctrl, g_serial_cfg_extend.p_baud_setting);
#else
    status = R_SCI_B_UART_BaudSet(&g_serial_ctrl, g_serial_cfg_extend.p_baud_setting);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Enable storing received data into the buffer. */
    g_serial_rx_enable = true;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Sends a formatted string to the UART interface.
 *
 * This function formats a string using a variable argument list and sends it to the UART interface.
 * It waits for the transmission to complete and returns the status of the operation.
 *
 * @param[in] p_format  Pointer to the formatted string.
 * @retval FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t serial_printf(char * p_format, ...)
{
    uint32_t status = FSP_SUCCESS;
    va_list aptr;

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
    volatile uint32_t timeout = length * (BSP_DELAY_UNITS_SECONDS / SERIAL_BAUD_RATE);

    /* Clear the serial transmit complete event. */
    g_serial_event &= ~UART_EVENT_TX_COMPLETE;

    /* Send the transmit buffer to the UART interface. */
#if (BSP_FEATURE_SCI_VERSION == 1U)
    status = R_SCI_UART_Write(&g_serial_ctrl, (uint8_t *)g_serial_tx_buffer, length);
#else
    status = R_SCI_B_UART_Write(&g_serial_ctrl, (uint8_t *)g_serial_tx_buffer, length);
#endif
    if (FSP_SUCCESS != status)
    {
        return status;
    }

    /* Wait for the transmission to complete. */
    while (UART_EVENT_TX_COMPLETE != (UART_EVENT_TX_COMPLETE & g_serial_event))
    {
        timeout --;
        R_BSP_SoftwareDelay(SERIAL_TIME_US_X10, BSP_DELAY_UNITS_MICROSECONDS);

        if (SERIAL_DATA_ZERO == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Reads data from the UART receive buffer.
 *
 * This function reads the received data from the UART buffer and returns the number of bytes read.
 * It supports reading a single character or the entire buffer that ends with an Enter character.
 *
 * @param[out] p_buffer     Pointer to the buffer to store the received data.
 * @param[in]  buffer_size  Size of the buffer.
 * @retval Number of bytes read from the receive buffer.
 **********************************************************************************************************************/
uint32_t serial_read(void * const p_buffer, uint32_t buffer_size)
{
    uint32_t read_len = SERIAL_DATA_ZERO;

    /* Disable storing received data into the buffer. */
    g_serial_rx_enable = false;

    /* Read only the newest character if buffer size is 1. */
    if ((1U == buffer_size) && (SERIAL_DATA_ZERO != g_serial_rx_count))
    {
        g_serial_rx_count --;
        *(char *)p_buffer = g_serial_rx_buffer[g_serial_rx_count];
        return 1U;
    }

    /* Read the entire buffer that ends with the Enter character. */
    else
    {
        read_len = (g_serial_rx_length >= buffer_size) ? buffer_size: g_serial_rx_length;
        memcpy(p_buffer, g_serial_rx_buffer, read_len);
    }

    /* Clear all control variables before receiving data. */
    g_serial_event &= ~(UART_EVENT_RX_COMPLETE | UART_EVENT_RX_CHAR);
    g_serial_rx_length = SERIAL_DATA_ZERO;
    g_serial_rx_count = SERIAL_DATA_ZERO;

    /* Enable storing received data into the buffer. */
    g_serial_rx_enable = true;

    return read_len;
}

/*******************************************************************************************************************//**
 * @brief Checks if there is complete data in the UART receive buffer.
 *
 * This function checks if a complete message (ending with Enter) has been received.
 *
 * @retval SERIAL_TRUE if has a complete data, otherwise SERIAL_FALSE.
 **********************************************************************************************************************/
uint32_t serial_has_data(void)
{
    if (UART_EVENT_RX_COMPLETE & g_serial_event)
    {
        return SERIAL_TRUE;
    }
    return SERIAL_FALSE;
}

/*******************************************************************************************************************//**
 * @brief Checks if there is any character in the UART receive buffer.
 *
 * This function checks if a character has been received but not yet processed.
 *
 * @retval SERIAL_TRUE if has any character, otherwise SERIAL_FALSE.
 **********************************************************************************************************************/
uint32_t serial_has_key(void)
{
    if (UART_EVENT_RX_CHAR & g_serial_event)
    {
        return SERIAL_TRUE;
    }
    return SERIAL_FALSE;
}

#endif /* USE_VIRTUAL_COM */
