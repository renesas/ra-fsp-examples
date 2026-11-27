/***********************************************************************************************************************
 * File Name    : uart_ep.h
 * Description  : Contains macro definitions and function declarations for uart_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UART_EP_H_
#define UART_EP_H_

#include "hal_data.h"

/* Carriage return */
#define CARRIAGE_ASCII              (13U)

/* ASCII value of zero */
#define ZERO_ASCII                  (48U)

/* ASCII value for nine */
#define NINE_ASCII                  (57U)

 /* Expected input data length */
#define DATA_LENGTH                 (4U)

/* UART error event bits mapped in registers */
#define UART_ERROR_EVENTS           (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING |\
                                     UART_EVENT_ERR_PARITY)

#define RESET_VALUE                 (0x00)

#define EP_INFO                     "\r\nValid input range: 1 - 100 to set LED intensity."\
                                    "\r\nUser input: "

#define EP_GUI                      "\r\nInvalid input. Valid input range is from 1 - 100."\
                                    "\r\n\r\nUser input: "

#if (BSP_PERIPHERAL_SCI_B_PRESENT)
#define R_SCI_UART_OPEN(ctrl, cfg)                          (R_SCI_B_UART_Open((ctrl), (cfg)))
#define R_SCI_UART_WRITE(ctrl, msg, msg_len)                (R_SCI_B_UART_Write((ctrl), (msg), (msg_len)))
#define R_SCI_UART_CLOSE(ctrl)                              (R_SCI_B_UART_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_PRESENT)
#define R_SCI_UART_OPEN(ctrl, cfg)                          (R_SCI_UART_Open((ctrl), (cfg)))
#define R_SCI_UART_WRITE(ctrl, msg, msg_len)                (R_SCI_UART_Write((ctrl), (msg), (msg_len)))
#define R_SCI_UART_CLOSE(ctrl)                              (R_SCI_UART_Close(ctrl))
#endif /* BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT */

/* Function declarations */
fsp_err_t uart_operation(void);
fsp_err_t uart_print_user_msg(const char *p_msg);
fsp_err_t uart_init(void);
fsp_err_t uart_write(uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes);
void uart_deinit(void);

#endif /* UART_EP_H_ */
