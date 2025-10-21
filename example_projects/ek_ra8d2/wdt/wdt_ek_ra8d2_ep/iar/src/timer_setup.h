/***********************************************************************************************************************
 * File Name    : timer_setup.h
 * Description  : Contains data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

/* WDT detect reset value */
#define SYSTEM_RSTSR1_WDTRF_DETECT_WDT_RESET        (1u)

#define BUFFER_SIZE                                 (16u)

/* Number of counts for printing WDT refresh status */
#define WDT_REFRESH_COUNTER_VALUE                   (3u)

/* The user command input value */
#define ENABLE_WDT                                  (1u)

/* Function declarations */
fsp_err_t init_gpt_module(void);
fsp_err_t timer_start(void);
void deinit_gpt_module(void);

#endif /* TIMER_SETUP_H_ */
