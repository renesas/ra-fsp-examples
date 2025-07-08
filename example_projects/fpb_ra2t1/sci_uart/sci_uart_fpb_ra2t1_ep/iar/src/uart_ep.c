/***********************************************************************************************************************
 * File Name    : uart_ep.c
 * Description  : Contains UART functions definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "uart_ep.h"
#include "timer_pwm.h"

/*******************************************************************************************************************//**
 * @addtogroup r_sci_uart_ep
 * @{
 **********************************************************************************************************************/

/* Private global variables */
/* Temporary buffer to save data from receive buffer for further processing */
static uint8_t g_temp_buffer[DATA_LENGTH] = {RESET_VALUE};

/* Counter to update g_temp_buffer index */
static volatile uint8_t g_counter_var = RESET_VALUE;

/* Flag to check whether data is received or not */
static volatile uint8_t g_data_received_flag = false;

/* Flag for user callback */
static volatile uint8_t g_uart_event = RESET_VALUE;

/*****************************************************************************************************************
 *  @brief       SCI_UART Example project to demonstrate the functionality.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS     Upon success
 *  @retval      Any Other Error code apart from FSP_SUCCESS
 ****************************************************************************************************************/
fsp_err_t uart_ep_demo(void)
{
    fsp_err_t err = FSP_SUCCESS;
    volatile bool b_valid_data = true;

    while (true)
    {
        if(g_data_received_flag)
        {
            g_data_received_flag  = false;

            uint8_t input_length = RESET_VALUE;
            volatile uint32_t intensity = RESET_VALUE;

            /* Calculate g_temp_buffer length */
            input_length = ((uint8_t)(strlen((char *) &g_temp_buffer)));

            /* Check if input data length is in limit */
            if (DATA_LENGTH > (uint8_t)input_length)
            {
                /* This loop validates input data byte by byte to filter out decimals. (floating point input)
                 * Any such data will be considered as invalid. */
                for(int buf_index = RESET_VALUE; buf_index < input_length; buf_index++)
                {
                    if(ZERO_ASCII <= g_temp_buffer[buf_index] && NINE_ASCII >= g_temp_buffer[buf_index])
                    {
                        /* Set b_valid_data Flag as data is valid */
                        b_valid_data = true;
                    }
                    else
                    {
                        /* Clear data_valid flag as data is not valid, Clear the buffer and break the loop */
                        memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                        b_valid_data = false;
                        break;
                    }
                }

                /* All bytes in data are integers, convert input to integer value to set intensity */
                intensity = ((uint32_t)(atoi((char *) &g_temp_buffer)));

                /* Validation input data is in 1 - 100 range */
                if(MAX_INTENISTY < intensity || RESET_VALUE == intensity)
                {
                    /* Resetting the g_temp_buffer as data is out of limit */
                    memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                    b_valid_data = false;

                    /* Application is being run on Serial terminal hence transmitting error message to the same */
                    err = uart_print_user_msg((uint8_t *)"\r\nInvalid input. Input range is from 1 - 100\r\n");
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT ("\r\n **  UART WRITE FAILED  ** \r\n");
                        return err;
                    }
                }
            }
            else
            {
                /* Clear data_valid flag as data is not valid, Clear the g_temp_buffer */
                memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                b_valid_data = false;

                err = uart_print_user_msg((uint8_t *)"\r\nInvalid input. Input range is from 1 - 100\r\n");
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n **  UART WRITE FAILED  ** \r\n");
                    return err;
                }
            }

            /* Set intensity only for valid data */
            if(b_valid_data)
            {
                /* Change intensity of LED */
                err = set_intensity(intensity, TIMER_PIN);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT ("\r\n** GPT failed while changing intensity ** \r\n");
                    return err;
                }
                /* Resetting the temporary buffer */
                memset(g_temp_buffer, RESET_VALUE, DATA_LENGTH);
                b_valid_data = false;

                err = uart_print_user_msg((uint8_t *)"\r\nSet next value\r\n");
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
            }
        } 
    }
}

/*******************************************************************************************************************//**
 * @brief       Initialize SCI_UART module.
 * @param[in]   None
 * @retval      FSP_SUCCESS     Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t uart_initialize(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART channel with baud rate 115200 */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_UART_Open (&g_uart_ctrl, &g_uart_cfg);
#else
    err = R_SCI_UART_Open (&g_uart_ctrl, &g_uart_cfg);
#endif
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_"UART_TYPE"_Open API failed  **\r\n");
    }
    return err;
}

/*****************************************************************************************************************
 *  @brief       Print the user message to terminal.
 *  @param[in]   p_msg
 *  @retval      FSP_SUCCESS                Upon success
 *  @retval      FSP_ERR_TRANSFER_ABORTED   Upon event failure
 *  @retval      Any Other Error code apart from FSP_SUCCESS,  Unsuccessful write operation
 ****************************************************************************************************************/
fsp_err_t uart_print_user_msg(uint8_t *p_msg)
{
    fsp_err_t err   = FSP_SUCCESS;
    uint8_t msg_len = RESET_VALUE;
    uint32_t local_timeout = (DATA_LENGTH * UINT16_MAX);
    uint8_t *p_temp_ptr = (uint8_t *)p_msg;

    /* Calculate length of message received */
    msg_len = ((uint8_t)(strlen((char *)p_temp_ptr)));

    /* Reset callback capture variable */
    g_uart_event = RESET_VALUE;

    /* Writing to terminal */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_UART_Write (&g_uart_ctrl, p_msg, msg_len);
#else
    err = R_SCI_UART_Write (&g_uart_ctrl, p_msg, msg_len);
#endif

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_"UART_TYPE"_Write API Failed  **\r\n");
        return err;
    }

    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_event) && (--local_timeout))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_event)
        {
            APP_ERR_PRINT ("\r\n**  UART Error Event Received  **\r\n");
            return FSP_ERR_TRANSFER_ABORTED;
        }
    }
    if(RESET_VALUE == local_timeout)
    {
        err = FSP_ERR_TIMEOUT;
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       De-initialize SCI UART module.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_uart(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close module */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err =  R_SCI_B_UART_Close (&g_uart_ctrl);
#else
    err =  R_SCI_UART_Close (&g_uart_ctrl);
#endif

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_"UART_TYPE"_Close API failed  ** \r\n");
    }
}

/*****************************************************************************************************************
 *  @brief      UART user callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args)
{
    /* Logged the event in global variable */
    g_uart_event = (uint8_t)p_args->event;

    /* Reset g_temp_buffer index if it exceeds than buffer size */
    if(DATA_LENGTH == g_counter_var)
    {
        g_counter_var = RESET_VALUE;
    }

    if(UART_EVENT_RX_CHAR == p_args->event)
    {
        switch (p_args->data)
        {
            /* If Enter is pressed by user, set flag to process the data */
            case CARRIAGE_ASCII:
            {
                g_counter_var = RESET_VALUE;
                g_data_received_flag  = true;
                break;
            }
            /* Read all data provided by user until enter button is pressed */
            default:
            {
                g_temp_buffer[g_counter_var++] = (uint8_t ) p_args->data;
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_sci_uart_ep)
 **********************************************************************************************************************/
