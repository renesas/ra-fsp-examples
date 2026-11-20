/***********************************************************************************************************************
 * File Name    : gpt_ep.h
 * Description  : Contains macros and functions used in gpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_EP_H_
#define GPT_EP_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

fsp_err_t timer_init(void);
fsp_err_t timer_start_measure(void);
fsp_err_t timer_get_measure(uint32_t * p_time);
void timer_deinit(void);
#endif /* GPT_EP_H_ */
