/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : mipi_table.h
 * Description  : Contains definitions for MIPI descriptors.
 **********************************************************************************************************************/


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef MIPI_TABLE_H_
#define MIPI_TABLE_H_

#define REGFLAG_DELAY                   0xFE
#define REGFLAG_END_OF_TABLE            0xFD
#define MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG      ((mipi_dsi_cmd_id_t) 0xFE)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE    ((mipi_dsi_cmd_id_t) 0xFD)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    unsigned char        size;
    unsigned char        buffer[10];
    mipi_dsi_cmd_id_t    cmd_id;
    mipi_dsi_cmd_flag_t flags;
} lcd_table_setting_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
//extern const lcd_table_setting_t g_lcd_init_focuslcd[];

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void mipi_dsi_push_table (const lcd_table_setting_t *table);

#endif /* MIPI_TABLE_H_ */
