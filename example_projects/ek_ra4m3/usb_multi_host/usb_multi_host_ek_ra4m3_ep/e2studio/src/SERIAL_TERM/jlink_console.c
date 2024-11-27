/***********************************************************************************************************************
 * File Name    : jlink_console.c
 * Description  : Handles user input and displays information on the terminal.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "usb_hcdc_hmsc_app.h"

#if (USE_VIRTUAL_COM == 1)
#include "serial_config.h"
#endif

int8_t g_new_api_key_str[KEY_SIZE_IN_BYTES + 8];

#if (USE_VIRTUAL_COM == 1)
static char s_print_buffer[BUFFER_SIZE_UP] ={NULL_CHAR};
static uint8_t g_out_of_band_received[TRANSFER_LENGTH];
static uint32_t g_out_of_band_index = RESET_VALUE;
static volatile bool g_transfer_complete = false;
uint32_t g_receive_complete  = 0;

/**********************************************************************************************************************
 * @brief     This function is called to configure the SCI UART module.
 * @param[IN] None
 * @retval    None
 *********************************************************************************************************************/
void jlink_console_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Configure the UART driver */
    fsp_err = R_SCI_B_UART_Open (&g_serial_ctrl, &g_serial_cfg);
    assert(FSP_SUCCESS == fsp_err);

    R_BSP_PinAccessEnable ();

#ifdef BOARD_RA8D1_EK

    /* Certain SCI9 pins cannot be configured correctly through the pin configurator at the moment. */
    R_IOPORT_PinCfg (&g_ioport_ctrl, BSP_IO_PORT_10_PIN_14,
                     ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9));
    R_IOPORT_PinCfg (&g_ioport_ctrl, BSP_IO_PORT_10_PIN_15,
                     ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9));
#endif
} /* End of function jlink_console_init() */

/*******************************************************************************************************************//**
 *  @brief      This function is used to de-initialize the SCI UART module
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void uart_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize SCI UART if it opened */
    if (MODULE_CLOSE != g_serial_ctrl.open)
    {
        err = R_SCI_B_UART_Close(&g_serial_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_SCI_B_UART_Close API failed\r\n");
        }
    }
}

/**********************************************************************************************************************
 *  @brief      This function is used to transfer a message to the host PC
 *  @param[in]  format   pointer to message buffer
 *  @retval     none
 *********************************************************************************************************************/
void Jlink_console_write(char *format, ...)
{
    uint8_t status = FSP_SUCCESS;
    size_t len = RESET_VALUE;
    unsigned char g_tx_buffer[BUFFER_SIZE_UP] =
    { RESET_VALUE };
    va_list aptr;

    va_start(aptr, format);
    vsprintf ((char*) g_tx_buffer, format, aptr);
    va_end(aptr);

    /* Calculate length of message */
    len = strlen ((char*) g_tx_buffer);

    /* Reset g_transfer_complete flag */
    g_transfer_complete = false;

    /* Write message to UART interface */
    status = (uint8_t) R_SCI_B_UART_Write (&g_serial_ctrl, g_tx_buffer, len);
    assert(FSP_SUCCESS == status);

    /* Check for event transfer complete */
    while (!g_transfer_complete)
    {
        vTaskDelay (1);
    }
} /* End of function jlink_console_init() */

/**********************************************************************************************************************
 *  @brief      This function is used to handle UART events.
 *  @param[in]  p_args
 *  @retval     none
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
            if ((sizeof(g_out_of_band_received)) > g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if (UART_DATA_BITS_8 >= g_serial_cfg.data_bits)
                {
                    g_out_of_band_received[g_out_of_band_index++] = (uint8_t) p_args->data;
                }
                else
                {
                    uint16_t *p_dest = (uint16_t*) &g_out_of_band_received[g_out_of_band_index];
                    *p_dest = (uint16_t) p_args->data;
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
            g_transfer_complete = true;
            break;
        }

        default:
        {
        }
    }
} /* End of function jlink_console_callback() */

/**********************************************************************************************************************
 *  @brief      This function is used to get new characters.
 *  @param[in]  pBuf  Pointer to the buffer where characters will be stored.
 *  @retval     ret   The number of characters retrieved
 *********************************************************************************************************************/
static uint8_t get_new_chars(uint8_t *pBuf)
{
    uint8_t x = 0U;
    uint8_t ret = 0U;

    if (g_out_of_band_index == 0)
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

    memset (g_out_of_band_received, 0, g_out_of_band_index + 1);
    ret = (uint8_t) g_out_of_band_index;

    g_out_of_band_index = 0U;

    return ret;
} /* End of function get_new_chars() */

/**********************************************************************************************************************
 *  @brief      This function is used to get input from the user via a serial port.
 *  @param[in]  is_single_key_input: Only receives 1 byte.
 *  @retval     none.
 *********************************************************************************************************************/
bool handle_virtual_com_input(bool is_single_key_input)
{
    uint8_t fifo_chars[KEY_SIZE_IN_BYTES + 8];
    int8_t  key_str[KEY_SIZE_IN_BYTES + 8];
    char c;
    uint8_t idx = 0U;
    uint16_t x = 0U;
    bool exit_loop = FALSE;
    uint8_t num_new_chars = 0U;
    fsp_err_t status = FSP_SUCCESS;

    if (!is_single_key_input)
    {
        while (exit_loop == false)
        {
            if (false == check_usb_connection (true,true))
            {
                return INPUT_STATUS_NO_INPUT;
            }

            num_new_chars = get_new_chars (fifo_chars);

            if (num_new_chars > 0)
            {
                for (x = 0; x < num_new_chars; x++)
                {
                    c = (char) fifo_chars[x];

                    if (c == CARRIAGE_RETURN)
                    {
                        exit_loop = TRUE;
                    }
                    else if (c == BACKSPACE)
                    {
                        if (idx > 0)
                        {
                            key_str[--idx] = 0;
                            sprintf (s_print_buffer, "%c %c", c, c);
                            Jlink_console_write (s_print_buffer);
                        }
                    }
                    else if (idx < KEY_SIZE_IN_BYTES)
                    {
                        key_str[idx++] = (char) c;
                        sprintf (s_print_buffer, "%c", c);
                        Jlink_console_write (s_print_buffer);
                    }
                    else
                    {
                        /* Length limit reached, do not transfer any more characters to buffer */
                    }
                }

                num_new_chars = 0U;
            }

            vTaskDelay (1);
        }
    }

    else
    {
        g_receive_complete = false;
        status = R_SCI_B_UART_Read (&g_serial_ctrl, (uint8_t*) key_str, 1);
        assert(FSP_SUCCESS == status);

        /* Check for event transfer complete */
        while (!g_receive_complete)
        {
            if (false == check_usb_connection (true,true))
            {
                /* Abort the read process when the USB device is disconnected */
                status = R_SCI_B_UART_Abort(&g_serial_ctrl,UART_DIR_RX);
                assert(FSP_SUCCESS == status);

                return INPUT_STATUS_NO_INPUT;
            }
            vTaskDelay (1);
        }
        Jlink_console_write ((char *)&key_str);
    }

    memcpy(g_new_api_key_str, key_str, sizeof(key_str));
    return INPUT_STATUS_HAVE_INPUT;

}
#endif /* End of USE_VIRTUAL_COM == 1 */

#if (USE_VIRTUAL_COM == 0)

/**********************************************************************************************************************
 *  @brief      This function is used to get input from the user via the J-Link RTT interface.
 *  @param[in]  none
 *  @retval     none
 *********************************************************************************************************************/
bool handle_rtt_segger_input(bool is_single_key_input)
{
    FSP_PARAMETER_NOT_USED(is_single_key_input);
    char rByte[BUFFER_SIZE_DOWN];
    uint32_t num_bytes = BYTES_RECEIVED_ZERO;

    while (BYTES_RECEIVED_ZERO == num_bytes)
    {
        if (false == check_usb_connection (true,true))
        {
            return INPUT_STATUS_NO_INPUT;
        }
        if (APP_CHECK_DATA)
        {
            num_bytes = (uint32_t) APP_READ(rByte);
            if (BYTES_RECEIVED_ZERO == num_bytes)
            {
                APP_PRINT("\r\nNo Input\r\n");
            }
        }
        vTaskDelay (1);
    }
    memcpy (g_new_api_key_str, rByte, (size_t) num_bytes);
    return INPUT_STATUS_HAVE_INPUT;
}
#endif /* End of USE_VIRTUAL_COM == 0 */

void trim_new_line(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == NEWLINE_CHAR) {
        str[len - 1] = NULL_CHAR;
        if (len > 1 && str[len - 2] == CARRIAGE_CHAR) {
            str[len - 2] = NULL_CHAR;
        }
    }
}

/*******************************************************************************************************************
 * End  Of File
 ******************************************************************************************************************/
