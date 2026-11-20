/***********************************************************************************************************************
 * File Name    : drw_ep.c
 * Description  : Contains data structures and functions used in drw_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "drw_ep.h"

/* Global variables */
d2_device * gp_davey = NULL;
volatile uint8_t g_vsync_flag = false;

/* Variables for different figures */
/* Circle variables */
static d2_point s_c_c_x1 = CIRCLE_VAL_X, s_c_c_y1 = CIRCLE_VAL_Y, s_c_r = CIRCLE_VAL_R, s_c_w = CIRCLE_VAL_W;
static bool s_c_left_flag = true, s_c_right_flag = false;

/* Triangle variables */
static d2_point s_t_x1 = TRIANGLE_X1, s_t_y1 = TRIANGLE_Y1, s_t_x2 = TRIANGLE_X2, s_t_y2 = TRIANGLE_Y2,
                s_t_x3 = TRIANGLE_X3, s_t_y3 = TRIANGLE_Y3;
static bool s_t_top_flag = true, s_t_bottom_flag = false;

/* Box variables */
static d2_point s_b_x1 = BOX_VAL_X1, s_b_y1 = BOX_VAL_Y1, s_b_l = BOX_VAL_LEN, s_b_w = BOX_VAL_WIDTH;
static bool s_p_top_flag = false, s_p_bottom_flag = true;

/* Line variables */
static d2_point s_l_x1 = LINE_VAL_X1, s_l_y1 = LINE_VAL_Y1, s_l_x2 = LINE_VAL_X2, s_l_y2 = LINE_VAL_Y2,
                s_l_w = LINE_VAL_WIDTH;
static bool s_l_left_flag = false, s_l_right_flag = true;

#if defined(BOARD_RA8D1_EK)
static volatile bool s_message_sent = false;

/* This table of commands was adapted from sample code provided by FocusLCDs
 * Page Link: https://focuslcds.com/product/4-5-tft-display-capacitive-tp-e45ra-mw276-c/
 * File Link: https://focuslcds.com/wp-content/uploads/Code/E45RA-MW276-C-2LANE-MIPI-CODE.txt */
static const lcd_table_setting_t s_lcd_init_focuslcd[] =
{
    /* Change to Page 1 CMD */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x08, 0x10}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Output SDA */
    {2, {0x21, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* DE = 1 Active */

    /* Resolution setting 480 X 800 */
    {2, {0x30, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x31, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Inversion setting */

    {2, {0x40, 0x14}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* BT 15 */
    {2, {0x41, 0x33}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* avdd +5.2v, avee-5.2v */

    /* VGL=DDVDL+VCL-VCIP,VGH=2DDVDH-DDVDL */
    {2, {0x42, 0x02}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x43, 0x09}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VGH clamp level */
    {2, {0x44, 0x06}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VGL clamp level */
    {2, {0x50, 0x70}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VREG1 */
    {2, {0x51, 0x70}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VREG2 */
    {2, {0x52, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Flicker MSB */
    {2, {0x53, 0x48}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Flicker LSB */
    {2, {0x60, 0x07}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
    {2, {0x61, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
    {2, {0x62, 0x08}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
    {2, {0x63, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */

    /* Positive Gamma Control 1 */
    {2, {0xa0, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 2 */
    {2, {0xa1, 0x03}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 3 */
    {2, {0xa2, 0x09}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 4 */
    {2, {0xa3, 0x0d}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 5 */
    {2, {0xa4, 0x06}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 6 */
    {2, {0xa5, 0x16}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 7 */
    {2, {0xa6, 0x09}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 8 */
    {2, {0xa7, 0x08}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 9 */
    {2, {0xa8, 0x03}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 10 */
    {2, {0xa9, 0x07}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 11 */
    {2, {0xaa, 0x06}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 12 */
    {2, {0xab, 0x05}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 13 */
    {2, {0xac, 0x0d}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 14 */
    {2, {0xad, 0x2c}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 15 */
    {2, {0xae, 0x26}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Positive Gamma Control 16 */
    {2, {0xaf, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 1 */
    {2, {0xc0, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 2 */
    {2, {0xc1, 0x04}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 3 */
    {2, {0xc2, 0x0b}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 4 */
    {2, {0xc3, 0x0f}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 5 */
    {2, {0xc4, 0x09}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 6 */
    {2, {0xc5, 0x18}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 7 */
    {2, {0xc6, 0x07}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 8 */
    {2, {0xc7, 0x08}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 9 */
    {2, {0xc8, 0x05}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 10 */
    {2, {0xc9, 0x09}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 11 */
    {2, {0xca, 0x07}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 12 */
    {2, {0xcb, 0x05}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 13 */
    {2, {0xcc, 0x0c}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 14 */
    {2, {0xcd, 0x2d}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 15 */
    {2, {0xce, 0x28}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Negative Gamma Correction 16 */
    {2, {0xcf, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Change to Page 6 CMD for GIP timing */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x06}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x00, 0x21}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x01, 0x09}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x02, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x03, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x04, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x05, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x06, 0x80}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x07, 0x05}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x08, 0x02}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x09, 0x80}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0a, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0b, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0c, 0x0a}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0d, 0x0a}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0e, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0f, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x10, 0xe0}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x11, 0xe4}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x12, 0x04}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x13, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x14, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x15, 0xc0}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x16, 0x08}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x17, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x18, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x19, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1a, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1b, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1c, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1d, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */

    {2, {0x20, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x21, 0x23}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x22, 0x45}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x23, 0x67}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x24, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x25, 0x23}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x26, 0x45}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x27, 0x67}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */

    {2, {0x30, 0x01}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x31, 0x11}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x32, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x33, 0xee}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x34, 0xff}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x35, 0xcb}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x36, 0xda}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x37, 0xad}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x38, 0xbc}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x39, 0x76}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3a, 0x67}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3b, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3c, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3d, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3e, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3f, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x40, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */

    {2, {0x53, 0x10}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GOUT VGLO Control */
    {2, {0x54, 0x10}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GOUT VGHO Control */

    /* Change to Page 7 CMD for Normal command */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x07}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x18, 0x1d}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* VREG1/2OUT ENABLE */
    {2, {0x26, 0xb2}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x02, 0x77}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0xe1, 0x79}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x17, 0x22}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* VGL_REG Disable */

    /* Change to Page 0 CMD for Normal command */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x00}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    /* Sleep out command may not be issued within 120 ms of GPIO HW reset. Wait to ensure timing maintained */
    {120, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG, (mipi_dsi_cmd_flag_t)0},
    {2, {0x11, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Sleep-Out */
    {5, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG, (mipi_dsi_cmd_flag_t)0},               /* Delay 5msec */
    {2, {0x29, 0x00}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Display on */
    {2, {0x3a, 0x70}, MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* 24-bit / pixel */

    {0x00, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE, (mipi_dsi_cmd_flag_t)0},          /* End of table */
};
#endif /* BOARD_RA8D1_EK */

/* Private functions */
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
static d2_s32 drw_set(uint32_t * framebuffer);
#else
static d2_s32 drw_set(uint8_t * framebuffer);
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */
static void drw_animation(void);
#if defined(BOARD_RA8D1_EK)
static fsp_err_t mipi_dsi_push_table(const lcd_table_setting_t * table);
#endif /* BOARD_RA8D1_EK */

/*******************************************************************************************************************//**
 * @brief     This function initializes the GLCDC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t glcdc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GLCDC module */
    err = R_GLCDC_Open(&g_display_ctrl, &g_display_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined(BOARD_RA8D1_EK)
    /* Initialize LCD */
    err = mipi_dsi_push_table(s_lcd_init_focuslcd);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA8D1_EK */

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function changes the frame buffer for the GLCDC module.
 * @param[in] p_api_ctrl                Pointer to the GLCDC control block.
 * @param[in] framebuffer               Pointer to the frame buffer.
 * @param[in] layer                     Frame layer to be updated.
 * @retval    FSP_SUCCESS               Changed successfully.
 * @retval    Any other error codes     Failed to change.
 **********************************************************************************************************************/
fsp_err_t glcdc_buffer_change(display_ctrl_t const * const p_api_ctrl, uint8_t * const framebuffer,
                              display_frame_layer_t layer)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GLCDC_BufferChange(p_api_ctrl, framebuffer, layer);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the GLCDC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t glcdc_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GLCDC_Stop(&g_display_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened GLCDC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void glcdc_deinit(void)
{
    /* De-initialize the opened GLCDC module */
    if (g_display_ctrl.state)
    {
        R_GLCDC_Close(&g_display_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the DRW module.
 * @param[in] None.
 * @retval    D2_OK                     Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
d2_s32 drw_init(void)
{
    d2_s32 drw_err = D2_OK;

    /* Open the D2 driver and initialize the hardware */
    gp_davey = d2_opendevice(BITFIELD_VALUE);
    drw_err = d2_inithw(gp_davey, BITFIELD_VALUE);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the transparency and the alpha mode */
    drw_err = d2_setalpha(gp_davey, ALPHA_VALUE);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    drw_err = d2_setalphamode(gp_davey, d2_am_constant);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set alpha blend mode */
    drw_err = d2_setblendmode(gp_davey, d2_bm_alpha, d2_bm_one_minus_alpha);

    return drw_err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened DRW module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void drw_deinit(void)
{
    if (NULL != gp_davey)
    {
        (void)d2_deinithw(gp_davey);
        (void)d2_closedevice(gp_davey);

        gp_davey = NULL;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function draws graphical objects to the frame buffer using the DRW.
 * @param[in] framebuffer               Pointer to the frame buffer.
 * @retval    D2_OK                     Displayed successfully.
 * @retval    Any other error codes     Failed to display.
 **********************************************************************************************************************/
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
d2_s32 display_draw(uint32_t * framebuffer)
{
    d2_s32 drw_err = D2_OK;

    /* DRW operations happen here */
    /* Animation of different figure */
    drw_animation();

    /* Set frame buffer properties */
    drw_err = drw_set(framebuffer);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to red */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, RED_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a circle */
    drw_err = d2_rendercircle(gp_davey, (d2_point)(s_c_c_x1 << SHIFT_VALUE), (d2_point)(s_c_c_y1 << SHIFT_VALUE),
                              (d2_width)(s_c_r << SHIFT_VALUE), (d2_width)(s_c_w << SHIFT_VALUE));
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to green */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, GREEN_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a triangle */
    drw_err = d2_rendertri(gp_davey, (d2_point)(s_t_x1 << SHIFT_VALUE), (d2_point)(s_t_y1 << SHIFT_VALUE),
                           (d2_point)(s_t_x2 << SHIFT_VALUE), (d2_point)(s_t_y2 << SHIFT_VALUE),
                           (d2_point)(s_t_x3 << SHIFT_VALUE), (d2_point)(s_t_y3 << SHIFT_VALUE), d2_edge2_shared);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to blue */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, BLUE_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a box */
    drw_err = d2_renderbox(gp_davey, (d2_point)(s_b_x1 << SHIFT_VALUE), (d2_point)(s_b_y1 << SHIFT_VALUE),
                           (d2_width)(s_b_l << SHIFT_VALUE), (d2_width)(s_b_w << SHIFT_VALUE));
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to yellow */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, YELLOW_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a line */
    drw_err = d2_renderline(gp_davey, (d2_point)(s_l_x1 << SHIFT_VALUE), (d2_point)(s_l_y1 << SHIFT_VALUE),
                            (d2_point)(s_l_x2 << SHIFT_VALUE), (d2_point)(s_l_y2 << SHIFT_VALUE),
                            (d2_width)(s_l_w << SHIFT_VALUE), d2_le_exclude_both);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* End the display list, then call d2_start frame to begin execution */
    drw_err = d2_endframe(gp_davey);

    return drw_err;
}
#else
d2_s32 display_draw(uint8_t * framebuffer)
{
    d2_s32 drw_err = D2_OK;

    /* DRW operations happens here */
    /* Animation of different figures */
    drw_animation();

    /* Set frame buffer properties */
    drw_err = drw_set(framebuffer);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to red */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, RED_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a circle */
    drw_err = d2_rendercircle(gp_davey, (d2_point)(s_c_c_x1 << SHIFT_VALUE), (d2_point)(s_c_c_y1 << SHIFT_VALUE),
                              (d2_width)(s_c_r << SHIFT_VALUE), (d2_width)(s_c_w << SHIFT_VALUE));
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to green */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, GREEN_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a triangle */
    drw_err = d2_rendertri(gp_davey, (d2_point)(s_t_x1 << SHIFT_VALUE), (d2_point)(s_t_y1 << SHIFT_VALUE),
                           (d2_point)(s_t_x2 << SHIFT_VALUE), (d2_point)(s_t_y2 << SHIFT_VALUE),
                           (d2_point)(s_t_x3 << SHIFT_VALUE), (d2_point)(s_t_y3 << SHIFT_VALUE), d2_edge2_shared);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to blue */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, BLUE_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a box */
    drw_err = d2_renderbox(gp_davey, (d2_point)(s_b_x1 << SHIFT_VALUE), (d2_point)(s_b_y1 << SHIFT_VALUE),
                           (d2_width)(s_b_l << SHIFT_VALUE), (d2_width)(s_b_w << SHIFT_VALUE));
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the render color to yellow */
    drw_err = d2_setcolor(gp_davey, ARRAY_INDEX, YELLOW_COLOR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Render a line */
    drw_err = d2_renderline(gp_davey, (d2_point)(s_l_x1 << SHIFT_VALUE), (d2_point)(s_l_y1 << SHIFT_VALUE),
                            (d2_point)(s_l_x2 << SHIFT_VALUE), (d2_point)(s_l_y2 << SHIFT_VALUE),
                            (d2_width)(s_l_w << SHIFT_VALUE), d2_le_exclude_both);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* End the display list, then call d2_startframe to begin execution */
    drw_err = d2_endframe(gp_davey);

    return drw_err;
}
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

/*******************************************************************************************************************//**
 * @brief     This function sets the DRW rendering settings and frame buffer.
 * @param[in] framebuffer               Pointer to the frame buffer.
 * @retval    D2_OK                     Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
static d2_s32 drw_set(uint32_t * framebuffer)
{
    d2_s32 drw_err = D2_OK;

    /* Start a frame */
    drw_err = d2_startframe(gp_davey);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the frame buffer */
    drw_err = d2_framebuffer(gp_davey, framebuffer, DRW_TEST_IMAGE_RES_H, DRW_TEST_IMAGE_RES_H,
                             DRW_TEST_IMAGE_RES_V, d2_mode_rgb888);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Clear the frame buffer */
    drw_err = d2_clear(gp_davey, BUFFER_CLEAR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set anti-aliasing and line cap settings */
    drw_err = d2_setantialiasing(gp_davey, ANTI_ALIASING_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    drw_err = d2_setlinecap(gp_davey, d2_lc_round);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    drw_err = d2_setlinejoin(gp_davey, d2_lj_none);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set render mode */
    drw_err = d2_selectrendermode(gp_davey, d2_rm_solid);

    return drw_err;
}
#else
static d2_s32 drw_set(uint8_t * framebuffer)
{
    d2_s32 drw_err = D2_OK;

    /* Start a frame */
    drw_err = d2_startframe(gp_davey);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set the frame buffer */
    drw_err = d2_framebuffer(gp_davey, framebuffer, DRW_TEST_IMAGE_RES_H, DRW_TEST_IMAGE_RES_H,
                             DRW_TEST_IMAGE_RES_V, d2_mode_rgb565);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Clear the frame buffer */
    drw_err = d2_clear(gp_davey, BUFFER_CLEAR_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set anti-aliasing and line cap settings */
    drw_err = d2_setantialiasing(gp_davey, ANTI_ALIASING_VAL);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    drw_err = d2_setlinecap(gp_davey, d2_lc_round);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    drw_err = d2_setlinejoin(gp_davey, d2_lj_none);
    if (D2_OK != drw_err)
    {
        return drw_err;
    }

    /* Set render mode */
    drw_err = d2_selectrendermode(gp_davey, d2_rm_solid);

    return drw_err;
}
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

/*******************************************************************************************************************//**
 * @brief     This function updates the animation state of graphical objects.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void drw_animation(void)
#if defined(BOARD_RA8D1_EK)
{
    /* Circle animation */
    if ((TRIANGLE_Y2 > s_c_c_y1) && s_c_left_flag)
    {
        s_c_c_y1 += 2;
    }
    if (TRIANGLE_Y2 <= s_c_c_y1)
    {
        s_c_right_flag = true;
        s_c_left_flag = false;
    }

    if ((CIRCLE_VAL_Y < s_c_c_y1) && s_c_right_flag)
    {
        s_c_c_y1 -= 2;
    }
    if (CIRCLE_VAL_Y >= s_c_c_y1)
    {
        s_c_left_flag = true;
        s_c_right_flag = false;
    }

    /* Line animation */
    if ((BOX_VAL_Y1 < s_l_y1) && s_l_right_flag)
    {
        s_l_y1 -= 2;
        s_l_y2 -= 2;
    }
    if (BOX_VAL_Y1 >= s_l_y2)
    {
        s_l_left_flag = true;
        s_l_right_flag = false;
    }

    if ((LINE_VAL_Y1 > s_l_y1) && s_l_left_flag)
    {
        s_l_y1 += 2;
        s_l_y2 += 2;
    }
    if (LINE_VAL_Y1 <= s_l_y1)
    {
        s_l_right_flag = true;
        s_l_left_flag = false;
    }

    /* Triangle animation */
    if ((TRIANGLE_X1 >= s_t_x1) && s_t_top_flag)
    {
        s_t_x1++;
        s_t_x2++;
        s_t_x3++;
    }
    if (TRIANGLE_X1 < s_t_x1)
    {
        s_t_top_flag = false;
        s_t_bottom_flag = true;
    }

    if ((LINE_VAL_X2 <= s_t_x1) && s_t_bottom_flag)
    {
        s_t_x1--;
        s_t_x2--;
        s_t_x3--;
    }
    if (LINE_VAL_X2 > s_t_x1)
    {
        s_t_top_flag = true;
        s_t_bottom_flag = false;
    }

    /* Polygon animation */
    if ((BOX_VAL_X1 < s_b_x1) && s_p_bottom_flag)
    {
        s_b_x1--;
    }
    if (BOX_VAL_X1 >= s_b_x1)
    {
        s_p_top_flag = true;
        s_p_bottom_flag = false;
    }

    if ((BOX_PARAM > s_b_x1) && s_p_top_flag)
    {
        s_b_x1++;
    }
    if (BOX_PARAM <= s_b_x1)
    {
        s_p_bottom_flag = true;
        s_p_top_flag = false;
    }
}
#else
{
    /* Circle animation */
    if ((TRIANGLE_X2 > s_c_c_x1) && s_c_left_flag)
    {
        s_c_c_x1 += STEP_X;
    }
    if (TRIANGLE_X2 <= s_c_c_x1)
    {
        s_c_right_flag = true;
        s_c_left_flag = false;
    }

    if ((CIRCLE_VAL_X < s_c_c_x1) && s_c_right_flag)
    {
        s_c_c_x1 -= STEP_X;
    }
    if (CIRCLE_VAL_X >= s_c_c_x1)
    {
        s_c_left_flag = true;
        s_c_right_flag = false;
    }

    /* Line animation */
    if ((BOX_VAL_X1 < s_l_x1) && s_l_right_flag)
    {
        s_l_x1 -= STEP_X;
        s_l_x2 -= STEP_X;
    }
    if (BOX_VAL_X1 >= s_l_x1)
    {
        s_l_left_flag = true;
        s_l_right_flag = false;
    }

    if ((LINE_VAL_X1 > s_l_x1) && s_l_left_flag)
    {
        s_l_x1 += STEP_X;
        s_l_x2 += STEP_X;
    }
    if (LINE_VAL_X1 <= s_l_x1)
    {
        s_l_right_flag = true;
        s_l_left_flag = false;
    }

    /* Triangle animation */
    if ((LINE_VAL_Y1 > s_t_y1) && s_t_top_flag)
    {
        s_t_y1 += STEP_Y;
        s_t_y2 += STEP_Y;
        s_t_y3 += STEP_Y;
    }
    if (LINE_VAL_Y1 <= s_t_y1)
    {
        s_t_top_flag = false;
        s_t_bottom_flag = true;
    }

    if ((TRIANGLE_Y1 < s_t_y1) && s_t_bottom_flag)
    {
        s_t_y1 -= STEP_Y;
        s_t_y2 -= STEP_Y;
        s_t_y3 -= STEP_Y;
    }
    if (TRIANGLE_Y1 >= s_t_y1)
    {
        s_t_top_flag = true;
        s_t_bottom_flag = false;
    }

    /* Polygon animation */
    if ((BOX_PARAM < s_b_y1) && s_p_bottom_flag)
    {
        s_b_y1 -= STEP_Y;
    }
    if (BOX_PARAM >= s_b_y1)
    {
        s_p_top_flag = true;
        s_p_bottom_flag = false;
    }

    if ((BOX_VAL_Y1 > s_b_y1) && s_p_top_flag)
    {
        s_b_y1 += STEP_Y;
    }
    if (BOX_VAL_Y1 <= s_b_y1)
    {
        s_p_bottom_flag = true;
        s_p_top_flag = false;
    }
}
#endif /* BOARD_RA8D1_EK */

#if defined(BOARD_RA8D1_EK)
/*******************************************************************************************************************//**
 * @brief     This function initializes the LCD.
 * @param[in] table                     Pointer to the LCD controller initialization structure.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t mipi_dsi_push_table(const lcd_table_setting_t * table)
{
    fsp_err_t err = FSP_SUCCESS;
    const lcd_table_setting_t *p_entry = table;

    while (MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE != p_entry->cmd_id)
    {
        mipi_dsi_cmd_t msg =
        {
            .channel = 0,
            .cmd_id = (mipi_cmd_id_t)p_entry->cmd_id,
            .flags = p_entry->flags,
            .tx_len = p_entry->size,
            .p_tx_buffer = p_entry->buffer,
        };

        if ((mipi_cmd_id_t)MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG == msg.cmd_id)
        {
            R_BSP_SoftwareDelay(table->size, BSP_DELAY_UNITS_MILLISECONDS);
        }
        else
        {
            s_message_sent = false;

            /* Send a command to the peripheral device */
            err = R_MIPI_DSI_Command(&g_mipi_dsi0_ctrl, &msg);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Wait */
            while (!s_message_sent);
        }
        p_entry++;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for MIPI DSI module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void mipi_dsi_callback(mipi_dsi_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case MIPI_DSI_EVENT_SEQUENCE_0:
        {
            if (MIPI_DSI_SEQUENCE_STATUS_DESCRIPTORS_FINISHED == p_args->tx_status)
            {
                s_message_sent = true;
            }
            break;
        }

        default:
        {
            break;
        }
    }
}
#endif /* BOARD_RA8D1_EK */

/*******************************************************************************************************************//**
 * @brief     This function is a callback for GLCDC module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void glcdc_callback(display_callback_args_t * p_args)
{
    if (p_args->event == DISPLAY_EVENT_LINE_DETECTION)
    {
        g_vsync_flag = true;
    }
}
