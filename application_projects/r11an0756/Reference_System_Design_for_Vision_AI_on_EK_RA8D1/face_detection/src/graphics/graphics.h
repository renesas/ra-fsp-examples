/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : graphics.h
 * Description  : This file defines the definitions and graphic system initialization functions and frame update service.
 **********************************************************************************************************************/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include "hal_data.h"
#include "common_util.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define LCD_HPIX       (480)
#define LCD_STRIDE     (480)
#define LCD_VPIX       (854)

#define LCD_COLOR_SIZE (2)
#define LCD_BUF_NUM    (2)

#define SCREEN_MODE d2_mode_rgb565

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
extern d2_device * d2_handle;

extern bool in_transition(void);
extern void graphics_start_frame();
extern void graphics_end_frame();
extern void graphics_swap_buffer();
extern face_det_err_t drw_display_init(void);

#endif
