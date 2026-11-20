/***********************************************************************************************************************
 * File Name    : mipi_dsi_ep.c
 * Description  : Contains data structures and functions used in mipi_dsi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "mipi_dsi_ep.h"

/* Global variables */
volatile bool g_vsync_flag = false;
volatile bool g_irq_state = false;
volatile bool g_timer_overflow = false;

/* Private variables */
static volatile bool s_message_sent = false;
static volatile mipi_dsi_phy_status_t s_phy_status;
static timer_info_t s_timer_info =
{
    .clock_frequency = RESET_VALUE,
    .count_direction = RESET_VALUE,
    .period_counts = RESET_VALUE
};

/* This table of commands was adapted from sample code provided by FocusLCD
 * Page Link: https://focuslcds.com/product/4-5-tft-display-capacitive-tp-e45ra-mw276-c/
 * File Link: https://focuslcds.com/content/E45RA-MW276-C_init_code.txt */
const lcd_table_setting_t g_lcd_init_focuslcd[] =
{
    /* Change to page 1 CMD */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x08, 0x10}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Output SDA */
    {2, {0x21, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* DE = 1 active */

    /* Resolution setting 480 X 800 */
    {2, {0x30, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x31, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Inversion setting */

    {2, {0x40, 0x14}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* BT 15 */
    {2, {0x41, 0x33}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* avdd+5.2v, avee-5.2v */

    /* VGL=DDVDL+VCL-VCIP, VGH=2DDVDH-DDVDL */
    {2, {0x42, 0x02}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x43, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VGH clamp level */
    {2, {0x44, 0x06}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VGL clamp level */
    {2, {0x50, 0x70}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VREG1 */
    {2, {0x51, 0x70}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VREG2 */
    {2, {0x52, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Flicker MSB */
    {2, {0x53, 0x48}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Flicker LSB */
    {2, {0x60, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
    {2, {0x61, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
    {2, {0x62, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
    {2, {0x63, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */

    {2, {0xa0, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 1 */
    {2, {0xa1, 0x03}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 2 */
    {2, {0xa2, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 3 */
    {2, {0xa3, 0x0d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 4 */
    {2, {0xa4, 0x06}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 5 */
    {2, {0xa5, 0x16}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 6 */
    {2, {0xa6, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 7 */
    {2, {0xa7, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 8 */
    {2, {0xa8, 0x03}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 9 */
    {2, {0xa9, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 10 */
    {2, {0xaa, 0x06}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 11 */
    {2, {0xab, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 12 */
    {2, {0xac, 0x0d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 13 */
    {2, {0xad, 0x2c}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 14 */
    {2, {0xae, 0x26}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 15 */
    {2, {0xaf, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 16 */

    /* Negative Gamma Correction 1 */
    {2, {0xc0, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 2 */
    {2, {0xc1, 0x04}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 3 */
    {2, {0xc2, 0x0b}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 4 */
    {2, {0xc3, 0x0f}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 5 */
    {2, {0xc4, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 6 */
    {2, {0xc5, 0x18}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 7 */
    {2, {0xc6, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 8 */
    {2, {0xc7, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 9 */
    {2, {0xc8, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 10 */
    {2, {0xc9, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 11 */
    {2, {0xca, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 12 */
    {2, {0xcb, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 13 */
    {2, {0xcc, 0x0c}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 14 */
    {2, {0xcd, 0x2d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 15 */
    {2, {0xce, 0x28}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Negative Gamma Correction 16 */
    {2, {0xcf, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Change to Page 6 CMD for GIP timing */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x06}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x00, 0x21}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x01, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x02, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x03, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x04, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x05, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x06, 0x80}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x07, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x08, 0x02}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x09, 0x80}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0a, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0b, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0c, 0x0a}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0d, 0x0a}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0e, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x0f, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x10, 0xe0}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x11, 0xe4}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x12, 0x04}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x13, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x14, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x15, 0xc0}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x16, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x17, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x18, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x19, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1a, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1b, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1c, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
    {2, {0x1d, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */

    {2, {0x20, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x21, 0x23}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x22, 0x45}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x23, 0x67}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x24, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x25, 0x23}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x26, 0x45}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
    {2, {0x27, 0x67}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */

    {2, {0x30, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x31, 0x11}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x32, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x33, 0xee}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x34, 0xff}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x35, 0xcb}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x36, 0xda}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x37, 0xad}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x38, 0xbc}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x39, 0x76}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3a, 0x67}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3b, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3c, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3d, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3e, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x3f, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
    {2, {0x40, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */

    {2, {0x53, 0x10}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GOUT VGLO Control */
    {2, {0x54, 0x10}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GOUT VGHO Control */

    /* Change to Page 7 CMD for Normal command */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x07}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x18, 0x1d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* VREG1/2OUT ENABLE */
    {2, {0x26, 0xb2}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x02, 0x77}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0xe1, 0x79}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
    {2, {0x17, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* VGL_REG Disable */

    /* Change to Page 0 CMD for Normal command */
    {6, {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x00}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},

    /* Sleep out command may not be issued within 120 ms of GPIO HW reset. Wait to ensure timing maintained */
    {120, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG, (mipi_dsi_cmd_flag_t)0},
    {2, {0x11, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Sleep-Out */
    {5, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG, (mipi_dsi_cmd_flag_t)0},           /* Delay 5msec */
    {2, {0x29, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Display on */
    {2, {0x3a, 0x70}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* 24-bit/pixel */
    {0x00, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE, (mipi_dsi_cmd_flag_t)0},      /* End of table */
};

/*******************************************************************************************************************//**
 * @brief     This function pushes an LCD controller initialization table to the display.
 * @param[in] table                     Pointer to the LCD controller initialization table.
 * @retval    FSP_SUCCESS               Pushed successfully.
 * @retval    Any other error codes     Failed to push.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_push_table(const lcd_table_setting_t * table)
{
    fsp_err_t err = FSP_SUCCESS;
    const lcd_table_setting_t * p_entry = table;

    while (MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE != p_entry->cmd_id)
    {
        mipi_dsi_cmd_t msg =
        {
            .channel     = 0,
            .cmd_id      = p_entry->cmd_id,
            .flags       = p_entry->flags,
            .tx_len      = p_entry->size,
            .p_tx_buffer = p_entry->buffer,
        };

        if (MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG == msg.cmd_id)
        {
            R_BSP_SoftwareDelay(p_entry->size, BSP_DELAY_UNITS_MILLISECONDS);
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

            /* Wait for the command to be sent */
            while (!s_message_sent);
        }
        p_entry++;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function clears the MIPI DSI PHY status and starts entering Ultra-low Power State (ULPS).
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_ulps_enter_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear the PHY status before waiting for a new PHY event */
    s_phy_status = MIPI_DSI_PHY_STATUS_NONE;

    err = R_MIPI_DSI_UlpsEnter(&g_mipi_dsi0_ctrl, (mipi_dsi_lane_t) MIPI_DSI_LANE_DATA_ALL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function clears the MIPI DSI PHY status and starts exiting Ultra-low Power State (ULPS).
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_ulps_exit_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear the PHY status before waiting for a new PHY event */
    s_phy_status = MIPI_DSI_PHY_STATUS_NONE;

    err = R_MIPI_DSI_UlpsExit(&g_mipi_dsi0_ctrl, (mipi_dsi_lane_t) (MIPI_DSI_LANE_DATA_ALL));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens the GLCDC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_display_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GLCDC_Open(&g_display_ctrl, &g_display_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts video mode on the GLCDC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_display_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GLCDC_Start(&g_display_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function updates the GLCDC buffer pointer on the next Vsync.
 * @param[in] p_buffer                  Pointer to the frame buffer to display.
 * @retval    FSP_SUCCESS               Updated successfully.
 * @retval    Any other error codes     Failed to update.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_display_buffer_change(uint8_t * p_buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GLCDC_BufferChange(&g_display_ctrl, p_buffer, DISPLAY_FRAME_LAYER_1);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens the GPT module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_gpt_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the GPT timer clock information used to calculate the timer period.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_timer_info_get(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_InfoGet(&g_timer0_ctrl, &s_timer_info);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the GPT module to always display without entering ULPS.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_gpt_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Stop(&g_timer0_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function resets the GPT module count without changing its period.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Reset successfully.
 * @retval    Any other error codes     Failed to reset.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_gpt_reset(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Reset(&g_timer0_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens the I2C Master module used to communicate with the touch controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_touch_i2c_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_IIC_MASTER_Open(&g_i2c_master_ctrl, &g_i2c_master_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens the ICU module used for the touch controller interrupt.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_touch_irq_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables the ICU module used for the touch controller interrupt.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t mipi_dsi_touch_irq_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for a MIPI DSI PHY event.
 * @param[in] event                     Expected event.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t wait_for_mipi_dsi_event(mipi_dsi_phy_status_t event)
{
    uint32_t timeout = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_ICLK) / 10;

    while (--timeout && ((s_phy_status & event) != event))
    {
        /* Do nothing */
    }

    return timeout ? FSP_SUCCESS : FSP_ERR_TIMEOUT;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the timer period.
 * @param[in] period_time               Desired timer period in seconds.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t timer_period_set(uint32_t period_time)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Calculate the desired period based on the current clock. Note that this calculation could overflow if the
     * desired period is larger than UINT32_MAX / pclkd_freq_hz. A cast to uint64_t is used to prevent this */
    uint32_t period_counts = (uint32_t) (((uint64_t) s_timer_info.clock_frequency * period_time) / GPT_UNITS_SECONDS);

    /* Set the calculated period */
    err = R_GPT_PeriodSet(&g_timer0_ctrl, period_counts);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_GPT_Reset(&g_timer0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    g_timer_overflow = false;

    err = R_GPT_Start(&g_timer0_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function resets the touch screen controller after power on.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void touch_screen_reset(void)
{
     /* Reset touch chip by setting GPIO reset pin low */
     R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_RST, BSP_IO_LEVEL_LOW);
     R_IOPORT_PinCfg(&g_ioport_ctrl, PIN_DISPLAY_INT, IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW);
     R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MICROSECONDS);

     /* Set INT high to select the device slave address before releasing reset */
     R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_INT, BSP_IO_LEVEL_HIGH);
     R_BSP_SoftwareDelay(200, BSP_DELAY_UNITS_MICROSECONDS);

     /* Release touch chip from reset */
     R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_RST, BSP_IO_LEVEL_HIGH);
     R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

     /* Hold INT low before releasing control of the interrupt pin */
     R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_INT, BSP_IO_LEVEL_LOW);
     R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

      /* Release touch chip interrupt pin for control */
     R_IOPORT_PinCfg(&g_ioport_ctrl, PIN_DISPLAY_INT, IOPORT_CFG_PORT_DIRECTION_INPUT |\
                     IOPORT_CFG_EVENT_RISING_EDGE | IOPORT_CFG_IRQ_ENABLE);
}

/*******************************************************************************************************************//**
 * @brief     This function closes all the opened modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void mipi_dsi_deinit(void)
{
    /* Close the opened GPT module */
    if (g_timer0_ctrl.open)
    {
        R_GPT_Close(&g_timer0_ctrl);
    }

    /* Close the opened GLCDC module */
    if (g_display_ctrl.state)
    {
        R_GLCDC_Close(&g_display_ctrl);
    }

    /* Close the opened ICU module */
    if (g_external_irq_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    }

    /* Close the opened I2C Master module */
    if (g_i2c_master_ctrl.open)
    {
        R_IIC_MASTER_Close(&g_i2c_master_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for IRQ module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t * p_args)
{
    if (g_external_irq_cfg.channel == p_args->channel)
    {
        g_irq_state = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for GLCDC module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void glcdc_callback(display_callback_args_t * p_args)
{
    if (DISPLAY_EVENT_LINE_DETECTION == p_args->event)
    {
        g_vsync_flag = true;
    }
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

        case MIPI_DSI_EVENT_PHY:
        {
            s_phy_status |= p_args->phy_status;
            break;
        }

        default:
        {
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for GPT module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t * p_args)
{
    /* Check for the event */
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_timer_overflow = true;
    }
}
