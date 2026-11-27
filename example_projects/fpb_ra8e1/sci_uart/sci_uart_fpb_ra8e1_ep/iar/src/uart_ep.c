/***********************************************************************************************************************
 * File Name    : uart_ep.c
 * Description  : Contains UART function definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include "uart_ep.h"
#include "timer_pwm.h"

/* Temporary buffer to save data from receive buffer for further processing */
static uint8_t g_temp_buffer[DATA_LENGTH] = {RESET_VALUE};

/* Counter to update g_temp_buffer index */
static volatile uint8_t g_counter_var = RESET_VALUE;

/* Flag to check whether data is received or not */
static volatile uint8_t g_data_received_flag = false;

/* Flag for user callback */
static volatile uint8_t g_uart_tx_event = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function performs UART operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t uart_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    volatile bool b_valid_data = true;

    uart_print_user_msg(EP_INFO);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    while (true)
    {
        if (g_data_received_flag)
        {
            g_data_received_flag = false;

            uint8_t input_length = RESET_VALUE;
            volatile uint32_t intensity = RESET_VALUE;

            /* Calculate g_temp_buffer length */
            input_length = ((uint8_t)(strlen((char *) &g_temp_buffer)));

            /* Check if input data length is in limit */
            if (DATA_LENGTH > (uint8_t)input_length)
            {
                /* This loop validates input data byte by byte to filter out decimals (floating point input).
                 * Any such data will be considered as invalid. */
                for (int buf_index = RESET_VALUE; buf_index < input_length; buf_index++)
                {
                    if (ZERO_ASCII <= g_temp_buffer[buf_index] && NINE_ASCII >= g_temp_buffer[buf_index])
                    {
                        /* Set b_valid_data flag as data is valid */
                        b_valid_data = true;
                    }
                    else
                    {
                        /* Clear b_valid_data flag as data is not valid, clear the buffer and break the loop */
                        memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                        b_valid_data = false;
                        break;
                    }
                }

                /* All bytes in data are integers, convert input to integer value to set intensity */
                intensity = ((uint32_t)(atoi((char *) &g_temp_buffer)));

                /* Validation input data is in 1 - 100 range */
                if (MAX_INTENSITY < intensity || RESET_VALUE == intensity)
                {
                    /* Reset the g_temp_buffer as data is out of limit */
                    memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                    b_valid_data = false;

                    /* Application is being run on Serial terminal hence transmitting error message to the same */
                    err = uart_print_user_msg(EP_GUI);
                    if (FSP_SUCCESS != err)
                    {
                        return err;
                    }
                }
            }
            else
            {
                /* Clear b_data_valid flag as data is not valid, clear the g_temp_buffer */
                memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                b_valid_data = false;

                err = uart_print_user_msg(EP_GUI);
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
            }

            /* Set intensity only for valid data */
            if (b_valid_data)
            {
                /* Clear b_data_valid flag and g_temp_buffer */
                memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                b_valid_data = false;

                /* Change intensity of LED */
                err = set_intensity(intensity, TIMER_PIN);
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                err = uart_print_user_msg("\r\nLED intensity set successfully");
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                err = uart_print_user_msg("\r\n\r\nSet next value. User input: ");
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes SCI UART module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART channel with baud rate 115200 */
    err = R_SCI_UART_OPEN(&g_uart_ctrl, &g_uart_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function transmits user specified number of bytes from the source buffer pointer.
 * @param[in] p_api_ctrl                Pointer to the control structure.
 * @param[in] p_src                     Pointer to the source buffer.
 * @param[in] bytes                     The number of bytes to transfer.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
fsp_err_t uart_write(uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit data */
    err = R_SCI_UART_WRITE(p_api_ctrl, p_src, bytes);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function prints the user message to terminal.
 * @param[in] p_msg                     Pointer to the null-terminated message string to be transmitted.
 * @retval    FSP_SUCCESS               Printed successfully.
 * @retval    Any other error codes     Failed to print.
 **********************************************************************************************************************/
fsp_err_t uart_print_user_msg(const char *p_msg)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t msg_len = RESET_VALUE;
    uint32_t local_timeout = (DATA_LENGTH * UINT16_MAX);

    /* Calculate length of the user message */
    msg_len = (uint8_t)strlen(p_msg);

    /* Reset callback capture variable */
    g_uart_tx_event = RESET_VALUE;

    /* Write the message to the terminal */
    err = uart_write(&g_uart_ctrl, (uint8_t *)p_msg, msg_len);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_tx_event) && (--local_timeout))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_tx_event)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }
    }

    if (RESET_VALUE == local_timeout)
    {
        return FSP_ERR_TIMEOUT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes SCI UART module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void uart_deinit(void)
{
    /* Close the opened SCI UART module */
    if (g_uart_ctrl.open)
    {
        R_SCI_UART_CLOSE(&g_uart_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     UART user callback.
 * @param[in] p_args        Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args)
{
    if (NULL == p_args)
    {
        return;
    }

    /* Transmit complete event */
    if (UART_EVENT_TX_COMPLETE == p_args->event)
    {
        g_uart_tx_event = UART_EVENT_TX_COMPLETE;
    }

    /* Character received */
    if (UART_EVENT_RX_CHAR == p_args->event)
    {
        /* Reset g_temp_buffer index if it exceeds than buffer size */
        if (DATA_LENGTH <= g_counter_var)
        {
            g_counter_var = RESET_VALUE;
        }

        switch (p_args->data)
        {
            /* If the Enter button is pressed by the user, set flag to process the data */
            case CARRIAGE_ASCII:
            {
                g_counter_var = RESET_VALUE;
                g_data_received_flag = true;
                break;
            }

            /* Read all data provided by the user until the Enter button is pressed */
            default:
            {
                g_temp_buffer[g_counter_var++] = (uint8_t)p_args->data;
                break;
            }
        }
    }
}
