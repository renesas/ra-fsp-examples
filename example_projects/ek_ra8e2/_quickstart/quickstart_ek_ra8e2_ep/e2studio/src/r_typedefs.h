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
 *         : 20.05.2025 1.00     First Release
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

#define FULL_IMAGE_WIDTH            (800) /* Full screen image width (in pixels) */
#define FULL_IMAGE_HEIGHT           (480)  /* Full screen image height (in pixels) */

/* in case <stdio.h> has defined it. */
#ifndef NULL
#define NULL (0)
#endif

#if !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
#define     false (0)
#define     true  (1)
#endif

/* Suppress Unused Parameter warning */
#define     UNUSED_PARAM(param)             ((void)(param))

/* Suppress Unused Variable warning */
#define     UNUSED_VARIABLE(param)          ((void)(param))

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef int ( * test_fn)(void);

typedef int          int_t;
typedef unsigned int bool_t;
typedef char         char_t;

/* typedef signed char         int8_t;   defined stdint.h */
/* typedef signed short        int16_t;  defined stdint.h */
/* typedef signed long         int32_t;  defined stdint.h */
/* typedef signed long long    int64_t;  defined stdint.h */

typedef unsigned int        uint_t;
/* typedef unsigned char       uint8_t;   defined stdint.h */
/* typedef unsigned short      uint16_t;  defined stdint.h */
/* typedef unsigned long       uint32_t;  defined stdint.h */
/* typedef unsigned long long  uint64_t;  defined stdint.h */

typedef float       float32_t;
typedef double      float64_t;
typedef long double float128_t;

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint16_t       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    uint8_t        pixel_data[800 * 480 * 2 + 1];
} st_full_image_rgb565_t;

typedef struct
{
    uint16_t       width;
    uint16_t       height;
    uint16_t       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    uint8_t        pixel_data[620 * 223 * 2 + 1];
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
