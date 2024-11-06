/**********************************************************************************************************************
 * File Name    : serial_data.h
 * Description  : Contains declarations of data structures and functions used in serial_data.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#ifndef SERIAL_TERM_SERIAL_DATA_H_
#define SERIAL_TERM_SERIAL_DATA_H_

#include "r_sci_b_uart.h"
#include "r_dtc.h"

/* UART Instance. */
extern const uart_instance_t g_serial;

/* Access the UART instance using these structures when calling API functions directly */
extern sci_b_uart_instance_ctrl_t g_serial_ctrl;
extern const uart_cfg_t g_serial_cfg;
extern const sci_b_uart_extended_cfg_t g_serial_cfg_extend;

/* Declare callback function of UART */
#ifndef serial_callback
void serial_callback(uart_callback_args_t *p_args);
#endif

/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer_rx;
extern const transfer_instance_t g_transfer_tx;

/* Access the DTC instance using these structures when calling API functions directly. */
extern dtc_instance_ctrl_t g_transfer_rx_ctrl;
extern const transfer_cfg_t g_transfer_rx_cfg;

extern dtc_instance_ctrl_t g_transfer_tx_ctrl;
extern const transfer_cfg_t g_transfer_tx_cfg;

#endif /* SERIAL_TERM_SERIAL_DATA_H_ */
#endif /* USE_VIRTUAL_COM */
