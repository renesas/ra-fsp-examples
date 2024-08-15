/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : display_mipi_ili9806e.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * mipi_display_data.h
 *
 *  Created on: Aug 17, 2023
 *      Author: Austin.Hansen
 */

#ifndef DISPLAY_MIPI_ILI9806E_DISPLAY_MIPI_ILI9806E_H_
#define DISPLAY_MIPI_ILI9806E_DISPLAY_MIPI_ILI9806E_H_

#include "r_mipi_dsi.h"
#include "r_glcdc.h"

fsp_err_t display_mipi_ili9806e_init(display_instance_t const * const p_display);

#endif                                 /* DISPLAY_MIPI_ILI9806E_DISPLAY_MIPI_ILI9806E_H_ */
