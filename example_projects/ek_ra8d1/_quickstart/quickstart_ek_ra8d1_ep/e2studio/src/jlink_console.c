/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : usb_console_main.c
 * Description  : Entry function.
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "r_typedefs.h"

#include "common_utils.h"
#include "common_init.h"
#include "board_cfg.h"
#include "jlink_console.h"

#define TRANSFER_LENGTH    (1024)

static uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
static uint32_t g_out_of_band_index = 0;
static uint8_t  s_rx_buf;
static uint8_t  s_rx_buf_fifo[41];

uint32_t g_transfer_complete = 0;
uint32_t g_receive_complete  = 0;

static void Jlink_console_read(const char * buffer);
static void Jlink_console_write(const char_t * buffer);

extern sci_b_baud_setting_t g_jlink_console_baud_setting;

/**********************************************************************************************************************
 * Function Name: jlink_console_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void jlink_console_init (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

/*
 *  g_jlink_console_baud_setting.baudrate_bits_b.abcse = 1;
 *  g_jlink_console_baud_setting.baudrate_bits_b.abcs  = 0;
 *  g_jlink_console_baud_setting.baudrate_bits_b.bgdm  = 0;
 *  g_jlink_console_baud_setting.baudrate_bits_b.cks   = 0;
 *  g_jlink_console_baud_setting.baudrate_bits_b.brr   = 11;
 *  g_jlink_console_baud_setting.baudrate_bits_b.mddr  = (uint8_t) 256;
 *  g_jlink_console_baud_setting.baudrate_bits_b.brme  = false;
 */
    fsp_err = R_SCI_B_UART_Open(&g_jlink_console_ctrl, &g_jlink_console_cfg);

    R_BSP_PinAccessEnable();

#define BOARD_RA8D1_EK
#ifdef BOARD_RA8D1_EK

    /* Certain SCI9 pins cannot be configured correctly through the pin configurator at the moment. */
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_14,
                    ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9));
    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_15,
                    ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9));
#endif
}

/**********************************************************************************************************************
 * End of function jlink_console_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_to_console
 * Description  : .
 * Argument     : p_data
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t print_to_console (char_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    Jlink_console_write(p_data);

    return err;
}

/**********************************************************************************************************************
 * End of function print_to_console
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: input_from_console
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
int8_t input_from_console (void)
{
    start_key_check();

    while (key_pressed() == false)
    {
        vTaskDelay(1);
    }

    return (int8_t) get_detected_key();
}

/**********************************************************************************************************************
 * End of function input_from_console
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: Jlink_console_write
 * Description  : .
 * Argument     : buffer
 * Return Value : .
 *********************************************************************************************************************/
static void Jlink_console_write (const char_t * buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer_complete = false;

    err = R_SCI_B_UART_Write(&g_jlink_console_ctrl, (uint8_t *) buffer, strlen(buffer));

    assert(FSP_SUCCESS == err);

    while (!g_transfer_complete)
    {
        vTaskDelay(1);
    }
}

/**********************************************************************************************************************
 * End of function Jlink_console_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: start_key_check_fifo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void start_key_check_fifo (void)
{
    s_rx_buf           = 0;
    g_receive_complete = false;
}

/**********************************************************************************************************************
 * End of function start_key_check_fifo
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: start_key_check
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void start_key_check (void)
{
    s_rx_buf           = 0;
    g_receive_complete = false;

    R_SCI_B_UART_Read(&g_jlink_console_ctrl, &s_rx_buf, 1);
}

/**********************************************************************************************************************
 * End of function start_key_check
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_detected_key
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
uint8_t get_detected_key (void)
{
    return s_rx_buf;
}

/**********************************************************************************************************************
 * End of function get_detected_key
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: key_pressed
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t key_pressed (void)
{
    return g_receive_complete;
}

/**********************************************************************************************************************
 * End of function key_pressed
 *********************************************************************************************************************/

/* callback from driver */

/**********************************************************************************************************************
 * Function Name: jlink_console_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void jlink_console_callback (uart_callback_args_t * p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if ((sizeof(g_out_of_band_received)) > g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if (UART_DATA_BITS_8 >= g_jlink_console_cfg.data_bits)
                {
                    g_out_of_band_received[g_out_of_band_index++] = (uint8_t) p_args->data;
                }
                else
                {
                    uint16_t * p_dest = (uint16_t *) &g_out_of_band_received[g_out_of_band_index];
                    *p_dest              = (uint16_t) p_args->data;
                    g_out_of_band_index += 2;
                }
            }

            break;
        }

        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_complete = 1;
            break;
        }

        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_complete = 1;
            break;
        }

        default:
        {
        }
    }
}

/**********************************************************************************************************************
 * End of function jlink_console_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_new_chars
 * Description  : .
 * Argument     : pBuf
 * Return Value : .
 *********************************************************************************************************************/
uint8_t get_new_chars (uint8_t * pBuf)
{
    uint8_t x   = 0U;
    uint8_t ret = 0U;

    /* Check if single character received. */
    if (g_receive_complete)
    {
        pBuf[0]            = s_rx_buf;
        g_receive_complete = 0;

        return 1;
    }
    else if (g_out_of_band_index == 0)
    {
        return 0;
    }
    else
    {
        /* Continue. */
    }

    for (x = 0; x < g_out_of_band_index; x++)
    {
        pBuf[x] = g_out_of_band_received[x];
    }

    memset(g_out_of_band_received, 0, g_out_of_band_index + 1);
    ret = g_out_of_band_index;

    g_out_of_band_index = 0U;

    return ret;
}

/**********************************************************************************************************************
 * End of function get_new_chars
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup hal_entry)
 **********************************************************************************************************************/
