/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : bg_font_18_full.h
 * Description  : This file defines the exported MIPI LCD text print functions.
 **********************************************************************************************************************/

/* GIMP RGB C-Source image dump (bg_font_18_full.h) */
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"

#ifndef BG_FONT_18_FULL_H_
#define BG_FONT_18_FULL_H_

FSP_CPP_HEADER
void print_bg_font_18(d2_device *handle, d2_point _xs, d2_point _ys, float scaling, char *_str);
FSP_CPP_FOOTER

#endif // BG_FONT_18_FULL_H_

