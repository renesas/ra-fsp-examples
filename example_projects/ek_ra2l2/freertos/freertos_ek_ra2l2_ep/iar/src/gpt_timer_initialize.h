/***********************************************************************************************************************
 * File Name    : gpt_timer_initialize.h
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef GPT_TIMER_INITIALIZE_H_
#define GPT_TIMER_INITIALIZE_H_

/* Function initializes the timer instance */
fsp_err_t gpt_timer_init(gpt_instance_ctrl_t * p_timer_ctrl, const timer_cfg_t * p_timer_cfg);

#endif /* GPT_TIMER_INITIALIZE_H_ */
