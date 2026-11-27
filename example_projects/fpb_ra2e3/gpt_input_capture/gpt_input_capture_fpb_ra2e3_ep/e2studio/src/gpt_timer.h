/***********************************************************************************************************************
 * File Name    : gpt_timer.h
 * Description  : Contains declarations of data structures and functions used in gpt_timer.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_TIMER_H_
#define GPT_TIMER_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Extern variables - defined in gpt_timer.c */
extern volatile bool g_b_start_measurement;
extern uint32_t g_capture_overflow;
extern uint64_t g_capture_count;

/* Function declarations */
fsp_err_t init_gpt(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t enable_gpt(timer_ctrl_t * const p_ctrl);
fsp_err_t start_gpt(timer_ctrl_t * const p_ctrl);
fsp_err_t info_get_gpt(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info);
void deinit_gpt(void);

#endif /* GPT_TIMER_H_ */
