/***********************************************************************************************************************
 * File Name    : sau_uart_ep.h
 * Description  : Contains data structures and functions used in sau_uart_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SAU_UART_EP_H_
#define SAU_UART_EP_H_

/* Macro for RTT Viewer */
#define RTT_DELAY_VALUE             (200U)

/* Macros for SAU UART receive */
#define NULL_CHAR                   ('\0')
#define CR_ASCII                    (0x0D)
#define LF_ASCII                    (0x0A)
#define BUFFER_LEN                  (256U)

/* Macros for SAU UART transmit */
#define TIME_OUT_FOR_ONE_BYTE       (1000)
#define TIME_OUT_VALUE              (1U)
#define TIME_OUT_UNIT               (BSP_DELAY_UNITS_MICROSECONDS)

/* Macros for echo message */
#define ECHO_MESSAGE                "Received a string from RA board: "
#define ECHO_MESSAGE_LEN            (strlen(ECHO_MESSAGE))

/* Public function declaration */
void sau_uart_ep_entry(void);

#endif /* SAU_UART_EP_H_ */
