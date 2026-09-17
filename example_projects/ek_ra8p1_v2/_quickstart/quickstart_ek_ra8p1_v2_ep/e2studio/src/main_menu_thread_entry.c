/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
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

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

#ifdef USE_TO_MANIPLATE_OPTIONATION_AT_FUNCTION_LEVEL

/* Put function between push n pop */
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma GCC pop_options

#endif

uint8_t g_sdram_buffer_welcome[DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1 * 2 + 1] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");
uint8_t g_sdram_buffer_led[DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1 * 2 + 1] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");
st_overlay_image_rgb565_t g_menu_overlay_image BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");


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

    /* Initialise user level serial console support using SEGGER serial driver DEBUG1 */
    jlink_console_init ();

    sprintf (s_print_buffer, "%s%s%s", gp_clear_screen, gp_cursor_home,gp_hide_cursor);
    print_to_console(s_print_buffer);

    /* Intentional throttling here to stop welcome message from being ignored */
    vTaskDelay(1000);

#ifdef SHOW_FSP_VERSION
    sprintf (s_print_buffer, "\x1b[2m\x1b[37%s!\r\n", FSP_VERSION_BUILD_STRING);
    print_to_console(s_print_buffer);
    vTaskDelay(100);
#endif // SHOW_FSP_VERSION

    sprintf (s_print_buffer, "\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n", g_product_full_name);
    print_to_console(s_print_buffer);
    vTaskDelay(100);

    sprintf (s_print_buffer, "Configuring connected devices, please wait\r\n");
    print_to_console(s_print_buffer);

    common_init();

    /* The graphics images in the QSPI flash were written in SPI mode.
     * Due to byte pairs being swapped when reading from the flash in high-speed QUAD mode,
     * bytes within a byte-pair must be swapped after reading the image into SDRAM in order
     * for the graphics to appear normal on the display.
     */
    memcpy(&g_sdram_buffer_welcome[0], &g_ref_000_welcome_screen.pixel_data[0], sizeof(g_sdram_buffer_welcome));
    memcpy(&g_sdram_buffer_led[0], &g_ref_001_user_led_background.pixel_data[0], sizeof(g_sdram_buffer_led));

    for (uint32_t x = 0; x < remaining; x+=2)
    {
        b0 = g_sdram_buffer_welcome[x + 0];
        b1 = g_sdram_buffer_welcome[x + 1];

        g_sdram_buffer_welcome[x + 0] = b1;
        g_sdram_buffer_welcome[x + 1] = b0;

        b0 = g_sdram_buffer_led[x + 0];
        b1 = g_sdram_buffer_led[x + 1];

        g_sdram_buffer_led[x + 0] = b1;
        g_sdram_buffer_led[x + 1] = b0;

    }

    /* Load menu overlay */
    memcpy(&g_menu_overlay_image.pixel_data, &g_ref_menu_overlay_image.pixel_data, (768 * 278 * 2 + 1));

    g_menu_overlay_image.width = 768;
    g_menu_overlay_image.height = 278;
    g_menu_overlay_image.bytes_per_pixel = 2;

    remaining = g_menu_overlay_image.width * g_menu_overlay_image.height * 2;

    for (uint32_t x = 0; x < remaining; x+=2)
    {
        b0 = g_menu_overlay_image.pixel_data[x + 0];
        b1 = g_menu_overlay_image.pixel_data[x + 1];

        g_menu_overlay_image.pixel_data[x + 0] = b1;
        g_menu_overlay_image.pixel_data[x + 1] = b0;
    }

    /* Each call to main_display_menu() implements the main menu
     * as the code is organised to return to the start after each sub-demo is run. */
    while (1)
    {
        main_display_menu();
        vTaskDelay(100);
    }
}
/**********************************************************************************************************************
 End of function main_menu_thread_entry
 *********************************************************************************************************************/

