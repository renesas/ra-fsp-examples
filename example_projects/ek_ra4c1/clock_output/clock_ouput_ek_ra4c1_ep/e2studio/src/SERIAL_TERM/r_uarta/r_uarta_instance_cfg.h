/***********************************************************************************************************************
* File Name    : r_uarta_instance_cfg.h
* Description  : Contains data structures and functions used in r_uarta_instance_cfg.c
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef R_UARTA_INSTANCE_CFG_H_
#define R_UARTA_INSTANCE_CFG_H_

#if (USE_VIRTUAL_COM == 1)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

#if BSP_PERIPHERAL_UARTA_PRESENT
#include "r_uarta.h"
#include "../r_uart_api.h"

#if !defined(BOARD_RA4C1_EK)
/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern const uart_instance_t g_serial;
extern uarta_instance_ctrl_t g_serial_ctrl;
extern const uart_cfg_t g_serial_cfg;
extern const uarta_extended_cfg_t g_serial_cfg_extend;

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
#ifndef serial_callback
void serial_callback(uart_callback_args_t *p_args);
#endif

#endif /* BSP_PERIPHERAL_UARTA_PRESENT */
#endif /* USE_VIRTUAL_COM */
#endif /* R_UARTA_INSTANCE_CFG_H_ */
#endif /* BOARD_RA4C1_EK */
