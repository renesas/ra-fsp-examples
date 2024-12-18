/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : display_mipi.h
 * Description  : This file provides the exported mipi control functions.
 **********************************************************************************************************************/

#ifndef DISPLAY_MIPI_H_
#define DISPLAY_MIPI_H_

#include "common_util.h"
#include "hal_data.h"

face_det_err_t display_mipi_focuslcd_init(display_instance_t const * const p_display);
void mipi_dsi_enable_backlight(void);
void mipi_dsi_hw_reset(void);

#endif /* DISPLAY_MIPI_H_ */
