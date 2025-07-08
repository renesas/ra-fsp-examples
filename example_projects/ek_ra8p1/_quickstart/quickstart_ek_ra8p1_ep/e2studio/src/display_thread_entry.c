/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : display_thread_entry.c
 * Version      : 1.0
 * Description  : FreeRTOS thread display_thread reuired by framework.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2018 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "hal_data.h"
#include "display_thread.h"
#include "common_init.h"
#include "common_utils.h"
#include "board_hw_cfg.h"
#include "r_display_api.h"
#include "display_thread_entry.h"

#include "menu_main.h"
#include "menu_kis.h"
#include "menu_led.h"
#include "menu_ext.h"
#include "menu_lcd.h"
#include "menu_ns.h"

#include "tp_thread_entry.h"

#include "images/user_font_title/user_font_title_if.h"
#include "images/user_font_body/user_font_body_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define R_CAMERA_LCD_TITLE_ENABLED_CFG (ON_CFG_)

#define PLCD_BLEN           (BSP_IO_PORT_05_PIN_14)
#define X1_CO_ORDINATE      (0U)
#define Y1_CO_ORDINATE      (0U)
#define COLOR_BAND_COUNT    (8U)
#define INC_DEC_VALUE       (1)

/* Allow defined number of loops of the main display before the back light is turned on */
#define R_DISPLAY_THREAD_INTIALISATION_SETTLE_FRAMES (5)

/* Variables to store resolution information */
uint16_t g_vr_size = FULL_IMAGE_HEIGHT;
uint16_t g_hz_size = FULL_IMAGE_WIDTH;

/* Variables used for buffer usage */
uint32_t g_buffer_size;
uint8_t * g_p_single_buffer, * g_p_double_buffer;

/* User defined functions */
uint8_t sdram_buffer[DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1 * 2] BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram") = "";
st_full_image_rgb565_t user_clean_background __attribute__ ((section (".sdram"))) = {1024, 600, 2, {""}};

void * graphics_get_back_buffer(void);

void graphics_reset_blit_buffer(void);
void glcdc_vsync_isr(display_callback_args_t *p_args);

/* API call used by rest of system */
bool_t set_display_image_full(uint32_t image);
void call_user_draw(void);

st_drv_background_t g_current_backgroud = {LCD_FULL_BG_DEFAULT, true, true, true, NULL, NULL};

/******************************************************************************
 * Static global variables
 *****************************************************************************/
static uint32_t g_vsync_flag = 0;
static uint8_t s_drw_buf = 0;
static uint8_t g_update_fps_text = 1;

static void draw_menu_icon(void);
static void draw_exit_icon(void);

static void draw_overlay(void);
static void set_background(void);
static void draw_background(void);
static void initialise_primary_display(void);
static void set_display_image_text(st_rgb565_text_block_page_t *text);

uint8_t  SDRAM_pixel_data[1024 * 600 * 4  + 1]  BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");
static d2_renderbuffer * renderbuffer;
static uint8_t * gp_camera_buffer;

st_rgb565_text_block_page_t gs_page = {
        15, // Number of lines required (including blanks)
        " GETTING STARTED          ", /* Title */
        {
            {{PAGE_LEFT_EDGE, LINE(0) + BLOCK(0)},""},
            {{PAGE_LEFT_EDGE, LINE(1) + BLOCK(0)},"INSTRUCTIONS"},
            {{PAGE_LEFT_EDGE, LINE(2) + BLOCK(0)},"1) Tap MENU on the top left to view the application menu & tap icons "},
            {{PAGE_LEFT_EDGE, LINE(3) + BLOCK(0)},"to launch applications."},
            {{PAGE_LEFT_EDGE, LINE(4) + BLOCK(0)},"2) Tap X on the top right to return to the home screen or close the application menu."},
            {{PAGE_LEFT_EDGE, LINE(5) + BLOCK(0)}," "},

            {{PAGE_LEFT_EDGE, LINE(6) + BLOCK(1)},"NOTES"},
            {{PAGE_LEFT_EDGE, LINE(7) + BLOCK(2)},"Connect Serial Terminal Emulator to DEBUG1 (J10)"},
            {{PAGE_LEFT_EDGE, LINE(8) + BLOCK(2)},"Connection settings: "},
            {{PAGE_LEFT_EDGE + 245, LINE(8) + BLOCK(2)},"Speed 115200, Data 8-bit, Parity None, Stop 1-bit, "},
            {{PAGE_LEFT_EDGE + 245, LINE(9) + BLOCK(2)},"Flow Control None"},

            {{PAGE_LEFT_EDGE, LINE(11)  + BLOCK(2)},"DISCLAIMER"},
            {{PAGE_LEFT_EDGE, LINE(12) + BLOCK(2)},"The Quick Start Example Project is provided for demonstration only."},
            {{PAGE_LEFT_EDGE, LINE(13) + BLOCK(2)},"Refer to renesas.com and the EK-RA8P1 Quick Start Guide for the full disclaimer text."},
        }
};

volatile d2_s16 origin_x = 15;
volatile d2_s16 box_x    = 50;
volatile d2_s16 box_y    = 40;

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
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_sdram_buffer_led[0], 1024, 1024, 600, d2_mode_rgb565);
    }
    else if ((g_current_backgroud.active_demo == 0) || (g_current_backgroud.active_demo == LCD_FULL_BG_WELCOME))
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_sdram_buffer_welcome[0], 1024, 1024, 600, d2_mode_rgb565);
    }
    else
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&sdram_buffer[0], 1024, 1024, 600, d2_mode_rgb565);
    }
}
/**********************************************************************************************************************
 End of function graphics_reset_blit_buffer
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: display_next_buffer_set
 * Description  : display_thread_entry task main thread
 * Return Value : NONE
 *********************************************************************************************************************/
void display_next_buffer_set(uint8_t* next_buffer)
{
    gp_camera_buffer = next_buffer;
}
/**********************************************************************************************************************
 End of function display_next_buffer_set
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
    fsp_err_t err;

    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, PLCD_BLEN, (bsp_io_level_t)BSP_IO_LEVEL_LOW);
    R_BSP_PinAccessDisable();

    while(false == ts_configured())
    {
        vTaskDelay(100);
    }


    memset(&fb_background[0][0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0 * 2);
    memset(&user_clean_background.pixel_data[0], 0, (size_t)(DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1 * 2));

    initialise_primary_display();

    gp_d2_handle = d2_opendevice(0);
    d2_inithw(gp_d2_handle, 0);

    renderbuffer = d2_newrenderbuffer(gp_d2_handle, 20, 20);

    d2_selectrenderbuffer(gp_d2_handle, renderbuffer);

    s_drw_buf = 0;

    // Set various D2 parameters
    d2_setblendmode(gp_d2_handle, d2_bm_alpha, d2_bm_zero);
    d2_setalphamode(gp_d2_handle, d2_am_constant);
    d2_setalpha(gp_d2_handle, 0xff);
    d2_setantialiasing(gp_d2_handle, 0);
    d2_setlinecap(gp_d2_handle, d2_lc_butt);
    d2_setlinejoin(gp_d2_handle, d2_lj_max);

    /* turn off camera stream */

    R_GLCDC_BufferChange(&g_plcd_display_ctrl, (uint8_t * const) gp_camera_buffer, DISPLAY_FRAME_LAYER_1);

    d2_color col = 0;
    uint16_t settle_frames = R_DISPLAY_THREAD_INTIALISATION_SETTLE_FRAMES;

    set_gs_page();
    set_background();

    while(1)
    {
        if(settle_frames-- == 0)
        {
            R_BSP_PinAccessEnable();
            R_IOPORT_PinWrite(&g_ioport_ctrl, PLCD_BLEN, (bsp_io_level_t)BSP_IO_LEVEL_HIGH);
            R_BSP_PinAccessDisable();
        }

        graphics_wait_vsync();

        d2_selectrenderbuffer(gp_d2_handle, renderbuffer);
        d2_framebuffer(gp_d2_handle, graphics_get_draw_buffer(), LCD_HPIX, LCD_HPIX, LCD_VPIX * 2, EP_SCREEN_MODE_TP);

        d2_cliprect(gp_d2_handle, 0, 0, LCD_HPIX-1, LCD_VPIX-1 );

        d2_setcolor(gp_d2_handle, 0, col);

        set_background();

        if((g_current_backgroud.update == false))
        {
            draw_background();
        /* dedicated draw routing for hybrid screens (led\lcd demo etc)*/
            call_user_draw();

        /* Constant icons (remember exit is NOT shown when welcome screen is displayed */
            if (g_current_backgroud.menu_icon_active)
            {
                draw_menu_icon();
            }

            if ((g_current_backgroud.exit_icon_active) && (!g_overlay_selected))
            {
                draw_exit_icon();
            }

            draw_overlay();

            d2_executerenderbuffer(gp_d2_handle, renderbuffer, 0);
            d2_flushframe(gp_d2_handle);

            s_drw_buf = s_drw_buf ? 0 : 1;

            if(LCD_FULL_BG_CAMERA_BACKGROUND == g_current_backgroud.active_demo)
            {
                do{
                /* Update the layer to display in the GLCDC (will be set on next Vsync) */
                    R_GLCDC_BufferChange(&g_plcd_display_ctrl, (uint8_t * const) gp_camera_buffer, DISPLAY_FRAME_LAYER_1);
                }while (err == FSP_ERR_INVALID_UPDATE_TIMING);
            }
            do{
            /* Update the layer to display in the GLCDC (will be set on next Vsync) */
                err = R_GLCDC_BufferChange(g_plcd_display.p_ctrl, graphics_get_back_buffer(), DISPLAY_FRAME_LAYER_2);
            }while (err == FSP_ERR_INVALID_UPDATE_TIMING);
        }
        vTaskDelay(10);
    }
}
/**********************************************************************************************************************
 End of function display_thread_entry
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: call_user_draw
 * Description  : Draw contents of glcd graphics layers.
 *                Layer 1 contains the camera image, which is allways running when the camera is connected.
 *                Layer 2 contains the foreground (text/graphics) and can be set to transparent to allow camera image
 *                to show.
 *                required src can be determined by checking the active_demo member of g_current_backgroud.
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
                d2_setcolor(gp_d2_handle, 0, 0x00000000);
                d2_renderbox( gp_d2_handle, (d2_point)0 << 4, (d2_point)0 << 4,(d2_point)(1024) << 4, (d2_point)(600) << 4);

                // confirm that selection didn't changed while erasing foreground */
                if(g_current_backgroud.p_text_block != NULL)
                {
                    set_display_image_text(g_current_backgroud.p_text_block);
                }
            }
            break;

        case 2: // led demo
            if (g_current_backgroud.p_bg_src == &g_ref_001_user_led_background.pixel_data[0])
            {
                /* Ensure led screen is set as background */
                graphics_reset_blit_buffer();
                menu_led_update_screen(gp_d2_handle);
            }
            break;

        case 4: // lcd demo
            d2_clear(gp_d2_handle, 0x00000000);


#if R_CAMERA_LCD_TITLE_ENABLED_CFG
            /* Enable title  on LCD for Camera demo */
            lv_point_t cursor_pos   = {TITLE_LEFT_EDGE,18};

            /* Set title */
            user_font_title_draw_line(/* gp_d2_handle, */ &cursor_pos, g_camera_welcome_message_str);

            cursor_pos.x  = TITLE_LEFT_EDGE;
            cursor_pos.y += TITLE_LINE_SPACING;

            DRAW_TITLE_FONT_UNDERLINE;
#endif
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
static void draw_overlay(void)
{
    if (true == g_overlay_selected)
    {
        d2_setblitsrc(gp_d2_handle, (uint8_t *)&g_menu_overlay_image.pixel_data[0], g_menu_overlay_image.width,
                      g_menu_overlay_image.width, g_menu_overlay_image.height, d2_mode_rgb565);

        // Param gp_d2_handle, Source width/height    Source position, Destination scale , Destination position on lcd
        d2_blitcopy(gp_d2_handle,
                (d2_s32) g_menu_overlay_image.width, (d2_s32) g_menu_overlay_image.height,
                (d2_blitpos) 0, (d2_blitpos) 0,
                (d2_width) ((uint32_t)(g_menu_overlay_image.width) << 4),
                (d2_width) ((uint32_t)(g_menu_overlay_image.height) << 4),
                (d2_point) (128 << 4), (d2_point) (74 << 4),
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
static void draw_background(void)
{
    graphics_reset_blit_buffer();

    /* display as VGA 640x480 on mipi lcd FIXED SCALE AND POSITION */
    // Param gp_d2_handle, Source width/height    Source position, Destination scale , Destination position on lcd
    d2_blitcopy(gp_d2_handle,
            (d2_s32) 1024, (d2_s32) 600,
            (d2_blitpos) 0, (d2_blitpos) 0,
            (d2_width) ((uint32_t)(1024) << 4), (d2_width) ((uint32_t)(600) << 4),
            (d2_point) (0 << 4), (d2_point) (0 << 4),
            d2_tm_filter);
};
/**********************************************************************************************************************
 End of function draw_background
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_background icons
 * Description  : set available icons and oter related elements for the current background.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static void set_background(void)
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

    if (g_current_backgroud.active_demo != LCD_FULL_BG_EXTERNAL_MEMORY)
    {
        g_current_backgroud.menu_icon_active = true;
    }
    else
    {
        g_current_backgroud.menu_icon_active = false;
    }
}
/**********************************************************************************************************************
 End of function set_background
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

    d2_s16 border_x = 970;
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
 * Function Name: initialise_primary_display
 * Description  : perform startup sequence for glcd, and set system wide screen parameters
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static void initialise_primary_display(void)
{
    /* Fill framebuffers with data for easier debugging */
    memset(fb_background[0], 0x00, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0 * 2);

    fsp_err_t result;

    /* Get LCDC configuration */
    g_hz_size = (g_plcd_display_cfg.input[0].hsize);
    g_vr_size = (g_plcd_display_cfg.input[0].vsize);

    /* Initialize buffer pointers */
    g_buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL);
    g_p_single_buffer = (uint8_t *) g_plcd_display_cfg.input[0].p_base;
    /* Double buffer for drawing color bands with good quality */
    g_p_double_buffer = g_p_single_buffer + g_buffer_size;

    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, PLCD_BLEN, (bsp_io_level_t)BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(40, BSP_DELAY_UNITS_MILLISECONDS);

    /* Initialize GLCDC driver */
    result = R_GLCDC_Open(&g_plcd_display_ctrl, &g_plcd_display_cfg);

    /* Handle error */
    if(FSP_SUCCESS != result)
    {
        /* GLCDC initialization failed  */
        APP_ERR_TRAP(result);
    }

    /* Start GLCDC display output */
    result = R_GLCDC_Start(&g_plcd_display_ctrl);

    /* Handle error */
    if(FSP_SUCCESS != result)
    {
        /* GLCDC initialization failed  */
        APP_ERR_TRAP(result);
    }
}
/**********************************************************************************************************************
 End of function initialise_primary_display
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
 * Function Name: set_display_image_text
 * Description  : Uses available fonts to display text on the LCD
 * Argument     : st_rgb565_text_block_page_t - Data structure represent text page on LCD
 * Return Value : None
 *********************************************************************************************************************/
static void set_display_image_text(st_rgb565_text_block_page_t *text)
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
 End of function set_gs_page
 *********************************************************************************************************************/

#define DIM_FRAME_COUNT_LIMIT (15)

/*******************************************************************************************************************//**
 * Simple function to wait for vertical sync semaphore
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: graphics_wait_vsync
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void graphics_wait_vsync(void)
{
    g_vsync_flag = 0;
    while (!g_vsync_flag);
    g_vsync_flag = 0;

}
/**********************************************************************************************************************
 End of function graphics_wait_vsync
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: graphics_get_draw_buffer
 * Description  : Get address of the buffer to use when setting the D2 framebuffer
 *                The returned address is technically a pointer to the currently displaying buffer.  By the time the
 *                DRW engine starts drawing to it it will no longer be the active frame.
 * Argument     : NONE
 * Return Value : Draw buffer pointer to use with d2_framebuffer
 *********************************************************************************************************************/
void * graphics_get_draw_buffer(void)
{
    return &(fb_foreground[s_drw_buf][0]);

}
/**********************************************************************************************************************
 End of function graphics_get_draw_buffer
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: graphics_get_back_buffer
* Description  : Get address of the buffer which is not active frame.
*                The returned address is technically a pointer to the currently displaying buffer.
* Argument     : NONE
* Return Value : Draw buffer pointer to use with d2_framebuffer
*********************************************************************************************************************/
void * graphics_get_back_buffer(void)
{
    uint8_t tmp = 0;

    if (s_drw_buf == 0)
    {
        tmp = 1;
    }
    return &(fb_foreground[tmp][0]);
}
/**********************************************************************************************************************
 End of function graphics_get_draw_buffer
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
    set_selected_menu_demo(choice);

    switch (choice)
    {
        case  LCD_FULL_BG_USER_LED:
            g_current_backgroud.p_bg_src = (uint8_t *)&g_ref_001_user_led_background.pixel_data[0];
            g_current_backgroud.menu_icon_active = true;
            g_current_backgroud.exit_icon_active = true;
            break;

        case  LCD_FULL_BG_CAMERA_BACKGROUND:
            g_current_backgroud.p_bg_src = (uint8_t *)&user_clean_background.pixel_data[0];
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
            g_current_backgroud.p_text_block = &g_kis_page;
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
            break;

        default:
    }

    g_current_backgroud.update = true;
}
/**********************************************************************************************************************
 End of function dsp_set_background
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 *  Function Name: glcdc_vsync_isr
 * Description  : Interrupt Callback function of LCDC.
 * Argument     : p_args
 * Return Value : NONE
 *********************************************************************************************************************/
void glcdc_vsync_isr(display_callback_args_t *p_args)
{

    FSP_PARAMETER_NOT_USED(p_args);

    g_update_fps_text = 1;

    switch (p_args->event)
    {
        case DISPLAY_EVENT_GR1_UNDERFLOW:
            __NOP();
            break;

        case DISPLAY_EVENT_GR2_UNDERFLOW:
            __NOP();
            break;

        default:
    }

    g_vsync_flag = 1;
    return;
}
/**********************************************************************************************************************
 End of function glcdc_vsync_isr
 *********************************************************************************************************************/
