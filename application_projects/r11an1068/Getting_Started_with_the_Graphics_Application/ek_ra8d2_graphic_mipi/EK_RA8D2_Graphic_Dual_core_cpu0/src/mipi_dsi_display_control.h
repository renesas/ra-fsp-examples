/***********************************************************************************************************************
 * File Name    : mipi_dsi_display_control.h
 * Description  : Contains data structures and functions used in mipi_dsi_display_control.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_data.h"

#ifndef MIPI_DSI_COMMAND_H_
#define MIPI_DSI_COMMAND_H_

#define MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG      (0xFE)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE    (0xFD)
#ifndef RESET_FLAG
#define RESET_FLAG      (0U)
#endif
#ifndef SET_FLAG
#define SET_FLAG        (1U)
#endif
typedef struct st_lcd_table_setting
{
    unsigned char        size;
    unsigned char        buffer[10];
    mipi_cmd_id_t        cmd_id;
    mipi_dsi_cmd_flag_t  flags;
} lcd_table_setting_t;

/* Function declare */
void e45ra_mw276_lcd_init(void);

#endif /* MIPI_DSI_COMMAND_H_ */
