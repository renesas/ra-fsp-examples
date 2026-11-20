/***********************************************************************************************************************
 * File Name    : glcdc_display.h
 * Description  : Contains variables and functions used in glcdc_display.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GLCDC_DISPLAY_H_
#define GLCDC_DISPLAY_H_

#include "hal_data.h"
#include "user_config.h"

#if (DISPLAY_OUTPUT == 1U)

/* External variable - Defined in glcdc_display.c */
extern volatile uint8_t g_vsync_flag;

/* Function declarations */
fsp_err_t glcdc_init(void);
fsp_err_t glcdc_buffer_change(uint8_t * const p_buffer);

#endif /* DISPLAY_OUTPUT */
#endif /* GLCDC_DISPLAY_H_ */
