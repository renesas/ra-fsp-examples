/***********************************************************************************************************************
 * File Name    : ov3640.c
 * Description  : Contains data structures and functions setup OV3640 camera used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ov3640.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup ceu_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
static volatile i2c_master_event_t g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

/* OV3640 YUV format Preview
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_fmt_yuv422 [] = {
    {0x304d, 0x45}, {0x30a7, 0x5e}, {0x3087, 0x16}, {0x309c, 0x1a},
    {0x30a2, 0xe4}, {0x30aa, 0x42}, {0x30b0, 0xff}, {0x30b1, 0xff},
    {0x30b2, 0x10}, {0x300e, 0x32}, {0x300f, 0x21}, {0x3010, 0x20},
    {0x3011, 0x04}, {0x304c, 0x81}, {0x30d7, 0x10}, {0x30d9, 0x0d},
    {0x30db, 0x08}, {0x3016, 0x82}, {0x3018, 0x38}, {0x3019, 0x30},
    {0x301a, 0x61}, {0x307d, 0x00}, {0x3087, 0x02}, {0x3082, 0x20},
    {0x3015, 0x12}, {0x3014, 0x04}, {0x3013, 0xf7}, {0x303c, 0x08},
    {0x303d, 0x18}, {0x303e, 0x06}, {0x303f, 0x0c}, {0x3030, 0x62},
    {0x3031, 0x26}, {0x3032, 0xe6}, {0x3033, 0x6e}, {0x3034, 0xea},
    {0x3035, 0xae}, {0x3036, 0xa6}, {0x3037, 0x6a}, {0x3104, 0x02},
    {0x3105, 0xfd}, {0x3106, 0x00}, {0x3107, 0xff}, {0x3300, 0x12},
    {0x3301, 0xde}, {0x3302, 0xef}, {0x3316, 0xff}, {0x3317, 0x00},
    {0x3312, 0x26}, {0x3314, 0x42}, {0x3313, 0x2b}, {0x3315, 0x42},
    {0x3310, 0xd0}, {0x3311, 0xbd}, {0x330c, 0x18}, {0x330d, 0x18},
    {0x330e, 0x56}, {0x330f, 0x5c}, {0x330b, 0x1c}, {0x3306, 0x5c},
    {0x3307, 0x11}, {0x336a, 0x52}, {0x3370, 0x46}, {0x3376, 0x38},
    {0x3300, 0x13}, {0x30b8, 0x20}, {0x30b9, 0x17}, {0x30ba, 0x04},
    {0x30bb, 0x08}, {0x3507, 0x06}, {0x350a, 0x4f}, {0x3100, 0x02},
    {0x3301, 0xde}, {0x3304, 0x00}, {0x3400, 0x00}, {0x3404, 0x02},
    {0x335f, 0x68}, {0x3360, 0x18}, {0x3361, 0x0c}, {0x3362, 0x12},
    {0x3363, 0x88}, {0x3364, 0xe4}, {0x3403, 0x42}, {0x3088, 0x02},
    {0x3089, 0x80}, {0x308a, 0x01}, {0x308b, 0xe0}, {0x308d, 0x04},
    {0x3086, 0x03}, {0x3086, 0x00}, {0x3011, 0x00}, {0x304c, 0x85},
#if (OV3640_TEST_PATTERN)
    {0x307B, 0x02}, {0x307D, 0x80}, {0x306C, 0x00},
#endif
    {OV3640_END_OF_ARRAY, 0xff}     //  End of file marker (0xFFFF)
};

/* OV3640 Capture YUV format, QXGA resolution
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_qxga [] = {
    {0x300e, 0x39} ,{0x300f, 0x21} ,{0x3010, 0x20} ,{0x304c, 0x81} ,
    {0x3302, 0xef} ,{0x335f, 0x68} ,{0x3360, 0x18} ,{0x3361, 0x0C} ,
    {0x3362, 0x68} ,{0x3363, 0x08} ,{0x3364, 0x04} ,{0x3403, 0x42} ,
    {0x3088, 0x08} ,{0x3089, 0x00} ,{0x308a, 0x06} ,{0x308b, 0x00} ,
    {OV3640_END_OF_ARRAY, 0xff}     //  End of file marker (0xFFFF)
};

/* OV3640 Capture scale down from QXGA to SXGA 1280x960
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_vga [] = {
   {0x3302, 0xef}, {0x335f, 0x68}, {0x3360, 0x18}, {0x3361, 0x0c},
   {0x3362, 0x12}, {0x3363, 0x88}, {0x3364, 0xe4}, {0x3403, 0x42},
   {0x3088, 0x02}, {0x3089, 0x80}, {0x308a, 0x01}, {0x308b, 0xe0},
   {0x304c, 0x84},
   {OV3640_END_OF_ARRAY, 0xff}      //  End of file marker (0xFFFF)
};

/* OV3640 Capture scale down from QXGA to VGA 680x480
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_sxga [] = {
    {0x3302, 0xef}, {0x335f, 0x68}, {0x3360, 0x18}, {0x3361, 0x0c},
    {0x3362, 0x35}, {0x3363, 0x08}, {0x3364, 0xc4}, {0x3403, 0x42},
    {0x3088, 0x05}, {0x3089, 0x00}, {0x308a, 0x03}, {0x308b, 0xc0},
    {0x304c, 0x81},
    {OV3640_END_OF_ARRAY, 0xff}     //  End of file marker (0xFFFF)
};

#if OV3640_TEST_PATTERN
/* OV3640 in test pattern mode, image data in one line is exported from e2 memory view */
const uint32_t g_ov3640_vga_color_one_line [] = {
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE86FE33, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xE954FE4B, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xC704D36D, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0x6548CC68, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x43FA4EB8, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x31EB3E7C, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x05750EE1, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x68800080, 0xFE80FE80,
};

const uint32_t g_ov3640_sxga_color_one_line [] = {
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
    0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE86FE24, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04,
    0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8DFE04, 0xFE8CFE04, 0xDA1CF892, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA,
    0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xD404D4DA, 0xCA04D36E, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404,
    0xC404C404, 0xC404C404, 0xC404C404, 0xC404C404, 0xD110C412, 0x1F7AB2B9, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA,
    0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x4EFA4EFA, 0x47FA4EDE, 0x3EFA3F2B, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23,
    0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x3EFA3E23, 0x25E13FCE, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0E700EFA,
    0x0E700EFA, 0x0E700EFA, 0x0E700EFA, 0x0A720EF7, 0x007F0193, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080, 0x00800080,
    0x00800080, 0x00800080, 0x00800080, 0xB5800680, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80, 0xFE80FE80,
};
#endif

/* Functions declarations */
static fsp_err_t i2c_master_wait_event (const i2c_master_event_t i2c_event);
static fsp_err_t ov3640_reg_write (uint16_t address, uint8_t data);
static fsp_err_t ov3640_reg_read (uint16_t address, uint8_t * p_data);
static fsp_err_t ov3640_write_array (sensor_reg_t const * p_array);
static fsp_err_t ov3640_software_reset (void);
static void ov3640_power (ov3640_power_t power_state);

/*******************************************************************************************************************//**
 * @brief       Wait for i2c master event.
 * @param[in]   i2c_event : i2c master events need to wait for
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t i2c_master_wait_event (const i2c_master_event_t i2c_event)
{
    fsp_err_t err      = FSP_SUCCESS;
    uint8_t   time_out = UINT8_MAX;

    /* Wait until event occur */
    while(i2c_event != g_i2c_event_for_ov3640)
    {
        /* Check for aborted event */
        if(I2C_MASTER_EVENT_ABORTED == g_i2c_event_for_ov3640)
        {
            APP_ERR_RETURN(FSP_ERR_TRANSFER_ABORTED,
                           " ** Error EVENT_ABORTED received during perform i2c operation **\r\n");
        }
        else
        {
            /* Start checking for time out to avoid infinite loop */
            time_out --;
            R_BSP_SoftwareDelay(OV3640_I2C_TIMEOUT_UNIT, BSP_DELAY_UNITS_MICROSECONDS);

            /* Check for time elapse*/
            if (RESET_VALUE == time_out)
            {
                APP_ERR_RETURN(FSP_ERR_TIMEOUT,
                               " ** I2C Master Callback event not received **\r\n");
            }
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Write data to camera register.
 * @param[in]   address : Register address
 * @param[in]   data : data to be written
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_reg_write (uint16_t address, uint8_t data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[3];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);
    i2c_buffer[2] = data;

    /* Reset callback event */
    g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_ov3640_ctrl, i2c_buffer, 3, false);
    APP_ERR_RETURN(err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RETURN(err, " ** i2c_master_wait_event FAILED ** \r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Read data from camera register.
 * @param[in]   address : Register address
 * @param[in]   p_data : pointer used to store register data
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_reg_read (uint16_t address, uint8_t * p_data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[2];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);

    /* Reset callback event */
    g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_ov3640_ctrl, i2c_buffer, 2, true);
    APP_ERR_RETURN(err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RETURN(err, " ** i2c_master_wait_event FAILED ** \r\n");

    /* Reset callback event */
    g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C read operation */
    err = R_IIC_MASTER_Read(&g_i2c_master_for_ov3640_ctrl, p_data, 1, false);
    APP_ERR_RETURN(err, " ** R_IIC_MASTER_Read API FAILED ** \r\n");

    /* Wait until read transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RETURN(err, " ** i2c_master_wait_event FAILED ** \r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Write data to camera registers.
 * @param[in]   p_array : pointer to array of structures containing a register address and value.
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_write_array (sensor_reg_t const * p_array)
{
    fsp_err_t err   = FSP_SUCCESS;
    uint8_t   value = RESET_VALUE;

    /* Check end of register array */
    while (OV3640_END_OF_ARRAY != p_array->reg)
    {
        /* Write data to the camera register */
        err = ov3640_reg_write(p_array->reg, p_array->val);
        APP_ERR_RETURN(err, " ** ov3640_reg_write FAILED ** \r\n");

        /* Read-back data from the camera register */
        err = ov3640_reg_read (p_array->reg, &value);
        APP_ERR_RETURN(err, " ** ov3640_reg_read FAILED ** \r\n");

        /* Compare data written and data read-back */
        if(value != p_array->val)
        {
            APP_ERR_RETURN(FSP_ERR_ASSERTION, " ** Data Write and read-back data do not match ** \r\n");
        }

        /* Select next register in register array */
        p_array ++;
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief       Software reset the camera
 * @param       None
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_software_reset (void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = ov3640_reg_write (OV3640_RESET_ADDRESS, OV3640_RESET_VALUE);
    APP_ERR_RETURN(err, " ** ov3640_reg_write FAILED ** \r\n");
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Power on the camera
 * @param[in]   power_state : Power state wants to set the camera
 * @retval      None
 **********************************************************************************************************************/
static void ov3640_power (ov3640_power_t power_state)
{
    R_BSP_PinAccessEnable();
    R_BSP_PinWrite(OV3640_CAM_PWR_ON, (bsp_io_level_t)power_state);
    R_BSP_PinAccessDisable();
}

/*******************************************************************************************************************//**
 * @brief       Initialization the camera.
 * @param       None
 * @retval      FSP_SUCCESS    Upon successful initialization.
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov3640_open (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Power on the camera */
    ov3640_power (OV3640_POWER_ON);

    /* Software reset the camera */
    err = ov3640_software_reset();
    APP_ERR_RETURN(err, " ** ov3640_software_reset FAILED ** \r\n");

    /* write array register initialize OV3640 */
    err = ov3640_write_array(g_ov3640_fmt_yuv422);
    APP_ERR_RETURN(err, " ** ov3640_write_array FAILED ** \r\n");

    /* Delay 50ms to complete write array register */
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Configure camera resolution.
 * @param[in]   p_array : pointer to array of structures containing a register address and value for set resolution
 * @retval      FSP_SUCCESS    Upon successful initialization.
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov3640_set_resolution (sensor_reg_t const * p_array)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Configure the OV3640 camera resolution as QXGA */
    err = ov3640_write_array(g_ov3640_qxga);
    APP_ERR_RETURN(err, " ** ov3640_write_array FAILED ** \r\n");

    /* Delay 50ms to complete write array register */
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    /* Configure down scale the OV3640 camera resolution from QXGA to selected resolution */
    err = ov3640_write_array(p_array);
    APP_ERR_RETURN(err, " ** ov3640_write_array FAILED ** \r\n");

    /* Delay 50ms to complete write array register */
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      iic_master callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void g_i2c_master_for_ov3640_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_i2c_event_for_ov3640 = p_args->event;
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup ceu_ep)
 **********************************************************************************************************************/
