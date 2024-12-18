/***********************************************************************************************************************
 * File Name    : common_utils.c
 * Description  : Contains data structures and functions used common to the AP
 ***********************************************************************************************************************/
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
 * Copyright (C) 2019-2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "r_sci_uart.h"
#include "Console_Thread.h"
#include "r_typedefs.h"

const char_t * const gp_cursor_store = "\x1b[s";
const char_t * const gp_cursor_restore = "\x1b[u";
const char_t * const gp_cursor_temp = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[9;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[10;41H\x1b[K";

const char_t * const gp_green_fg = "\x1b[32m";
const char_t * const gp_white_fg = "\x1b[37m";

const char_t * const gp_clear_screen = "\x1b[2J";
const char_t * const gp_cursor_home = "\x1b[H";

 char __buff[1024];

 int __printf (const char *format, ...);
/*******************************************************************************************************************//**
 * @brief       Initialize  UART.
 * @param[in]   None
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t uart_initialize(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART channel with baud rate 115200 */
    err = R_SCI_UART_Open (&g_console_uart_ctrl, &g_console_uart_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Open API failed  **\r\n");
    }
    return err;
}

/**************************************************************************************
 * @brief    Delay Function:   Produce Delay specified number of microseconds
 * @param[in]  us (uint16_t)   number of microseconds to delay
 * @retval     None
 **************************************************************************************/
void delayMicroseconds(uint32_t us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS);
}

/**************************************************************************************
 * @brief      Delay Function:   delay specified number of Milliseconds
 * @param[in]  ms (uint16_t) number of Milliseconds to delay
 * @retval     None
 **************************************************************************************/
void delay(uint32_t ms)
{
    R_BSP_SoftwareDelay(ms, BSP_DELAY_UNITS_MILLISECONDS);
}

/**************************************************************************************
 * @brief      Print user data over uart
 * @param[in]  format
 * @retval
 **************************************************************************************/
int __printf (const char *format, ...)
{
   va_list arg;
   int done;

   va_start (arg, format);
   done = vsprintf(__buff, format, arg);
   va_end (arg);

   return done;
}
