/***********************************************************************************************************************
 * File Name    : DRW_ep.c
 * Description  : Contains data structures and functions used in DRW_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2022-2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "DRW_ep.h"
#include "hal_data.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup DRW_ep
 * @{
 **********************************************************************************************************************/
/* DRW Variables */
static d2_device * gp_davey;
static d2_s32             DRW_err;

/* Variables for different figures */
/* Circle variables */
static d2_point g_c_c_x1 = CIRCLE_VAL_X, g_c_c_y1 = CIRCLE_VAL_Y, g_c_r = CIRCLE_VAL_R, g_c_w = CIRCLE_VAL_W;
static bool g_c_left_flag = true, g_c_right_flag = false;
/* Triangle variables */
static d2_point g_t_x1 = TRIANGLE_X1, g_t_y1 = TRIANGLE_Y1, g_t_x2 = TRIANGLE_X2, g_t_y2 = TRIANGLE_Y2, g_t_x3 = TRIANGLE_X3, g_t_y3 = TRIANGLE_Y3;
static bool g_t_top_flag = true, g_t_bottom_flag = false;
/* Box variables */
static d2_point g_b_x1 = BOX_VAL_X1, g_b_y1 = BOX_VAL_Y1, g_b_l = BOX_VAL_LEN, g_b_w = BOX_VAL_WIDTH;
static bool g_p_top_flag = false, g_p_bottom_flag = true;
/* line variables */
static d2_point g_l_x1 = LINE_VAL_X1, g_l_y1 = LINE_VAL_Y1, g_l_x2 = LINE_VAL_X2, g_l_y2 = LINE_VAL_Y2, g_l_w = LINE_VAL_WIDTH;
static bool g_l_left_flag = false, g_l_right_flag = true;

/* GLCDC Variables */
/* Variables to store resolution information */
static uint16_t g_hz_size = RESET_VALUE;
static uint16_t g_vr_size = RESET_VALUE;
/* Variables used for buffer usage */
static uint32_t g_buffer_size = RESET_VALUE;
#if defined(BOARD_RA8D1_EK)
static uint32_t * g_p_single_buffer, * g_p_double_buffer;
#else
static uint8_t * g_p_single_buffer, * g_p_double_buffer;
#endif
volatile uint8_t g_vsync_flag = RESET_FLAG;

#if defined(BOARD_RA8D1_EK)
volatile bool g_message_sent  = RESET_FLAG;

/* This table of commands was adapted from sample code provided by FocusLCD
 * Page Link: https://focuslcds.com/product/4-5-tft-display-capacitive-tp-e45ra-mw276-c/
 * File Link: https://focuslcds.com/wp-content/uploads/Code/E45RA-MW276-C-2LANE-MIPI-CODE.txt
 */
const lcd_table_setting_t g_lcd_init_focuslcd[] =
{
 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE,        MIPI_DSI_CMD_FLAG_LOW_POWER},        // Change to Page 1 CMD
 {2,  {0x08, 0x10},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Output SDA
 {2,  {0x21, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // DE = 1 Active

 {2,  {0x30, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Resolution setting 480 X 800
 {2,  {0x31, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Inversion setting

 {2,  {0x40, 0x14},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // BT 15
 {2,  {0x41, 0x33},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // avdd +5.2v,avee-5.2v
 {2,  {0x42, 0x02},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // VGL=DDVDL+VCL-VCIP,VGH=2DDVDH-DDVDL
 {2,  {0x43, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Set VGH clamp level
 {2,  {0x44, 0x06},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Set VGL clamp level
 {2,  {0x50, 0x70},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Set VREG1
 {2,  {0x51, 0x70},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Set VREG2
 {2,  {0x52, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Flicker MSB
 {2,  {0x53, 0x48},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Flicker LSB
 {2,  {0x60, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Timing Adjust
 {2,  {0x61, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Timing Adjust
 {2,  {0x62, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Timing Adjust
 {2,  {0x63, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Timing Adjust

 {2,  {0xa0, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 1
 {2,  {0xa1, 0x03},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 2
 {2,  {0xa2, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 3
 {2,  {0xa3, 0x0d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 4
 {2,  {0xa4, 0x06},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 5
 {2,  {0xa5, 0x16},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 6
 {2,  {0xa6, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 7
 {2,  {0xa7, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 8
 {2,  {0xa8, 0x03},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 9
 {2,  {0xa9, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 10
 {2,  {0xaa, 0x06},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 11
 {2,  {0xab, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 12
 {2,  {0xac, 0x0d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 13
 {2,  {0xad, 0x2c},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 14
 {2,  {0xae, 0x26},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 15
 {2,  {0xaf, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Positive Gamma Control 16

 {2,  {0xc0, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 1
 {2,  {0xc1, 0x04},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 2
 {2,  {0xc2, 0x0b},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 3
 {2,  {0xc3, 0x0f},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 4
 {2,  {0xc4, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 5
 {2,  {0xc5, 0x18},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 6
 {2,  {0xc6, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 7
 {2,  {0xc7, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 8
 {2,  {0xc8, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 9
 {2,  {0xc9, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 10
 {2,  {0xca, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 11
 {2,  {0xcb, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 12
 {2,  {0xcc, 0x0c},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 13
 {2,  {0xcd, 0x2d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 14
 {2,  {0xce, 0x28},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 15
 {2,  {0xcf, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // Negative Gamma Correction 16

 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x06}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE,        MIPI_DSI_CMD_FLAG_LOW_POWER},        // Change to Page 6 CMD for GIP timing
 {2,  {0x00, 0x21},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x01, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x02, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x03, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x04, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x05, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x06, 0x80},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x07, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x08, 0x02},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x09, 0x80},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x0a, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x0b, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x0c, 0x0a},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x0d, 0x0a},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x0e, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x0f, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x10, 0xe0},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x11, 0xe4},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x12, 0x04},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x13, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x14, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x15, 0xc0},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x16, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x17, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x18, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x19, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x1a, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x1b, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x1c, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1
 {2,  {0x1d, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 1

 {2,  {0x20, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x21, 0x23},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x22, 0x45},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x23, 0x67},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x24, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x25, 0x23},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x26, 0x45},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2
 {2,  {0x27, 0x67},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 2

 {2,  {0x30, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x31, 0x11},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x32, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x33, 0xee},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x34, 0xff},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x35, 0xcb},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x36, 0xda},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x37, 0xad},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x38, 0xbc},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x39, 0x76},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x3a, 0x67},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x3b, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x3c, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x3d, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x3e, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x3f, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3
 {2,  {0x40, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GIP Control 3

 {2,  {0x53, 0x10},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GOUT VGLO Control
 {2,  {0x54, 0x10},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // GOUT VGHO Control

 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x07}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE,        MIPI_DSI_CMD_FLAG_LOW_POWER},        // Change to Page 7 CMD for Normal command
 {2,  {0x18, 0x1d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // VREG1/2OUT ENABLE
 {2,  {0x26, 0xb2},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x02, 0x77},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0xe1, 0x79},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x17, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // VGL_REG Disable

 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x00}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE,        MIPI_DSI_CMD_FLAG_LOW_POWER},        // Change to Page 0 CMD for Normal command
 {120, {0},                                 MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG,   (mipi_dsi_cmd_flag_t)0},         // Sleep out command may not be issued within 120 ms of GPIO HW reset. Wait to ensure timing maintained.
 {2,  {0x11, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM,       MIPI_DSI_CMD_FLAG_LOW_POWER},// Sleep-Out
 {5,   {0},                                 MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG,   (mipi_dsi_cmd_flag_t)0},         // Delay 5msec
 {2,  {0x29, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM,       MIPI_DSI_CMD_FLAG_LOW_POWER},// Display on
 {2,  {0x3a, 0x70},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},      // 24-bit / pixel

 {0x00, {0},                                MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE, (mipi_dsi_cmd_flag_t)0},         // End of table
};
#endif

/* User defined functions */
void glcdc_callback(display_callback_args_t * args);
#if defined(BOARD_RA8D1_EK)
static void drw_set(uint32_t * framebuffer);
static void display_draw(uint32_t * framebuffer);
#else
static void drw_set(uint8_t * framebuffer);
static void display_draw(uint8_t * framebuffer);
#endif
static void drw_animation(void);
static void error_handler(d2_s32 DRW_err,d2_device * gp_davey);
static void glcdc_init(void);
static void drw_init(void);


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void DRW_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version    = {RESET_VALUE};
#if defined(BOARD_RA8D1_EK)
    uint32_t * p_framebuffer = NULL;
#else
    uint8_t * p_framebuffer = NULL;
#endif

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

#if defined(BOARD_RA8D1_EK)
    /* Initialize SDRAM. */
    bsp_sdram_init();
#endif

    /* Get LCDC configuration */
    g_hz_size = (g_display_cfg.input[ARRAY_INDEX].hsize);
    g_vr_size = (g_display_cfg.input[ARRAY_INDEX].vsize);
    /* Initialize buffer pointers */
    g_buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL);
#if defined(BOARD_RA8D1_EK)
    g_p_single_buffer = (uint32_t *) g_display_cfg.input[ARRAY_INDEX].p_base;
#else
    g_p_single_buffer = (uint8_t *) g_display_cfg.input[ARRAY_INDEX].p_base
#endif
    /* Double buffer for drawing color bands with good quality */
    g_p_double_buffer = g_p_single_buffer + g_buffer_size;

    /* GLCDC init */
    glcdc_init();

    /* Wait for a Vsync event */
    while(!g_vsync_flag);
    g_vsync_flag = RESET_FLAG;

    /* DRW initialization */
    drw_init();

    while(true)
    {
        /* Swap the active framebuffer */
        p_framebuffer = (p_framebuffer == g_p_single_buffer) ? g_p_double_buffer : g_p_single_buffer;

        /* Wait for a Vsync event */
        while(!g_vsync_flag);
        g_vsync_flag = RESET_FLAG;

        /* Now that the framebuffer is ready, update the GLCDC buffer pointer on the next Vsync */
#if defined(BOARD_RA8D1_EK)
        err = R_GLCDC_BufferChange(&g_display_ctrl, (uint8_t*) p_framebuffer, DISPLAY_FRAME_LAYER_1);
#else
        err = R_GLCDC_BufferChange(&g_display_ctrl, p_framebuffer, DISPLAY_FRAME_LAYER_1);
#endif
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            /* GLCDC initialization failed  */
            APP_ERR_PRINT("\r\n ** GLCDC Buffer change FAILED ** \r\n");
            APP_ERR_TRAP(err);
        }

        /* Draw the new framebuffer now */
        display_draw(p_framebuffer);
    }

    APP_PRINT("\r\n ** Image rendering successful ** \r\n");

    /* Wait for rendering operations to finish */
    DRW_err = d2_flushframe(gp_davey);
    error_handler(DRW_err, gp_davey);

    /* De-initialize hardware and close the handle */
    DRW_err = d2_deinithw(gp_davey);
    error_handler(DRW_err, gp_davey);

    DRW_err = d2_closedevice(gp_davey);
    error_handler(DRW_err, gp_davey);

    /* Stop GLCDC */
    err = R_GLCDC_Stop(&g_display_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
        APP_ERR_PRINT("\r\n ** GLCDC driver stop FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\n ** GLCDC driver stopped ** \r\n");
    }

    /* Close GLCDC */
    err = R_GLCDC_Close(&g_display_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
        APP_ERR_PRINT("\r\n ** GLCDC driver close FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\n ** GLCDC driver closed ** \r\n");
    }

}

/*******************************************************************************************************************//**
 * User defined function to initialize glcdc module
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
static void glcdc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initialize GLCDC driver */
    err = R_GLCDC_Open(&g_display_ctrl, &g_display_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
        APP_ERR_PRINT("\r\n ** GLCDC driver initialization FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\n ** GLCDC driver initialization SUCCESS ** \r\n");
    }

#if defined(BOARD_RA8D1_EK)
    /* Initialize LCD. */
    mipi_dsi_push_table(g_lcd_init_focuslcd);
#endif

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
        APP_ERR_PRINT("\r\n ** GLCDC driver start FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\n ** GLCDC driver start SUCCESS ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * User defined function to initialize drw module
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
static void drw_init(void)
{
    /* Open the D2 driver and initialize the hardware */
    gp_davey = d2_opendevice(BITFIELD_VALUE);
    DRW_err = d2_inithw(gp_davey, BITFIELD_VALUE);
    if (D2_OK == DRW_err)
    {
        APP_PRINT("\r\n ** DRW driver initialization SUCCESS ** \r\n");
    }
    else
    {
        error_handler(DRW_err, gp_davey);
    }

    /* set the transparency and the alpha mode */
    DRW_err = d2_setalpha(gp_davey, ALPHA_VALUE);
    if (D2_OK == DRW_err)
    {
        APP_PRINT("\r\n ** Transparency set successfully ** \r\n");
    }
    else
    {
        error_handler(DRW_err, gp_davey);
    }

    DRW_err = d2_setalphamode(gp_davey, d2_am_constant);
    if (D2_OK == DRW_err)
    {
        APP_PRINT("\r\n ** Alphamode set successfully ** \r\n");
    }
    else
    {
        error_handler(DRW_err, gp_davey);
    }

    /* Set alphablend mode */
    DRW_err = d2_setblendmode(gp_davey, d2_bm_alpha, d2_bm_one_minus_alpha);
    if (D2_OK == DRW_err)
    {
        APP_PRINT("\r\n ** Alphablend mode set succesfully ** \r\n");
    }
    else
    {
        error_handler(DRW_err, gp_davey);
    }
}

/*******************************************************************************************************************//**
 * Error handler for DRW.
 *
 * @param[in]  err       Error returned from DRW APIs
 * @param[in]  gp_dave   Pointer to device structure
 * @retval     none
 **********************************************************************************************************************/
static void error_handler(d2_s32 err, d2_device * gp_dave)
{
    if (D2_OK != err)
    {
        /* print the error message */
        APP_ERR_PRINT(d2_geterrorstring( gp_dave ));
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * Callback functions for GLCDC interrupts
 *
 * @param[in]  p_args    Callback arguments
 * @retval     none
 **********************************************************************************************************************/
void glcdc_callback (display_callback_args_t * p_args)
{
    if (p_args->event == DISPLAY_EVENT_LINE_DETECTION)
    {
        g_vsync_flag = SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 * User-defined function to draw the current display to a framebuffer
 *
 * @param[in]  framebuffer    Pointer to frame buffer
 * @retval     none
 **********************************************************************************************************************/
#if defined(BOARD_RA8D1_EK)
static void display_draw (uint32_t * framebuffer)
{
    /* DRW operations happens here */
    /*Animation of different figures */
    drw_animation();

    /* set frame buffer properties */
    drw_set(framebuffer);

    /* Set the render color to red */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, RED_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a circle */
    DRW_err = d2_rendercircle(gp_davey,(d2_point)(g_c_c_x1 << SHIFT_VALUE), (d2_point)(g_c_c_y1 << SHIFT_VALUE), (d2_width)(g_c_r << SHIFT_VALUE), (d2_width)(g_c_w << SHIFT_VALUE));
    error_handler(DRW_err, gp_davey);

    /* Set the render color to green */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, GREEN_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a triangle */
    DRW_err = d2_rendertri(gp_davey, (d2_point)( g_t_x1 << SHIFT_VALUE),(d2_point)( g_t_y1 << SHIFT_VALUE),(d2_point)( g_t_x2 << SHIFT_VALUE),(d2_point)( g_t_y2 << SHIFT_VALUE),(d2_point)( g_t_x3 << SHIFT_VALUE),(d2_point)( g_t_y3 << SHIFT_VALUE),d2_edge2_shared);
    error_handler(DRW_err, gp_davey);

    /* Set the render color to blue */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, BLUE_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a box */
    DRW_err = d2_renderbox(gp_davey,(d2_point)(g_b_x1 << SHIFT_VALUE), (d2_point)(g_b_y1 << SHIFT_VALUE), (d2_width)(g_b_l << SHIFT_VALUE), (d2_width)(g_b_w << SHIFT_VALUE));
    error_handler(DRW_err, gp_davey);

    /* Set the render color to yellow */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, YELLOW_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a line */
    DRW_err = d2_renderline(gp_davey, (d2_point) (g_l_x1 << SHIFT_VALUE), (d2_point) (g_l_y1 << SHIFT_VALUE), (d2_point) (g_l_x2 << SHIFT_VALUE), (d2_point) (g_l_y2 << SHIFT_VALUE),
                            (d2_width) (g_l_w << SHIFT_VALUE), d2_le_exclude_both);
    error_handler(DRW_err, gp_davey);

    /* End the display list, then call d2_start frame to begin execution */
    DRW_err = d2_endframe(gp_davey);
    error_handler(DRW_err, gp_davey);
}
#else
static void display_draw (uint8_t * framebuffer)
{
    /* DRW operations happens here */
    /*Animation of different figures */
    drw_animation();

    /* set frame buffer properties */
    drw_set(framebuffer);

    /* Set the render color to red */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, RED_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a circle */
    DRW_err = d2_rendercircle(gp_davey,(d2_point)(g_c_c_x1 << SHIFT_VALUE), (d2_point)(g_c_c_y1 << SHIFT_VALUE), (d2_width)(g_c_r << SHIFT_VALUE), (d2_width)(g_c_w << SHIFT_VALUE));
    error_handler(DRW_err, gp_davey);

    /* Set the render color to green */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, GREEN_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a triangle */
    DRW_err = d2_rendertri(gp_davey, (d2_point)( g_t_x1 << SHIFT_VALUE),(d2_point)( g_t_y1 << SHIFT_VALUE),(d2_point)( g_t_x2 << SHIFT_VALUE),(d2_point)( g_t_y2 << SHIFT_VALUE),(d2_point)( g_t_x3 << SHIFT_VALUE),(d2_point)( g_t_y3 << SHIFT_VALUE),d2_edge2_shared);
    error_handler(DRW_err, gp_davey);

    /* Set the render color to blue */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, BLUE_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a box */
    DRW_err = d2_renderbox(gp_davey,(d2_point)(g_b_x1 << SHIFT_VALUE), (d2_point)(g_b_y1 << SHIFT_VALUE), (d2_width)(g_b_l << SHIFT_VALUE), (d2_width)(g_b_w << SHIFT_VALUE));
    error_handler(DRW_err, gp_davey);

    /* Set the render color to yellow */
    DRW_err = d2_setcolor(gp_davey, ARRAY_INDEX, YELLOW_COLOR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Render a line */
    DRW_err = d2_renderline(gp_davey, (d2_point) (g_l_x1 << SHIFT_VALUE), (d2_point) (g_l_y1 << SHIFT_VALUE), (d2_point) (g_l_x2 << SHIFT_VALUE), (d2_point) (g_l_y2 << SHIFT_VALUE),
                            (d2_width) (g_l_w << SHIFT_VALUE), d2_le_exclude_both);
    error_handler(DRW_err, gp_davey);

    /* End the display list, then call d2_startframe to begin execution */
    DRW_err = d2_endframe(gp_davey);
    error_handler(DRW_err, gp_davey);
}
#endif
/*******************************************************************************************************************//**
 * User-defined function to set framebuffer properties
 *
 * @param[in]  framebuffer    Pointer to frame buffer
 * @retval     none
 **********************************************************************************************************************/
#if defined(BOARD_RA8D1_EK)
static void drw_set(uint32_t * framebuffer)
{
    /* Start a frame */
    DRW_err = d2_startframe(gp_davey);
    error_handler(DRW_err, gp_davey);

    /* Set the frame buffer */
    DRW_err = d2_framebuffer(gp_davey, framebuffer, DRW_TEST_IMAGE_RES_H, DRW_TEST_IMAGE_RES_H, DRW_TEST_IMAGE_RES_V, d2_mode_rgb888);
    error_handler(DRW_err, gp_davey);
    /* Clear the frame buffer */
    DRW_err = d2_clear(gp_davey, BUFFER_CLEAR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Set anti-aliasing and line cap settings */
    DRW_err = d2_setantialiasing(gp_davey, ANTI_ALIASING_VAL);
    error_handler(DRW_err, gp_davey);

    DRW_err = d2_setlinecap(gp_davey, d2_lc_round);
    error_handler(DRW_err, gp_davey);

    DRW_err = d2_setlinejoin(gp_davey, d2_lj_none);
    error_handler(DRW_err, gp_davey);

    /* Set render mode */
    DRW_err = d2_selectrendermode(gp_davey , d2_rm_solid);
    error_handler(DRW_err, gp_davey);
}
#else
static void drw_set(uint8_t * framebuffer)
{
    /* Start a frame */
    DRW_err = d2_startframe(gp_davey);
    error_handler(DRW_err, gp_davey);

    /* Set the frame buffer */
    DRW_err = d2_framebuffer(gp_davey, framebuffer, DRW_TEST_IMAGE_RES_H, DRW_TEST_IMAGE_RES_H, DRW_TEST_IMAGE_RES_V, d2_mode_rgb888);
    error_handler(DRW_err, gp_davey);

    /* Clear the frame buffer */
    DRW_err = d2_clear(gp_davey, BUFFER_CLEAR_VAL);
    error_handler(DRW_err, gp_davey);

    /* Set anti-aliasing and line cap settings */
    DRW_err = d2_setantialiasing(gp_davey, ANTI_ALIASING_VAL);
    error_handler(DRW_err, gp_davey);

    DRW_err = d2_setlinecap(gp_davey, d2_lc_round);
    error_handler(DRW_err, gp_davey);

    DRW_err = d2_setlinejoin(gp_davey, d2_lj_none);
    error_handler(DRW_err, gp_davey);

    /* Set render mode */
    DRW_err = d2_selectrendermode(gp_davey , d2_rm_solid);
    error_handler(DRW_err, gp_davey);
}
#endif

/*******************************************************************************************************************//**
 * User defined function to handle animation of different figures
 *
 * @param      none
 * @retval     none
 **********************************************************************************************************************/
static void drw_animation(void)
#if defined(BOARD_RA8D1_EK)
{
    /* circle animation */
    if((g_c_c_y1 < TRIANGLE_Y2) && (g_c_left_flag == true))
    {
        g_c_c_y1 = g_c_c_y1 + 2;
    }
    if(g_c_c_y1 >= TRIANGLE_Y2)
    {
        g_c_right_flag = true;
        g_c_left_flag = false;
    }

    if((g_c_c_y1 > CIRCLE_VAL_Y) && (g_c_right_flag == true))
    {
        g_c_c_y1 = g_c_c_y1 - 2;
    }
    if(g_c_c_y1 <= CIRCLE_VAL_Y)
    {
        g_c_left_flag = true;
        g_c_right_flag = false;
    }

    /* line animation */
    if((g_l_y1 > BOX_VAL_Y1) && (g_l_right_flag == true))
    {
        g_l_y1 = g_l_y1 - 2;
        g_l_y2 = g_l_y2 - 2;
    }
    if(g_l_y2 <= BOX_VAL_Y1)
    {
        g_l_left_flag = true;
        g_l_right_flag = false;
    }

    if((g_l_y1 < LINE_VAL_Y1) && (g_l_left_flag == true))
    {
        g_l_y1 = g_l_y1 + 2;
        g_l_y2 = g_l_y2 + 2;
    }
    if(g_l_y1 >= LINE_VAL_Y1)
    {
        g_l_right_flag = true;
        g_l_left_flag = false;
    }

    /* triangle animation */
    if((g_t_x1 <= TRIANGLE_X1) && (g_t_top_flag == true))
    {
        g_t_x1++;
        g_t_x2++;
        g_t_x3++;
    }
    if(g_t_x1 > TRIANGLE_X1)
    {
        g_t_top_flag = false;
        g_t_bottom_flag = true;
    }

    if((g_t_x1 >= LINE_VAL_X2) && (g_t_bottom_flag == true))
    {
        g_t_x1--;
        g_t_x2--;
        g_t_x3--;
    }
    if(g_t_x1 < LINE_VAL_X2)
    {
        g_t_top_flag = true;
        g_t_bottom_flag = false;
    }

    /* Polygon animation */
    if((g_b_x1 > BOX_VAL_X1) && (g_p_bottom_flag == true))
    {
        g_b_x1--;
    }
    if(g_b_x1 <= BOX_VAL_X1)
    {
        g_p_top_flag = true;
        g_p_bottom_flag = false;
    }

    if((g_b_x1 < BOX_PARAM) && (g_p_top_flag == true))
    {
        g_b_x1++;
    }
    if(g_l_y1 < BOX_PARAM)
    {
        g_p_bottom_flag = true;
        g_p_top_flag = false;
    }

}
#else
{
    /* circle animation */
    if((g_c_c_x1 < TRIANGLE_X2) && (g_c_left_flag == true))
    {
        g_c_c_x1++;
    }
    if(g_c_c_x1 >= TRIANGLE_X2)
    {
        g_c_right_flag = true;
        g_c_left_flag = false;
    }

    if((g_c_c_x1 > CIRCLE_VAL_X) && (g_c_right_flag == true))
    {
        g_c_c_x1--;
    }
    if(g_c_c_x1 <= CIRCLE_VAL_X)
    {
        g_c_left_flag = true;
        g_c_right_flag = false;
    }

    /* line animation */
    if((g_l_x1 > BOX_VAL_X1) && (g_l_right_flag == true))
    {
        g_l_x1--;
        g_l_x2--;
    }
    if(g_l_x1 <= BOX_VAL_X1)
    {
        g_l_left_flag = true;
        g_l_right_flag = false;
    }

    if((g_l_x1 < LINE_VAL_X1) && (g_l_left_flag == true))
    {
        g_l_x1++;
        g_l_x2++;
    }
    if(g_l_x1 >= LINE_VAL_X1)
    {
        g_l_right_flag = true;
        g_l_left_flag = false;
    }

    /* triangle animation */
    if((g_t_y1 < LINE_VAL_Y1) && (g_t_top_flag == true))
    {
        g_t_y1++;
        g_t_y2++;
        g_t_y3++;
    }
    if(g_t_y1 >= LINE_VAL_Y1)
    {
        g_t_top_flag = false;
        g_t_bottom_flag = true;
    }

    if((g_t_y1 > TRIANGLE_Y1) && (g_t_bottom_flag == true))
    {
        g_t_y1--;
        g_t_y2--;
        g_t_y3--;
    }
    if(g_t_y1 <= TRIANGLE_Y1)
    {
        g_t_top_flag = true;
        g_t_bottom_flag = false;
    }

    /* Polygon animation */
    if((g_b_y1 > BOX_PARAM) && (g_p_bottom_flag == true))
    {
        g_b_y1--;
    }
    if(g_b_y1 <= BOX_PARAM)
    {
        g_p_top_flag = true;
        g_p_bottom_flag = false;
    }

    if((g_b_y1 < BOX_VAL_Y1) && (g_p_top_flag == true))
    {
        g_b_y1++;
    }
    if(g_l_x1 >= BOX_VAL_Y1)
    {
        g_p_bottom_flag = true;
        g_p_top_flag = false;
    }
}
#endif

#if defined(BOARD_RA8D1_EK)
/*******************************************************************************************************************//**
 * @brief      Initialize LCD
 *
 * @param[in]  table  LCD Controller Initialization structure.
 * @retval     None.
 **********************************************************************************************************************/
void mipi_dsi_push_table (const lcd_table_setting_t *table)
{
    fsp_err_t err = FSP_SUCCESS;
    const lcd_table_setting_t *p_entry = table;

    while (MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE != p_entry->cmd_id)
    {
        mipi_dsi_cmd_t msg =
        {
          .channel = 0,
          .cmd_id = p_entry->cmd_id,
          .flags = p_entry->flags,
          .tx_len = p_entry->size,
          .p_tx_buffer = p_entry->buffer,
        };

        if (MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG == msg.cmd_id)
        {
            R_BSP_SoftwareDelay (table->size, BSP_DELAY_UNITS_MILLISECONDS);
        }
        else
        {
            g_message_sent = false;
            /* Send a command to the peripheral device */
            err = R_MIPI_DSI_Command (&g_mipi_dsi0_ctrl, &msg);
            handle_error(err, "** MIPI DSI Command API failed ** \r\n");
            /* Wait */
            while (!g_message_sent);
        }
        p_entry++;
    }
}


/*******************************************************************************************************************//**
 *  @brief       This function handles errors, closes all opened modules, and prints errors.
 *
 *  @param[in]   err       error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error (fsp_err_t err,  const char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_ERR_PRINT(err_str);

        /* Close opened GLCD module*/
        if(RESET_VALUE != g_display_ctrl.state)
        {
            if(FSP_SUCCESS != R_GLCDC_Close (&g_display_ctrl))
            {
                APP_ERR_PRINT("GLCDC Close API failed\r\n");
            }
        }

        APP_ERR_TRAP(err);
    }
}


/*******************************************************************************************************************//**
 * @brief      Callback functions for MIPI DSI interrupts
 *
 * @param[in]  p_args    Callback arguments
 * @retval     none
 **********************************************************************************************************************/
void mipi_dsi_callback(mipi_dsi_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case MIPI_DSI_EVENT_SEQUENCE_0:
        {
            if (MIPI_DSI_SEQUENCE_STATUS_DESCRIPTORS_FINISHED == p_args->tx_status)
            {
                g_message_sent = SET_FLAG;
            }
            break;
        }
        default:
        {
            break;
        }

    }
}
#endif
/*******************************************************************************************************************//**
 * @} (end addtogroup DRW_ep)
 **********************************************************************************************************************/
