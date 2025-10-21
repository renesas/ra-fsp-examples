/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_typedefs.h
 * Version      : 1.0
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "common_data.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef RENESAS_APPLICATION_SYSTEM_INC_R_TYPEDEFS_H_
#define RENESAS_APPLICATION_SYSTEM_INC_R_TYPEDEFS_H_


/* Suppress compiler warning: unused parameter 'xxx' [-Wunused-parameter] */
#define     UNUSED_PARAM(param)             ((void)(param))

#define FULL_IMAGE_WIDTH            (1024) /* Full screen image width (in pixels) */
#define FULL_IMAGE_HEIGHT           (600)  /* Full screen image height (in pixels) */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef int ( * test_fn)(void);

typedef int          int_t;
typedef unsigned int bool_t;
typedef char         char_t;

typedef float       float32_t;
typedef double      float64_t;
typedef long double float128_t;

/* Image prototypes plcd screen 1024x600*/
/* trigraph ??/ ignored, use -trigraphs to enable fix write an occurrence of "??" as "?\?" */
typedef struct  {
  uint16_t     width;
  uint16_t     height;
  uint16_t     bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  uint8_t      pixel_data[1024 * 600 * 2 + 1];
} st_plcd_full_screen_t;

typedef struct  {
  uint16_t     width;
  uint16_t     height;
  uint16_t     bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  uint8_t      pixel_data[1024 * 600 * 3  + 1];
} st_plcd_24bpp_full_screen_t;

typedef struct  {
  uint16_t     width;
  uint16_t     height;
  uint16_t     bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  uint16_t     reserved;
  uint8_t      pixel_data[1024 * 600 * 4  + 1];
} st_plcd_rgba_32b_screen_t;


/* Image prototypes mipi screen 854x480 Rotated */
typedef struct  {
  uint16_t     width;
  uint16_t     height;
  uint16_t     bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  uint8_t      pixel_data[480 * 854 * 2  + 1];
} st_mipi_full_screen_t;

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint16_t       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    uint8_t        pixel_data[DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1 * 2 + 1];
} st_full_image_rgb565_t;

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint16_t       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    uint8_t        pixel_data[768 * 278 * 2 + 1];
} st_overlay_image_rgb565_t;

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint16_t       offset;
    uint16_t       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    uint8_t        pixel_data[40 * 48 * 2 + 1];
} st_font_title_rgb565_t;

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint16_t       offset;
    uint16_t       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    uint8_t        pixel_data[26 * 34 * 2 + 1];
} st_font_body_rgb565_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* RENESAS_APPLICATION_SYSTEM_INC_R_TYPEDEFS_H_ */

