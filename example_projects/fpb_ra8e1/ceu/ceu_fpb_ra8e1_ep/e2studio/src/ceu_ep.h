/***********************************************************************************************************************
 * File Name    : ceu_ep.h
 * Description  : Contains macros, variables, and functions used in ceu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CEU_EP_H_
#define CEU_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE         (0x00)

/* Macro for the buffer size */
#define BUFF_SIZE           (16U)

/* External variable - Defined in ceu_ep.c */
extern volatile bool g_capture_ready;

/* Function declarations */
fsp_err_t ceu_init(capture_ctrl_t * const p_ctrl, capture_cfg_t const * const p_cfg);
fsp_err_t ceu_capture_start(capture_ctrl_t * const p_ctrl, uint8_t * const p_buffer);
void ceu_deinit(capture_ctrl_t * const p_ctrl);
fsp_err_t iic_master_init(i2c_master_ctrl_t * const p_api_ctrl, i2c_master_cfg_t const * const p_cfg);
void iic_master_deinit(i2c_master_ctrl_t * const p_api_ctrl);
fsp_err_t gpt_init(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t gpt_start(timer_ctrl_t * const p_ctrl);
void gpt_deinit(timer_ctrl_t * const p_ctrl);

#endif /* CEU_EP_H_ */
