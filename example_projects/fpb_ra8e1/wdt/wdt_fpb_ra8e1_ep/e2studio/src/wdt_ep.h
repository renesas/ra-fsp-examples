/***********************************************************************************************************************
 * File Name    : wdt_ep.h
 * Description  : Contains data structures and functions used in wdt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef WDT_EP_H_
#define WDT_EP_H_

#include "hal_data.h"

/* Macros for the reset value */
#define RESET_VALUE                             (0x00)

/* Macros for the status flag */
#define SET_FLAG                                (0x01)
#define RESET_FLAG                              (0x00)

/* WDT detect reset value */
#define SYSTEM_RSTSR1_WDTRF_DETECT_WDT_RESET    (1U)

/* Function declarations */
fsp_err_t init_wdt_module(void);
void check_reset_status(void);
void enable_wdt_count_in_debug_mode(void);

#endif /* WDT_EP_H_ */
