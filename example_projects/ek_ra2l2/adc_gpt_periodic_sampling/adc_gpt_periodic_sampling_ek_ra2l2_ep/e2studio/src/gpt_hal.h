/***********************************************************************************************************************
 * File Name    : gpt_hal.h
 * Description  : Contains data structures and functions used in gpt_hal.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef GPT_HAL_H_
#define GPT_HAL_H_

/* Function declarations */
fsp_err_t init_hal_gpt(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t gpt_timer_start(timer_ctrl_t * const p_ctrl_timer);
void deinit_hal_gpt(timer_ctrl_t * const p_ctrl_timer );

#endif /* GPT_HAL_H_ */
