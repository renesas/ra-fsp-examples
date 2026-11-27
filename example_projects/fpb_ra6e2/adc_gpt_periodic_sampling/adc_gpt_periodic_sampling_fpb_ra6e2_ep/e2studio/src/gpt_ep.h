/***********************************************************************************************************************
 * File Name    : gpt_ep.h
 * Description  : Contains functions used in gpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_EP_H_
#define GPT_EP_H_

#include "hal_data.h"

/* Function declarations */
fsp_err_t gpt_init(void);
fsp_err_t gpt_start(void);
void gpt_deinit(void);

#endif /* GPT_EP_H_ */
