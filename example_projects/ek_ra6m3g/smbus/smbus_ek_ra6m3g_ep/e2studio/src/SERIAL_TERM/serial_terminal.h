/***********************************************************************************************************************
 * File Name    : serial_terminal.h
 * Description  : Contains declarations of data structures and functions used in serial_terminal.c
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#ifndef SER_TERM_SERIAL_TERMINAL_H_
#define SER_TERM_SERIAL_TERMINAL_H_

/* Public function declarations */
#include "serial_data.h"

fsp_err_t uart_init (void);
void uart_deinit (void);
uint8_t serial_write_message (char * format, ...);

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
