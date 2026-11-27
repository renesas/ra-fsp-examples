/***********************************************************************************************************************
 * File Name    : agt_ep.h
 * Description  : Contains declarations of data structures and functions used in agt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AGT_EP_H_
#define AGT_EP_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Macros for the status flag */
#define RESET_FLAG                  (0x00)
#define SET_FLAG                    (0x01)

/* Macro for timer status */
#define ALREADY_RUNNING             (0x02)

/* Macros for period valid range */
#define TIME_PERIOD_MAX             (2000U)
#define TIME_PERIOD_MIN             (0U)

/* Macro for the null character */
#define NULL_CHAR                   ('\0')

/* Macro for user input buffer */
#define BUFFER_SIZE                 (16U)

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Extern variable - Used for the on-board LED */
extern bsp_leds_t g_bsp_leds;

/* Extern variables - Defined in agt_ep.c */
extern volatile uint8_t g_periodic_timer_flag;
extern volatile bool g_error_flag;

/* Function declarations */
fsp_err_t agt_init(void);
fsp_err_t agt_start_oneshot_timer(void);
void one_shot_timer_callback(timer_callback_args_t *p_args);
void periodic_timer_callback(timer_callback_args_t *p_args);
void agt_deinit(void);

#endif /* AGT_EP_H_ */
