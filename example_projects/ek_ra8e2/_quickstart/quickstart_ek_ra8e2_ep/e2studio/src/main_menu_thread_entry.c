/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : main_menu_thread_entry.c
 * Version      : 1.0
 * Description  : Contains user entry .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "main_menu_thread.h"
#include "jlink_console.h"
#include "common_init.h"
#include "common_data.h"
#include "menu_main.h"

#include "board_cfg_switch.h"
#include "ospi_b_ep.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
bool_t g_initialise_continue = false;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
//static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

#ifdef USE_TO_MANIPLATE_OPTIONATION_AT_FUNCTION_LEVEL

/* Put function between push n pop */
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma GCC pop_options

#endif

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
uint8_t g_sdram_buffer_welcome[DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0 * 2 + 1] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");
uint8_t g_sdram_buffer_led[DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0 * 2 + 1] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");
uint8_t g_sdram_buffer_lcd[DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0 * 2 + 1] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: reset_peripherals
 * Description  : Add any initial hardware configuration here.
 *                If none is required leave empty.
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
static void reset_peripherals()
{
    R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_07_PIN_08, BSP_IO_LEVEL_LOW);

    vTaskDelay (1000);

}
/**********************************************************************************************************************
 End of function reset_peripherals
 *********************************************************************************************************************/

#if 0
void vApplicationIdleHook (void)
{
    __NOP();
}
#endif

/**********************************************************************************************************************
 * Function Name: main_menu_thread_entry
 * Description  : Main Menu entry function to prepare and contain the main thread loop.
 * Argument     : pvParameters
 *                Note: pvParameters contains TaskHandle_t
 * Return Value : NONE
 *********************************************************************************************************************/
void main_menu_thread_entry(void *pvParameters)
{
    uint8_t b0 = 0;
    uint8_t b1 = 0;
    uint32_t remaining = DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1;
    FSP_PARAMETER_NOT_USED (pvParameters);
    g_initialise_continue = false;

    reset_peripherals();

    /* initialise common peripherals and board configuration */
    common_init();


    /* Before code runs and accessed graphics
     * archive ospi images into sdram */
    memcpy(&g_sdram_buffer_welcome[0], &g_ref_000_welcome_screen.pixel_data[0], sizeof(g_ref_000_welcome_screen.pixel_data));
    memcpy(&g_sdram_buffer_led[0], &g_ref_001_user_led_background.pixel_data[0], sizeof(g_ref_001_user_led_background.pixel_data));
    memcpy(&g_sdram_buffer_lcd[0], &g_ref_002_user_lcd_background.pixel_data[0], sizeof(g_ref_002_user_lcd_background.pixel_data));

    for (uint32_t x = 0; x < remaining; x+=2)
    {
        /* byte swap g_ref_000_welcome_screen */
        b0 = g_sdram_buffer_welcome[x + 0];
        b1 = g_sdram_buffer_welcome[x + 1];

        g_sdram_buffer_welcome[x + 0] = b1;
        g_sdram_buffer_welcome[x + 1] = b0;

        /* byte swap g_ref_002_user_led_background */
        b0 = g_sdram_buffer_led[x + 0];
        b1 = g_sdram_buffer_led[x + 1];

        g_sdram_buffer_led[x + 0] = b1;
        g_sdram_buffer_led[x + 1] = b0;

        /* byte swap g_ref_002_user_lcd_background */
        b0 = g_sdram_buffer_lcd[x + 0];
        b1 = g_sdram_buffer_lcd[x + 1];

        g_sdram_buffer_lcd[x + 0] = b1;
        g_sdram_buffer_lcd[x + 1] = b0;
    }

    /* Initialise user level serial console support using SEGGER serial driver DEBUG1 */
    jlink_console_init ();

    /* Only release the display and TP threads after switches and OSPI are set,
     * otherwise a conflict is possible on the i2c bus */
    g_initialise_continue = true;

    /* Inialise uer level serial console support using SEGGER serial driver DEBUG1 */
    jlink_console_init ();

    while (1)
    {
        main_display_menu();
        vTaskDelay(100);
    }
}
/**********************************************************************************************************************
 End of function main_menu_thread_entry
 *********************************************************************************************************************/
