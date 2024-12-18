/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : dsi_layer.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "r_ioport.h"
#include "r_mipi_dsi_api.h"

#include "hal_data.h"
#include "dsi_layer.h"

#define PIN_DISPLAY_RESET        BSP_IO_PORT_10_PIN_01
#define PIN_DISPLAY_BACKLIGHT    BSP_IO_PORT_04_PIN_04

/**********************************************************************************************************************
 * Function Name: dsi_layer_hw_reset
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void dsi_layer_hw_reset ()
{
<<<<<<< HEAD:application_projects/r11an0496/secure_key_inject_update_ra6m3/src/key_update_key.c
    /* uint32_t keytype; */
    0x00000000,
    /* uint32_t shared_key_number; */
    0x00000000,
    /* uint8_t wufpk[32]; */
    {
        0x71, 0xA5, 0xC6, 0xA5, 0x72, 0x0E, 0x0B, 0xC7, 0x31, 0x16, 0x4C, 0xB9, 0x6E, 0xD6, 0xA8, 0x8E,
        0x73, 0x64, 0xAC, 0x8C, 0x02, 0xBE, 0x58, 0xE1, 0x12, 0xA3, 0x27, 0x0F, 0xB9, 0x75, 0x4F, 0x57
    },
    /* uint8_t initial_vector[16]; */
    {
        0x4C, 0xFD, 0x27, 0x1A, 0x37, 0xE5, 0x8F, 0xA1, 0x02, 0x77, 0x91, 0x1F, 0x1F, 0xDF, 0xCD, 0x6F
    },
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void dsi_layer_enable_backlight ()
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_BACKLIGHT, BSP_IO_LEVEL_HIGH);
    R_BSP_PinAccessDisable();
}

/**********************************************************************************************************************
 * End of function dsi_layer_enable_backlight
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dsi_layer_disable_backlight
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void dsi_layer_disable_backlight (void)
{
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_BACKLIGHT, BSP_IO_LEVEL_LOW);
    R_BSP_PinAccessDisable();
}

/**********************************************************************************************************************
 * End of function dsi_layer_disable_backlight
 *********************************************************************************************************************/
