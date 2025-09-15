/***********************************************************************************************************************
 * File Name    : tau_hal.h
 * Description  : Contains data structures and functions used in tau_hal.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TAU_HAL_H_
#define TAU_HAL_H_

/* Function declarations */
fsp_err_t init_hal_tau(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t tau_timer_start(timer_ctrl_t * const p_ctrl_timer);
void deinit_hal_tau(timer_ctrl_t * const p_ctrl_timer );

#endif /* TAU_HAL_H_ */
