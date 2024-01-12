/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

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
