/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : led_screen_demo.c
 * Version      : .
 * Description  : The led demo screen display.
 *********************************************************************************************************************/

#include <math.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
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
#include "graphics/graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "fg_font_22_full.h"
#include "touch_gt911.h"
#include "gimp.h"

#define NUM_WEATHER_IMAGE_ICONS    (4)
#define RIGHT_HAND_DETECT          (554)
#define LEFT_HAND_DETECT           (300)

extern void   reset_transition(void);
extern bool_t in_transition(void);
extern void   show_menu_icon(void);
extern bool_t process_main_menu(void);

extern uint32_t get_image_data(st_image_data_t ref);
extern uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);
extern bool_t   convert_gmt_to_region(uint32_t region, rtc_time_t * time);

extern bool_t https_weather_update(void);

// Simulated / live data if available
extern char_t * gp_weather_full_table[16];

static char_t       s_print_buffer[BUFFER_LINE_LENGTH] = {};
static char_t       s_print_buffer2[64]                = {};
extern touch_data_t g_touch_data;

static void select_detection_box(int32_t * _chosen);

static int32_t selected_icon;

char_t touches[][32] =
{
    {"SWIPE_NONE"     },
    {"SWIPE_DETECTING"},
    {"SWIPE_RIGHT"    },
    {"SWIPE_LEFT"     }
};

typedef enum
{
    SWIPE_NONE,
    SWIPE_DETECTING,
    SWIPE_RIGHT,
    SWIPE_LEFT,
} swipe_event_t;

#define RESET_DETECT                  10
#define GI_WEATHER_ICON_SCREEN_MAX    15
#define NUM_ICONS_AVAILABLE           4

#define FG_PANNEL_VERTICAL            (130)

#define TEMP_C_DISP_STR               "'C"
#define TEMP_F_DISP_STR               "'F"

#define TEMP_C_KEY_STR                "temp_c"
#define TEMP_F_KEY_STR                "temp_f"

#define WIND_DISP_MPH_STR             "miles/hr"
#define WIND_DISP_KMH_STR             "km/hr"

#define WIND_MPH_KEY_STR              "wind_mph"
#define WIND_KMH_KEY_STR              "wind_kph"

typedef struct st_weather_screen_indicator_def
{
    uint32_t id;                       // Weather Screen
    uint32_t timezone_id;              // Nearest Timezone (from WTZ page)
    char_t * p_name_str;               // User Readable Name
    char_t * p_temp_key_str;           // Lookup key for temperature
    char_t * p_wind_key_str;           // lookup key for wind speed
    char_t * p_temp_str;               // Display for temperature
    char_t * p_wind_str;               // Display for wind speed
} st_weather_screen_indicator_def_t;

static st_weather_screen_indicator_def_t weather_screen_control[] =
{
    {GI_WEATHER_HONG_KONG,      GI_TOKYO,                "Hong Kong",                 TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR, WIND_DISP_KMH_STR},
    {GI_WEATHER_KYOTO,          GI_TOKYO,                "Kyoto",                     TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR, WIND_DISP_KMH_STR},
    {GI_WEATHER_LONDON,         GI_LONDON,               "London",                    TEMP_C_KEY_STR,
     WIND_MPH_KEY_STR, TEMP_C_DISP_STR, WIND_DISP_MPH_STR},
    {GI_WEATHER_MIAMI,          GI_MEXICO_CITY,          "Miami",                     TEMP_F_KEY_STR,
     WIND_MPH_KEY_STR, TEMP_F_DISP_STR, WIND_DISP_MPH_STR},
    {GI_WEATHER_MUNICH,         GI_BERLIN,               "Munich",                    TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_NEW_YORK,       GI_SAN_FRANSICSCO,       "New York",                  TEMP_F_KEY_STR,
     WIND_MPH_KEY_STR, TEMP_F_DISP_STR,
     WIND_DISP_MPH_STR},
    {GI_WEATHER_PARIS,          GI_BERLIN,               "Paris",                     TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_PRAGUE,         GI_BERLIN,               "Prague",                    TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_QUEENSTOWN,     GI_AUCKLAND,             "Queenstown",                TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_RIO_DE_JANERIO, GI_SAO_PAULO,            "Rio de Janeiro",            TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_ROME,           GI_BERLIN,               "Rome",                      TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_SAN_FRANCISCO,  GI_SAN_FRANSICSCO,       "San Francisco",             TEMP_F_KEY_STR,
     WIND_MPH_KEY_STR, TEMP_F_DISP_STR,
     WIND_DISP_MPH_STR},
    {GI_WEATHER_SHANGHAI,       GI_BEIJING,              "Shanghai",                  TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_SINGAPORE,      GI_SINGAPHORE,           "Singapore",                 TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_SYDNEY,         GI_SYDNEY,               "Sydney",                    TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR},
    {GI_WEATHER_TORONTO,        GI_TORONTO,              "Toronto",                   TEMP_C_KEY_STR,
     WIND_KMH_KEY_STR, TEMP_C_DISP_STR,
     WIND_DISP_KMH_STR}
};

#define STD_SIZE    8
#define MED_SIZE    12
#define LDG_SIZE    48

static char_t str_condition[LDG_SIZE] = {};

uint16_t static_x = 10;
uint16_t static_y = 74;

#define LINE_1_FORMAT    "%s %s             "
#define LINE_2_FORMAT    "%s %s from %s"
#define LINE_3_FORMAT    "%s%%"

static char_t clear_line[56] = "                                  ";
static char_t line1[96]      = {};
static char_t line2[56]      = {};
static char_t line3[56]      = {};

static bool_t first_call = true;
static st_gimp_fg_icon_image_t icon_erase;

static bool_t  update_choice          = false;
static int32_t current_screen         = 0;
static int32_t first_chosen           = -1;
static int32_t current_weather_screen = GI_WEATHER_HONG_KONG;

static display_runtime_cfg_t glcd_layer_change;

void        do_weather_screen(void);
static void draw_with_alpha(st_gimp_fg_icon_image_t * object, uint16_t hoffset, uint16_t voffset);

/**********************************************************************************************************************
 * Function Name: draw_with_alpha
 * Description  : .
 * Arguments    : object
 *              : hoffset
 *              : voffset
 * Return Value : .
 *********************************************************************************************************************/
static void draw_with_alpha (st_gimp_fg_icon_image_t * object, uint16_t hoffset, uint16_t voffset)
{
    /* GLCDC Graphics Layer 1 size must be 182 x 200 */
    uint8_t * buf_ptr = (uint8_t *) fb_foreground;
    display_input_cfg_t const * p_input = &g_display0.p_cfg->input[1]; // Layer 2

    /* Supports up to 4 bytes per pixel  */
    if ((uint32_t) object->bytes_per_pixel > 4)
    {

        /* The input images specified in object has been corrupted */
        return;
    }

    /* clear screen */

// memset(buf_ptr, 0, p_input->hstride * p_input->vsize * 4);

    /* offset 200 horizontal */
    buf_ptr += (hoffset * (p_input->hstride * 4)); // Horizontal offset
    buf_ptr += (voffset * 4);                      // Vertical offset

    uint32_t bpp = object->bytes_per_pixel;

    for (uint32_t v = 0; v < object->height; v++)
    {
        uint32_t v_offset = v * object->width * bpp;
        for (uint32_t h = 0; h < (object->width * bpp); h += bpp)
        {
            buf_ptr[0] = object->pixel_data[v_offset + h + 2]; // B
            buf_ptr[1] = object->pixel_data[v_offset + h + 1]; // G
            buf_ptr[2] = object->pixel_data[v_offset + h];     // R
            buf_ptr[3] = object->pixel_data[v_offset + h + 3]; // A                                     // A (Unused)
            buf_ptr   += 4;
        }

        buf_ptr += ((p_input->hstride - p_input->hsize) * 4);

        /* align image size to be drawn with the surface size */
        buf_ptr += ((p_input->hsize - object->width) * bpp);
    }
}

/**********************************************************************************************************************
 * End of function draw_with_alpha
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: select_detection_box
 * Description  : .
 * Argument     : _chosen
 * Return Value : .
 *********************************************************************************************************************/
static void select_detection_box (int32_t * _chosen)
{
    *_chosen = -1;

    if (g_touch_data.num_points > 0)
    {
        update_choice = true;
    }

    if (update_choice)
    {
        update_choice = false;
        *_chosen      = SWIPE_NONE;

        if (g_touch_data.point[0].x > RIGHT_HAND_DETECT)
        {
            *_chosen = SWIPE_RIGHT;
        }

        if (g_touch_data.point[0].x < LEFT_HAND_DETECT)
        {
            *_chosen = SWIPE_LEFT;
        }
    }
}

/**********************************************************************************************************************
 * End of function select_detection_box
 *********************************************************************************************************************/

#define ANIMATION_FRAME_RESET_VALUE    6
#define NUM_VIEWER_IMAGES              16
uint16_t SELECTED = 0;

static uint32_t decide_icon(uint32_t            region,
                            /* time */ char_t * str_cur_time,
                            char              * condition,
                            bool_t              snow,
                            bool_t              windy);

static uint32_t s_animation_frame = ANIMATION_FRAME_RESET_VALUE;

/**********************************************************************************************************************
 * Function Name: reset_animation
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void reset_animation (void)
{
    bool_t     is_am      = false;
    rtc_time_t local_time = {};

    s_animation_frame = ANIMATION_FRAME_RESET_VALUE;

    glcd_layer_change.layer.coordinate.x = FG_PANNEL_VERTICAL;
    glcd_layer_change.layer.coordinate.y = 950; // need to center horizontical;

    glcd_layer_change.input = g_display0.p_cfg->input[1];
    (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);

    if (https_weather_update() == true)
    {
        /* update live data; */
    }

    static char_t str_condition2[LDG_SIZE] = {};
    static char_t str_cur_time[LDG_SIZE]   = "14:00";
    static char_t str_temp[STD_SIZE]       = {};
    static char_t str_wind_speed[STD_SIZE] = {};
    static char_t str_wind_dir[STD_SIZE]   = {};
    static char_t str_humidity[STD_SIZE]   = {};
    static char_t str_snow[STD_SIZE]       = {};
    static char_t str_windy[STD_SIZE]      = {};

    memset(str_condition2, 0, LDG_SIZE);
    memset(str_cur_time, 0, LDG_SIZE);
    memset(str_temp, 0, STD_SIZE);
    memset(str_wind_speed, 0, STD_SIZE);
    memset(str_wind_dir, 0, STD_SIZE);
    memset(str_humidity, 0, STD_SIZE);
    memset(str_snow, 0, STD_SIZE);
    memset(str_windy, 0, STD_SIZE);

    bool_t snow  = false;
    bool_t windy = false;

    is_am =
        convert_gmt_to_region(weather_screen_control[current_screen % NUM_ICONS_AVAILABLE].timezone_id, &local_time);

    if ((is_am == false) && (local_time.tm_hour != 12))
    {
        sprintf(str_cur_time, "%02d:00", local_time.tm_hour + 12);
    }
    else
    {
        sprintf(str_cur_time, "%02d:00", local_time.tm_hour);
    }

    char_t * p1 = strstr(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE], str_cur_time);

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, weather_screen_control[current_screen].p_temp_key_str, &str_temp[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, (size_t) len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    /* find weather_screen_control[current_screen].p_temp_key_str */
                    char_t * p3 = strstr(s_print_buffer, weather_screen_control[current_screen].p_temp_key_str);
                    if (p3 != NULL)    // "temp_f":77.0,
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, (size_t) len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                            // print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

                            // print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, ":");
                            if (p5 != NULL)
                            {
                                sprintf(str_temp, "%s", (p5 + 1));
                            }
                        }
                    }
                }
            }
        }
    }

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, "condition", &str_condition[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, (size_t) len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    /* find weather_screen_control[current_screen].p_temp_key_str */
                    char_t * p3 = strstr(s_print_buffer, "condition");
                    if (p3 != NULL)    // "condition"
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, (size_t) len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

// print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

// print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, "\":\"");
                            if (p5 != NULL)
                            {
                                sprintf(str_condition2, "%s", (p5 + 3));
                            }
                        }
                    }
                }
            }
        }
    }

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, weather_screen_control[current_screen].p_wind_key_str, &str_wind_speed[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, (size_t) len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    // find weather_screen_control[current_screen].p_temp_key_str
                    char_t * p3 = strstr(s_print_buffer, weather_screen_control[current_screen].p_wind_key_str);
                    if (p3 != NULL)    // p_wind_key_str
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, (size_t) len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                            // print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

                            // print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, ":");
                            if (p5 != NULL)
                            {
                                sprintf(str_wind_speed, "%s", (p5 + 1));
                            }
                        }
                    }
                }
            }
        }
    }

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, weather_screen_control[current_screen].p_wind_key_str, &str_wind_speed[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, (size_t) len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    /* find weather_screen_control[current_screen].p_temp_key_str */
                    char_t * p3 = strstr(s_print_buffer, weather_screen_control[current_screen].p_wind_key_str);
                    if (p3 != NULL)    // p_wind_key_str
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, (size_t) len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                            // print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

                            // print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, ":");
                            if (p5 != NULL)
                            {
                                sprintf(str_wind_speed, "%s", (p5 + 1));
                            }
                        }
                    }
                }
            }
        }
    }

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, "wind_dir", &str_wind_dir[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, (size_t) len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    // find weather_screen_control[current_screen].p_temp_key_str
                    char_t * p3 = strstr(s_print_buffer, "wind_dir");
                    if (p3 != NULL)    // wind_dir
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, (size_t) len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                            // print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

                            // print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, ":");
                            if (p5 != NULL)
                            {
                                sprintf(str_wind_dir, "%s", (p5 + 1));
                            }
                        }
                    }
                }
            }
        }
    }

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, "humidity", &str_humidity[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    // find weather_screen_control[current_screen].p_temp_key_str
                    char_t * p3 = strstr(s_print_buffer, "humidity");
                    if (p3 != NULL)    // humidity
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                            // print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

                            // print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, ":");
                            if (p5 != NULL)
                            {
                                sprintf(str_humidity, "%s", (p5 + 1));
                            }
                        }
                    }
                }
            }
        }
    }

    // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, "will_it_snow", &str_snow[0]);
    {
        if (p1 != NULL)
        {
            char_t * p2 = strstr(p1, "uv");

            if (p2 != NULL)
            {
                int len = p2 - p1;

                if (len > 0)
                {
                    sprintf(s_print_buffer, "len [%d]\r\n", len);

                    // print_to_console(s_print_buffer);

                    memcpy(s_print_buffer, p1, len);
                    s_print_buffer[len] = '\0';

                    // print_to_console(s_print_buffer);

                    // find weather_screen_control[current_screen].p_temp_key_str
                    char_t * p3 = strstr(s_print_buffer, "will_it_snow");
                    if (p3 != NULL)    // humidity
                    {
                        char_t * p4   = strstr(p3, ",");
                        int      len2 = p4 - p3;

                        if (len2 > 0)
                        {
                            memcpy(s_print_buffer2, p3, len2);

                            sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                            // print_to_console(s_print_buffer);

                            s_print_buffer2[len2] = '\0';

                            // print_to_console(s_print_buffer2);

                            char_t * p5 = strstr(s_print_buffer2, ":");
                            if (p5 != NULL)
                            {
                                sprintf(str_snow, "%s", (p5 + 1));
                                if (atoi(str_snow) == 1)
                                {
                                    snow = true;
                                }
                            }
                        }
                    }
                }
            }

            // json_get_data_from_key(gp_weather_full_table[current_screen % NUM_ICONS_AVAILABLE],str_cur_time, "will_it_rain", &str_rain[0]);
            {
                if (p1 != NULL)
                {
                    char_t * p2 = strstr(p1, "uv");

                    if (p2 != NULL)
                    {
                        int len = p2 - p1;

                        if (len > 0)
                        {
                            sprintf(s_print_buffer, "len [%d]\r\n", len);

                            // print_to_console(s_print_buffer);

                            memcpy(s_print_buffer, p1, len);
                            s_print_buffer[len] = '\0';

                            // print_to_console(s_print_buffer);

                            // find weather_screen_control[current_screen].p_temp_key_str
                            char_t * p3 = strstr(s_print_buffer, "wind_mph");
                            if (p3 != NULL) // humidity
                            {
                                char_t * p4   = strstr(p3, ",");
                                int      len2 = p4 - p3;

                                if (len2 > 0)
                                {
                                    memcpy(s_print_buffer2, p3, len2);

                                    sprintf(s_print_buffer, "len2 [%d]\r\n", len2);

                                    // print_to_console(s_print_buffer);

                                    s_print_buffer2[len2] = '\0';

                                    // print_to_console(s_print_buffer2);

                                    char_t * p5 = strstr(s_print_buffer2, ":");
                                    if (p5 != NULL)
                                    {
                                        sprintf(str_windy, "%s", (p5 + 1));
                                        if (atof(str_windy) > 8.0)
                                        {
                                            windy = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

/*
 *  char_t *pt = strstr(str_condition,":") + 2;
 *  char_t *pt2 = strstr(pt,"\"");
 * pt2 = '\0';
 */
    selected_icon = decide_icon(current_screen % NUM_ICONS_AVAILABLE,
                                /* time */ str_cur_time,
                                &str_condition,
                                snow,
                                windy);

    sprintf(line1, LINE_1_FORMAT, str_temp, weather_screen_control[current_screen].p_temp_str);
    sprintf(line2, LINE_2_FORMAT, str_wind_speed, weather_screen_control[current_screen].p_wind_str, str_wind_dir);
    sprintf(line3, LINE_3_FORMAT, str_humidity);

    char_t bigg[128] = {};

    sprintf(bigg,
            "%s at %s: [%s] - %s%s, [%s] - %s%s %s'\r\n",
            weather_screen_control[current_screen].p_name_str,
            str_cur_time,
            weather_screen_control[current_screen].p_temp_key_str,
            str_temp,
            weather_screen_control[current_screen].p_temp_str,
            weather_screen_control[current_screen].p_wind_key_str,
            str_wind_speed,
            weather_screen_control[current_screen].p_wind_str,
            str_humidity);

// print_to_console(bigg);
}

/**********************************************************************************************************************
 * End of function reset_animation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: process_animation
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void process_animation (void)
{
    if (s_animation_frame != 0)
    {
        s_animation_frame--;
        if (s_animation_frame == 0)
        {
            first_chosen = -1;
        }
    }
}

/**********************************************************************************************************************
 * End of function process_animation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: inc_new_screen
 * Description  : .
 * Argument     : index
 * Return Value : .
 *********************************************************************************************************************/
static void inc_new_screen (int32_t * index)
{
// sprintf(s_print_buffer, "inc_new_screen index [%d]\r\n", (size_t)*index);
// print_to_console(s_print_buffer);

    (*index)++;

    if ((*index) > GI_WEATHER_ICON_SCREEN_MAX)
    {
        *index = 0;
    }

    current_weather_screen = GI_WEATHER_HONG_KONG + (*index);

    reset_transition();
    reset_animation();
}

/**********************************************************************************************************************
 * End of function inc_new_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: dec_new_screen
 * Description  : .
 * Argument     : index
 * Return Value : .
 *********************************************************************************************************************/
static void dec_new_screen (int32_t * index)
{
// sprintf(s_print_buffer, "dec_new_screen index [%d]\r\n", (size_t)*index);
// print_to_console(s_print_buffer);

    (*index)--;

    if ((*index) < 0)
    {
        *index = GI_WEATHER_ICON_SCREEN_MAX;
    }

    current_weather_screen = GI_WEATHER_HONG_KONG + (*index);

    reset_transition();
    reset_animation();
}

/**********************************************************************************************************************
 * End of function dec_new_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: draw_indicator
 * Description  : .
 * Argument     : scrn
 * Return Value : .
 *********************************************************************************************************************/
static void draw_indicator (int32_t scrn)
{
    /* Draw image position indicator circles */
    uint32_t indicator_width   = LCD_VPIX / 2;
    uint32_t indicator_spacing = indicator_width / (NUM_VIEWER_IMAGES - 1);

    if (first_call == true)
    {
        first_call = false;
        reset_animation();
    }

    d2_setalpha(d2_handle, 0xFF);
    for (uint32_t i = 0; i < NUM_VIEWER_IMAGES; i++)
    {
        uint32_t pos_x = (80);
        uint32_t pos_y = ((LCD_VPIX / 2) - (indicator_width / 2)) + (i * indicator_spacing);
        d2_rendercircle(d2_handle, (d2_point) (pos_x << 4), (d2_point) (pos_y) << 4, 7 << 4, 0);
    }

    // highlight the circle
    {
        uint32_t pos_indicator = ((LCD_VPIX / 2) - (indicator_width / 2)) + (scrn * indicator_spacing);
        d2_rendercircle(d2_handle, (d2_point) (80) << 4, (d2_point) (pos_indicator << 4), 11 << 4, 3 << 4);
    }
}

/**********************************************************************************************************************
 * End of function draw_indicator
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: decide_icon
 * Description  : .
 * Arguments    : region
 *              : str_cur_time
 *              : condition
 *              : snow
 *              : windy
 * Return Value : .
 *********************************************************************************************************************/
static uint32_t decide_icon (uint32_t            region,
                             /* time */ char_t * str_cur_time,
                             char              * condition,
                             bool_t              snow,
                             bool_t              windy)
{
    uint32_t decision = GI_ICON_SUN;

// char_t str_snow[4] = {};
// char_t str_wind[12] = {};

    bool_t selected = false;

    /* Ensure region is valid */
    region = region % NUM_ICONS_AVAILABLE;

    if ((selected == false) && (strstr(condition, "drizzle") != NULL))
    {
// print_to_console("Selecting rain icon\r\n");
        decision = GI_ICON_RAIN;
        selected = true;
    }

    if ((selected == false) && (strstr(condition, "rain possible") != NULL))
    {
// print_to_console("Selecting rain icon\r\n");
        decision = GI_ICON_RAIN;
        selected = true;
    }

    if ((selected == false) && (strstr(str_condition, "rain") != NULL))
    {
        // print_to_console("Selecting rain icon\r\n");
        decision = GI_ICON_RAIN;
        selected = true;
    }

    if ((selected == false) && (snow == true))
    {
        // print_to_console("Selecting snow icon\r\n");
        decision = GI_ICON_SNOW;
        selected = true;
    }

    if ((selected == false) && (windy == true))
    {
// print_to_console("Selecting wind icon\r\n");
        decision = GI_ICON_WIND;
        selected = true;
    }

    return decision;
}

/**********************************************************************************************************************
 * End of function decide_icon
 *********************************************************************************************************************/

volatile uint16_t v_icon_offset = 118;

/**********************************************************************************************************************
 * Function Name: do_weather_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_weather_screen (void)
{
    st_gimp_image_t * img           = NULL;
    static int32_t    chosen        = -1;
    static bool_t     new_selection = false;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    initialise_fg_font_22_full();

/*
 *  if (sdram_loaded == false)
 *  {
 *      sdram_loaded = true;
 *      uint32_t  num_images = NUM_WEATHER_IMAGE_ICONS;
 *      st_gimp_weather_image_t *image_tmp = NULL;
 *
 *      for (uint32_t current_image = 0; current_image < num_images; current_image++)
 *      {
 *          image_tmp =                 (st_gimp_weather_image_t*) get_sub_image_data(current_weather_screen, current_image);
 *          memcpy(&sdram_weather_images[current_image], image_tmp, sizeof (st_gimp_weather_image_t));
 *      }
 *  }
 */
    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(current_weather_screen);

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

        draw_indicator(current_screen);
        show_menu_icon();

        // Move graphics off-screen
        glcd_layer_change.layer.coordinate.x = FG_PANNEL_VERTICAL;
        glcd_layer_change.layer.coordinate.y = 950; // need to center horizontical;

        glcd_layer_change.input = g_display0.p_cfg->input[1];
        (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);

        /* draw text demo */
        st_gimp_fg_icon_image_t * simg = NULL;

        print_fg_font_22(static_x, static_y - 0, clear_line);  // clear line1
        print_fg_font_22(static_x, static_y - 0, line1);

        print_fg_font_22(static_x, static_y - 38, clear_line); // clear line2
        print_fg_font_22(static_x, static_y - 38, line2);

        print_fg_font_22(static_x, static_y - 74, clear_line); // clear line2
        print_fg_font_22(static_x, static_y - 74, line3);

        simg = (st_gimp_fg_icon_image_t *) get_sub_image_data(GI_WEATHER_ICON_SCREEN, selected_icon);

        switch (selected_icon)
        {
            case GI_ICON_RAIN:
            {
                v_icon_offset = 90;
                break;
            }

            case GI_ICON_SNOW:
            {
                v_icon_offset = 80;
                break;
            }

            case GI_ICON_SUN:
            {
                v_icon_offset = 80;
                break;
            }

            case GI_ICON_WIND:
            {
                v_icon_offset = 106;
                break;
            }

            default:
        }

        icon_erase.bytes_per_pixel = 4;
        icon_erase.height          = 126;
        icon_erase.width           = 113;

        memset(icon_erase.pixel_data, 0, icon_erase.width * icon_erase.height * icon_erase.bytes_per_pixel);

        draw_with_alpha(&icon_erase, 380, 80);
        draw_with_alpha((simg), 380, v_icon_offset); // From bottom left of layer to bottom left of image
    }
    else
    {
        select_detection_box(&chosen);

        // detect quit demo screen
        if (process_main_menu())
        {
            current_screen         = 0;
            new_selection          = false;
            first_chosen           = -1;
            chosen                 = -1;
            current_weather_screen = GI_WEATHER_HONG_KONG;

            /* Move graphics off-screen */
            glcd_layer_change.layer.coordinate.y = 950; // need to center horizontical;

            glcd_layer_change.input = g_display0.p_cfg->input[1];
            (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);

            /* This demo is closing so clear forground */
            uint8_t * buf_ptr = (uint8_t *) fb_foreground;
            display_input_cfg_t const * p_input = &g_display0.p_cfg->input[1]; // Layer 2

            memset(buf_ptr, 0, p_input->hstride * p_input->vsize * 4);
        }
        else
        {
            process_animation();

            /* Move graphics on-screen */
            glcd_layer_change.layer.coordinate.x = FG_PANNEL_VERTICAL;
            glcd_layer_change.layer.coordinate.y = 250; // need to center horizontical;

            glcd_layer_change.input = g_display0.p_cfg->input[1];
            (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);
        }

        if ((chosen >= 0) && (first_chosen != chosen))
        {
            new_selection = true;
            first_chosen  = chosen;
            first_call    = true;
        }

        if (new_selection)
        {
            new_selection = false;

            // sprintf(s_print_buffer, "do_weather_screen Direction [%s]\r\n", touches[first_chosen]);
            // print_to_console(s_print_buffer);

            switch (first_chosen)
            {
                case SWIPE_RIGHT:
                {
                    inc_new_screen(&current_screen);
                    break;
                }

                case SWIPE_LEFT:
                {
                    dec_new_screen(&current_screen);
                    break;
                }

                default:
                {
                }
            }
        }
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_weather_screen
 *********************************************************************************************************************/
