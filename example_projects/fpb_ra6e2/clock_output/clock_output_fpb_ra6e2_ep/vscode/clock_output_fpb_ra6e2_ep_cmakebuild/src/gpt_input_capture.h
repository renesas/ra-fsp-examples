/***********************************************************************************************************************
 * File Name    : gpt_input_capture.h
 * Description  : Contains declarations of data structures and functions used in gpt_input_capture.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_INPUT_CAPTURE_H_
#define GPT_INPUT_CAPTURE_H_

#if (GPT_MEASURE_CLKOUT)

#include "hal_data.h"

/* Extern variable - defined in gpt_input_capture.c */
extern uint32_t g_clkout_value;

/* Public function declarations */
fsp_err_t gpt_capture_operation(void);
void gpt_deinit(void);

#endif /* GPT_MEASURE_CLKOUT */

#endif /* GPT_INPUT_CAPTURE_H_ */
