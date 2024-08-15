/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : dsi_layer.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#pragma once

#include "r_mipi_dsi.h"

#define REGFLAG_DELAY           0xFE
#define REGFLAG_END_OF_TABLE    0xFD

typedef struct
{
    unsigned char size;
    unsigned char buffer[10];
    uint8_t       msg_id;
    uint8_t       flags;
} LCD_setting_table;

void dsi_layer_hw_reset(void);
void dsi_layer_enable_backlight(void);
void dsi_layer_disable_backlight(void);
