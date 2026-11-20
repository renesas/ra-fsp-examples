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

#include "hal_data.h"

/* Macros for the status flag */
#define SET_FLAG                    (0x01)
#define RESET_FLAG                  (0x00)

/* Macro for timer status */
#define ALREADY_RUNNING             (0x02)

/* Macros for period valid range */
#define TIME_PERIOD_MAX             (2000U)
#define TIME_PERIOD_MIN             (0U)

/* Macro for null character */
#define NULL_CHAR                   ('\0')

/* Macro for user input buffer */
#define BUFFER_SIZE                 (16U)

/* Macro for reset value */
#define RESET_VALUE                 (0x00)

/* Extern variables */
extern bsp_leds_t g_bsp_leds;
extern volatile uint8_t g_periodic_timer_flag;
extern volatile uint32_t g_error_flag;

/* Function declarations */
fsp_err_t agt_init(void);
fsp_err_t agt_start(timer_ctrl_t * const p_ctrl);
fsp_err_t agt_info_get(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info);
fsp_err_t agt_period_set(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);
fsp_err_t agt_status_get(timer_ctrl_t * const p_ctrl, timer_status_t * const p_status);
fsp_err_t agt_stop(timer_ctrl_t * const p_ctrl);
void agt_deinit(void);

#endif /* AGT_EP_H_ */
