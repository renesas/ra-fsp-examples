/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : bg_font_18_full.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/* GIMP RGB C-Source image dump (bg_font_18_full.h) */

#include "r_typedefs.h"
#include "bsp_api.h"
#include "dave_driver.h"
#include "gimp.h"
#include "camera_layer.h"
#include "graphics/graphics.h"

#ifndef BG_FONT_18_FULL_H_
 #define BG_FONT_18_FULL_H_

extern bool_t initialise_bg_font_18_full(void);
extern void   print_bg_font_18(d2_device * handle, d2_point _xs, d2_point _ys, char * _str);
extern void   print_bg_font_18_direct(d2_point _xs, d2_point _ys, char * _str);

#endif                                 // BG_FONT_18_FULL_H_
