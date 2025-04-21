/***********************************************************************************************************************
 * File Name    : serial_data.h
 * Description  : Contains declarations of data structures and functions used in serial_data.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#ifndef SERIAL_TERM_SERIAL_DATA_H_
#define SERIAL_TERM_SERIAL_DATA_H_

#include "r_uarta.h"
#include "r_dtc.h"

/* UART on UARTA Instance. */
extern const uart_instance_t g_uart;

/* Access the UARTA instance using these structures when calling API functions directly */
extern uarta_instance_ctrl_t g_uart_ctrl;
extern const uart_cfg_t g_uart_cfg;
extern const uarta_extended_cfg_t g_uart_cfg_extend;

/* Declare callback function of UARTA */
#ifndef serial_callback
void serial_callback(uart_callback_args_t *p_args);
#endif


#endif /* SERIAL_TERM_SERIAL_DATA_H_ */
#endif /* USE_VIRTUAL_COM */
