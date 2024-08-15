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
    /* Reset Display - active low
     * NOTE: Sleep out may not be issued for 120 ms after HW reset */
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_RESET, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(9, BSP_DELAY_UNITS_MICROSECONDS); // Set active for 5-9 us
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_RESET, BSP_IO_LEVEL_HIGH);
}

/**********************************************************************************************************************
 * End of function dsi_layer_hw_reset
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dsi_layer_enable_backlight
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void dsi_layer_enable_backlight ()
{
    R_BSP_PinAccessEnable();
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
