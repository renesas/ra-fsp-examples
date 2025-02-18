/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : emWin_thread_entry.c
 * Description  : Thread handles graphics framework setup and envokes the emWin MainTask thread.
 **********************************************************************************************************************/

#include "emWin_thread.h"
#include "hal_data.h"
#include "GUI.h"
#include "mipi_table.h"
#include "Application.h"

static volatile mipi_dsi_phy_status_t g_phy_status;
static volatile bool g_message_sent = 0;

/* Sequence of descriptors to initialize the EK-RA8D1's LCD panel MIPI controller */
static const lcd_table_setting_t g_lcd_init_focuslcd[] =
{

    {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x00}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page  CMD for Normal command
    {120, {0}, REGFLAG_DELAY, 0},                                                                                        // Sleep out command may not be issued within 120 ms of GPIO HW reset. Wait to ensure timing maintained.
    {2,  {0x11, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Sleep-Out
    {5,  {0}, REGFLAG_DELAY, 0},
    {2,  {0x29, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_0_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Display on
    {2,  {0x3a, 0x70},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // 24 bit

    {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page 1
    {2,  {0x08, 0x10},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Output SDA
    {2,  {0x21, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // DE = 1 Active

    {2,  {0x30, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Resolution setting 480 X 854
    {2,  {0x31, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Inversion setting

    {2,  {0x40, 0x14},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // BT 15
    {2,  {0x41, 0x33},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // avdd +5.2v,avee-5.2v
    {2,  {0x42, 0x02},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // VGL=DDVDH+VCIP -DDVDL,VGH=2DDVDL-VCIP
    {2,  {0x43, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // SET VGH clamp level
    {2,  {0x44, 0x06},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // SET VGL clamp level
    {2,  {0x50, 0x70},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // VREG1
    {2,  {0x51, 0x70},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // VREG2
    {2,  {0x52, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Flicker MSB
    {2,  {0x53, 0x48},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Flicker LSB
    {2,  {0x60, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust
    {2,  {0x61, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust
    {2,  {0x62, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust
    {2,  {0x63, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Timing Adjust

    {2,  {0xa0, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Positive Gamma
    {2,  {0xa1, 0x03},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa2, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa3, 0x0d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa4, 0x06},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa5, 0x16},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa6, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa7, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa8, 0x03},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xa9, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xaa, 0x06},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xab, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xac, 0x0d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xad, 0x2c},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xae, 0x26},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xaf, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,  {0xc0, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Negative Gamma
    {2,  {0xc1, 0x04},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc2, 0x0b},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc3, 0x0f},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc4, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc5, 0x18},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc6, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc7, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc8, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xc9, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xca, 0x07},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xcb, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xcc, 0x0c},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xcd, 0x2d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xce, 0x28},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xcf, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x06}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page 6 CMD for GIP timing
    {2,  {0x00, 0x21},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // GIP Control 1
    {2,  {0x01, 0x09},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x02, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x03, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x04, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x05, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x06, 0x80},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x07, 0x05},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x08, 0x02},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x09, 0x80},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x0a, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x0b, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x0c, 0x0a},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x0d, 0x0a},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x0e, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x0f, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x10, 0xe0},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x11, 0xe4},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x12, 0x04},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x13, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x14, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x15, 0xc0},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x16, 0x08},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x17, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x18, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x19, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x1a, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x1b, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x1c, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x1d, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,  {0x20, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // GIP Control 2
    {2,  {0x21, 0x23},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x22, 0x45},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x23, 0x67},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x24, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x25, 0x23},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x26, 0x45},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x27, 0x67},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,  {0x30, 0x01},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // GIP Control 3
    {2,  {0x31, 0x11},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x32, 0x00},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x33, 0xee},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x34, 0xff},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x35, 0xcb},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x36, 0xda},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x37, 0xad},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x38, 0xbc},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x39, 0x76},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x3a, 0x67},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x3b, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x3c, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x3d, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x3e, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x3f, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x40, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {2,  {0x53, 0x10},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // GOUT VGLO Control
    {2,  {0x54, 0x10},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    // GOUT VGHO Control

    {6,  {0xFF, 0xFF, 0x98, 0x06, 0x04, 0x07}, MIPI_DSI_CMD_ID_DCS_LONG_WRITE, MIPI_DSI_CMD_FLAG_LOW_POWER},    // Change to Page 7 CMD
    {2,  {0x18, 0x1d},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x26, 0xb2},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x02, 0x77},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0xe1, 0x79},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //
    {2,  {0x17, 0x22},                         MIPI_DSI_CMD_ID_DCS_SHORT_WRITE_1_PARAM, MIPI_DSI_CMD_FLAG_LOW_POWER},    //

    {0x00, {0},                                REGFLAG_END_OF_TABLE, 0},
};

/* Pre-set weather forecast */
static day_info_t forecast[7] =
{
    { "SUN", "Sunny",         "72", "81", "84/73"},
    { "MON", "Partly Cloudy", "50", "74", "77/67"},
    { "TUE", "Partly Cloudy", "52", "71", "74/65"},
    { "WED", "Rain",          "99", "65", "68/60"},
    { "THU", "Showers",       "85", "73", "76/70"},
    { "FRI", "Partly Cloudy", "50", "80", "83/72"},
    { "SAT", "Sunny",         "45", "89", "92/77"}
};

/* emWin Thread entry function */
/* pvParameters contains TaskHandle_t */
void emWin_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Initialize application data, once after reset */
    gDataApp.bgrd_darmode           = 0;
    gDataApp.pwm_duty_cycle         = 50;
    gDataApp.text_color             = 0;
    gDataApp.forecast_info          = &forecast[0];
    gDataApp.thermo_target_temp     = 70;
    gDataApp.current_forecast       = WEEK_DAY_SUN;
    gDataApp.sys_weather_type       = SYS_WEATHER_SUNNY;
    gDataApp.total_forecast         = 7;

    /* Block until capacitive touch panel reset completes */
    BaseType_t xResult = xSemaphoreTake(g_touch_reset_semaphore, portMAX_DELAY);
    if(xResult != pdTRUE)
    {
        __asm("BKPT #0\n");
    }

    /* Initialize SDRAM */
    bsp_sdram_init();

    /* Start AppWizard control */
    MainTask();

    while(true)
    {
        vTaskDelay (1);
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
    switch(p_args->event)
    {
        case MIPI_DSI_EVENT_POST_OPEN:
        {
            /* Initialize sequence for LCD. MIPI operation is in Command Mode */
            mipi_dsi_push_table(g_lcd_init_focuslcd);
        }
        break;
        case MIPI_DSI_EVENT_SEQUENCE_0:
        {
            if(MIPI_DSI_SEQUENCE_STATUS_DESCRIPTORS_FINISHED == p_args->tx_status)
            {
                /* Set MIPI command received semaphore */
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                BaseType_t xResult = xSemaphoreGiveFromISR(g_mipi_semaphore, &xHigherPriorityTaskWoken);
                if(xResult != pdTRUE)
                {
                    __asm("BKPT #0\n");
                }

                /* Return to the highest priority available task */
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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

    return;
}

/**********************************************************************************************************************
 *  @brief     Transmits a table of descriptors for the MIPI Sequence Operation.
 *
 * @param[in]  Pointer to descriptor structure array.
 * @retval     None.
 **********************************************************************************************************************/
void mipi_dsi_push_table (const lcd_table_setting_t *table)
{
    fsp_err_t fsp_err = FSP_SUCCESS;
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
            const TickType_t xDelay = table->size / portTICK_PERIOD_MS;
            vTaskDelay(xDelay);
        }
        else
        {
            /* Send a command to the peripheral device */
            fsp_err = R_MIPI_DSI_Command (&g_mipi_dsi0_ctrl, &msg);
            APP_ERR_TRAP(fsp_err);

            /* Wait until confirmed message sent */
            BaseType_t xResult = xSemaphoreTake(g_mipi_semaphore, portMAX_DELAY);
            if(xResult != pdTRUE)
            {
                __asm("BKPT #0\n");
            }

        }
        p_entry++;
    }
}
