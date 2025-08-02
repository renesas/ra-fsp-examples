/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : display_thread_entry.c
 * Version      : 1.0
 * Description  : I/F for FreeRTOS thread display_thread.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "jlink_console.h"
#include "menu_led.h"
#include "display_thread.h"
#include "board_sdram.h"
#include "common_init.h"
#include "common_utils.h"
#include "touch_GT911.h"

#include "hal_data.h"
#include "tp_thread_entry.h"
#include "display_thread_entry.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define PLCD_BLEN      (BSP_IO_PORT_04_PIN_04)
#define OVERLAY_HIGHLIGHT_COLOR           (0xFFFF) // d2_mode_argb1555 WHITE

#define DISPLAY_SCREEN_BYTES_PER_PIXEL    (2)
#define DISPLAY_SCREEN_BUFF_NUMBER        (2)

#define DISPLAY_SCREEN_BUFF_D2_COLOR_CODE (d2_mode_rgb565)

/* Variables to store resolution information */
static uint16_t g_hz_size;
static uint16_t g_vr_size;

/* Variables used for buffer usage */
static uint32_t g_buffer_size;
static uint8_t * g_p_single_buffer, * g_p_double_buffer;

void call_user_draw(void);

#ifndef R_RELEASE_MODE
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};
#endif

st_drv_background_t g_current_backgroud = {LCD_FULL_BG_DEFAULT, true, true, true, NULL, NULL};
#define X1_CO_ORDINATE      (0U)
#define Y1_CO_ORDINATE      (0U)
#define COLOR_BAND_COUNT    (8U)
#define INC_DEC_VALUE       (1)

#define PLCD_BLEN      (BSP_IO_PORT_04_PIN_04)

void draw_background(void);
void draw_overlay(void);

void dsp_set_background(uint32_t choice);
void lcd_wait_vsync(uint32_t wait_count);
void set_gs_page(void);
uint32_t graphics_backup_buffer_pointer_get(void);
void graphics_start_frame(void);
void graphics_end_frame(void);
void graphics_swap_buffer(void);
void graphics_reset_blit_buffer(void);

static fsp_err_t drw_init(void);

static void draw_menu_icon(void);
static void draw_exit_icon(void);

/*
* Private global variables
*/
static volatile uint32_t vsync_count = 0;
static uint32_t req_Layer1_Swap = 0;
static uint32_t req_Layer2_Swap = 0;
static uint8_t  s_drw_buf = 1;
static d2_renderbuffer * renderbuffer;

static d2_s16 origin_x = 15;
static d2_s16 box_x    = 50;
static d2_s16 box_y    = 40;

st_rgb565_text_block_page_t gs_page =
{
    14, // Number of lines required (including blanks)
    " GETTING STARTED", /* Title */
    {
        {{PAGE_LEFT_EDGE, LINE(0) + BLOCK(0)},""},
        {{PAGE_LEFT_EDGE, LINE(1) + BLOCK(0)},"INSTRUCTIONS"},
        {{PAGE_LEFT_EDGE, LINE(2) + BLOCK(0)},"1) Tap MENU on the top left to view the application menu & tap"},
        {{PAGE_LEFT_EDGE, LINE(3) + BLOCK(0)},"icons to launch applications."},
        {{PAGE_LEFT_EDGE, LINE(4) + BLOCK(0)},"2) Tap X on the top right to return to the home screen or close"},
        {{PAGE_LEFT_EDGE, LINE(5) + BLOCK(0)},"the application menu."},

        {{PAGE_LEFT_EDGE, LINE(6) + BLOCK(1)},"NOTES"},
        {{PAGE_LEFT_EDGE, LINE(7) + BLOCK(1)},"Connect Serial Terminal Emulator to DEBUG1 (J10), Settings:"},
        {{PAGE_LEFT_EDGE, LINE(8) + BLOCK(1)},"Speed 115200, Data 8-bit, Parity None, Stop 1-bit, Flow Control None"},

        {{PAGE_LEFT_EDGE, LINE(9)  + BLOCK(2)},"DISCLAIMER"},
        {{PAGE_LEFT_EDGE, LINE(10) + BLOCK(2)},"The Quick Start Example Project is provided for demonstration only."},
        {{PAGE_LEFT_EDGE, LINE(11) + BLOCK(2)},"Refer to renesas.com and the EK-RA8E2 Quick Start Guide for"},
        {{PAGE_LEFT_EDGE, LINE(12) + BLOCK(2)},"the full disclaimer text."},
    }
};

uint8_t sdram_buffer[DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram") = "";
st_full_image_rgb565_t user_clean_background __attribute__ ((section (".sdram"))) = {FULL_IMAGE_WIDTH, FULL_IMAGE_HEIGHT, 2, {""}};

/**********************************************************************************************************************
 * Function Name: graphics_reset_blit_buffer
 * Description  : Reset the d2_setblitsrc pointer to the active background image.
 *                the required src can be determined by checking the active_demo member of g_current_backgroud.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
void graphics_reset_blit_buffer(void)
{
    if(g_current_backgroud.active_demo == LCD_FULL_BG_USER_LED)
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_sdram_buffer_led[0], LCD_HPIX, LCD_STRIDE, LCD_VPIX, EP_SCREEN_MODE_FG);
    }
    else if(g_current_backgroud.active_demo == LCD_FULL_BG_USER_LCD)
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_sdram_buffer_lcd[0], LCD_HPIX, LCD_STRIDE, LCD_VPIX, EP_SCREEN_MODE_FG);
    }
//    else if ((g_current_backgroud.active_demo == 0) || (g_current_backgroud.active_demo == LCD_FULL_BG_WELCOME))
    else if (g_current_backgroud.active_demo == LCD_FULL_BG_WELCOME)
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_sdram_buffer_welcome[0], LCD_HPIX, LCD_STRIDE, LCD_VPIX, EP_SCREEN_MODE_FG);
    }
    else
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&sdram_buffer[0], LCD_HPIX, LCD_STRIDE, LCD_VPIX, EP_SCREEN_MODE_FG);
    }
}
/**********************************************************************************************************************
 End of function graphics_reset_blit_buffer
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: display_thread_entry
 * Description  : display_thread_entry task main thread
 *                Upper layer interface for processing the connected MIPI display
 *                Calls to MIPI layer originate here).
 *                Note: pvParameters contains TaskHandle_t
 * Return Value : NONE
 *********************************************************************************************************************/
void display_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t result = FSP_ERR_UNSUPPORTED;
    fsp_err_t err    = FSP_ERR_UNSUPPORTED;

    configure_led(LED_CONFIGURED_NORMAL);

    bsp_sdram_init();

    while (g_initialise_continue == false)
    {
        vTaskDelay(10);
    }

    while (!display_ts_detected())
    {
        vTaskDelay(10);
    }

    /* Initialise buffers */
    memset(fb_background[0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);
    memset(fb_background[1], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);

    set_gs_page();

    memset(&sdram_buffer[0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);
    memset(&user_clean_background.pixel_data[0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);

    /* Hold display thread till touch screen controller has responded, ie panel is connected */
    while (!jlink_configured())
    {
        vTaskDelay(10);
    }

    while (!ts_configured())
    {
        vTaskDelay(10);
    }

    /* Get LCDC configuration */
    g_hz_size = (g_display_cfg.input[0].hsize);
    g_vr_size = (g_display_cfg.input[0].vsize);

    /* Initialize buffer pointers */
    g_buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL);
    g_p_single_buffer = (uint8_t *) g_display_cfg.input[0].p_base;

    /* Double buffer for drawing color bands with good quality */
    g_p_double_buffer = g_p_single_buffer + g_buffer_size;

    drw_init();

    /* Initialize GLCDC driver */
    result = R_GLCDC_Open(&g_display_ctrl, &g_display_cfg);

    /* Handle error */
    if (FSP_SUCCESS != result)
    {
        /* GLCDC initialization failed  */
#ifndef R_RELEASE_MODE
        print_to_console("\r\n ** GLCDC driver initialization FAILED ** \r\n");
#endif
    }
    else
    {
        /* GLCDC initialization OK  */
#ifndef R_RELEASE_MODE
        print_to_console("\r\n ** GLCDC driver initialization PASSED ** \r\n");
#endif
    }

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
#ifndef R_RELEASE_MODE
        print_to_console("\r\n ** GLCDC driver start FAILED ** \r\n");
#endif
    }
    else
    {
#ifndef R_RELEASE_MODE
        print_to_console("\r\n ** GLCDC driver started ** \r\n");
#endif
    }

#ifndef R_RELEASE_MODE
    print_to_console((void*)"display_thread_entry: initialised\r\n");
#endif

    /* Ensure GS page is the 1st thing user sees on LCD */
    set_gs_page();
    uint8_t settle_frames = 3;

    renderbuffer = d2_newrenderbuffer(gp_d2_handle, 20, 20);
    d2_selectrenderbuffer(gp_d2_handle, renderbuffer);


    /* Must call graphics_reset_blit_buffer so that background is redrawn */
    graphics_reset_blit_buffer();

    while (1)
    {
        graphics_start_frame();

        draw_background();

        /* dedicated draw routing for hybrid screens (led\lcd demo etc)*/
        call_user_draw();

        /* Constant icons (remember exit is NOT shown when welcome screen is displayed */
        if (g_current_backgroud.menu_icon_active)
        {
            draw_menu_icon();
        }

        if (g_current_backgroud.exit_icon_active)
        {
            draw_exit_icon();
        }

        draw_overlay();

        /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
        graphics_end_frame();

        switch (settle_frames)
        {
            case 0:
                /* Display is on, nothing to do */
                break;
            case 1:
                /* Turn display On */
                settle_frames--;
                R_BSP_PinAccessEnable();
                R_IOPORT_PinWrite(&g_ioport_ctrl, PLCD_BLEN, (bsp_io_level_t)BSP_IO_LEVEL_HIGH);
                vTaskDelay(40);

#ifndef R_RELEASE_MODE
                print_to_console("\r\nBacklight should be on\r\n");
#endif

                R_BSP_PinAccessDisable();
                break;

            default:
                settle_frames--;
        }
        vTaskDelay (10);
    }
}
/**********************************************************************************************************************
 End of function display_thread_entry
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: call_user_draw
 * Description  : Draw contents of glcd graphics layers.
 *                Note - required src can be determined by checking the active_demo member of g_current_backgroud.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
void call_user_draw(void)
{
    switch (g_current_backgroud.active_demo)
    {
        case 0: // Getting Started
        case LCD_FULL_BG_GETTING_STARTED: // Getting Started
        case LCD_FULL_BG_KIT_INFORMATION: // KIS
        case LCD_FULL_BG_EXTERNAL_MEMORY: // EXT Memory
        case LCD_FULL_BG_NEXT_STEPS:      // Next Steps
            if(g_current_backgroud.p_text_block != NULL)
            {
                set_display_image_text(g_current_backgroud.p_text_block);
            }
            break;

        case LCD_FULL_BG_USER_LED: // led demo
            menu_led_update_screen(gp_d2_handle);
            break;

        case LCD_FULL_BG_USER_LCD: // lcd demo
            menu_lcd_update_screen(gp_d2_handle);
            break;

        default:
            /* don't draw anything */
    }
}
/**********************************************************************************************************************
 End of function call_user_draw
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: draw_overlay
 * Description  : draw menu overlay onto Layer 2 of glcd
 *                image will only draw when g_overlay_selected is set true.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
void draw_overlay(void)
{
    if (true == g_overlay_selected)
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_menu_overlay_image.pixel_data[0], g_menu_overlay_image.width, g_menu_overlay_image.width, g_menu_overlay_image.height, d2_mode_rgb565);

        // Param gp_d2_handle, Source width/height    Source position, Destination scale , Destination position on lcd
        d2_blitcopy(gp_d2_handle,
                (d2_s32) g_menu_overlay_image.width, (d2_s32) g_menu_overlay_image.height,
                (d2_blitpos) 0, (d2_blitpos) 0,
                (d2_width) ((uint32_t)(g_menu_overlay_image.width) << 4), (d2_width) ((uint32_t)(g_menu_overlay_image.height) << 4),
                (d2_point) (90 << 4), (d2_point) (74 << 4),
                d2_tm_filter);

        /* Must call graphics_reset_blit_buffer so that background is redrawn */
        graphics_reset_blit_buffer();
    }
}
/**********************************************************************************************************************
 End of function draw_overlay
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: draw_background
 * Description  : draw image contents onto Layer 2 of glcd
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
 
void draw_background(void)
{
    /* Only update background reference when absolutely necessary */
    if((g_current_backgroud.update == true))
    {
        memset(fb_background[0], 0x00, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);

        g_current_backgroud.update = false;
        g_current_backgroud.active_demo = g_selected_menu;

        /* Must call graphics_reset_blit_buffer so that background is redrawn */
        graphics_reset_blit_buffer();
    }

    /* display as VGA 640x480 on mipi lcd FIXED SCALE AND POSITION */
    // Param gp_d2_handle, Source width/height    Source position, Destination scale , Destination position on lcd
    d2_blitcopy(gp_d2_handle,
            (d2_s32) FULL_IMAGE_WIDTH, (d2_s32) FULL_IMAGE_HEIGHT,
            (d2_blitpos) 0, (d2_blitpos) 0,
            (d2_width) ((uint32_t)(FULL_IMAGE_WIDTH) << 4), (d2_width) ((uint32_t)(FULL_IMAGE_HEIGHT) << 4),
            (d2_point) (0 << 4), (d2_point) (0 << 4),
            d2_tm_filter);
};
/**********************************************************************************************************************
 End of function draw_background
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: draw_menu_icon
 * Description  : draw menu icon using Dav/2D rendering functions
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static void draw_menu_icon(void)
{
    d2_s16 origin_y = 20;
    d2_s16 offset = 0;
    d2_color col;

    col = 0xFFFFFF;
    d2_setcolor(gp_d2_handle, 0, col);
    d2_renderline(gp_d2_handle, (d2_point) ((origin_x) << 4), (d2_point) ((origin_y + offset) << 4), (d2_point) ((origin_x + 40) << 4), (d2_point) ((origin_y + offset) << 4), (d2_point) (2 << 4), 0);

    offset += 10;
    d2_setcolor(gp_d2_handle, 0, col);
    d2_renderline(gp_d2_handle, (d2_point) ((origin_x) << 4), (d2_point) ((origin_y + offset) << 4), (d2_point) ((origin_x + 40) << 4), (d2_point) ((origin_y + offset) << 4), (d2_point) (2 << 4), 0);

    offset += 10;
    d2_setcolor(gp_d2_handle, 0, col);
    d2_renderline(gp_d2_handle, (d2_point) ((origin_x) << 4), (d2_point) ((origin_y + offset) << 4), (d2_point) ((origin_x + 40) << 4), (d2_point) ((origin_y + offset) << 4), (d2_point) (2 << 4), 0);

    d2_s16 border_x = 10;
    d2_s16 border_y = 10;

    d2_setcolor(gp_d2_handle, 0, col);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x) << 4),         (d2_point) ((border_y) << 4), (d2_point) ((border_x + box_x) << 4),      (d2_point) ((border_y) << 4),      (d2_point) (2 << 4), 0);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x) << 4),         (d2_point) ((border_y + box_y) << 4), (d2_point) ((border_x + box_x) << 4), (d2_point) ((border_y + box_y) << 4), (d2_point) (2 << 4), 0);

    d2_renderline(gp_d2_handle, (d2_point) ((border_x + box_x) << 4), (d2_point) ((border_y) << 4), (d2_point) ((border_x + box_x) << 4),      (d2_point) ((border_y + box_y) << 4), (d2_point) (2 << 4), 0);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x) << 4),         (d2_point) ((border_y) << 4), (d2_point) ((border_x) << 4),              (d2_point) ((border_y + box_y) << 4), (d2_point) (2 << 4), 0);
}
/**********************************************************************************************************************
 End of function draw_menu_icon
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: draw_exit_icon
 * Description  : draw exit icon using Dav/2D rendering functions
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static void draw_exit_icon(void)
{
    d2_color col;

    d2_s16 border_x = 740;
    d2_s16 border_y = 10;

    col = 0xFFFFFF;
    d2_setcolor(gp_d2_handle, 0, col);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x) << 4),         (d2_point) ((border_y) << 4), (d2_point) ((border_x + box_x) << 4),         (d2_point) ((border_y) << 4),         (d2_point) (2 << 4), 0);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x) << 4),         (d2_point) ((border_y + box_y) << 4), (d2_point) ((border_x + box_x) << 4), (d2_point) ((border_y + box_y) << 4), (d2_point) (2 << 4), 0);

    d2_renderline(gp_d2_handle, (d2_point) ((border_x + box_x) << 4), (d2_point) ((border_y) << 4), (d2_point) ((border_x + box_x) << 4),         (d2_point) ((border_y + box_y) << 4), (d2_point) (2 << 4), 0);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x) << 4),         (d2_point) ((border_y) << 4), (d2_point) ((border_x) << 4),                 (d2_point) ((border_y + box_y) << 4), (d2_point) (2 << 4), 0);

    d2_renderline(gp_d2_handle, (d2_point) ((border_x + 4) << 4),         (d2_point) ((border_y + 4) << 4),         (d2_point) ((border_x + box_x - 4) << 4), (d2_point) ((border_y + box_y - 4) << 4), (d2_point) (2 << 4), 0);
    d2_renderline(gp_d2_handle, (d2_point) ((border_x + 4) << 4),         (d2_point) ((border_y + box_y - 4) << 4), (d2_point) ((border_x + box_x - 4) << 4), (d2_point) ((border_y + 4) << 4),         (d2_point) (2 << 4), 0);

}
/**********************************************************************************************************************
 End of function draw_exit_icon
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: drw_init
 * Description  : perform startup sequence for glcd, and set system wide screen parameters
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static fsp_err_t drw_init(void)
{
    /* Initialize D/AVE 2D driver */
     gp_d2_handle = d2_opendevice(0);
     d2_inithw(gp_d2_handle, 0);

     /* Clear both buffers */
     d2_framebuffer(gp_d2_handle, fb_foreground, FULL_IMAGE_WIDTH, FULL_IMAGE_WIDTH, FULL_IMAGE_HEIGHT * DISPLAY_SCREEN_BUFF_NUMBER, DISPLAY_SCREEN_BUFF_D2_COLOR_CODE);
     d2_clear(gp_d2_handle, 0x00000000);

     /* Set various D2 parameters */
     d2_setblendmode(gp_d2_handle, d2_bm_alpha, d2_bm_zero);
     d2_setalphamode(gp_d2_handle, d2_am_constant);
     d2_setalpha(gp_d2_handle, 0xff);
     d2_setantialiasing(gp_d2_handle, 1);
     d2_setlinecap(gp_d2_handle, d2_lc_butt);
     d2_setlinejoin(gp_d2_handle, d2_lj_max);

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 End of function drw_init
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: lcd_wait_vsync
 * Description  : Wait in this function till required number of vsync events.
 * Argument     : uint32_t wait_count - number of events to wait
 * Return Value : None
 *********************************************************************************************************************/
 void lcd_wait_vsync(uint32_t wait_count)
{
    vsync_count = wait_count;
    while (vsync_count > 0)
    {
#if 2 == BSP_CFG_RTOS                  /* FreeRTOS */
        vTaskDelay(1);
#endif
    }
}
/**********************************************************************************************************************
 End of function lcd_wait_vsync
 *********************************************************************************************************************/
 
 /**********************************************************************************************************************
 * Function Name: set_gs_page
 * Description  : Set active lcd screen to represent getting started page.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
 void set_gs_page(void)
 {
     g_show_getting_started_page = true;
     dsp_set_background(LCD_FULL_BG_GETTING_STARTED);
 }
/**********************************************************************************************************************
 End of function set_gs_page
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dsp_set_background
 * Description  : Populate g_current_backgroud structure for the current lcd screen.
 * Argument     : choice        - uint8 to represent which screen is required, the available options are defined in
 *                common_init.h - see comment available option for LCD screens in the demo
 * Return Value : NONE
 *********************************************************************************************************************/
void dsp_set_background(uint32_t choice)
{
    g_current_backgroud.p_text_block = NULL;

    switch (choice)
    {
        case  LCD_FULL_BG_USER_LED:
            g_current_backgroud.p_bg_src = (uint8_t *)&g_ref_001_user_led_background.pixel_data[0];
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = true;
            break;

        case  LCD_FULL_BG_USER_LCD:
            g_current_backgroud.p_bg_src = (uint8_t *)&g_ref_002_user_lcd_background.pixel_data[0];
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = true;
            break;

        case  LCD_FULL_BG_KIT_INFORMATION:
            g_current_backgroud.p_bg_src = (uint8_t *)&user_clean_background.pixel_data[0];
            g_current_backgroud.p_text_block = &g_kis_page;
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = true;
            break;

        case  LCD_FULL_BG_EXTERNAL_MEMORY:
            g_current_backgroud.p_bg_src = (uint8_t *)&user_clean_background.pixel_data[0];
            g_current_backgroud.p_text_block = &g_ext_page;
            g_current_backgroud.menu_icon_active = false;
            g_current_backgroud.exit_icon_active = false;
            break;

        case  LCD_FULL_BG_NEXT_STEPS:
            g_current_backgroud.p_bg_src = (uint8_t *)&user_clean_background.pixel_data[0];
            g_current_backgroud.p_text_block = &g_ns_page;
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = true;
            break;

        case  LCD_FULL_BG_GETTING_STARTED:
            g_current_backgroud.p_bg_src = (uint8_t *)&user_clean_background.pixel_data[0];
            g_current_backgroud.p_text_block = &gs_page;
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = true;

            // Special code to re-show getting started
            g_selected_menu = 0;
            g_current_backgroud.active_demo = 0;
            break;

        case  LCD_FULL_BG_WELCOME:
            g_current_backgroud.p_bg_src = (uint8_t *)&g_ref_000_welcome_screen.pixel_data[0];
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = false;
            g_selected_menu = LCD_FULL_BG_WELCOME;
            break;

        default:
    }

    g_current_backgroud.update = true;
}
/**********************************************************************************************************************
 End of function dsp_set_background
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_display_image_text
 * Description  : function to draw text on the inactive buffer.
 *                The st_rgb565_text_block_page_t supplies inouts to the tile block and the body block.
 *                Title block use font defined in linotype_240_title.h
 *                Body block use font defined in linotype_240_body.h
 *                Note this function is a companion to dsp_set_background.
 *                Both manage swapping the active and inactive buffers, call only 1 each display loop.
 * Argument     : st_rgb565_text_block_page_t - structure describing the testual layout of the screen
 * Return Value : NONE
 *********************************************************************************************************************/
void set_display_image_text(st_rgb565_text_block_page_t *text)
{
    lv_point_t cursor_pos   = {TITLE_LEFT_EDGE,18};

    /* Set title */
    user_font_title_draw_line(/* gp_d2_handle, */ &cursor_pos, text->title);

    cursor_pos.x  = TITLE_LEFT_EDGE;
    cursor_pos.y += TITLE_LINE_SPACING;

    DRAW_TITLE_FONT_UNDERLINE;

    /*  Each line */
    for (uint32_t line = 0 ; line < text->text_lines; line++)
    {
        /* Write line in appropriate font */
        cursor_pos.x = text->text_block[line].offset.x;
        cursor_pos.y = text->text_block[line].offset.y;

        user_font_body_draw_line(/* gp_d2_handle, */ &cursor_pos, text->text_block[line].line);
    }

    /* Must call graphics_reset_blit_buffer so that background is redrawn */
    graphics_reset_blit_buffer();
}
/**********************************************************************************************************************
 End of function set_display_image_text
 *********************************************************************************************************************/


/*********************************************************************************************************************
 *  System uses double buffer. Swap the current buffer.
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_swap_buffer(void)
{
    s_drw_buf = (s_drw_buf == 0) ? 1 : 0;

    /* Update the layer to display in the GLCDC (will be set on next Vsync) */
    R_GLCDC_BufferChange(g_display.p_ctrl, fb_foreground[s_drw_buf], DISPLAY_FRAME_LAYER_2);
}
/**********************************************************************************************************************
 End of function graphics_swap_buffer
 *********************************************************************************************************************/
/*********************************************************************************************************************
 *  Get pointer address of backup buffer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
uint32_t graphics_backup_buffer_pointer_get(void)
{
    uint8_t backup_buffer = (s_drw_buf == 0) ? 1 : 0;


#ifdef USE_FG
    return (uint32_t)&fb_foreground[backup_buffer][0];
#else
    return (uint32_t)&fb_background[backup_buffer][0];
#endif


}
/**********************************************************************************************************************
 End of function graphics_backup_buffer_pointer_get
 *********************************************************************************************************************/
 
/*********************************************************************************************************************
 *  Start a new frame with the current draw buffer.
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_start_frame(void)
{
    /* Start a new display list */
    d2_startframe(gp_d2_handle);

    /* Set the new buffer to the current draw buffer */
    d2_framebuffer(gp_d2_handle, &(fb_foreground[s_drw_buf][0]), FULL_IMAGE_WIDTH, FULL_IMAGE_WIDTH, FULL_IMAGE_HEIGHT, DISPLAY_SCREEN_BUFF_D2_COLOR_CODE);
}
/**********************************************************************************************************************
 End of function graphics_start_frame
 *********************************************************************************************************************/

/*********************************************************************************************************************
 *  Wait the current frame to end and swap the framebuffer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_end_frame(void)
{
    /* End the current display list */
    d2_endframe(gp_d2_handle);

    /* Flip the frame buffer */
    graphics_swap_buffer();
}
/**********************************************************************************************************************
 End of function graphics_end_frame
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: vsync_callback
 * Description  : Interrupt Callback function of LCDC.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void vsync_callback (display_callback_args_t * p_args)
{
    if( p_args->event == DISPLAY_EVENT_LINE_DETECTION )
    {
        if(req_Layer1_Swap == 1)
        {
            req_Layer1_Swap = 0;
        }

        if(req_Layer2_Swap == 1)
        {
            req_Layer2_Swap = 0;
        }

        if (vsync_count > 0)
        {
            vsync_count--;
        }
    }
}
/**********************************************************************************************************************
 End of function vsync_callback
 *********************************************************************************************************************/
