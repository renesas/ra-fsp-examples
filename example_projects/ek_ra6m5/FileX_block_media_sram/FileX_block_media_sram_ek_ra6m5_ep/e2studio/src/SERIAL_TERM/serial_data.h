/***********************************************************************************************************************
 * File Name    : serial_data.h
 * Description  : Contains declarations of data structures and functions used in serial_data.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

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
