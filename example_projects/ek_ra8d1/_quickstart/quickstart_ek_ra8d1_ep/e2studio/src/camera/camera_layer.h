/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : camera_layer.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * camera_layer.h
 *
 *  Created on: 20 Apr 2023
 *      Author: b3800117
 */

#include "hal_data.h"
#include "r_typedefs.h"

#include "arducam.h"

#ifndef CAMERA_CAMERA_LAYER_H_
 #define CAMERA_CAMERA_LAYER_H_

typedef struct ov_reg
{
    uint16_t      reg_num;
    unsigned char value;
} st_ov_reg_t;

typedef enum
{
    BSP_CAM_YUV422  = 0,
    BSP_CAM_RAW_RGB = 1,
} bsp_camera_output_t;

typedef enum
{
    POWER_UP   = BSP_IO_LEVEL_LOW,
    POWER_DOWN = BSP_IO_LEVEL_HIGH,
} bsp_camera_power_t;

typedef enum
{
    BSP_CAM_FRAMERATE_10FPS  = 100,
    BSP_CAM_FRAMERATE_20FPS  = 50,
    BSP_CAM_FRAMERATE_33FPS  = 30,
    BSP_CAM_FRAMERATE_40FPS  = 25,
    BSP_CAM_FRAMERATE_50FPS  = 20,
    BSP_CAM_FRAMERATE_100FPS = 10,
} bsp_camera_framerate_t;

typedef enum
{
    BSP_CAM_VGA   = 0,                 // 640x480
    BSP_CAM_CIF   = 1,                 // 352x288
    BSP_CAM_QVGA  = 2,                 // 320x240
    BSP_CAM_QCIF  = 3,                 // 176x144
    BSP_CAM_QQVGA = 4,                 // 160x120
} bsp_camera_resolution_t;

extern ceu_instance_ctrl_t g_ceu_ctrl;

typedef enum
{
    BSP_CAM_VGA_WIDTH    = 640,
    BSP_CAM_VGA_HEIGHT   = 480,
    BSP_CAM_CIF_WIDTH    = 352,
    BSP_CAM_CIF_HEIGHT   = 288,
    BSP_CAM_QVGA_WIDTH   = 320,
    BSP_CAM_QVGA_HEIGHT  = 240,
    BSP_CAM_QCIF_WIDTH   = 176,
    BSP_CAM_QCIF_HEIGHT  = 144,
    BSP_CAM_QQVGA_WIDTH  = 160,
    BSP_CAM_QQVGA_HEIGHT = 120,
} bsp_camera_size_list_t;

typedef enum
{
    BSP_CAM_WIDTH  = BSP_CAM_QVGA_WIDTH,
    BSP_CAM_HEIGHT = BSP_CAM_QVGA_HEIGHT,
} bsp_camera_current_size_t;

 #define BSP_CAM_BYTE_PER_PIXEL    (2)

extern bool_t camera_init(bool_t use_test_mode);

extern void bsp_camera_capture_image(void);

extern void new_frame(void);

#endif                                 /* CAMERA_CAMERA_LAYER_H_ */
