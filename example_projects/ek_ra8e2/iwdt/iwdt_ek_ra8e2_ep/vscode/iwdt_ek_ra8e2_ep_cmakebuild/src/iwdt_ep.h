/***********************************************************************************************************************
 * File Name    : iwdt_ep.h
 * Description  : Contains declarations of data structures and functions used in iwdt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IWDT_EP_H_
#define IWDT_EP_H_

#include "hal_data.h"

/* IWDT/NMI detect reset value */
#define SYSTEM_RSTSR1_IWDTRF_DETECT_RESET      (1U)
#define SYSTEM_RSTSR1_SWRF_DETECT_RESET        (1U)

#define RESET_VALUE                            (0x00)

/* Reset cause returned by check_reset_status() */
typedef enum
{
    RESET_NONE   = 0,
    RESET_IWDT   = 1,
    RESET_NMI_SW = 2
} reset_cause_t;

/* Function declarations */
fsp_err_t iwdt_open(void);
fsp_err_t iwdt_refresh(void);
void enable_iwdt_count_in_debug_mode(void);
reset_cause_t check_reset_status(void);

#endif /* IWDT_EP_H_ */
