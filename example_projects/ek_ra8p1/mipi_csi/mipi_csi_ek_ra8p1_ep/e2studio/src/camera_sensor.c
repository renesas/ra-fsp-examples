/***********************************************************************************************************************
 * File Name    : camera_sensor.c
 * Description  : Contains data structures and functions setup camera sensors used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "camera_sensor.h"

/***********************************************************************************************************************
 * @addtogroup camera_sensor
 * @{
 **********************************************************************************************************************/

uint32_t g_plck_hz = RESET_VALUE;

const camera_config_t camera_profiles[RES_MAX] = {
    [RES_1024x600]    = {1024, 600},
    [RES_VGA]         = {640,  480},
    [RES_QVGA]        = {320,  240},
};

static sensor_reg_t ov5640_mipi[] =
{
 /* Power down */
 {SYS_CTRL0_REG, SYS_CTRL0_SW_PWDN},

 /* System setting. */
 {SCCB_SYS_CTRL1_REG, 0x13}, {SCCB_SYS_CTRL1_REG, 0x03}, {SYS_RESET00_REG, 0x00},
 {SYS_CLK_ENABLE00_REG, 0xff}, {SYS_RESET02_REG, 0x1c}, {SYS_CLK_ENABLE02_REG, 0xc3},
 {IO_MIPI_CTRL00_REG, 0x45}, {SYSTEM_CONTROL1_REG, 0x08}, {SC_PLL_CTRL0_REG, 0x18},
 {FMT_MUX_CTRL_REG, 0x00}, {FMT_CTRL00_REG, 0x32},
 {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52}, {0x370c, 0x03},
 {TIMING_TC_REG20_REG, 0x40}, {TIMING_TC_REG21_REG, 0x01},
 {0x3630, 0x36},
 {0x3631, 0x0e}, {0x3632, 0xe2}, {0x3633, 0x12}, {0x3621, 0xe0},
 {0x3704, 0xa0},
 {0x3703, 0x5a}, {0x3715, 0x78}, {0x3717, 0x01}, {0x370b, 0x60}, {0x3705, 0x1a},
 {0x3905, 0x02},
 {0x3906, 0x10}, {0x3901, 0x0a}, {0x3731, 0x12}, {0x3600, 0x08}, {0x3601, 0x33},
 {0x302d, 0x60}, {0x3620, 0x52}, {0x371b, 0x20}, {0x471c, 0x50},
 {0x3a13, 0x43}, {0x3a18, 0x00}, {0x3a19, 0xf8}, {0x3635, 0x13},
 {0x3636, 0x03}, {0x3634, 0x40}, {0x3622, 0x01},
 {BLC_CTRL01_REG, 0x02}, {BLC_CTRL04_REG, 0x02}, {BLC_CTRL05_REG, 0x1a}, {ISP_CTRL01_REG, 0xa3},

 /* AEC */
 { AEC_PK_MANUAL_REG, 0x07 }, { 0x3500, 0x00 }, { 0x3501, 0xFF }, { 0x3502, 0x00 },
 { AEC_REAL_GAIN_H_REG, 0x00 }, { AEC_REAL_GAIN_L_REG, 0x20},
 { AEC_CTRL0F_REG, 0x30}, { AEC_CTRL10_REG, 0x28}, { AEC_CTRL1B_REG, 0x30}, { AEC_CTRL1E_REG, 0x26},
 { AEC_CTRL11_REG, 0x60}, { AEC_CTRL1F_REG, 0x14},
 { 0x3a18, 0x00 }, { 0x3a19, 0x7c },

 /* AWB */
 { AWB_MAN_CTRL, AWB_MANUAL_ENABLE}, { AWB_R_GAIN_H, 0x07 }, { AWB_R_GAIN_L, 0x70 },
 { AWB_G_GAIN_H, 0x04 }, { AWB_G_GAIN_L, 0x00 }, { AWB_B_GAIN_H, 0x05 }, { AWB_B_GAIN_L, 0xC0 },

 /* Gamma */
 {0x5480, 0x01}, {0x5481, 0x08}, {0x5482, 0x14}, {0x5483, 0x28},
 {0x5484, 0x51}, {0x5485, 0x65}, {0x5486, 0x71}, {0x5487, 0x7d},
 {0x5488, 0x87}, {0x5489, 0x91}, {0x548a, 0x9a}, {0x548b, 0xaa},
 {0x548c, 0xb8}, {0x548d, 0xcd}, {0x548e, 0xdd}, {0x548f, 0xea},
 {0x5490, 0x1d}, //skip

 /* UV adjust. */
 {SDE_CTRL0_REG, 0x06}, {SDE_CTRL3_REG, 0x40}, {SDE_CTRL4_REG, 0x10}, {SDE_CTRL9_REG, 0x10},
 {SDE_CTRL10_REG, 0x00}, {SDE_CTRL11_REG, 0xf8},

/* Lens correction. */
 {0x5800, 0x23}, {0x5801, 0x14}, {0x5802, 0x0f}, {0x5803, 0x0f},
 {0x5804, 0x12}, {0x5805, 0x26}, {0x5806, 0x0c}, {0x5807, 0x08},
 {0x5808, 0x05}, {0x5809, 0x05}, {0x580a, 0x08}, {0x580b, 0x0d},
 {0x580c, 0x08}, {0x580d, 0x03}, {0x580e, 0x00}, {0x580f, 0x00},
 {0x5810, 0x03}, {0x5811, 0x09}, {0x5812, 0x07}, {0x5813, 0x03},
 {0x5814, 0x00}, {0x5815, 0x01}, {0x5816, 0x03}, {0x5817, 0x08},
 {0x5818, 0x0d}, {0x5819, 0x08}, {0x581a, 0x05}, {0x581b, 0x06},
 {0x581c, 0x08}, {0x581d, 0x0e}, {0x581e, 0x29}, {0x581f, 0x17},
 {0x5820, 0x11}, {0x5821, 0x11}, {0x5822, 0x15}, {0x5823, 0x28},
 {0x5824, 0x46}, {0x5825, 0x26}, {0x5826, 0x08}, {0x5827, 0x26},
 {0x5828, 0x64}, {0x5829, 0x26}, {0x582a, 0x24}, {0x582b, 0x22},
 {0x582c, 0x24}, {0x582d, 0x24}, {0x582e, 0x06}, {0x582f, 0x22},
 {0x5830, 0x40}, {0x5831, 0x42}, {0x5832, 0x24}, {0x5833, 0x26},
 {0x5834, 0x24}, {0x5835, 0x22}, {0x5836, 0x22}, {0x5837, 0x26},
 {0x5838, 0x44}, {0x5839, 0x24}, {0x583a, 0x26}, {0x583b, 0x28},
 {0x583c, 0x42}, {0x583d, 0xce}, {0x5000, 0xa7},

 /* Flip. */
 {TIMING_TC_REG20_REG, 0x40},

 /* Mirror. */
 {TIMING_TC_REG21_REG, 0x06},

 /* Timming. */
 {TIMING_X_INC, 0x31}, {TIMING_Y_INC, 0x31}, {TIMING_VS, 0x04},

 /* Output size */
 {TIMING_DVPHO_H, (uint8_t)(CAMERA_IMAGE_WIDTH>>8)}, {TIMING_DVPHO_L, (uint8_t)CAMERA_IMAGE_WIDTH},
 {TIMING_DVPVO_H, (uint8_t)(CAMERA_IMAGE_HEIGHT>>8)}, {TIMING_DVPVO_L, (uint8_t)CAMERA_IMAGE_HEIGHT},

 {VFIFO_CTRL0C_REG, 0x22}, {PCLK_PERIOD_REG, 0x10}, {PCLK_DIV_REG, 0x02}, {ISP_CTRL01_REG, 0xa3},

 /* Disable the test pattern */
 { 0x503D, 0x00 },
 /* Select color bar when enabling the test pattern */
 { 0x4741, 0x00 },

 /* End of file marker (0xFFFF) */
 {CONFIG_TABLE_END_DETECT, 0x00ff},

};

sensor_reg_t live_camera[] =
{

 { 0x503D, 0x00 },
 { REQUEST_SOFTWARE_WAIT, 50},

 /* End of file marker (0xFFFF) */
 {CONFIG_TABLE_END_DETECT, 0x00ff}

};

sensor_reg_t color_bar_test_pattern[] =
{

 { 0x503D, 0x80 },
 { REQUEST_SOFTWARE_WAIT, 50},

 /* End of file marker (0xFFFF) */
 {CONFIG_TABLE_END_DETECT, 0x00ff}

};

const uint32_t g_test_pattern_color_one_line_1024x600 [] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6,
    0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0xFFE7FFE6, 0x4FE067E5, 0x3FFF37FF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x2FE937EB, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0,
    0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x3FE06FE5,
    0xF81FF01F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF01DF81F, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020, 0xF020F020, 0xE840F020,
    0xF020F020, 0xE840F020, 0xD800F000, 0x181C181D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D,
    0x001D001D, 0x001C001D, 0x001D001D, 0x001C001D, 0x001C001D, 0x00080007, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

const uint32_t g_test_pattern_color_one_line_vga [] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0x6FE5FFE7, 0x3FFF37FF, 0x3FFF3FFF,
    0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF,
    0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF,
    0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF,
    0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF,
    0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x3FFF3FFF, 0x3FFF37FF, 0x27E02FE9,
    0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0,
    0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0,
    0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0,
    0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0,
    0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x2FE027E0, 0x2FE027E0, 0x27E027E0, 0x6FE52FE0,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81EF81F, 0xF020F81D, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040,
    0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040,
    0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040,
    0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040,
    0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040, 0xF020E820, 0xF020F040,
    0xF020E820, 0xD800F040, 0x001D181D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x00070007, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

const uint32_t g_test_pattern_color_one_line_qvga [] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFE6FFFF, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7,
    0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0xFFE6FFE7, 0x3FFF37FF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x3FFF3FFF,
    0x37FF3FFF, 0x3FFF3FFF, 0x37FF3FFF, 0x2FE93FFF, 0x27E027E0, 0x27E02FE0, 0x27E02FE0, 0x2FE027E0,
    0x27E027E0, 0x27E02FE0, 0x27E02FE0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x27E02FE0, 0x2FE027E0,
    0x27E027E0, 0x27E02FE0, 0x27E02FE0, 0x2FE027E0, 0x27E027E0, 0x27E02FE0, 0x27E02FE0, 0x2FE027E0,
    0xF81FF01F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F,
    0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81FF81F, 0xF81DF81F, 0xF020E820, 0xF040F020, 0xF020E820,
    0xF040F020, 0xF020E820, 0xF040F020, 0xF020E820, 0xF040F020, 0xF020E820, 0xF040F020, 0xF020E820,
    0xF040F020, 0xF020E820, 0xF040F020, 0xF020E820, 0xF040F020, 0xF020E820, 0xF040F020, 0xF020E820,
    0xF040F020, 0x001D181D, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C,
    0x001D001D, 0x001D001C, 0x001D001D, 0x001D001C, 0x001D001D, 0x0008001C, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};


/* Private functions declarations */
static void camera_hw_reset(void);
static uint32_t camera_configure_clocks(void);
static fsp_err_t camera_set_mipi_virtual_channel(uint32_t vchannel);
static fsp_err_t camera_fps_set(void);

/***********************************************************************************************************************
 *  Function Name: camera_write_array
 *  Description  : This function is used to write data to camera registers.
 *  Arguments    : p_array        pointer to configuration array
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t camera_write_array (sensor_reg_t const * p_array)
{
    fsp_err_t err   = FSP_SUCCESS;
    uint8_t   value = RESET_VALUE;

    /* Check end of register array */
    while (CONFIG_TABLE_END_DETECT != p_array->reg)
    {
        if(p_array->reg != REQUEST_SOFTWARE_WAIT)
        {

            /* Write data to the camera register */
            err = write_reg_16bit(p_array->reg, p_array->val);
            APP_ERR_RET( FSP_SUCCESS != err, err, " ** write_reg_16bit FAILED ** \r\n");

            /* Read-back data from the camera register */
            err = read_reg_16bit (p_array->reg, &value);
            APP_ERR_RET( FSP_SUCCESS != err, err, " ** read_reg_16bit FAILED ** \r\n");

            /* Compare data written and data read-back */
            if(value != p_array->val)
            {
                APP_ERR_RET( FSP_SUCCESS != err, FSP_ERR_ASSERTION, " ** Data Write and read-back data do not match ** \r\n");
            }

        }
        else
        {
            R_BSP_SoftwareDelay(p_array->val, BSP_DELAY_UNITS_MILLISECONDS);
        }
        /* Select next register in register array */
        p_array ++;
    }

    return err;
}
/***********************************************************************************************************************
* End of function camera_write_array
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_open
 *  Description  : This function is used to initialize the camera sensor.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t camera_open (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT module */
    err = R_GPT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_GPT_Open API FAILED ** \r\n");

    /* Start GPT module to provide the 24MHz clock frequency output for the camera clock source */
    err = R_GPT_Start(&g_timer_periodic_ctrl);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_GPT_Start API FAILED ** \r\n");

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /* Change the I2C slave address to control camera sensor */
    err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_for_peripheral_ctrl, REG_CAM_I2C_SLAVE_ADDR, I2C_MASTER_ADDR_MODE_7BIT);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_SlaveAddressSet API FAILED ** \r\n");

    /* Reset using GPIO */
    camera_hw_reset();

    /* write array register initialize camera */
    err = camera_write_array(ov5640_mipi);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** camera_write_array FAILED ** \r\n");

    /* Delay 50ms to complete write array register */
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    /* Configure the required clocks for camera */
    err = camera_configure_clocks();
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** camera_configure_clocks FAILED ** \r\n");

    /* Set the target FPS for camera */
    err = camera_fps_set();
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** camera_fps_set FAILED ** \r\n");

    /* Set the mipi virtual channel */
    err = camera_set_mipi_virtual_channel(DEFAULT_MIPI_CHANNEL);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** camera_set_mipi_virtual_channel FAILED ** \r\n");

    return err;
}
/***********************************************************************************************************************
* End of function camera_open
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_hw_reset
 *  Description  : This function is used to reset camera pin.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void camera_hw_reset(void)
{
    CAMERA_RST_PIN_SET(BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
    CAMERA_RST_PIN_SET(BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
}
/***********************************************************************************************************************
* End of function camera_hw_reset
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_configure_clocks
 *  Description  : This function is used to configure the internal clock of camera sensor.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t camera_configure_clocks(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* 0x3035 SC PLL CONTRL1 (Default: 0x11) */
    uint8_t sys_clock_div = 1;  /* Bit[7:4]: System clock divider */
    uint8_t mipi_clock_div = 2; /* Bit[3:0]: Scale divider for MIPI */
    uint8_t reg_0x3035_value = (uint8_t)(sys_clock_div << 4 | mipi_clock_div << 0);

    /* Sys Clock Div: 1 --- MIPI Clock Div: 2 */
    err = write_reg_16bit(0x3035, reg_0x3035_value);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Set 0x3035 register failed\r\n");

    /* 0x3036 SC PLL CONTRL2 (Default: 0x38)*/
    uint8_t pll_multiplier = 123; /* Bit[7:0]: PLL multiplier (4~252, even integer from 128+) */
    APP_ERR_RET((((pll_multiplier < 4) || (pll_multiplier > 252)) ||
                 ((pll_multiplier >= 128) && ((pll_multiplier % 2) != 0))),
                 FSP_ERR_ASSERTION, "pll_multiplier is invalid\r\n");
    uint8_t reg_0x3036_value = (uint8_t)(pll_multiplier);

    /* PLL multiplier: 123 */
    err = write_reg_16bit(0x3036, reg_0x3036_value);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Set 0x3036 register failed\r\n");

    /* 0x3037 SC PLL CONTRL3 (Default: 0x13) */
    uint8_t pll_root_div = 1; /* Bit[4]: PLL root divider --- 0: Bypass, 1: Divided by 2 */
    APP_ERR_RET( !((pll_root_div == 1) || (pll_root_div == 2) ), FSP_ERR_ASSERTION,
                "pll_root_div is invalid\r\n");
    uint8_t pll_pre_div = 8;  /* Bit[3:0]: PLL pre-divider: 1,2,3,4,6,8 */
    APP_ERR_RET( !((pll_pre_div == 1) || (pll_pre_div == 2) || (pll_pre_div == 3) ||
                  (pll_pre_div == 4) || (pll_pre_div == 6) || (pll_pre_div == 8) ), FSP_ERR_ASSERTION,
                "pll_pre_div is invalid\r\n");

    uint8_t reg_0x3037_value = (uint8_t)((pll_root_div==2) << 4 | (pll_pre_div) << 0);

    /* PLL 2x-root-div: Bypass --- PLL Pre-div: 8 */
    err = write_reg_16bit(0x3037, reg_0x3037_value);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Set 0x3037 register failed\r\n");

    /* 0x3108 SYSTEM ROOT DIVIDER - Pad Clock Divider for SCCB Clock (Default: 0x01)
     * (divider 1: 00b(0x0), divider 2: 01b(0x1), divider 4: 10b(0x2), divider 8: 11b(0x3)) */
    uint8_t pclk_root_div   = 2; /* Bit[5:4]: PCLK root divider */
    uint8_t sclk2x_root_div = 1; /* Bit[3:2]: sclk2x root divider */
    uint8_t sclk_root_div   = 4; /* Bit[1:0]: SCLK root divider */

    /* The elements at 1, 2, 4, 8  are valid */
    uint8_t reg_0x3108_div_lut[9] = {0xF, 0x0, 0x1, 0xF, 0x2, 0xF, 0xF, 0xF, 0x3};
    if(!(reg_0x3108_div_lut[pclk_root_div] < 0xF)){ return FSP_ERR_ASSERTION; }
    if(!(reg_0x3108_div_lut[sclk2x_root_div] < 0xF)){ return FSP_ERR_ASSERTION; }
    if(!(reg_0x3108_div_lut[sclk_root_div] < 0xF)){ return FSP_ERR_ASSERTION; }

    uint8_t reg_0x3108_value = (uint8_t)(reg_0x3108_div_lut[pclk_root_div] << 4 | \
            reg_0x3108_div_lut[sclk2x_root_div] << 2 | \
            reg_0x3108_div_lut[sclk_root_div] << 0);

    /* System divider */
    err = write_reg_16bit(0x3108, reg_0x3108_value);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Set 0x3108 register failed\r\n");

    uint32_t base_pll_hz = (((uint32_t)MIPI_XCLK_HZ / (pll_root_div * pll_pre_div)) * pll_multiplier);
    if(!(800000000 >= base_pll_hz)){ return FSP_ERR_ASSERTION; }

    uint32_t g_sys_clk_hz = base_pll_hz/ (uint32_t)(sys_clock_div  * sclk2x_root_div * sclk_root_div);
    uint32_t mipi_clk_hz = base_pll_hz / (uint32_t)(mipi_clock_div * pclk_root_div);
    if(!(g_sys_clk_hz >= mipi_clk_hz)){ return FSP_ERR_ASSERTION; }

    /* Calculate pixel clock to calculate HTS and VTS */
    g_plck_hz = base_pll_hz / pclk_root_div;
    if (g_plck_hz > PLCK_MAX_HZ)
    {
        g_plck_hz = PLCK_MAX_HZ;
    }

    return err;
}
/***********************************************************************************************************************
* End of function camera_configure_clocks
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_fps_set
 *  Description  : This function is used to set HTS and VTS based on the resolution.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t camera_fps_set(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* OV5640 full resolution */
    const float sensor_max_x = 2592.0f;
    const float sensor_max_y = 1944.0f;

    /* Pixel aspect ratio */
    float pixel_ratio = sensor_max_x / sensor_max_y;

    /* Calculate VTS and HTS using square root model */
    float vts_f = sqrtf((float)g_plck_hz / FPS_TARGET / pixel_ratio);
    float hts_f = vts_f * pixel_ratio;

    uint16_t vts = (uint16_t)(vts_f + 0.5f);  /* Rounded */
    uint16_t hts = (uint16_t)(hts_f + 0.5f);  /* Rounded */

    /* Set HTS: Register 0x380C (MSB), 0x380D (LSB) */
    err = write_reg_16bit(TIMING_HTS_H, (hts >> 8) & 0xFF);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit failed\r\n");
    err = write_reg_16bit(TIMING_HTS_L, hts & 0xFF);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit failed\r\n");

    /* Set VTS: Register 0x380E (MSB), 0x380F (LSB) */
    err = write_reg_16bit(TIMING_VTS_H, (vts >> 8) & 0xFF);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit failed\r\n");
    err = write_reg_16bit(TIMING_VTS_L, vts & 0xFF);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function camera_fps_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_set_mipi_virtual_channel
 *  Description  : This function is used to set virtual channel.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t camera_set_mipi_virtual_channel(uint32_t vchannel)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t tmp;

    err  = read_reg_16bit(0x4814, &tmp);
    APP_ERR_RET( FSP_SUCCESS != err, err, "read_reg_16bit failed\r\n");

    tmp = tmp & (uint8_t)~(3 << 6);
    tmp = tmp | (uint8_t)(vchannel << 6);
    err = write_reg_16bit(0x4814, tmp);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit failed\r\n");

  return err;
}
/***********************************************************************************************************************
* End of function camera_set_mipi_virtual_channel
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_stream_on
 *  Description  : This function is used to power up and turn on stream camera.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t camera_stream_on(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = write_reg_16bit(SYS_CTRL0_REG, SYS_CTRL0_SW_PWUP);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit to power up camera failed\r\n");

    err = write_reg_16bit(0x4202, 0x00);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit to turn on stream camera failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function camera_stream_on
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_stream_off
 *  Description  : This function is used to turn off stream camera and power down.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t camera_stream_off(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = write_reg_16bit(0x4202, 0x0f);
    APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit to turn off stream camera failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function camera_stream_off
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup camera_sensor)
 **********************************************************************************************************************/
