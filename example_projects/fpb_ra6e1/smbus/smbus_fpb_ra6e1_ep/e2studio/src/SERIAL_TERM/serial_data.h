/***********************************************************************************************************************
 * File Name    : serial_data.h
 * Description  : Contains declarations of data structures and functions used in serial_data.c
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#ifndef SERIAL_TERM_SERIAL_DATA_H_
#define SERIAL_TERM_SERIAL_DATA_H_

/* This content is copied from hal_data.h */
#include "r_sci_b_uart.h"
#include "r_dtc.h"

/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer1;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer1_ctrl;
extern const transfer_cfg_t g_transfer1_cfg;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;
/** UART on SCI Instance. */
extern const uart_instance_t g_uart;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_uart_instance_ctrl_t g_uart_ctrl;
extern const uart_cfg_t g_uart_cfg;
extern const sci_b_uart_extended_cfg_t g_uart_cfg_extend;

#ifndef serial_callback
void serial_callback(uart_callback_args_t *p_args);
#endif

#endif /* SERIAL_TERM_SERIAL_DATA_H_ */
#endif /* USE_VIRTUAL_COM */
