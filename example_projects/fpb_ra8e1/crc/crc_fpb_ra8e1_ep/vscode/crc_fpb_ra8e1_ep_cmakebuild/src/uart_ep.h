/***********************************************************************************************************************
 * File Name    : uart_ep.h
 * Description  : Contains macros, variables, and functions used in uart_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef UART_EP_H_
#define UART_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macros based on the specific UART version */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
#define INTERFACE                                       "SCI_B interface"
#define LOOKBACK                                        "SCI_B_UART lookback connection"
#define R_UART_OPEN(ctrl, cfg)                          (R_SCI_B_UART_Open((ctrl), (cfg)))
#define R_UART_WRITE(ctrl, src, len)                    (R_SCI_B_UART_Write((ctrl), (src), (len)))
#define R_UART_READ(ctrl, dest, len)                    (R_SCI_B_UART_Read((ctrl), (dest), (len)))
#define R_UART_CLOSE(ctrl)                              (R_SCI_B_UART_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_PRESENT)
#define INTERFACE                                       "SCI interface"
#define LOOKBACK                                        "SCI UART lookback connection"
#define R_UART_OPEN(ctrl, cfg)                          (R_SCI_UART_Open((ctrl), (cfg)))
#define R_UART_WRITE(ctrl, src, len)                    (R_SCI_UART_Write((ctrl), (src), (len)))
#define R_UART_READ(ctrl, dest, len)                    (R_SCI_UART_Read((ctrl), (dest), (len)))
#define R_UART_CLOSE(ctrl)                              (R_SCI_UART_Close(ctrl))
#else
#define INTERFACE                                       "SAU interface"
#define LOOKBACK                                        "SAU UART lookback connection"
#define R_UART_OPEN(ctrl, cfg)                          (R_SAU_UART_Open((ctrl), (cfg)))
#define R_UART_WRITE(ctrl, src, len)                    (R_SAU_UART_Write((ctrl), (src), (len)))
#define R_UART_READ(ctrl, dest, len)                    (R_SAU_UART_Read((ctrl), (dest), (len)))
#define R_UART_CLOSE(ctrl)                              (R_SAU_UART_Close(ctrl))
#endif /* UART version */

/* Extern variables - defined in uart_ep.c */
extern volatile bool g_b_uart_rx_flag;
extern volatile bool g_b_uart_tx_flag;

/* Function declarations */
fsp_err_t uart_init(void);
fsp_err_t uart_write(uint8_t const * const p_src, uint32_t const bytes);
fsp_err_t uart_read(uint8_t * const p_dest, uint32_t const bytes);
void uart_deinit(void);

#endif /* UART_EP_H_ */
