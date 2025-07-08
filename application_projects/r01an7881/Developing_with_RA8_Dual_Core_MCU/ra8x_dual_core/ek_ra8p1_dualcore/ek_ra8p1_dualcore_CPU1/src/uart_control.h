/***********************************************************************************************************************
 * File Name    : uart_control.h
 * Description  :This file contains the declarations for UART initialization,
 *               transmission, and reception used to interface with a terminal
 *               over a UART peripheral.
 * This header file is intended for use with the corresponding implementation
 * in uart_control.c.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UART_CONTROL_H_
#define UART_CONTROL_H_
#include <apn_cpu1.h>

/* Macro for UART control */
#define UART_EP_INFO_SIZE               (512U)
#define UART_ERROR_CODE_SIZE            (32U)
#define UART_ERROR_INFO_SIZE            (128U)
#define UART_TIMEOUT_ONE_BYTE           (1000)
#define UART_CHAR_TIMEOUT_VALUE         (1U)
#define UART_CHAR_TIMEOUT_UNIT          (BSP_DELAY_UNITS_MICROSECONDS)

/* Macro for ASCII character */
#define CR                              ('\r')
#define LF                              ('\n')
#define BACKSPACE                       (0x08)

/* Public function declarations */
fsp_err_t uart_init (void);
fsp_err_t uart_write_message (char * format, ...);
void      uart_deinit (void);

#endif /* UART_CONTROL_H_ */
