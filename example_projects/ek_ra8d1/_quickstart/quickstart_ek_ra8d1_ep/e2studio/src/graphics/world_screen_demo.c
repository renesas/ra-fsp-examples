/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : world_screen
 * Version      : .
 * Description  : The led demo screen display.
 *********************************************************************************************************************/

#include <math.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "bsp_api.h"

#include "common_utils.h"
#include "common_init.h"
#include "board_cfg.h"
#include "menu_camview.h"
#include "jlink_console.h"

#include "r_ioport.h"
#include "r_mipi_dsi_api.h"
#include "r_rtc_api.h"

#include "hal_data.h"
#include "dsi_layer.h"

#include "graphics/graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "bg_font_18_full.h"
#include "touch_gt911.h"
#include "gimp.h"

#define WC_OFFSET                  6

#define CURRENCY_HB_WIDTH_SIZE     (48)
#define CURRENCY_HB_HEIGHT_SIZE    (48)

extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t   in_transition(void);
extern void     show_menu_icon(void);
extern bool_t   process_main_menu(void);

typedef struct st_world_screen_indicator_def
{
    uint32_t           id;
    char_t           * p_name_str;
    st_lcd_point_def_t center;
    int32_t            utcOffset;
    bool_t             in_use;         // blink indicator position in radians
} st_world_screen_indicator_def_t;

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

static st_world_screen_indicator_def_t world_screen_control[] =
{
    {GI_SAN_FRANSICSCO, "San Francisco, United States",
        {143 - 15,          243 - 15},                     -8,
        false},
    {GI_MEXICO_CITY,    "Mexico City, Mexico",
        {196 - 15,          196 - 15},                     -6,
        false},
    {GI_TORONTO,        "Toronto, Canada",
        {241 - 15,          242 - 15},                     -5,
        false},
    {GI_SANTIAGO,       "Santiago, Chile",
        {241 - 15,
        61 - 15}, -3,
        false},
    {GI_SAO_PAULO,      "Sao Paulo, Brazil",
        {312 - 15,
        123 - 15}, -3,
        false},
    {GI_LONDON,         "London, England",
        {352,
        254}, +0,
        false},
    {GI_BERLIN,         "Berlin, Germany",
        {402,
        236}, +1,
        false},
    {GI_CAPE_TOWN,      "Cape Town, South Africa",
        {433 - 15,
        66 - 15}, +2,
        false},
    {GI_MOSCOW,         "Moscow, Russia",
        {500 - 15,
        280 - 15}, +3,
        false},
    {GI_NEW_DELHI,      "New Delhi, India",
        {545 - 15,
        198 - 15}, +5,
        false},
    {GI_SINGAPHORE,     "Singapore, Singapore",
        {607 - 15,
        145 - 15}, +8,
        false},
    {GI_BEIJING,        "Beijing, China",
        {629 - 15,
        199 - 15}, +8,
        false},
    {GI_TOKYO,          "Tokyo, Japan",
        {677 - 15,
        234 - 15}, +9,
        false},
    {GI_SYDNEY,         "Sydney, Australia",
        {701 - 15,
        69 - 15}, +11,
        false},
    {GI_AUCKLAND,       "Auckland, New Zealand",
        {732 - 15,          61 - 15},                      +13, false}
};

static void reset_rtc(void);
static void show_detection_boxes(void);
static void select_detection_box(int32_t * _chosen);

bool_t convert_gmt_to_region(uint32_t region, rtc_time_t * time);

extern bool_t https_local_time_update(void);

extern char_t       g_current_time_raw_str[];
extern touch_data_t g_touch_data;

void do_world_screen(void);

extern uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);

/**********************************************************************************************************************
 * Function Name: convert_gmt_to_region
 * Description  : .
 * Arguments    : region
 *              : time
 * Return Value : .
 *********************************************************************************************************************/
bool_t convert_gmt_to_region (uint32_t region, rtc_time_t * time)
{
    bool_t  is_am   = true;
    int32_t local_h = 0;

    if (https_local_time_update())
    {
        reset_rtc();
    }

    R_RTC_CalendarTimeGet(&g_rtc_ctrl, time);
    local_h = (int16_t) time->tm_hour;

    time->tm_hour += (int) (time->tm_hour + world_screen_control[region].utcOffset);
    local_h       += world_screen_control[region].utcOffset;;

    /* special case for 9, "New Delhi, India", */
    if (region == 9)
    {
        time->tm_min += 30;
        if (time->tm_min > 59)
        {
            local_h      += 1;
            time->tm_min %= 60;
        }
    }

    if (local_h < 0)
    {
        local_h += 12;
    }

    if (local_h > 23)
    {
        local_h %= 24;
    }

    if (local_h > 12)
    {
        local_h %= 12;
        is_am    = false;
    }

    if (local_h == 12)
    {
        is_am = false;
    }

    time->tm_hour = local_h;

    return is_am;
}

/**********************************************************************************************************************
 * End of function convert_gmt_to_region
 *********************************************************************************************************************/

bool_t show_it = false;

/**********************************************************************************************************************
 * Function Name: reset_rtc
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void reset_rtc (void)
{
    rtc_time_t set_time         = {};
    char_t     index_minuite[8] = {};
    char_t     index_hours[8]   = {};

    char_t * p_index_localtime = strstr(g_current_time_raw_str, "localtime\"");
    p_index_localtime = strstr(p_index_localtime, " ");

    memcpy(index_hours, strstr(p_index_localtime, " ") + 1, 2);

    p_index_localtime++;

    memcpy(index_minuite, strstr(p_index_localtime, ":") + 1, 2);

    set_time.tm_hour = atoi(index_hours);
    set_time.tm_min  = atoi(index_minuite);

    /* Set the calendar time */
    R_RTC_CalendarTimeSet(&g_rtc_ctrl, &set_time);
}

/**********************************************************************************************************************
 * End of function reset_rtc
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_world_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_world_screen (void)
{
    static int32_t first_chosen   = -1;
    static int32_t chosen         = -1;
    static bool_t  new_selection  = false;
    static bool_t  update_time    = false;
    static int32_t refresh_screen = 0;

    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        first_chosen  = -1;
        chosen        = -1;
        new_selection = false;
        update_time   = false;

        img = (st_gimp_image_t *) get_image_data(GI_WORLD_SCREEN);
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

        /* foreground */
        show_menu_icon();
    }
    else
    {
        select_detection_box(&chosen);

        if ((chosen >= 0) && (first_chosen != chosen))
        {
            new_selection  = true;
            first_chosen   = chosen;
            refresh_screen = 2;
        }

        show_detection_boxes();
        process_main_menu();
    }

    if (new_selection == true)
    {
        new_selection = false;
        update_time   = true;
        print_to_console("\r\n");
    }

    if (update_time == true)
    {
        /* Get the calendar time */
        rtc_time_t             local_time = {};
        bool_t                 is_am      = false;
        st_gimp_city_image_t * simg       = NULL;

        if (refresh_screen > 0)
        {
            refresh_screen--;
            img = (st_gimp_image_t *) get_image_data(GI_WORLD_SCREEN);
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

            /* foreground */
            show_menu_icon();
        }

        is_am = convert_gmt_to_region(world_screen_control[first_chosen].id, &local_time);

/*
 *      sprintf(s_print_buffer, "Selected Country from [%s]: Current Time [%d:%d.%d] %s utcOffset %d       \r",
 *              world_screen_control[first_chosen].p_name_str,
 *              local_time.tm_hour, local_time.tm_min, local_time.tm_sec,
 *              is_am ? "AM":"PM",
 *              (size_t)world_screen_control[first_chosen].utcOffset
 *          );
 *      print_to_console(s_print_buffer);
 */
        img = (st_gimp_image_t *) get_image_data(GI_WORLD_SCREEN);
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

        /* foreground */
        show_menu_icon();

        simg = (st_gimp_city_image_t *) get_sub_image_data(GI_TIMEZONES_LOCALE, world_screen_control[first_chosen].id);

        d2_setblitsrc(d2_handle,
                      simg->pixel_data,
                      (d2_s32) simg->width,
                      (d2_s32) simg->width,
                      (d2_s32) simg->height,
                      EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    (d2_s32) simg->width,
                    (d2_s32) simg->height,
                    (d2_blitpos) 0,
                    0,                                                                        // Source position
                    (d2_width) ((simg->width) << 4),
                    (d2_width) ((simg->height) << 4),                                         // Destination width/height
                    (d2_width) ((world_screen_control[first_chosen].center.verticle) << 4),
                    (d2_width) ((world_screen_control[first_chosen].center.horizontal) << 4), // Destination position
                    d2_tm_filter);

        char msg3[32] = {};

        sprintf(msg3, "%2d:%02d.%02d %s ", local_time.tm_hour, local_time.tm_min, local_time.tm_sec,
                is_am ? "AM" : "PM");

        d2_point vpos = world_screen_control[first_chosen].center.verticle + 2;
        d2_point hpos = world_screen_control[first_chosen].center.horizontal + 16;

        /* do font */
        print_bg_font_18(d2_handle, vpos, hpos, (char *) msg3);
    }

#ifdef ENABLE_TIMEZONE_DEBUG
    if (true == key_pressed())
    {
        int16_t c = get_detected_key();
        if (('n' == c) || ('N' == c))
        {
            rtc_time_t time = {};

            R_RTC_CalendarTimeGet(&g_rtc_ctrl, &time);
            time.tm_hour += 1;
            time.tm_hour %= 24;
            R_RTC_CalendarTimeSet(&g_rtc_ctrl, &time);

            test_timezones();
        }

        start_key_check();
    }
#endif

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_world_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: select_detection_box
 * Description  : .
 * Argument     : _chosen
 * Return Value : .
 *********************************************************************************************************************/
static void select_detection_box (int32_t * _chosen)
{
    *_chosen = -1;                     // by default no flag is chosen

    volatile touch_coord_t cp;
    int32_t                limit = (sizeof(world_screen_control)) / sizeof(st_world_screen_indicator_def_t);

    /* Store cp locally so it won't change during processing */
    cp.x = g_touch_data.point[0].x;
    cp.y = g_touch_data.point[0].y;

    for (int32_t sel = 0; sel < limit; sel++)
    {
        d2_point min_horizontal = world_screen_control[sel].center.horizontal;
        d2_point max_horizontal = world_screen_control[sel].center.horizontal + CURRENCY_HB_WIDTH_SIZE;

        d2_point min_verticle = world_screen_control[sel].center.verticle;
        d2_point max_verticle = (world_screen_control[sel].center.verticle + CURRENCY_HB_HEIGHT_SIZE);

        if (g_touch_data.num_points > 0)
        {
            if (
                (cp.x > min_horizontal) && (cp.x <= max_horizontal) &&
                (cp.y > min_verticle) && (cp.y <= max_verticle))
            {
                *_chosen = sel;
            }
        }
    }
}

/**********************************************************************************************************************
 * End of function select_detection_box
 *********************************************************************************************************************/

static bool_t shown = true;

/* DEBUG FUNCTIONS */

/**********************************************************************************************************************
 * Function Name: show_detection_boxes
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void show_detection_boxes (void)
{

    // For Debug use only, show the detection boxes as solid colour blocks
    return;

    uint32_t sel = 0;

    uint32_t limit = (sizeof(world_screen_control)) / sizeof(st_world_screen_indicator_def_t);

    /* toggle showing hit boxes (for alignment test) */
    show_it = !show_it;

    if (show_it)
    {
        for (sel = 0; sel <= limit; sel++)
        {
            d2_setcolor(d2_handle, 0, 0x8f0000);
            d2_renderquad(d2_handle, (d2_point) (world_screen_control[sel].center.verticle << 4),
                          (d2_point) ((world_screen_control[sel].center.horizontal) << 4),
                          (d2_point) ((world_screen_control[sel].center.verticle + CURRENCY_HB_HEIGHT_SIZE) << 4),
                          (d2_point) ((world_screen_control[sel].center.horizontal) << 4),
                          (d2_point) ((world_screen_control[sel].center.verticle + CURRENCY_HB_HEIGHT_SIZE) << 4),
                          (d2_point) ((world_screen_control[sel].center.horizontal + CURRENCY_HB_WIDTH_SIZE) << 4),
                          (d2_point) (world_screen_control[sel].center.verticle << 4),
                          (d2_point) ((world_screen_control[sel].center.horizontal + CURRENCY_HB_WIDTH_SIZE) << 4), 0);

            if (!shown)
            {
                sprintf(s_print_buffer,
                        "%s\tHITBOX POS [%03d, %03d]\r\n",
                        world_screen_control[sel].p_name_str,
                        world_screen_control[sel].center.horizontal,
                        world_screen_control[sel].center.verticle);
                print_to_console(s_print_buffer);
            }
        }

        shown = true;
    }
}

/**********************************************************************************************************************
 * End of function show_detection_boxes
 *********************************************************************************************************************/
