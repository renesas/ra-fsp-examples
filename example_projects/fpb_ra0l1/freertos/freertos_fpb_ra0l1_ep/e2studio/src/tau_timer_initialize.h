/***********************************************************************************************************************
 * File Name    : tau_timer_initialize.h
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TAU_TIMER_INITIALIZE_H_
#define TAU_TIMER_INITIALIZE_H_

/* Function initializes the timer instance */
fsp_err_t tau_timer_init(tau_instance_ctrl_t * p_timer_ctrl, const timer_cfg_t * p_timer_cfg);

#endif /* TAU_TIMER_INITIALIZE_H_ */
