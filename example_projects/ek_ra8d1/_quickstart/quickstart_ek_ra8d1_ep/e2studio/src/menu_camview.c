/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "menu_camview.h"
#include "jlink_console.h"

#include "r_ioport.h"
#include "r_mipi_dsi_api.h"

#include "hal_data.h"
#include "dsi_layer.h"

#include "camera_layer.h"
#include "graphics\graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "bg_font_18_full.h"

#include "touch_gt911.h"
#include "gimp.h"

// #define SHOW_DETECTION_BOXES (1)

#define CONNECTION_ABORT_CRTL       (0x00)
#define MENU_EXIT_CRTL              (0x20)

#define SLIDE_SHOW_RESET            (60)
#define SLIDE_SHOW_RESET_WEATHER    (150)

#define WEATHER_ICON_HOFFSET        (308)
#define WEATHER_ICON_VOFFSET        (48)

#define MODULE_NAME                 "\r\n%d. INTERACTIVE AI, CONNECTIVITY & HMI DEMOS\r\n\r\n"

#define SUB_OPTIONS                 "To explore various Interactive Connectivity & HMI Demos\r\n\r\n"                  \
                                    "a) Please connect the MIPI Graphic and Camera Expansion Boards included in\r\n"   \
                                    "   the EK-RA8D1 kit to the EK-RA8D1 board.\r\n"                                   \
                                    "b) Please connect the Ethernet cable included in the EK-RA8D1 kit to the EK-\r\n" \
                                    "   RA8D1 board and your Internet Router or Switch.\r\n"                           \
                                    "c) Reset the kit and follow the instructions.\r\n\r\n"                            \

#define EP_INFO                     "(1)"

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

// static st_gimp_image_t * new_img = NULL;

#define SPLASH_SCREEN           (0)
#define MAIN_SCREEN             (1)
#define MENU_SCREEN             (2)
#define HELP_SCREEN             (3)
#define KIS_SCREEN              (4)
#define CAM_SCREEN              (5)
#define LED_SCREEN              (6)
#define WEATHER_SCREEN          (7)
#define AI_FACE_RECONITION      (8)
#define AI_OBJECT_RECONITION    (9)
#define CURRENCY_SCREEN         (10)
#define TIMEZONE_SCREEN         (11)
#define SECRET_SCREEN           (12)

#define SLIDESHOW_MODE          (42)
#define SLIDESHOW_AUTO_MODE     (48)
#define MAX_SCRRENS_FOR_DEMO    (28)

#define OFF                     (0U)
#define ON                      (1U)

extern bool_t init_ts(void);
extern void   test_tp();
extern void   enable_ts(void);
extern void   clear_touch(void);
extern void   do_led_screen(void);

extern void do_kis_screen(void);
extern void do_face_reconition_screen(void);
extern void do_object_detection_screen(void);
extern void do_currency_screen(void);
extern void do_world_screen(void);
extern void do_weather_screen(void);

extern void disable_led_operation(void);
extern void enable_led_operation(void);

extern bool_t is_camera_mode(void);
extern bool_t is_ethernet_mode(void);
extern bool_t is_startup_mode_in_error(void);

extern uint8_t fb_foreground[1][DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1];

volatile uint8_t * live_bg_ptr = (uint8_t *) fb_background[1];
volatile uint8_t * upd_bg_ptr  = (uint8_t *) fb_background[0];

extern void   init_sdram(void);
extern void   glcdc_init();
extern void   init_ospi(void);
extern bool_t can_swap_demos(void);
extern void   reset_transition(void);
extern bool_t in_transition(void);
extern void   show_menu_icon(void);
extern bool_t process_main_menu(void);

extern fsp_err_t display_mipi_ili9806e_init(display_instance_t const * const p_display);

extern touch_data_t g_touch_data;

#define NUM_WEATHER_IMAGE_ICONS    (2)
#define NUM_VIEWER_IMAGES          16
#define REFRESH_RATE               8

/* RESOLUTION FROM CAMERA */
#define CAM_IMG_SIZE_X             320
#define CAM_IMG_SIZE_Y             238  /* Trim the Right Hand Edge hiding corruption */

#define HP_SIZE                    (70) /* Hit Box for ICONS */
#define HB_SIZE                    (45) /* Hit Box for Hamburger and CLose */

#ifdef FULL_SCREEN

// full screen
 #define CAM_LAYER_SIZE_X          480 // 000 --> 480
 #define CAM_LAYER_SIZE_Y          854 // 000 --> 854

#else

/* normal screen */
 #define CAM_LAYER_SIZE_X          476 /* 000 --> LCD_VPIX */
 #define CAM_LAYER_SIZE_Y          360 /* 000 --> 480 */

#endif

bool_t   show_bg     = true;
uint32_t demo_screen = SPLASH_SCREEN;  // DAV/2D SPLASH SCREEN
uint32_t slide_show  = 7;              // LED

extern uint8_t bsp_camera_out_buffer565[];
extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t   camera_init(bool_t use_test_mode);

static bool_t   g_camera_initialised = false;
static bool_t   g_ts_initialised     = false;
static uint32_t g_transition_frame   = 2;

/**********************************************************************************************************************
 * Function Name: reset_transition
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void reset_transition (void)
{
    g_transition_frame = 3;
}

/**********************************************************************************************************************
 * End of function reset_transition
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: in_transition
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t in_transition (void)
{
    while (can_swap_demos() == false)
    {
        vTaskDelay(1);
    }

    if (g_transition_frame > 0)
    {
        g_transition_frame--;

        return true;
    }
    else
    {
        /* Re-enable IRQn interrupt once payload reception completes */
        R_ICU_ExternalIrqEnable((void *) &g_external_irq3_ctrl);

        return false;
    }
}

/**********************************************************************************************************************
 * End of function in_transition
 *********************************************************************************************************************/
static bool_t menu_in_use   = false;
static bool_t previous_menu = 1;

extern bool_t ia_processing(void);

/**********************************************************************************************************************
 * Function Name: process_main_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t process_main_menu (void)
{
    d2_point detection_x     = 0;
    d2_point detection_y     = 0;
    bool_t   button_detected = false;  // if demo is detected then return false; if close or menu buttons detected return true;

#ifdef SHOW_DETECTION_BOXES
    {
        /* TOP ROW */
        detection_x = 335 - (HP_SIZE / 2);

        /* COL 4 */
        detection_y = 592 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0xffffff);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* COL 3 */
        detection_y = 428 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0xff0000);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* COL 2 */
        detection_y = 265 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0x0000ff);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* COL 1 */
        detection_y = 100 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0x00ff00);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* BOTTOM ROW */
        detection_x = 193 - (HP_SIZE / 2);

        /* COL 4 */
        detection_y = 592 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0x8f8f8f);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* COL 3 */
        detection_y = 428 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0x4f4f4f);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* COL 2 */
        detection_y = 265 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0xCfCfCf);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);

        /* COL 1 */
        detection_y = 100 - (HP_SIZE / 2);
        d2_setcolor(d2_handle, 0, 0xCf004f);
        d2_renderquad(d2_handle, (d2_point) (detection_x << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y) << 4),
                      (d2_point) ((detection_x + HP_SIZE) << 4), (d2_point) ((detection_y + HP_SIZE) << 4),
                      (d2_point) (detection_x << 4), (d2_point) ((detection_y + HP_SIZE) << 4), 0);
    }
#endif                                 /*  SHOW_DETECTION_BOXES */

    if (g_touch_data.num_points >= 4)
    {
        demo_screen = SECRET_SCREEN;
        reset_transition();
        menu_in_use = false;

        return true;
    }

    /* Check for show mwnu) */
    if (g_touch_data.num_points > 0)
    {
        uint8_t active_tp = g_touch_data.num_points - 1;
        bool_t  valid     = false;

        /* Support on 1 tp */
        {
            /* Process when menu is open */
            if (true == menu_in_use)
            {
                /* CLOSE HORIZONTAL DETECTION */
                detection_x = 433 - (HB_SIZE / 2);
                detection_y = 818 - (HB_SIZE / 2);

                if ((g_touch_data.point[active_tp].y < (detection_x + HP_SIZE)) &&
                    (g_touch_data.point[active_tp].y > detection_x) &&
                    (g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                    (g_touch_data.point[active_tp].x > detection_y))
                {
                    /* Process CLOSE While menu is shown */
                    reset_transition();
                    previous_menu = demo_screen;
                    demo_screen   = MAIN_SCREEN;
                    menu_in_use   = false;
                    clear_touch();
                }

                {
                    detection_x = 335 - (HP_SIZE / 2);
                    if ((g_touch_data.point[active_tp].y < (detection_x + HP_SIZE)) &&
                        (g_touch_data.point[active_tp].y > detection_x))
                    {
                        /* TOP 1 KIS SCREEN */
                        detection_y = 100 - (HP_SIZE / 2);
                        if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                            (g_touch_data.point[active_tp].x > detection_y))
                        {
                            valid       = true;
                            demo_screen = KIS_SCREEN;
                        }

                        /* TOP COL 2 HELP */
                        detection_y = 265 - (HP_SIZE / 2);
                        if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                            (g_touch_data.point[active_tp].x > detection_y))
                        {
                            valid       = true;
                            demo_screen = HELP_SCREEN;
                        }

                        /* Top COL 3 LED SCREEN */
                        detection_y = 428 - (HP_SIZE / 2);
                        if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                            (g_touch_data.point[active_tp].x > detection_y))
                        {
                            valid       = true;
                            demo_screen = LED_SCREEN;
                            disable_led_operation();
                        }

                        /* TOP COL 4 CURRENCY SCREEN */
                        detection_y = 592 - (HP_SIZE / 2);
                        if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                            (g_touch_data.point[active_tp].x > detection_y))
                        {
                            valid       = true;
                            demo_screen = CURRENCY_SCREEN;
                        }
                    }
                    else
                    {
                        detection_x = 193 - (HP_SIZE / 2);
                        if ((g_touch_data.point[active_tp].y < (detection_x + HP_SIZE)) &&
                            (g_touch_data.point[active_tp].y > detection_x))
                        {
                            {
                                /* BOT COL 1 WORLD TIMEZONE */
                                detection_y = 100 - (HP_SIZE / 2);
                                if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                                    (g_touch_data.point[active_tp].x > detection_y))
                                {
                                    /* Weather menu */
                                    valid       = true;
                                    demo_screen = TIMEZONE_SCREEN;
                                }

                                /* BOT COL 2 WEATHER */
                                detection_y = 265 - (HP_SIZE / 2);
                                if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                                    (g_touch_data.point[active_tp].x > detection_y))
                                {
                                    /* HELP menu */
                                    valid       = true;
                                    demo_screen = WEATHER_SCREEN;
                                }

                                /* Top COL 3 FACE RECONINTION SCREEN */
                                detection_y = 428 - (HP_SIZE / 2);
                                if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                                    (g_touch_data.point[active_tp].x > detection_y))
                                {
                                    valid       = true;
                                    demo_screen = AI_FACE_RECONITION;
                                }

                                /* BOT COL 4 IMAGE CLASSIFICATION SCREEN */
                                detection_y = 592 - (HP_SIZE / 2);
                                if ((g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                                    (g_touch_data.point[active_tp].x > detection_y))
                                {
                                    valid       = true;
                                    demo_screen = AI_OBJECT_RECONITION;
                                }
                            }
                        }
                    }
                }

                if (true == valid)
                {
                    reset_transition();
                    menu_in_use = false;

                    return false;
                }
            }
            else
            {
                /* COMMON VERTICLE DETECTION for Hamburger and close  */
                detection_x = 433 - (HB_SIZE / 2);

                /* HAMBURGER HORIZONTAL DETECTION */
                detection_y = 32 - (HB_SIZE / 2);

                /* Process when menu is closed */
                if ((g_touch_data.point[active_tp].y < (detection_x + HP_SIZE)) &&
                    (g_touch_data.point[active_tp].y > detection_x) &&
                    (g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                    (g_touch_data.point[active_tp].x > detection_y))

                {
                    if (LED_SCREEN == demo_screen)
                    {
                        enable_led_operation();
                    }

                    while (ia_processing())
                    {
                        vTaskDelay(1);
                    }

                    do_detection      = false;
                    do_classification = false;

                    previous_menu = demo_screen;
                    demo_screen   = MENU_SCREEN;
                    menu_in_use   = true;

                    clear_touch();
                    button_detected = true;
                    reset_transition();
                }

                // CLOSE HORIZONTAL DETECTION
                detection_y = 818 - (HB_SIZE / 2);

                if ((g_touch_data.point[active_tp].y < (detection_x + HP_SIZE)) &&
                    (g_touch_data.point[active_tp].y > detection_x) &&
                    (g_touch_data.point[active_tp].x < (detection_y + HP_SIZE)) &&
                    (g_touch_data.point[active_tp].x > detection_y))
                {
                    if (LED_SCREEN == demo_screen)
                    {
                        enable_led_operation();
                    }

                    while (ia_processing())
                    {
                        vTaskDelay(1);
                    }

                    do_detection      = false;
                    do_classification = false;

                    previous_menu = demo_screen;
                    demo_screen   = MAIN_SCREEN;
                    menu_in_use   = false;
                    clear_touch();
                    button_detected = true;
                    reset_transition();
                }
            }
        }
    }

/*Re-enable IRQn interrupt once payload reception completes */
    R_ICU_ExternalIrqEnable((void *) &g_external_irq3_ctrl);

    return button_detected;
}

/**********************************************************************************************************************
 * End of function process_main_menu
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: show_menu_icon
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void show_menu_icon (void)
{
    /* Draw hamburger menu icon */
    d2_setcolor(d2_handle, 0, 0xFFFFFF);
    d2_setalpha(d2_handle, 0xFF);
    d2_setlinecap(d2_handle, d2_lc_round);

    /* DRAW HAMBURGER */
    for (uint32_t i = 0; i < 3; i++)
    {
        d2_point mx = (d2_point) (8 << 4);
        d2_point my = (d2_point) ((450 - (17 * i)) << 4);
        d2_renderline(d2_handle, my, (mx), my, ((mx) + (50 << 4)), 7 << 4, 0);
    }

    /* DRAW X */
    if (demo_screen >= MAIN_SCREEN)
    {
        d2_point mx        = (d2_point) (793);
        d2_point my        = (d2_point) (450);
        d2_point msize     = (d2_point) (32);
        d2_width thickness = (d2_width) (7);

        d2_renderline(d2_handle,
                      (d2_point) (my << 4),
                      (d2_point) (mx << 4),
                      (d2_point) ((my - msize) << 4),
                      (d2_point) ((mx + msize) << 4),
                      (d2_width) (thickness << 4),
                      0);
        my = my - msize;
        d2_renderline(d2_handle,
                      (d2_point) (my << 4),
                      (d2_point) (mx << 4),
                      (d2_point) ((my + msize) << 4),
                      (d2_point) ((mx + msize) << 4),
                      (d2_width) (thickness << 4),
                      0);
    }
}

/**********************************************************************************************************************
 * End of function show_menu_icon
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: draw_menu_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void draw_menu_screen (void)
{
    st_gimp_image_t * img = NULL;

    while (ia_processing())
    {
        vTaskDelay(1);
    }

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_MAIN_MENU);
        d2_setblitsrc(d2_handle,
                      img->pixel_data,
                      (d2_s32) img->width,
                      (d2_s32) img->width,
                      (d2_s32) img->height,
                      EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    (d2_s32) img->width,
                    (d2_s32) img->height,            // Source width/height
                    (d2_blitpos) 0,
                    0,                               // Source position
                    (d2_width) ((img->width) << 4),
                    (d2_width) ((img->height) << 4), // Destination width/height
                    (d2_point) ((LCD_HPIX - img->width) << 4),
                    (d2_point) (0 << 4),             // Destination position
                    d2_tm_filter);

        /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
        d2_flushframe(d2_handle);
        graphics_end_frame();
    }
    else
    {
        process_main_menu();

        /* Wait for previous frame rendering to finish, then finalise this frame and flip the buffers */
        d2_flushframe(d2_handle);
        graphics_end_frame();
        graphics_swap_buffer();
    }
}

/**********************************************************************************************************************
 * End of function draw_menu_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_splash_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void do_splash_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        d2_clear(d2_handle, 0x101010);

        img = (st_gimp_image_t *) get_image_data(GI_SPLASH_SCREEN);
        d2_setblitsrc(d2_handle, img->pixel_data, LCD_HPIX, LCD_HPIX, LCD_VPIX, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    LCD_HPIX,
                    LCD_VPIX,                     // Source width/height
                    (d2_blitpos) 0,
                    0,                            // Source position
                    (d2_width) ((LCD_HPIX) << 4),
                    (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                    0,
                    0,                            // Destination position
                    d2_tm_filter);
    }
    else
    {
        process_main_menu();
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_splash_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_help_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void do_help_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_HELP_SCREEN);
        d2_setblitsrc(d2_handle, img->pixel_data, 480, 480, LCD_VPIX, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    480,
                    LCD_VPIX,                     // Source width/height
                    (d2_blitpos) 0,
                    0,                            // Source position
                    (d2_width) ((480) << 4),
                    (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                    0,
                    0,                            // Destination position
                    d2_tm_filter);

        show_menu_icon();
    }
    else
    {
        process_main_menu();
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_help_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_secret_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void do_secret_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_CREDITS);
        d2_setblitsrc(d2_handle, img->pixel_data, 480, 480, LCD_VPIX, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    480,
                    LCD_VPIX,                     // Source width/height
                    (d2_blitpos) 0,
                    0,                            // Source position
                    (d2_width) ((480) << 4),
                    (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                    0,
                    0,                            // Destination position
                    d2_tm_filter);

        print_bg_font_18(d2_handle, 5, 740, VERSION_STR);
        show_menu_icon();
    }
    else
    {
        process_main_menu();
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_secret_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_main_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void do_main_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_MAIN_SCREEN);
        d2_setblitsrc(d2_handle, img->pixel_data, 480, 480, LCD_VPIX, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    480,
                    LCD_VPIX,                     // Source width/height
                    (d2_blitpos) 0,
                    0,                            // Source position
                    (d2_width) ((480) << 4),
                    (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                    0,
                    0,                            // Destination position
                    d2_tm_filter);

        show_menu_icon();
    }
    else
    {
        process_main_menu();
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_main_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_camera_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void do_camera_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();

#if 1
    graphics_start_frame();

    /* Use background image */
    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_MAIN_BLANK);
        d2_setblitsrc(d2_handle, img->pixel_data, 480, 480, LCD_VPIX, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    480,
                    LCD_VPIX,                     // Source width/height
                    (d2_blitpos) 0,
                    0,                            // Source position
                    (d2_width) ((480) << 4),
                    (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                    0,
                    0,                            // Destination position
                    d2_tm_filter);
    }
    else
    {
        d2_setblitsrc(d2_handle,
                      bsp_camera_out_buffer565,
                      CAM_IMG_SIZE_X,
                      CAM_IMG_SIZE_X,
                      CAM_IMG_SIZE_Y,
                      d2_mode_rgb565);

        d2_blitcopy(d2_handle,
                    CAM_IMG_SIZE_X,
                    CAM_IMG_SIZE_Y,                                        // Source width/height
                    (d2_blitpos) 0,
                    0,                                                     // Source position
                    (d2_width) ((CAM_LAYER_SIZE_X) << 4),
                    (d2_width) ((CAM_LAYER_SIZE_Y) << 4),                  // Destination size width/height
                    (d2_width) (((480 - CAM_LAYER_SIZE_X) / 2) << 4),
                    (d2_width) (((LCD_VPIX - CAM_LAYER_SIZE_Y) / 2) << 4), // Destination offset position
                    d2_tm_filter);

        bsp_camera_capture_image();
    }

    show_menu_icon();
    process_main_menu();

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
#endif
}

/**********************************************************************************************************************
 * End of function do_camera_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_font_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void do_font_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();

    img = (st_gimp_image_t *) get_image_data(GI_MAIN_BLANK);
    d2_setblitsrc(d2_handle, img->pixel_data, 480, 480, LCD_VPIX, EP_SCREEN_MODE);

    d2_blitcopy(d2_handle,
                480,
                LCD_VPIX,                     // Source width/height
                (d2_blitpos) 0,
                0,                            // Source position
                (d2_width) ((480) << 4),
                (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                0,
                0,                            // Destination position
                d2_tm_filter);

    /* Reset alpha in case it was changed above */
    d2_setalpha(d2_handle, 0xFF);

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_font_screen
 *********************************************************************************************************************/

extern void reenable_backlight_control(void);
extern void stop_led_demo(void);

#define KEY_TIMEOUT    (60000000)

/**********************************************************************************************************************
 * Function Name: camview_display_menu
 * Description  : .
 * Return Value : The next steps screen.
 *********************************************************************************************************************/
test_fn camview_display_menu (bool_t first_call)
{
    int16_t  c        = -1;
    uint32_t ss_timer = 50;

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

// if (first_call == false)
    {
        /* Select text foreground */
        sprintf(s_print_buffer, "%s", gp_white_fg);
        print_to_console(s_print_buffer);

        sprintf(s_print_buffer, MODULE_NAME, g_selected_menu);
        print_to_console(s_print_buffer);
    }

    memset(fb_foreground, 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1);

    /* Initialize D/AVE 2D driver */
    d2_handle = d2_opendevice(0);
    d2_inithw(d2_handle, 0);

    /* Clear both buffers */
    d2_framebuffer(d2_handle, fb_background, LCD_HPIX, LCD_STRIDE, LCD_VPIX * LCD_BUF_NUM, EP_SCREEN_MODE);
    d2_clear(d2_handle, 0x00000000);

    /* Set various D2 parameters */
    d2_setblendmode(d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);
    d2_setalphamode(d2_handle, d2_am_constant);
    d2_setalpha(d2_handle, 0xff);
    d2_setantialiasing(d2_handle, 1);
    d2_setlinecap(d2_handle, d2_lc_butt);
    d2_setlinejoin(d2_handle, d2_lj_none);

    if (is_camera_mode() == true)
    {
        /* true  = use live camera feed */
        /* false = use test_mode */
        if (false == g_camera_initialised)
        {
            g_camera_initialised = camera_init(true);
            g_ts_initialised     = init_ts();
        }
    }
    else
    {
        i2c_master_status_t i2c_status;

        R_IIC_MASTER_StatusGet(&g_i2c_master1_ctrl, &i2c_status);

        /* only open if not opened */
        if (true != i2c_status.open)   // I2c not open
        {
            R_IIC_MASTER_Open(&g_i2c_master1_ctrl, &g_i2c_master1_cfg);
        }

        if (false == g_ts_initialised)
        {
            g_ts_initialised = init_ts();
        }
    }

    if (is_startup_mode_in_error() == true)
    {
        uint32_t count = KEY_TIMEOUT;

        R_GPT_Stop(g_blinker.p_ctrl);

        print_to_console((void *) "\n\rNote: Camera state cannot be determined, please check SW1 on target board.");

        while (1)
        {
            if (KEY_TIMEOUT == count)
            {
                R_BSP_PinAccessEnable();
                TURN_RED_OFF;
                R_BSP_PinAccessDisable();
            }

            if ((count--) == 0)
            {
                count = KEY_TIMEOUT * 2;

                R_BSP_PinAccessEnable();
                SYSTEM_ERROR;
                R_BSP_PinAccessDisable();

                print_to_console((void *) "\rNote: Camera state cannot be determined, please check SW1 on target board.");
            }
        }
    }

    if (true == g_ts_initialised)
    {
        print_to_console((void *) "\r\nThe AI, connectivity, and HMI applications are running on the EK-RA8D1 kit." \
                                  "\r\nTap the menu icon on the welcome screen displayed on the MIPI Graphics"      \
                                  "\r\nExpansion Board and launch various applications.\r\n");

        if (true == first_call)
        {
            glcdc_init();

            test_tp();                 /* Reset is performed in here. */

            /* Initialize and start display output */
            fsp_err_t err = display_mipi_ili9806e_init(&g_display0);
            vTaskDelay(100);

            if (FSP_SUCCESS != err)
            {
                /* Set red LED */
                R_BSP_PinWrite(g_bsp_leds.p_leds[BSP_LED_LED3], BSP_IO_LEVEL_HIGH);
            }

            /*    TS setup */
            enable_ts();
        }

        if (true == g_ts_initialised)
        {
            sprintf(s_print_buffer, MENU_RETURN_INFO);
            print_to_console(s_print_buffer);
        }

        bool_t blinky = OFF;

        start_key_check();
        demo_screen = SPLASH_SCREEN;
        reset_transition();

        /* Allow display backlight to enable  */
        reenable_backlight_control();

        R_ICU_ExternalIrqEnable((void *) &g_external_irq3_ctrl);
        menu_in_use = false;

        while (1)
        {
            switch (demo_screen)
            {
                case SPLASH_SCREEN:
                {
                    do_splash_screen();
                    if ((ss_timer--) == 0)
                    {
                        demo_screen = MAIN_SCREEN; // default
#ifdef OVERRIDE_ACTIVE_SCREEN
                        demo_screen = OVERRIDE_ACTIVE_SCREEN;
#endif
                        reset_transition();
                    }

                    break;
                }

                case MAIN_SCREEN:
                {
                    do_main_screen();
                    break;
                }

                case MENU_SCREEN:
                {
                    draw_menu_screen();
                    break;
                }

                case HELP_SCREEN:
                {
                    do_help_screen();
                    break;
                }

                case KIS_SCREEN:
                {
                    do_kis_screen();
                    break;
                }

                case CAM_SCREEN:
                {
                    if (true == g_camera_initialised)
                    {
                        do_camera_screen();
                    }

                    break;
                }

                case LED_SCREEN:
                {
                    do_led_screen();
                    break;
                }

                case WEATHER_SCREEN:
                {
                    do_weather_screen();
                    break;
                }

                case AI_FACE_RECONITION:
                {
                    do_face_reconition_screen();
                    break;
                }

                case SLIDESHOW_MODE:
                {
// REMOVED                    do_slideshow_screen();
                    break;
                }

                case SLIDESHOW_AUTO_MODE:
                {
// REMOVED                    do_slideshow_auto();
                    break;
                }

                case AI_OBJECT_RECONITION:
                {
                    do_object_detection_screen();
                    break;
                }

                case CURRENCY_SCREEN:
                {
                    do_currency_screen();
                    break;
                }

                case TIMEZONE_SCREEN:
                {
                    do_world_screen();
                    break;
                }

                case SECRET_SCREEN:
                {
                    do_secret_screen();
                    break;
                }

                case 99:
                {
                    do_font_screen();
                    break;
                }

                default:
                {
                    break;
                }
            }

            /* This task delay- allows other threads (ie AI) to run in parallel with screen */
            vTaskDelay(20);

            uint32_t next_demo_screen = demo_screen;
            if (demo_screen != next_demo_screen)
            {
                sprintf(s_print_buffer, "Showing Menu [%lu]\r\n", demo_screen);
                print_to_console(s_print_buffer);

                reset_transition();
                demo_screen = next_demo_screen;
            }

            blinky = !blinky;

            if (true == key_pressed())
            {
                c = get_detected_key();
                if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
                {
                    dsi_layer_disable_backlight();

                    memset(&fb_background[0][0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);
                    memset(&fb_background[1][0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);

                    stop_led_demo();

                    break;
                }

                start_key_check();
            }
        }
    }
    else
    {
        if (false == first_call)
        {
            sprintf(s_print_buffer, SUB_OPTIONS);
            print_to_console(s_print_buffer);

            print_to_console((void *) "Configuration switch (SW1) settings (Connectivity DEMO)\r\n");
            print_to_console((void *) "SW1-1 SW1-2 SW1-3 SW1-4 SW1-5 SW1-6 SW1-7 SW1-8 \r\n");
            print_to_console((void *) " OFF   OFF   OFF   OFF   ON    OFF   ON    OFF  \r\n");

            print_to_console((void *) "Configuration switch (SW1) settings (AI Camera DEMO)\r\n");
            print_to_console((void *) "SW1-1 SW1-2 SW1-3 SW1-4 SW1-5 SW1-6 SW1-7 SW1-8 \r\n");
            print_to_console((void *) " OFF   OFF   ON    OFF   OFF   OFF   ON    OFF  \r\n");
            sprintf(s_print_buffer, MENU_RETURN_INFO);
            print_to_console(s_print_buffer);

            while (CONNECTION_ABORT_CRTL != c)
            {
                c = input_from_console();
                if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
                {
                    break;
                }
            }
        }
    }

    return 0;
}

/**********************************************************************************************************************
 * End of function camview_display_menu
 *********************************************************************************************************************/
