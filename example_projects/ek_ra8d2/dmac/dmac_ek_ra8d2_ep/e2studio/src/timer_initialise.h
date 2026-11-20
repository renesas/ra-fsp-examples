/***********************************************************************************************************************
 * File Name    : timer_initialise.h
 * Description  : Contains data structures and functions used in timer_initialise.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TIMER_INITIALISE_H_
#define TIMER_INITIALISE_H_

#include "hal_data.h"

/* This function initializes the GPT timer */
fsp_err_t gpt_timer_init(void);

/* This function de-initializes the opened GPT timer */
void gpt_timer_deinit(void);

/* This function initializes the AGT timer */
fsp_err_t agt_timer_init(void);

/* This function de-initializes the opened AGT timer */
void agt_timer_deinit(void);

#endif /* TIMER_INITIALISE_H_ */
