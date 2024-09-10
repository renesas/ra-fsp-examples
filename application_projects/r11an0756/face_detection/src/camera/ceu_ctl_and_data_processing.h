/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : ceu_ctl_and_data_processing.h
 * Description  : This file defines the ceu control and data processing functions.
 **********************************************************************************************************************/

#ifndef CEU_CTL_AND_DATA_PROCESSING_H_
#define CEU_CTL_AND_DATA_PROCESSING_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "hal_data.h"
#include "jlink_console.h"
#include "common_util.h"


/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define IMG_CHANNEL 2

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/


/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/


/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

extern uint8_t  camera_out_buffer[2][CAM_QVGA_WIDTH  * CAM_QVGA_HEIGHT * CAM_BYTE_PER_PIXEL];
extern int8_t   model_buffer_int8[];
extern uint8_t  camera_out_rot_buffer565[ CAM_QVGA_WIDTH  * CAM_QVGA_HEIGHT * CAM_BYTE_PER_PIXEL];
extern uint8_t s_ceu_buffer;
extern face_det_err_t rotate_camera_image(void);
extern face_det_err_t image_rgb565_to_int8 (const void * inbuf, void * outbuf, uint16_t in_width, uint16_t in_height, uint16_t out_width, uint16_t out_height);

#endif /* CEU_CTL_AND_DATA_PROCESSING_H_ */
