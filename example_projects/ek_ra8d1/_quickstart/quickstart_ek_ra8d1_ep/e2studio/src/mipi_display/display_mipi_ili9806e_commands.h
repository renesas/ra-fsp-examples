/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : display_mipi_ili9806e_commands.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * display_mipi_ili9806e_commands.h
 *
 *  Created on: Aug 17, 2023
 *      Author: Austin.Hansen
 */

#ifndef DISPLAY_MIPI_ILI9806E_DISPLAY_MIPI_ILI9806E_COMMANDS_H_
#define DISPLAY_MIPI_ILI9806E_DISPLAY_MIPI_ILI9806E_COMMANDS_H_

#define MIPI_DSI_DISPLAY_CONFIG_DATA_PAGE_CMD        (0xFF)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG      ((mipi_dsi_cmd_id_t) 0xFE)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE    ((mipi_dsi_cmd_id_t) 0xFD)

typedef struct st_mipi_setting_table
{
    const uint8_t             size;
    const uint8_t             buffer[10];
    const mipi_dsi_cmd_id_t   cmd_id;
    const mipi_dsi_cmd_flag_t flags;
} mipi_setting_table_t;

/* This table of commands was adapted from sample code provided by FocusLCD
 * Page Link: https://focuslcds.com/product/4-5-tft-display-capacitive-tp-e45ra-mw276-c/
 * File Link: https://focuslcds.com/content/E45RA-MW276-C_init_code.txt
 */
const mipi_setting_table_t p_command_list_ili9806e[] =
{
    {6,
      {
          0xFF,
          0xFF,
          0x98,
          0x06,
          0x04,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_LONG_WRITE,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page  CMD for Normal command
    {120,
      {
          0
      },
      MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG,
      (
          mipi_dsi_cmd_flag_t)
      0},                              // Sleep out command may not be issued within 120 ms of GPIO HW reset. Wait to ensure timing maintained.
    {2,
      {
          0x11,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Sleep-Out
    {5,
      {
          0
      },
      MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG,
      (
          mipi_dsi_cmd_flag_t)
      0},
    {2,
      {
          0x29,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Display on
    {2,
      {
          0x3a,
          0x70
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // 24 bit

    {6,
      {
          0xFF,
          0xFF,
          0x98,
          0x06,
          0x04,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_LONG_WRITE,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page 1
    {2,
      {
          0x08,
          0x10
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Output SDA
    {2,
      {
          0x21,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // DE = 1 Active

    {2,
      {
          0x30,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Resolution setting 480 X 854
    {2,
      {
          0x31,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Inversion setting

    {2,
      {
          0x40,
          0x14
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // BT 15
    {2,
      {
          0x41,
          0x33
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // avdd +5.2v,avee-5.2v
    {2,
      {
          0x42,
          0x02
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // VGL=DDVDH+VCIP -DDVDL,VGH=2DDVDL-VCIP
    {2,
      {
          0x43,
          0x09
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // SET VGH clamp level
    {2,
      {
          0x44,
          0x06
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // SET VGL clamp level
    {2,
      {
          0x50,
          0x70
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // VREG1
    {2,
      {
          0x51,
          0x70
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // VREG2
    {2,
      {
          0x52,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Flicker MSB
    {2,
      {
          0x53,
          0x48
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Flicker LSB
    {2,
      {
          0x60,
          0x07
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust
    {2,
      {
          0x61,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust
    {2,
      {
          0x62,
          0x08
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust
    {2,
      {
          0x63,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust

    {2,
      {
          0xa0,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Positive Gamma
    {2,
      {
          0xa1,
          0x03
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa2,
          0x09
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa3,
          0x0d
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa4,
          0x06
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa5,
          0x16
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa6,
          0x09
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa7,
          0x08
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa8,
          0x03
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xa9,
          0x07
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xaa,
          0x06
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xab,
          0x05
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xac,
          0x0d
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xad,
          0x2c
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xae,
          0x26
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xaf,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,
      {
          0xc0,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Negative Gamma
    {2,
      {
          0xc1,
          0x04
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc2,
          0x0b
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc3,
          0x0f
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc4,
          0x09
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc5,
          0x18
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc6,
          0x07
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc7,
          0x08
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc8,
          0x05
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xc9,
          0x09
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xca,
          0x07
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xcb,
          0x05
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xcc,
          0x0c
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xcd,
          0x2d
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xce,
          0x28
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xcf,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {6,
      {
          0xFF,
          0xFF,
          0x98,
          0x06,
          0x04,
          0x06
      },
      MIPI_DSI_CMD_ID_DCS_LONG_WRITE,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page 6 CMD for GIP timing
    {2,
      {
          0x00,
          0x21
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // GIP Control 1
    {2,
      {
          0x01,
          0x09
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x02,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x03,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x04,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x05,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x06,
          0x80
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x07,
          0x05
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x08,
          0x02
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x09,
          0x80
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x0a,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x0b,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x0c,
          0x0a
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x0d,
          0x0a
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x0e,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x0f,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x10,
          0xe0
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x11,
          0xe4
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x12,
          0x04
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x13,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x14,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x15,
          0xc0
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x16,
          0x08
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x17,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x18,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x19,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x1a,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x1b,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x1c,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x1d,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,
      {
          0x20,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // GIP Control 2
    {2,
      {
          0x21,
          0x23
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x22,
          0x45
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x23,
          0x67
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x24,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x25,
          0x23
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x26,
          0x45
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x27,
          0x67
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,
      {
          0x30,
          0x01
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // GIP Control 3
    {2,
      {
          0x31,
          0x11
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x32,
          0x00
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x33,
          0xee
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x34,
          0xff
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x35,
          0xcb
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x36,
          0xda
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x37,
          0xad
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x38,
          0xbc
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x39,
          0x76
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x3a,
          0x67
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x3b,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x3c,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x3d,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x3e,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x3f,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x40,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,
      {
          0x53,
          0x10
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // GOUT VGLO Control
    {2,
      {
          0x54,
          0x10
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // GOUT VGHO Control

    {6,
      {
          0xFF,
          0xFF,
          0x98,
          0x06,
          0x04,
          0x07
      },
      MIPI_DSI_CMD_ID_DCS_LONG_WRITE,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page 7 CMD for ???
    {2,
      {
          0x18,
          0x1d
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x26,
          0xb2
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x02,
          0x77
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0xe1,
          0x79
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,
      {
          0x17,
          0x22
      },
      MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM,
      MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {0x00,
      {
          0
      },
      MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE,
      (
          mipi_dsi_cmd_flag_t)
      0},                              // End of table
};

#endif /* DISPLAY_MIPI_ILI9806E_DISPLAY_MIPI_ILI9806E_COMMANDS_H_ */
