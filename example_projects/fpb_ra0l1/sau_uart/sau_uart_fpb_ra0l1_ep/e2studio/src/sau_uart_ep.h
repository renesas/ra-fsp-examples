/***********************************************************************************************************************
 * File Name    : sau_uart_ep.h
 * Description  : Contains data structures and functions used in sau_uart_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_UART_EP_H_
#define SAU_UART_EP_H_

#include <stdbool.h>
#include <hal_data.h>

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the closed module */
#define MODULE_CLOSE                (0U)

/* Macros for SAU UART receive */
#define NULL_CHAR                   ('\0')
#define CR_ASCII                    (0x0D)
#define LF_ASCII                    (0x0A)
#define BUFFER_LEN                  (256U)

/* Macros for SAU UART transmit */
#define TIME_OUT_FOR_ONE_BYTE       (1000U)
#define TIME_OUT_VALUE              (1U)
#define TIME_OUT_UNIT               (BSP_DELAY_UNITS_MICROSECONDS)

/* Macro for EP guide */
#define EP_GUIDE                    "\r\nUser input: "

/* Macros for echo message */
#define ECHO_MESSAGE                "\r\nReceived a string from RA board: "
#define ECHO_MESSAGE_LEN            (strlen(ECHO_MESSAGE))

/* Extern variables - defined in sau_uart_ep.c */
extern uint8_t g_rx_buffer[BUFFER_LEN];
extern uint8_t g_tx_buffer[BUFFER_LEN];
extern volatile uint16_t g_rx_len;
extern volatile bool g_rx_flag;

/* Public function declarations */
fsp_err_t sau_uart_init(void);
fsp_err_t sau_uart_send(uint8_t * p_data);
void sau_uart_deinit(void);

#endif /* SAU_UART_EP_H_ */
