/***********************************************************************************************************************
 * File Name    : uart_contol.h
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
#include <ui_thread.h>
#include "common_utils.h"

/* Public function declarations */
fsp_err_t uart_init(const uart_instance_t *uart_instance);
fsp_err_t uart_write_message (unsigned char * buffer, uint32_t length, char * format, ...);
void uart_deinit(const uart_instance_t *uart_instance);

/* Macro for UART */
#define UART_EP_INFO_SIZE               (512U)
#define UART_ERROR_CODE_SIZE            (32U)
#define UART_ERROR_INFO_SIZE            (128U)
#define UART_TIMEOUT_ONE_BYTE           (1000)
#define UART_CHAR_TIMEOUT_VALUE         (1U)
#define UART_CHAR_TIMEOUT_UNIT          (BSP_DELAY_UNITS_MICROSECONDS)
#define BUFFER_SIZE_UP                  (2048u)
#define BUFFER_SIZE_DOWN                (32u)

#endif /* UART_CONTROL_H_ */
