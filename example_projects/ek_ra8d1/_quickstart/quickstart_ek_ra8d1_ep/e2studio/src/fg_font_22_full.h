/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : fg_font_22_full.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/* GIMP RGB C-Source image dump (fg_font_22_full.h) */

#include "r_typedefs.h"
#include "bsp_api.h"
#include "dave_driver.h"
#include "gimp.h"
#include "camera_layer.h"
#include "graphics\graphics.h"

#ifndef FG_FONT_22_FULL_H_
 #define FG_FONT_22_FULL_H_

extern bool_t initialise_fg_font_22_full(void);
extern void   print_fg_font_22(uint16_t _xs, uint16_t _ys, char * _str);

#endif                                 // BG_FONT_18_FULL_H_
