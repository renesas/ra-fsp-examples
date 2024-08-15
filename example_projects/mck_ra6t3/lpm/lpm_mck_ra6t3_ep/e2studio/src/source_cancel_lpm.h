/***********************************************************************************************************************
 * File Name    : source_cancel_lpm.h
 * Description  : Contains macros, data structures and functions used in source_cancel_lpm.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SOURCE_CANCEL_LPM_H_
#define SOURCE_CANCEL_LPM_H_

#include "common_utils.h"

/**********************************************************************************************************************
* Macro definitions
**********************************************************************************************************************/
/* Macro for check DTC transfer */
#define DTC_DATA_RESET                           (0x0000)
#define DTC_DATA_VALUE                           (0xAAAA)

/**********************************************************************************************************************
* External variables
**********************************************************************************************************************/
extern volatile bool g_source_flag;

/**********************************************************************************************************************
* Public functions declarations
**********************************************************************************************************************/
fsp_err_t source_cancel_lpm_init(void);
void source_cancel_lpm_deinit(void);
fsp_err_t source_cancel_lpm_enable(void);
fsp_err_t source_cancel_lpm_disable(void);
fsp_err_t source_cancel_snooze_enable(void);
fsp_err_t source_cancel_snooze_disable(void);
void source_cancel_lpm_detect(void);
void source_cancel_lpm_flag_clear(void);
void source_internal_reset_handle(void);

#endif /* SOURCE_CANCEL_LPM_H_ */
