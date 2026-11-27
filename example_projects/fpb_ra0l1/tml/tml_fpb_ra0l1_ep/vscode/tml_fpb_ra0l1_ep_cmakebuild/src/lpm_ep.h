/***********************************************************************************************************************
 * File Name    : lpm_ep.h
 * Description  : Contains macros, data structures, and functions used in lpm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LPM_EP_H_
#define LPM_EP_H_

#include "tml_ep.h"
#include "console/uart_console.h"

/* Macros for delay time in application */
#define LPM_TIME_TRANSITION_VALUE       (1000U)
#define LPM_TIME_TRANSITION_UNIT        (BSP_DELAY_UNITS_MILLISECONDS)
#define LPM_NAME_PRINT_TIME             (10U)

/* Enumeration for low power modes use in example project */
typedef enum e_app_lpm_state
{
    APP_LPM_SLEEP_STATE = 0,
    APP_LPM_SW_STANDBY_STATE,
    APP_LPM_MAX_STATE
} app_lpm_states_t;

/* Function declarations */
fsp_err_t lpm_init(app_lpm_states_t lpm_mode);
fsp_err_t lpm_mode_enter(app_lpm_states_t lpm_mode);
void lpm_deinit(void);
fsp_err_t tml_set_timer_cancel_lpm(uint16_t * p_time_cancel_lpm);
fsp_err_t lpm_enter_procedure(void);
fsp_err_t lpm_exit_procedure(void);

#endif /* LPM_EP_H_ */
