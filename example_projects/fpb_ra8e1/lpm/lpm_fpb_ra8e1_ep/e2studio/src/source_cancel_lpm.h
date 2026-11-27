/***********************************************************************************************************************
 * File Name    : source_cancel_lpm.h
 * Description  : Contains macros, variables, and functions commonly used in source_cancel_lpm.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SOURCE_CANCEL_LPM_H_
#define SOURCE_CANCEL_LPM_H_

#include "lpm_ep.h"

/* Macros for check DTC transfer */
#define DTC_DATA_RESET      (0x0000)
#define DTC_DATA_VALUE      (0xAAAA)

/* Extern variable */
extern transfer_info_t g_dtc_cancel_snooze_info;

/* Extern variables - Defined in source_cancel_lpm.c */
extern volatile bool g_source_flag;
extern volatile bool g_external_irq_flag;
extern volatile bool g_timer_flag;

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
extern volatile bool g_snooze_flag;
extern uint16_t g_dtc_dest_data;
extern uint16_t g_dtc_src_data;
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

/* Function declarations */
fsp_err_t source_cancel_lpm_init(void);
fsp_err_t source_cancel_lpm_enable(void);
fsp_err_t source_cancel_lpm_disable(void);
fsp_err_t source_cancel_snooze_enable(void);
fsp_err_t source_cancel_snooze_disable(void);
void source_cancel_lpm_deinit(void);
void source_cancel_lpm_flag_clear(void);
void source_internal_reset_handle(void);

#endif /* SOURCE_CANCEL_LPM_H_ */
