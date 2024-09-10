/***********************************************************************************************************************
 * File Name    : timer_initialise.h
 * Description  : Contains data structures and functions used in timer_initialise.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#ifndef TIMER_INITIALISE_H_
#define TIMER_INITIALISE_H_

/* AGT timer initialization function */
fsp_err_t agt_timer_init(void);

/* AGT timer deinitialisation function */
void agt_timer_deinit(void);

#endif

