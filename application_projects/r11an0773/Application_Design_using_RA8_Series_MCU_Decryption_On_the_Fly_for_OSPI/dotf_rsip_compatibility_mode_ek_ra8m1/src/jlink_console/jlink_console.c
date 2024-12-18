/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "jlink_console.h"
#include "common_util.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/
/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/
/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
char sprintf_buffer[BUFFER_LINE_LENGTH] = {};
/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
static uint8_t  g_out_of_band_received[BUFFER_LINE_LENGTH];
static uint32_t g_out_of_band_index = 0;
static uint8_t s_rx_buf;
static uint32_t g_transfer_complete = 0;
static uint32_t g_receive_complete  = 0;

static fsp_err_t Jlink_console_write(const char *buffer);
static bool key_pressed(void);
static uint8_t get_detected_key(void);

/*********************************************************************************************************************
 *  Initialize the SCI UART
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/

fsp_err_t jlink_console_init (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_SCI_B_UART_Open(&g_jlink_console_ctrl, &g_jlink_console_cfg);

    return fsp_err;
}
/*********************************************************************************************************************
 *  Global API: Print a string buffer to Jlink console
 *  @param[IN]   None
 *  @retval      return success
***********************************************************************************************************************/
app_err_t print_to_console(char * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    app_err_t status = APP_SUCCESS;

    err = Jlink_console_write(p_data);
    if(FSP_SUCCESS != err)
    {
    	handle_error(err, APP_JLINK_SONSOLE_WRITE);
    	return APP_JLINK_SONSOLE_WRITE;
    }
    return (status);
}
/*********************************************************************************************************************
 *  Read user input from the Jlink console
 *  @param[IN]   None
 *  @retval      return input buffer
***********************************************************************************************************************/
int8_t input_from_console (void)
{
	fsp_err_t err = FSP_SUCCESS;

	s_rx_buf = 0;
	g_receive_complete = false;

	err = R_SCI_B_UART_Read(&g_jlink_console_ctrl, &s_rx_buf, 1);

    handle_error(err, APP_JLINK_SONSOLE_READ);


    while(key_pressed() == false)
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
    return ((int8_t)get_detected_key());
}
/*********************************************************************************************************************
 *  Local function: write a string over the JLink console
 *  @param[IN]   buffer: string buffer
 *  @retval      None
***********************************************************************************************************************/
static fsp_err_t Jlink_console_write(const char *buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer_complete = false;

    SCB_InvalidateDCache();
    err = R_SCI_B_UART_Write(&g_jlink_console_ctrl, (uint8_t *)buffer, strlen(buffer));

    handle_error(err, APP_JLINK_SONSOLE_WRITE);


    while (!g_transfer_complete)
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);;
    }
    return err;
}

/*********************************************************************************************************************
 *  Get key pressed
 *  @param[IN]   None
 *  @retval      uint8_t: key ascii
***********************************************************************************************************************/
uint8_t get_detected_key(void)
{
    return (s_rx_buf);
}


static bool key_pressed(void)
{
    return (g_receive_complete);
}
/*********************************************************************************************************************
 *  JLink console callback
 *  @param[IN]   uart_callback_args_t *p_args: callback information
 *  @retval      None
***********************************************************************************************************************/
/* callback from driver */
void jlink_console_callback(uart_callback_args_t *p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if (sizeof(g_out_of_band_received) > g_out_of_band_index)
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
            SCB_InvalidateDCache();
            g_transfer_complete = 1;
            break;
        }
        default:
        {
        }
    }
}



