/***********************************************************************************************************************
 * File Name    : mipi_dsi_display_control.c
 * Description  : This file contains the initialization sequence for the MIPI display panel.
 *                The initialization sequence is implemented as a command table and helper functions
 *                that write data to the panel through the MIPI-DSI host controller. This sequence configures
 *                parameters such as color format, resolution, orientation, and display-on command.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <mipi_dsi_display_control.h>

volatile uint8_t g_message_sent = RESET_FLAG;
volatile mipi_dsi_phy_status_t g_phy_status;


/* This table of commands was adapted from sample code provided by FocusLCD
 * Page Link: https://focuslcds.com/product/4-5-tft-display-capacitive-tp-e45ra-mw276-c/
 * File Link: https://focuslcds.com/content/E45RA-MW276-C_init_code.txt
 */
const lcd_table_setting_t g_lcd_init_focuslcd[] =
{
 /* Change to page 1 CMD */
 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x08, 0x10}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Output SDA */
 {2,  {0x21, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* DE = 1 active */

 /* Resolution setting 480 X 854 */
 {2,  {0x30, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x31, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Inversion setting */

 {2,  {0x40, 0x14}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* BT 15 */
 {2,  {0x41, 0x33}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* avdd +5.2v, avee-5.2v */
 /* VGL=DDVDL+VCL-VCIP, VGH=2DDVDH-DDVDL */
 {2,  {0x42, 0x02}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x43, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VGH clamp level */
 {2,  {0x44, 0x06}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VGL clamp level */
 {2,  {0x50, 0x70}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VREG1 */
 {2,  {0x51, 0x70}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Set VREG2 */
 {2,  {0x52, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Flicker MSB */
 {2,  {0x53, 0x48}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Flicker LSB */
 {2,  {0x60, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
 {2,  {0x61, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
 {2,  {0x62, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */
 {2,  {0x63, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Timing Adjust */

 {2,  {0xa0, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 1 */
 {2,  {0xa1, 0x03}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 2 */
 {2,  {0xa2, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 3 */
 {2,  {0xa3, 0x0d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 4 */
 {2,  {0xa4, 0x06}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 5 */
 {2,  {0xa5, 0x16}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 6 */
 {2,  {0xa6, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 7 */
 {2,  {0xa7, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 8 */
 {2,  {0xa8, 0x03}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 9 */
 {2,  {0xa9, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 10 */
 {2,  {0xaa, 0x06}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 11 */
 {2,  {0xab, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 12 */
 {2,  {0xac, 0x0d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 13 */
 {2,  {0xad, 0x2c}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 14 */
 {2,  {0xae, 0x26}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 15 */
 {2,  {0xaf, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Positive Gamma Control 16 */

 /* Negative Gamma Correction 1 */
 {2,  {0xc0, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 2 */
 {2,  {0xc1, 0x04}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 3 */
 {2,  {0xc2, 0x0b}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 4 */
 {2,  {0xc3, 0x0f}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 5 */
 {2,  {0xc4, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 6 */
 {2,  {0xc5, 0x18}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 7 */
 {2,  {0xc6, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 8 */
 {2,  {0xc7, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 9 */
 {2,  {0xc8, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 10 */
 {2,  {0xc9, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 11 */
 {2,  {0xca, 0x07}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 12 */
 {2,  {0xcb, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 13 */
 {2,  {0xcc, 0x0c}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 14 */
 {2,  {0xcd, 0x2d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 15 */
 {2,  {0xce, 0x28}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Negative Gamma Correction 16 */
 {2,  {0xcf, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},

 /* Change to Page 6 CMD for GIP timing */
 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x06}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x00, 0x21}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x01, 0x09}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x02, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x03, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x04, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x05, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x06, 0x80}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x07, 0x05}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x08, 0x02}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x09, 0x80}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x0a, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x0b, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x0c, 0x0a}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x0d, 0x0a}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x0e, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x0f, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x10, 0xe0}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x11, 0xe4}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x12, 0x04}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x13, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x14, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x15, 0xc0}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x16, 0x08}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x17, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x18, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x19, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x1a, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x1b, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x1c, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */
 {2,  {0x1d, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 1 */

 {2,  {0x20, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x21, 0x23}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x22, 0x45}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x23, 0x67}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x24, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x25, 0x23}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x26, 0x45}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */
 {2,  {0x27, 0x67}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 2 */

 {2,  {0x30, 0x01}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x31, 0x11}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x32, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x33, 0xee}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x34, 0xff}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x35, 0xcb}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x36, 0xda}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x37, 0xad}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x38, 0xbc}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x39, 0x76}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x3a, 0x67}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x3b, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x3c, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x3d, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x3e, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x3f, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */
 {2,  {0x40, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GIP Control 3 */

 {2,  {0x53, 0x10}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GOUT VGLO Control */
 {2,  {0x54, 0x10}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* GOUT VGHO Control */

 /* Change to Page 7 CMD for Normal command */
 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x07}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x18, 0x1d}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* VREG1/2OUT ENABLE */
 {2,  {0x26, 0xb2}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x02, 0x77}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0xe1, 0x79}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},
 {2,  {0x17, 0x22}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* VGL_REG Disable */

 /* Change to Page 0 CMD for Normal command */
 {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x00}, MIPI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},
 /* Sleep out command may not be issued within 120 ms of GPIO HW reset. Wait to ensure timing maintained. */
 {120, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG, (mipi_dsi_cmd_flag_t)0},
 {2,  {0x11, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Sleep-Out */
 {5,   {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG, (mipi_dsi_cmd_flag_t)0},          /* Delay 5msec */
 {2,  {0x29, 0x00}, MIPI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* Display on */
 {2,  {0x3a, 0x70}, MIPI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER}, /* 24-bit/pixel */

 {0x00, {0}, MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE, (mipi_dsi_cmd_flag_t)0}, /* End of table */
};

static void lcd_init_sequence(const lcd_table_setting_t *table);

void e45ra_mw276_lcd_init(void)
{
    /* initialize lcd*/
    lcd_init_sequence(g_lcd_init_focuslcd);
}

/*******************************************************************************************************************//**
 * @brief      Initialize LCD
 *
 * @param[in]  table  LCD Controller Initialization structure.
 * @retval     None.
 **********************************************************************************************************************/
static void lcd_init_sequence(const lcd_table_setting_t *table)
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
            if (FSP_SUCCESS != err)
            {
                __BKPT(0);
            }
            /* Wait */
            while (!g_message_sent);
        }
        p_entry++;
    }
}

/*******************************************************************************************************************//**
 * @brief      Callback functions for MIPI DSI interrupts
 *
 * @param[in]  p_args    Callback arguments
 * @retval     none
 **********************************************************************************************************************/
void mipi_dsi0_callback(mipi_dsi_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case MIPI_DSI_EVENT_POST_OPEN:
            e45ra_mw276_lcd_init();
            break;
        case MIPI_DSI_EVENT_SEQUENCE_0:
        {
            if (MIPI_DSI_SEQUENCE_STATUS_DESCRIPTORS_FINISHED == p_args->tx_status)
            {
                g_message_sent = SET_FLAG;
            }
            break;
        }
        case MIPI_DSI_EVENT_PHY:
        {
            g_phy_status |= p_args->phy_status;
            break;
        }
        default:
        {
            break;
        }

    }
}

