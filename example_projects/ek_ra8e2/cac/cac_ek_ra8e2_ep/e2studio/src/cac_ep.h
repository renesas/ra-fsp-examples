/***********************************************************************************************************************
 * File Name    : cac_ep.h
 * Description  : Contains declarations of data structures and functions used in cac_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CAC_EP_H_
#define CAC_EP_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"

/* Event flag to identify CAC events */
#define CAC_EVENT_VALUE         (0xFF)

/* To determine number of measurement clocks (CGC) available in CAC configurator */
#define TARGET_CGC_CLK_CNT      (0x06)

/* Reset value for variables */
#define RESET_VALUE             (0x00)

/* Extern variable - defined in cac_ep.c */
extern uint32_t g_cac_counter_val;

/* Function declarations */
fsp_err_t cgc_init(void);
fsp_err_t cac_init(void);
fsp_err_t cac_measurement_process(void);
void cac_deinit(void);
void cgc_deinit(void);

#endif /* CAC_EP_H_ */
