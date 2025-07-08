/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : jlink_console.c
 * Description  : Entry function.
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "r_typedefs.h"

#include "common_utils.h"
#include "common_init.h"
#include "board_cfg.h"
#include "jlink_console.h"

uint32_t g_transfer_done = 0;
uint32_t g_receive_done  = 0;

#define TRANSFER_LENGTH           (1024)

static bool_t s_configured = false;

static bool_t s_echo_enable= false;

static uint8_t  s_out_of_band_received[TRANSFER_LENGTH];

static uint32_t s_g_out_of_band_index = 0;

static uint8_t s_rx_buf;

static void jlink_console_write (const char_t * buffer);


/**********************************************************************************************************************
 * Function Name: jlink_configured
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
bool_t jlink_configured()
{
    bool_t isconfigured = s_configured;

    return (isconfigured);
}
/**********************************************************************************************************************
 End of function jlink_configured
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: jlink_set_echo_mode
 * Description  : .
 * Argument     : mode
 * Return Value : .
 *********************************************************************************************************************/
bool_t jlink_set_echo_mode(bool_t mode)
{
    bool_t current_mode = s_echo_enable;
    s_echo_enable = mode;

    return (current_mode);
}
/**********************************************************************************************************************
 End of function jlink_set_echo_mode
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: jlink_restore_echo_mode
 * Description  : .
 * Argument     : mode
 * Return Value : .
 *********************************************************************************************************************/
void jlink_restore_echo_mode(bool_t mode)
{
    s_echo_enable = mode;
}
/**********************************************************************************************************************
 End of function jlink_restore_echo_mode
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: jlink_console_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void jlink_console_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_SCI_UART_Open(&g_jlink_console_ctrl, &g_jlink_console_cfg);

    if (FSP_SUCCESS == fsp_err)
    {
        s_configured = true;
    }

    return;
}
/**********************************************************************************************************************
 End of function jlink_console_init
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: print_to_console
 * Description  : .
 * Argument     : p_data
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t print_to_console(char_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    {
        /* Intentional throttling here to stop serial console form filling g_jlink_console_queue */
        jlink_console_write(p_data);
    }

    return (err);
}
/**********************************************************************************************************************
 End of function print_to_console
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: print_to_jlink_console
 * Description  : .
 * Argument     : p_data
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t print_to_jlink_console(char_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    if (s_configured)
    {
        jlink_console_write(p_data);
    }

    return (err);
}
/**********************************************************************************************************************
 End of function print_to_jlink_console
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: start_key_check
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void start_key_check(void)
{
    s_rx_buf       = 0;
    g_receive_done = false;

    R_SCI_UART_Read(&g_jlink_console_ctrl, &s_rx_buf, 1);
}
/**********************************************************************************************************************
 End of function start_key_check
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: get_detected_key
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
uint8_t get_detected_key(void)
{
    if (true == s_echo_enable)
    {
        char_t out[] = " \0";

        out[0] = (char_t)s_rx_buf; /* Cast to char_t*/
        jlink_console_write(out);
    }
    return (s_rx_buf);
}
/**********************************************************************************************************************
 End of function get_detected_key
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: key_pressed
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t key_pressed(void)
{
    /* Refresh before the counter underflows to prevent reset or NMI. */
    return (g_receive_done);
}
/**********************************************************************************************************************
 End of function key_pressed
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: jlink_console_write
 * Description  : .
 * Argument     : buffer
 * Return Value : .
 *********************************************************************************************************************/
static void jlink_console_write(const char_t *buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer_done = false;

    err = R_SCI_UART_Write(&g_jlink_console_ctrl, (uint8_t *)buffer, strlen(buffer)); /* Cast to uint8_t* */

    assert (FSP_SUCCESS == err);

    while (!g_transfer_done)
    {
        vTaskDelay(10);
    }
    vTaskDelay(10);
}
/**********************************************************************************************************************
 End of function jlink_console_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: input_from_console
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
int8_t input_from_console(void)
{
    start_key_check();

    while (key_pressed() == false)
    {
        vTaskDelay(1);
    }
    return ((int8_t)get_detected_key()); /* Cast to int8_t */
}
/**********************************************************************************************************************
 End of function input_from_console
 *********************************************************************************************************************/


/* callback from driver */
/**********************************************************************************************************************
 * Function Name: jlink_console_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void jlink_console_callback(uart_callback_args_t *p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if ((sizeof(s_out_of_band_received)) > s_g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if (UART_DATA_BITS_8 >= g_jlink_console_cfg.data_bits)
                {
                    s_out_of_band_received[s_g_out_of_band_index++] = (uint8_t) p_args->data; /* Cast to uint8_t */
                }
                else
                {
                    /* Cast to uint16_t* */
                    uint16_t * p_dest      = (uint16_t *) &s_out_of_band_received[s_g_out_of_band_index];
                    *p_dest                = (uint16_t) p_args->data; /* Cast to uint */
                    s_g_out_of_band_index += 2;
                }
            }
            break;
        }

        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_done = 1;
            break;
        }

        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_done = 1;
            break;
        }

        default:
        {
        }
    }
}
/**********************************************************************************************************************
 End of function jlink_console_callback
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: get_new_chars
 * Description  : .
 * Argument     : pBuf
 * Return Value : .
 *********************************************************************************************************************/
uint8_t get_new_chars(uint8_t* pBuf)
{
    uint8_t x   = 0U;
    uint8_t ret = 0U;

    /* Check if single character received. */
    if (g_receive_done)
    {
        pBuf[0]        = s_rx_buf;
        g_receive_done = 0;
        return 1;
    }
    else if (0 == s_g_out_of_band_index)
    {
        return 0;
    }
    else
    {
        /* Continue. */
    }

    for (x = 0; x < s_g_out_of_band_index; x++)
    {
        pBuf[x] = s_out_of_band_received[x];
    }

    memset(s_out_of_band_received, 0, s_g_out_of_band_index + 1);
    ret = (uint8_t)s_g_out_of_band_index; /* cast to uint8_t */

    s_g_out_of_band_index = 0U;
    return ret;
}
/**********************************************************************************************************************
 End of function get_new_chars
 *********************************************************************************************************************/
