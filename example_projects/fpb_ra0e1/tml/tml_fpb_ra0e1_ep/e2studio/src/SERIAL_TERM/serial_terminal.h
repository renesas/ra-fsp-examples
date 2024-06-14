/***********************************************************************************************************************
 * File Name    : serial_terminal.h
 * Description  : Contains declarations of data structures and functions used in serial_terminal.c
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
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#ifndef SER_TERM_SERIAL_TERMINAL_H_
#define SER_TERM_SERIAL_TERMINAL_H_

/* Public function declarations */

fsp_err_t uart_init (void);
void uart_deinit (void);
uint8_t serial_write_message (char * format, ...);
#include "serial_data.h"

/* Macro for UART */
#define UART_EP_INFO_SIZE               (512U)
#define UART_ERROR_CODE_SIZE            (32U)
#define UART_ERROR_INFO_SIZE            (128U)
#define UART_TIMEOUT_ONE_BYTE           (1000)
#define UART_CHAR_TIMEOUT_VALUE         (1U)
#define UART_CHAR_TIMEOUT_UNIT          (BSP_DELAY_UNITS_MICROSECONDS)
#define BUFFER_SIZE_UP                  (2048)
#define BUFFER_SIZE_DOWN                (16)

/* Macro for ASCII character */
#define ENTER_KEY                 ('\r')

#endif /* SER_TERM_SERIAL_TERMINAL_H_ */
#endif /* USE_VIRTUAL_COM */
