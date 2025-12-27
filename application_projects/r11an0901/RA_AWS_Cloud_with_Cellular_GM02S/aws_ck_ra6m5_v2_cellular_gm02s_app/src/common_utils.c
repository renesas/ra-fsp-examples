/***********************************************************************************************************************
 * File Name    : common_utils.c
 * Description  : Contains data structures and functions used common to the AP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

/* Flag for user callback */
static volatile uint8_t g_uart_event = RESET_VALUE;

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

/*******************************************************************************************************************//**
 * @brief       Print user data on UART.
 * @param[in]   p_msg               Pointer to user msg
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t uart_print_user_msg(uint8_t *p_msg)
{
    fsp_err_t err   = FSP_SUCCESS;
    uint8_t msg_len = RESET_VALUE;
    uint32_t local_timeout = (DATA_LENGTH * UINT16_MAX);
    char *p_temp_ptr = (char *)p_msg;

    /* Calculate length of message received */
    msg_len = ((uint8_t)(strlen(p_temp_ptr)));

    /* Reset callback capture variable */
    g_uart_event = RESET_VALUE;

    /* Writing to terminal */
    err = R_SCI_UART_Write (&g_console_uart_ctrl, p_msg, msg_len);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Write API Failed  **\r\n");
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
