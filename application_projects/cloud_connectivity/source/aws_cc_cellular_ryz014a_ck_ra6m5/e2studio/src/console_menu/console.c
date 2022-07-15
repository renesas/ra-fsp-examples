/***********************************************************************************************************************
 * File Name    : console.c
 * Description  : Contains data structures and functions used for console
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "r_typedefs.h"
#include "hal_data.h"
#include "console_thread.h"
#include "console.h"

static uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
static uint32_t g_transfer_complete = 0;
static uint32_t g_receive_complete  = 0;
static uint32_t g_out_of_band_index = 0;

char g_write_buffer[TRANSFER_LENGTH]= {0};
uint8_t g_start_read_flag = false;
uint16_t g_counter = 0;

/*********************************************************************************************************************
 * @brief   open console
 *
 * This function opens SCI uart instance.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If console open is success.
 * @retval      Any other error         If console open is fail.
 *********************************************************************************************************************/
fsp_err_t open_console(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Baud rate is handled in FSP */
    fsp_err = R_SCI_UART_Open (&g_console_uart_ctrl, &g_console_uart_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}

/*********************************************************************************************************************
 * Function Name: get_colour
 *                Get the escape code string for the supplied colour tag
 * @param[in] char *string : the escape code
 * @param[out] bool_t *found : true if the tag was found, false if not
 * @retval the escape code for the colour tag, or the original string if there
 *         was no match
*********************************************************************************************************************/
static const char *get_colour(const char *string, bool_t *found)
{
    const char *colour_codes[] = {"[BLACK]", "\x1B[30m", "[RED]", "\x1B[91m", "[GREEN]", "\x1B[92m", "[YELLOW]", "\x1B[93m",
                                  "[BLUE]", "\x1B[94m", "[MAGENTA]", "\x1B[95m", "[CYAN]", "\x1B[96m", "[WHITE]", "\x1B[97m",
                                  "[ORANGE]", "\x1B[38;5;208m", "[PINK]", "\x1B[38;5;212m", "[BROWN]", "\x1B[38;5;94m",
                                  "[PURPLE]", "\x1B[35m"};
	uint8_t i;

    for (i = 0; i < 12; i++)
    {
        if (0 == strcmp (string, colour_codes[i << 1]))
        {
            *found = true;
            return colour_codes[(i << 1) + 1];
        }
    }

    *found = false;
    return (string);
}

/*********************************************************************************************************************
 * Function Name: detokenise
 *                Replace colour tokens with terminal colour escape codes
 * @param[in] char *input : input string possibly containing colour tokens
 * @param[in] output *output : string with colour tokens replaced with escape codes
 * @retval
 *
*********************************************************************************************************************/
static void detokenise(const char * input, char *output)
{
    int16_t start_bracket_index;
    int16_t end_bracket_index;
    int16_t start_bracket_output_index;
    size_t token_length;
    int16_t i;
    int16_t o;
    bool_t token_found;
    bool_t token_replaced;
    const char_t *colour_code;
    char_t token[MAXIMUM_TOKEN_LENGTH + 1];

    start_bracket_index = -1;
    end_bracket_index = -1;
    start_bracket_output_index = 0;
    o = 0;

    /* scan the input string */
    for (i = 0; '\0' != input[i]; i++)
    {
        token_replaced = false;

        /* token start? */
        if ('[' == input[i])
        {
            start_bracket_index = i;
            start_bracket_output_index = o;
        }

        /* token end? */
        if (']' == input[i])
        {
            end_bracket_index = i;

            /* check to see if we have a token */
            if (start_bracket_index >= 0)
            {
                token_length = (size_t) (end_bracket_index - start_bracket_index + 1);

                if ((token_length >= MINIMUM_TOKEN_LENGTH) && (token_length <= MAXIMUM_TOKEN_LENGTH))
                {
                    /* copy the token to a buffer */
                    strncpy (token, &input[start_bracket_index], token_length);
                    token[token_length] = '\0';

                    /* check for a valid token */
                    colour_code = get_colour (token, &token_found);

                    /* if we have a colour token, then replace it in the output with the associated escape code */
                    if (token_found)
                    {
                        strcpy (&output[start_bracket_output_index], colour_code);
                        o = (int16_t) (start_bracket_output_index + (int16_t) strlen (colour_code));
                        token_replaced = true;
                    }
                }
            }

            /* reset and start looking for another token */
            start_bracket_index = -1;
            end_bracket_index = -1;
        }

        /* if we didn't replace a token, then carry on copying input to output */
        if (!token_replaced)
        {
            output[o] = input[i];
            o++;
        }
    }

    /* terminate the output string */
    output[o] = '\0';
}

/*********************************************************************************************************************
 * @brief   Write data on console.
 *
 * This function write data on console using UART
 * @param[in]   *buffer                 Character pointer
 * @retval      None
 * @retval      None
 *********************************************************************************************************************/
void console_write(const char *buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer_complete = false;

    /* Uart write data */
    err = R_SCI_UART_Write (&g_console_uart_ctrl, (uint8_t*) buffer, strlen (buffer));

    assert(FSP_SUCCESS == err);

    while (!g_transfer_complete)
    {
        R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
    }
}

/*********************************************************************************************************************
 * @brief  wait for key pressed
 *
 * This function wait for key pressed.
 * @param[in]   None
 * @retval      char                    returns key pressed
 *********************************************************************************************************************/
char wait_for_keypress(void)
{
    uint8_t rx_buf = 0;

    /* Wait till UART receive compltetes */
    if (g_receive_complete == true)
        while (!g_receive_complete)
        {
            R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
        }

    g_receive_complete = false;

    /* Read UART data */
    R_SCI_UART_Read (&g_console_uart_ctrl, &rx_buf, 1);

    while (!g_receive_complete)
    {
        R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
    }

    return ((char) rx_buf);
}
/******************************************************************************
 * End of function wait_for_keypress
 ******************************************************************************/


#ifdef USE_DEBUG_CONSOLE
static void printf_colour_debug(const char *format, ...)
{
    va_list arglist;
    static char colour_format[PRINT_BUFFER];
    static char final_buffer[PRINT_BUFFER];

    va_start(arglist, format);

    /* replace colour tokens with terminal colour escape codes */
    detokenise(format, colour_format);

    vsprintf(final_buffer, colour_format, arglist);
    va_end(arglist);

    console_write(final_buffer);
}
#endif // USE_DEBUG_CONSOLE

/*****************************************************************************
 * Function Name: printf_colour
 *                As printf, but replaces colour tags with escape codes
 * @param[in] char *format : the format string
 * @param[in] ... : argument list, 0 or more parameters
 * @retval None
 ******************************************************************************/
void printf_colour(const char *format, ...)
{
    va_list arglist;
    static char colour_format[PRINT_BUFFER];
    static char final_buffer[PRINT_BUFFER];

    if (g_console_out_mutex != NULL)
    {
        /* See if we can obtain the mutex.  If the mutex is not available
         * wait 10 ticks to see if it becomes free.
         */
        if ( xSemaphoreTakeRecursive( g_console_out_mutex, ( TickType_t ) 10 ) == pdTRUE)
        {
            /*We were able to obtain the mutex and can now access the
             * shared resource.
             */

            va_start(arglist, format);

            /* replace colour tokens with terminal colour escape codes */
            detokenise (format, colour_format);

            vsprintf (final_buffer, colour_format, arglist);
            va_end(arglist);

#ifdef USE_DEBUG_INFO_CONSOLE
            TaskHandle_t h_task = xTaskGetCurrentTaskHandle();
            printf_colour_debug("[%p] ", h_task);
#endif // USE_DEBUG_INFO_CONSOLE

            console_write (final_buffer);

            xSemaphoreGiveRecursive(g_console_out_mutex);
            /* Now the mutex can be taken by other tasks */
        }
        else
        {
            /*
             *  We could not obtain the mutex and can therefore not access
             *  the shared resource safely.
             */
        }
    }
}
/******************************************************************************
 * End of function printf_colour
 ******************************************************************************/


/*********************************************************************************************************************
 * @brief  Callback from driver
 *
 * @param[in]   p_args                  Pointer to callback parameters
 * @retval
 *********************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args)
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
                if (UART_DATA_BITS_8 >= g_console_uart_cfg.data_bits)
                {
                    g_out_of_band_received[g_out_of_band_index] = (uint8_t) p_args->data;
                    if (g_start_read_flag == true)
                    {
                        /* Fill received data to write buffer when AWS credentials are being saved */
                        g_write_buffer[g_counter++] = (char) g_out_of_band_received[g_out_of_band_index];
                    }
                    g_out_of_band_index++;
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
            g_transfer_complete = 1;
            break;
        }
        default:
        {
        }
    }
}

/*****************************************************************************
 * Function Name: clear_receive_buffer
 * Clear buffer when AWS credentials are stored in flash
 * @param[in]
 * @param[in]
 * @retval None
 ******************************************************************************/
void clear_receive_buffer (void)
{
    memset (g_out_of_band_received, 0, TRANSFER_LENGTH);
    g_out_of_band_index = 0;
}
