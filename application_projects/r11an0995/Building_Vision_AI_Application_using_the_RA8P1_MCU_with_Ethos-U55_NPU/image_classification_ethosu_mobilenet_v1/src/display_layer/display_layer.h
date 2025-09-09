/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : display_layer.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#ifndef __DISPLAY_LAYER_H__
#define __DISPLAY_LAYER_H__

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
extern d2_device * d2_handle;


fsp_err_t drw_init(void);
fsp_err_t initialise_display(void);
void display_image_buffer_initialize(void);
void graphics_start_frame(void);
void graphics_end_frame(void);

void graphics_swap_buffer(void);


#endif /*__DISPLAY_LAYER_H__*/
