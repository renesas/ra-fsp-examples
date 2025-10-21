/***********************************************************************************************************************
 * File Name    : timer_initialise.h
 * Description  : Contains data structures and functions used in timer_initialise.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_INITIALISE_H_
#define TIMER_INITIALISE_H_

/* GPT timer initialization function */
fsp_err_t gpt_timer_init(void);

/* GPT timer de-initialization function */
void gpt_timer_deinit(void);

/* AGT timer initialization function */
fsp_err_t agt_timer_init(void);

/* AGT timer de-initialization function */
void agt_timer_deinit(void);

#endif
