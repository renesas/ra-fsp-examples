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
 * File Name    : led_screen_demo.c
 * Version      : .
 * Description  : The led demo screen display.
 *********************************************************************************************************************/

#include <math.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
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

#include "hal_data.h"
#include "dsi_layer.h"

#include "camera_layer.h"
#include "graphics\graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "touch_gt911.h"
#include "gimp.h"

#define MAX_SPEED_REFRESH                   (1)
#define REFRESH_RATE                        (8)
#define RADIANS_CONSTANT                    (0.0174533) /* Convert degrees into radians */
#define BRIGHTNESS_SCALE_FACTOR_CONSTANT    (0.38759)   // Scale factor to convert brightness angel from 0 -100
#define BRIGHTNESS_SCALE_FACTOR_CONSTANT    (0.38759)   /* Scale factor to convert brightness angel from 0 -100 */

#define USE_SDRAM_IMAGE_SOURCES             (1)

extern void     reset_transition(void);
extern bool_t   in_transition(void);
extern void     show_menu_icon(void);
extern bool_t   process_main_menu(void);
extern uint32_t get_image_data(st_image_data_t ref);
extern uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);

extern touch_data_t g_touch_data;

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

#define USE_BACKGROUND_IMAGE

/* The following defines configure the 6 detection boxes for this screen */
#define BLINKING_INDEX      (0)
#define BRIGHTNESS_INDEX    (1)

#define BL_WIDTH_SIZE       (230)
#define BL_HEIGHT_SIZE      (200)

#define HB_WIDTH_SIZE       (240)
#define HB_HEIGHT_SIZE      (75)

// LOSE LOG SCALE TO SET BLINKING RATE
#define BLINK_RATE_10       60000000   /* SLOWEST */
#define BLINK_RATE_20       57300000
#define BLINK_RATE_30       54600000
#define BLINK_RATE_40       51900000
#define BLINK_RATE_50       48120000
#define BLINK_RATE_60       43800000
#define BLINK_RATE_70       38400000
#define BLINK_RATE_80       31920000
#define BLINK_RATE_90       22200000
#define BLINK_RATE_100      6000000    /* FASTEST */

static uint32_t s_blinking_rate_map[] =
{
    BLINK_RATE_10,
    BLINK_RATE_20,
    BLINK_RATE_30,
    BLINK_RATE_40,
    BLINK_RATE_50,
    BLINK_RATE_60,
    BLINK_RATE_70,
    BLINK_RATE_80,
    BLINK_RATE_90,
    BLINK_RATE_100,
    BLINK_RATE_100
};

static double xf = 420.0;
static double yf = 420.0;

static double xef = 0.0;
static double yef = 0.0;

static double xcos = 15.0;
static double ysin = 15.0;

static double angle_radians = 15.0;

static double inner_length = 40.0;
static double outer_length = 100.0;

typedef enum e_led_screen_indictor
{
    LED_SCREEN_RED,
    LED_SCREEN_GREEN,
    LED_SCREEN_BLUE,
} led_screen_indictor_t;

typedef struct
{
    led_screen_indictor_t id;
    st_gimp_led_image_t * simg;        // sub_image_location

    // bool blink state
    // blink tmr value

    double   blink_center_x;           // blink indicator needle ceneter point x
    double   blink_center_y;           // blink indicator needle ceneter point y
    d2_width blink_dest_pos_x;         // blink image destination position x
    d2_width blink_dest_pos_y;         // blink image destination position y
    d2_width blink_position;           // blink indicator position in radians
    d2_width blink_as_percent;         // Blink indicator as % for driving pins
    d2_width blink_as_log_index;       // Blink indicator as scale 0 -> 10 for driving pins
    d2_width brightness_dest_pos_x;    // brightness image destination position x
    d2_width brightness_dest_pos_y;    // brightness image destination position y
    d2_width brightness_position;      // brightness indicator position in screen pixels
    d2_width brightness_as_percent;    // brightness indicator as % for driving pins
    d2_width brightness_as_log_index;  // Blink indicator as scale 0 -> 10 for driving pins
} st_led_screen_indicator_def_t;

st_led_screen_indicator_def_t led_control[] =
{
    {LED_SCREEN_RED,   NULL,   158.0,   288.0, 65, 38,  0,  85,  0, 74,  132,  0,  10, 0}, // RED
    {LED_SCREEN_GREEN, NULL,   428.0,   288.0, 65, 308, 0,  85,  0, 344, 132,  0,  10, 0}, // GREEN
    {LED_SCREEN_BLUE,  NULL,   698.0,   288.0, 65, 578, 0,  85,  0, 614, 132,  0,  10, 0}, // BLUE
};

typedef struct st_led_loc_def
{
    d2_point x_vert;
    d2_point y_hor;
} st_led_loc_def_t;

typedef struct st_led_hit_group_def
{
    st_led_loc_def_t red;
    st_led_loc_def_t green;
    st_led_loc_def_t blue;
} st_led_hit_group_def_t;

static const st_led_loc_def_t hb_limits[2] =
{
    // Blinking BLINKING_INDEX
    {BL_HEIGHT_SIZE, BL_WIDTH_SIZE},

    // Brightness BRIGHTNESS_INDEX
    {HB_HEIGHT_SIZE, HB_WIDTH_SIZE},
};

static const st_led_hit_group_def_t led_hb[2] =
{
    /* Blinking BLINKING_INDEX */
    {
        {308 - (BL_HEIGHT_SIZE / 2), 160 - (BL_WIDTH_SIZE / 2)},
        {308 - (BL_HEIGHT_SIZE / 2), 430 - (BL_WIDTH_SIZE / 2)},
        {308 - (BL_HEIGHT_SIZE / 2), 700 - (BL_WIDTH_SIZE / 2)},
    },

    /* Brightness BRIGHTNESS_INDEX */
    {
        {138 - (HB_HEIGHT_SIZE / 2), 152 - (HB_WIDTH_SIZE / 2)},
        {138 - (HB_HEIGHT_SIZE / 2), 422 - (HB_WIDTH_SIZE / 2)},
        {138 - (HB_HEIGHT_SIZE / 2), 692 - (HB_WIDTH_SIZE / 2)},
    }
};

void do_led_screen(void);
void disable_led_operation(void);
void enable_led_operation(void);

static void process_blink_hit_point(led_screen_indictor_t active, touch_coord_t point_on_screen);
static void draw_rate_indicaor(led_screen_indictor_t active);
static void draw_brightness_indicaor(led_screen_indictor_t active);

static void show_detection_boxes(void);

static uint8_t select_detection_box(void);

static d2_point brightness_offset = 0;

static st_gimp_led_image_t sdram_images[3] BSP_PLACE_IN_SECTION(".sdram") = {};

/**********************************************************************************************************************
 * Function Name: disable_led_operation
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void disable_led_operation (void)
{
    TURN_GREEN_OFF;
    TURN_RED_OFF;
}

/**********************************************************************************************************************
 * End of function disable_led_operation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: enable_led_operation
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void enable_led_operation (void)
{
    SYSTEM_OK;
    TURN_RED_OFF;
}

/**********************************************************************************************************************
 * End of function enable_led_operation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: process_blink_hit_point
 * Description  : .
 * Arguments    : active
 *              : point_on_screen
 * Return Value : .
 *********************************************************************************************************************/
static void process_blink_hit_point (led_screen_indictor_t active, touch_coord_t point_on_screen)
{
    st_led_loc_def_t g_red_rate_on_screen_cp = {};
    st_led_loc_def_t g_red_pos_on_screen_cp  = {};

    g_red_rate_on_screen_cp.x_vert = (d2_point) led_control[active].blink_center_x;
    g_red_rate_on_screen_cp.y_hor  = (d2_point) led_control[active].blink_center_y;

    g_red_pos_on_screen_cp.x_vert = (d2_point) point_on_screen.x;
    g_red_pos_on_screen_cp.y_hor  = (d2_point) point_on_screen.y;

    /* Need to generate vector from two points cp & center */
    volatile double x, y, result;

    x = (double) (g_red_rate_on_screen_cp.y_hor - g_red_pos_on_screen_cp.y_hor);
    y = (double) (g_red_rate_on_screen_cp.x_vert - g_red_pos_on_screen_cp.x_vert);

    result = atan2(y, x);
    result = (result * 180.0) / PI;

    d2_point as_percent = led_control[active].blink_as_percent;
    d2_point rend_ang   = (d2_point) result;

    rend_ang = 360 - rend_ang;
    rend_ang = rend_ang + 270;
    rend_ang = rend_ang % 360;

    /* Angle is between slow and fast */
    if ((rend_ang > 320) || (rend_ang < 222))
    {
        as_percent = rend_ang + 90;
        as_percent = as_percent - 52;
        as_percent = as_percent % 360;
        as_percent = 260 - as_percent;
        if (as_percent < 0)
        {
            as_percent = 100;
        }

        as_percent = (d2_point) (as_percent * 0.38759); // Scale to -100

        /* Store a valid position */
        vTaskDelay(10);

        led_control[active].blink_position     = rend_ang;
        led_control[active].blink_as_percent   = as_percent;
        led_control[active].blink_as_log_index = as_percent % 10;

        vTaskDelay(10);

        // R_GPT_Start (g_pwm_pins[TMR_DEF_LED_UX_BLINK_RED].p_timer->p_ctrl);

        /*
         * sprintf(s_print_buffer, "[%03d degrees] [%03d 0-100]", rend_ang, as_percent);
         * //print_to_console(s_print_buffer);
         */
    }

/*    For reference draw raw line between center of Blinkging grahpic & touchpoint */

// d2_setcolor(d2_handle, 0, 0xf7f7f7);
// d2_renderline(d2_handle, (((d2_point) g_red_rate_on_screen_cp.y_hor) << 4), (((d2_point) g_red_rate_on_screen_cp.x_vert) << 4), (((d2_point) g_red_pos_on_screen_cp.y_hor) << 4), (((d2_point) g_red_pos_on_screen_cp.x_vert) << 4), 7 << 4, 0);
}

/**********************************************************************************************************************
 * End of function process_blink_hit_point
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: draw_rate_indicaor
 * Description  : .
 * Argument     : active
 * Return Value : .
 *********************************************************************************************************************/
static void draw_rate_indicaor (led_screen_indictor_t active)
{
    // Coloured arc led frequency position
    d2_setcolor(d2_handle, 0, 0xf7f7f7);

    xf = led_control[active].blink_center_x;
    yf = led_control[active].blink_center_y;

    angle_radians = (led_control[active].blink_position * RADIANS_CONSTANT);

    /* Calculate line angle */
    xcos = cos(angle_radians);
    ysin = sin(angle_radians);

    /* Calculate Line co-ordinates */
    xef = xf + (xcos * inner_length);
    yef = yf + (ysin * inner_length);

    /* Line form outer edge  to inner edge mode */
    xf = xf + (xcos * outer_length);
    yf = yf + (ysin * outer_length);

    d2_renderline(d2_handle, (((d2_point) yf) << 4), (((d2_point) xf) << 4), (((d2_point) yef) << 4),
                  (((d2_point) xef) << 4), 7 << 4, 0);
}

/**********************************************************************************************************************
 * End of function draw_rate_indicaor
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: draw_brightness_indicaor
 * Description  : .
 * Argument     : active
 * Return Value : .
 *********************************************************************************************************************/
static void draw_brightness_indicaor (led_screen_indictor_t active)
{
    /* Coloured arc led brightness position */
    d2_setcolor(d2_handle, 0, 0xf7f7f7);

    brightness_offset = led_control[active].brightness_position;

    d2_point start_x = led_control[active].brightness_dest_pos_x + brightness_offset;
    d2_point start_y = led_control[active].brightness_dest_pos_y + 0;

    d2_point pos_x = led_control[active].brightness_dest_pos_x + brightness_offset;
    d2_point pos_y = led_control[active].brightness_dest_pos_y + 40;

    d2_renderline(d2_handle, (((d2_point) start_y) << 4), (((d2_point) start_x) << 4), (((d2_point) pos_y) << 4),
                  (((d2_point) pos_x) << 4), 7 << 4, 0);
}

/**********************************************************************************************************************
 * End of function draw_brightness_indicaor
 *********************************************************************************************************************/

#define GPT_EXAMPLE_MSEC_PER_SEC           (1000)
#define GPT_EXAMPLE_DESIRED_PERIOD_MSEC    (20)

double fw1 = 0.4166;
double fw2 = 0.7291;

/**********************************************************************************************************************
 * Function Name: select_detection_box
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static uint8_t select_detection_box (void)
{
    if (g_touch_data.num_points > 0)
    {
        volatile touch_coord_t cp;
        volatile double        brightness_indication = 0.0;
        bool_t                 touch_processed       = false;

        /* Store cp locally so it won't change during processing */
        cp.x = g_touch_data.point[0].x;
        cp.y = g_touch_data.point[0].y;

        sprintf(s_print_buffer, "Processing detection point: %d,%d - ", cp.x, cp.y);

////print_to_console(s_print_buffer);

        for (uint8_t group = BLINKING_INDEX; group <= BRIGHTNESS_INDEX; group++)
        {
            if (
                ((cp.x > (led_hb[group].red.y_hor)) && (cp.x < (led_hb[group].red.y_hor + hb_limits[group].y_hor))) && // horizontal on screen
                ((cp.y > (led_hb[group].red.x_vert)) && (cp.y < (led_hb[group].red.x_vert + hb_limits[group].x_vert))) // verticle on screen
                )
            {
                touch_processed = true;

                if (group == BLINKING_INDEX)
                {
                    process_blink_hit_point(LED_SCREEN_RED, cp);

                    sprintf(s_print_buffer, "RED BLINKING_INDEX [%d] : [%lu]\r\n",
                            (led_control[LED_SCREEN_RED].blink_as_percent / 10),
                            s_blinking_rate_map[(led_control[LED_SCREEN_RED].blink_as_percent / 10)]);

                    // print_to_console(s_print_buffer);

                    R_GPT_PeriodSet(g_led_scr_red_blink.p_ctrl,
                                    s_blinking_rate_map[(led_control[LED_SCREEN_RED].blink_as_percent / 10)]);
                }

                if (group == BRIGHTNESS_INDEX)
                {
// BROKEN                     process_brightness_hit_point(LED_SCREEN_RED, cp);

                    /* Calculate for % */
                    brightness_indication = (double) ((cp.x - led_hb[group].red.y_hor) * 0.4166) + 1.0;
                    brightness_offset     = (d2_point) brightness_indication;
                    led_control[LED_SCREEN_RED].brightness_as_percent = brightness_offset;

                    sprintf(s_print_buffer, "RED BRIGHTNESS_INDEX Detected [%d%%] :", brightness_offset);

                    // print_to_console(s_print_buffer);

                    /* Calculate for frame number */
                    brightness_indication = (double) ((cp.x - led_hb[group].red.y_hor) * 0.7291) + 1.0;
                    brightness_offset     = led_control[LED_SCREEN_RED].brightness_position;
                    brightness_offset     = (d2_point) brightness_indication;
                    led_control[LED_SCREEN_RED].brightness_position = brightness_offset;
                }
            }
            else if (
                ((cp.x > (led_hb[group].green.y_hor)) &&
                 (cp.x < (led_hb[group].green.y_hor + hb_limits[group].y_hor))) && // horizontal on screen
                ((cp.y > (led_hb[group].green.x_vert)) &&
                 (cp.y < (led_hb[group].green.x_vert + hb_limits[group].x_vert)))  // verticle on screen
                )
            {
                touch_processed = true;

                if (group == BLINKING_INDEX)
                {
                    process_blink_hit_point(LED_SCREEN_GREEN, cp);

                    sprintf(s_print_buffer, "GREEN BLINKING_INDEX [%d] : [%lu]\r\n",
                            (led_control[LED_SCREEN_GREEN].blink_as_percent / 10),
                            s_blinking_rate_map[(led_control[LED_SCREEN_GREEN].blink_as_percent / 10)]);

                    // print_to_console(s_print_buffer);

                    R_GPT_PeriodSet(g_led_scr_green_blink.p_ctrl,
                                    s_blinking_rate_map[(led_control[LED_SCREEN_GREEN].blink_as_percent / 10)]);
                }

                if (group == BRIGHTNESS_INDEX)
                {
                    /* BROKEN process_brightness_hit_point(LED_SCREEN_GREEN, cp); */
                    /* Calculate for % */
                    brightness_indication = (double) ((cp.x - led_hb[group].green.y_hor) * 0.4166) + 1.0;
                    brightness_offset     = (d2_point) brightness_indication;
                    led_control[LED_SCREEN_GREEN].brightness_as_percent = brightness_offset;

                    sprintf(s_print_buffer, "GREEN BRIGHTNESS_INDEX Detected [%d%%] :", brightness_offset);

                    // print_to_console(s_print_buffer);

                    /* Calculate for frame number */
                    brightness_indication = (double) ((cp.x - led_hb[group].green.y_hor) * 0.7291) + 1.0;
                    brightness_offset     = led_control[LED_SCREEN_GREEN].brightness_position;
                    brightness_offset     = (d2_point) brightness_indication;
                    led_control[LED_SCREEN_GREEN].brightness_position = brightness_offset;
                }
            }
            else if (
                ((cp.x > (led_hb[group].blue.y_hor)) && (cp.x < (led_hb[group].blue.y_hor + hb_limits[group].y_hor))) && // horizontal on screen
                ((cp.y > (led_hb[group].blue.x_vert)) && (cp.y < (led_hb[group].blue.x_vert + hb_limits[group].x_vert))) // verticle on screen
                )
            {
                touch_processed = true;

                if (group == BLINKING_INDEX)
                {
                    process_blink_hit_point(LED_SCREEN_BLUE, cp);

                    sprintf(s_print_buffer, "BLUE BLINKING_INDEX [%d] : [%lu]\r\n",
                            (led_control[LED_SCREEN_BLUE].blink_as_percent / 10),
                            s_blinking_rate_map[(led_control[LED_SCREEN_BLUE].blink_as_percent / 10)]);

                    // print_to_console(s_print_buffer);

                    R_GPT_PeriodSet(g_led_scr_blue_blink.p_ctrl,
                                    s_blinking_rate_map[(led_control[LED_SCREEN_BLUE].blink_as_percent / 10)]);
                }

                if (group == BRIGHTNESS_INDEX)
                {
                    /* Calculate for % */
                    brightness_indication = (double) ((cp.x - led_hb[group].blue.y_hor) * fw1);
                    brightness_offset     = (d2_point) brightness_indication;
                    led_control[LED_SCREEN_BLUE].brightness_as_percent = brightness_offset;

                    /* Calculate for frame number */
                    brightness_indication = (double) ((cp.x - led_hb[group].blue.y_hor) * fw2);
                    brightness_offset     = led_control[LED_SCREEN_BLUE].brightness_position;
                    brightness_offset     = (d2_point) brightness_indication;
                    led_control[LED_SCREEN_BLUE].brightness_position = brightness_offset;
                }
            }
        }

        if (false == touch_processed)
        {
            // print_to_console("Out of Bounds");
        }

////print_to_console("\r\n");
    }

    return 0;
}

/**********************************************************************************************************************
 * End of function select_detection_box
 *********************************************************************************************************************/

/*
 * RENDER ORDER
 * BACKGROUND
 *     !
 *     !
 *     V
 * FOREGROUND
 */

led_screen_indictor_t active          = LED_SCREEN_RED;
uint32_t              active_buffer   = 0;
bool_t                led_screen_open = false;
extern void      clear_touch(void);
extern fsp_err_t gpt_initialize(void);

void start_led_demo(void);

/**********************************************************************************************************************
 * Function Name: start_led_demo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void start_led_demo (void)
{
    R_GPT_Stop(g_pwm_pins[0].p_timer->p_ctrl);
    R_GPT_Stop(g_blinker.p_ctrl);

    R_GPT_Open(g_led_scr_red_blink.p_ctrl, g_led_scr_red_blink.p_cfg);
    R_GPT_Open(g_led_scr_green_blink.p_ctrl, g_led_scr_green_blink.p_cfg);
    R_GPT_Open(g_led_scr_blue_blink.p_ctrl, g_led_scr_blue_blink.p_cfg);

    R_GPT_PeriodSet(g_led_scr_red_blink.p_ctrl,
                    s_blinking_rate_map[(led_control[LED_SCREEN_RED].blink_as_percent / 10)]);
    R_GPT_PeriodSet(g_led_scr_green_blink.p_ctrl,
                    s_blinking_rate_map[(led_control[LED_SCREEN_GREEN].blink_as_percent / 10)]);
    R_GPT_PeriodSet(g_led_scr_blue_blink.p_ctrl,
                    s_blinking_rate_map[(led_control[LED_SCREEN_BLUE].blink_as_percent / 10)]);

    R_GPT_Start(g_pwm_pins[1].p_timer->p_ctrl);
    R_GPT_Start(g_pwm_pins[2].p_timer->p_ctrl);
    R_GPT_Start(g_pwm_pins[3].p_timer->p_ctrl);

    R_GPT_Start(g_led_scr_red_blink.p_ctrl);
    R_GPT_Start(g_led_scr_green_blink.p_ctrl);
    R_GPT_Start(g_led_scr_blue_blink.p_ctrl);

    clear_touch();
    led_screen_open = true;
}

/**********************************************************************************************************************
 * End of function start_led_demo
 *********************************************************************************************************************/

void stop_led_demo(void);

/**********************************************************************************************************************
 * Function Name: stop_led_demo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void stop_led_demo (void)
{
    led_screen_open = false;

    R_GPT_Stop(g_led_scr_red_blink.p_ctrl);
    R_GPT_Stop(g_led_scr_green_blink.p_ctrl);
    R_GPT_Stop(g_led_scr_blue_blink.p_ctrl);

    R_GPT_Stop(g_pwm_pins[1].p_timer->p_ctrl);
    R_GPT_Stop(g_pwm_pins[2].p_timer->p_ctrl);
    R_GPT_Stop(g_pwm_pins[3].p_timer->p_ctrl);

    R_GPT_Reset(g_led_scr_red_blink.p_ctrl);
    R_GPT_Reset(g_led_scr_green_blink.p_ctrl);
    R_GPT_Reset(g_led_scr_blue_blink.p_ctrl);

    R_GPT_Reset(g_pwm_pins[0].p_timer->p_ctrl);
    R_GPT_Reset(g_pwm_pins[1].p_timer->p_ctrl);
    R_GPT_Reset(g_pwm_pins[2].p_timer->p_ctrl);
    R_GPT_Reset(g_pwm_pins[3].p_timer->p_ctrl);

    R_GPT_Start(g_blinker.p_ctrl);

    R_BSP_PinAccessEnable();

    R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_00,
                    ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_GPT1));

    R_BSP_PinAccessDisable();

    enable_led_operation();
}

/**********************************************************************************************************************
 * End of function stop_led_demo
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: do_led_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_led_screen (void)
{
    st_gimp_image_t     * img  = NULL;
    st_gimp_led_image_t * simg = NULL;

    if (led_screen_open == false)
    {
        sprintf(s_print_buffer, "Initial RED BLINKING_INDEX [%d] : [%lu]\r\n",
                (led_control[LED_SCREEN_RED].blink_as_percent / 10),
                s_blinking_rate_map[(led_control[LED_SCREEN_RED].blink_as_percent / 10)]);

        // print_to_console(s_print_buffer);

        R_GPT_PeriodSet(g_led_scr_red_blink.p_ctrl,
                        s_blinking_rate_map[(led_control[LED_SCREEN_RED].blink_as_percent / 10)]);
        R_GPT_Start(g_led_scr_red_blink.p_ctrl);

        sprintf(s_print_buffer, "Initial GREEN BLINKING_INDEX [%d] : [%lu]\r\n",
                (led_control[LED_SCREEN_GREEN].blink_as_percent / 10),
                s_blinking_rate_map[(led_control[LED_SCREEN_GREEN].blink_as_percent / 10)]);

        // print_to_console(s_print_buffer);

        R_GPT_PeriodSet(g_led_scr_red_blink.p_ctrl,
                        s_blinking_rate_map[(led_control[LED_SCREEN_RED].blink_as_percent / 10)]);
        R_GPT_Start(g_led_scr_red_blink.p_ctrl);

        sprintf(s_print_buffer, "Initial BLUE BLINKING_INDEX [%d] : [%lu]\r\n",
                (led_control[LED_SCREEN_BLUE].blink_as_percent / 10),
                s_blinking_rate_map[(led_control[LED_SCREEN_BLUE].blink_as_percent / 10)]);

        // print_to_console(s_print_buffer);

        sprintf(s_print_buffer, "Initial RED BRIGHTNESS : [%d]\r\n", led_control[LED_SCREEN_RED].brightness_as_percent);

        // print_to_console(s_print_buffer);

        sprintf(s_print_buffer,
                "Initial GREEN BRIGHTNESS : [%d]\r\n",
                led_control[LED_SCREEN_GREEN].brightness_as_percent);

        // print_to_console(s_print_buffer);

        sprintf(s_print_buffer, "Initial BLUE BRIGHTNESS : [%d]\r\n",
                led_control[LED_SCREEN_BLUE].brightness_as_percent);

        // print_to_console(s_print_buffer);

        start_led_demo();
    }

    /* process control updates for next frame */
    select_detection_box();

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_LED_GRAPHICS_SCREEN_2);
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

        uint32_t              active_sub_image = 0;
        st_gimp_led_image_t * image_tmp        = NULL;

#if USE_SDRAM_IMAGE_SOURCES

/* load from ospi and store in SDRAM */
        image_tmp = (st_gimp_led_image_t *) get_sub_image_data(GI_LED_GRAPHICS_SCREEN, active_sub_image);
        memcpy(&sdram_images[active_sub_image], image_tmp, sizeof(st_gimp_led_image_t));
        led_control[active_sub_image].simg = &sdram_images[active_sub_image];
        active_sub_image++;

        image_tmp = (st_gimp_led_image_t *) get_sub_image_data(GI_LED_GRAPHICS_SCREEN, active_sub_image);
        memcpy(&sdram_images[active_sub_image], image_tmp, sizeof(st_gimp_led_image_t));
        led_control[active_sub_image].simg = &sdram_images[active_sub_image];
        active_sub_image++;

        image_tmp = (st_gimp_led_image_t *) get_sub_image_data(GI_LED_GRAPHICS_SCREEN, active_sub_image);
        memcpy(&sdram_images[active_sub_image], image_tmp, sizeof(st_gimp_led_image_t));
        led_control[active_sub_image].simg = &sdram_images[active_sub_image];
        active_sub_image++;
#else

/* Load from ospi read from ospi */
        led_control[active_sub_image].simg = (st_gimp_led_image_t *) get_sub_image_data(GI_LED_GRAPHICS_SCREEN,
                                                                                        active_sub_image);
        active_sub_image++;
        active_sub_image++;
        led_control[active_sub_image].simg = (st_gimp_led_image_t *) get_sub_image_data(GI_LED_GRAPHICS_SCREEN,
                                                                                        active_sub_image);
        active_sub_image++;
        led_control[active_sub_image].simg = (st_gimp_led_image_t *) get_sub_image_data(GI_LED_GRAPHICS_SCREEN,
                                                                                        active_sub_image);
        active_sub_image++;
#endif                                 // USE_SDRAM_IMAGE_SOURCES

        /* process control updates for next frame */
        for (led_screen_indictor_t active_led = LED_SCREEN_RED; active_led <= LED_SCREEN_BLUE; active_led++)
        {
            simg = led_control[active_led].simg;

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
                        0,                                                            // Source position
                        (d2_width) ((simg->width) << 4),
                        (d2_width) ((simg->height) << 4),                             // Destination width/height
                        (d2_width) ((led_control[active_led].blink_dest_pos_x) << 4),
                        (d2_width) ((led_control[active_led].blink_dest_pos_y) << 4), // Destination position
                        d2_tm_filter);
        }

        show_menu_icon();

        /* process control updates for next frame */

// select_detection_box();
    }
    else
    {
        /* process control updates for next frame */
        for (led_screen_indictor_t active_led = LED_SCREEN_RED; active_led <= LED_SCREEN_BLUE; active_led++)
        {
            simg = led_control[active_led].simg;

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
                        0,                                                            // Source position
                        (d2_width) ((simg->width) << 4),
                        (d2_width) ((simg->height) << 4),                             // Destination width/height
                        (d2_width) ((led_control[active_led].blink_dest_pos_x) << 4),
                        (d2_width) ((led_control[active_led].blink_dest_pos_y) << 4), // Destination position
                        d2_tm_filter);
        }

        /* Debug only (disabled in function) */
        show_detection_boxes();

        /* Process touch point checking tp against hit oboxes, Out of bounds if not hit box corresponds to tp */

// select_detection_box();

        /* Update LCD based upon touch events */
        draw_rate_indicaor(LED_SCREEN_RED);
        draw_rate_indicaor(LED_SCREEN_GREEN);
        draw_rate_indicaor(LED_SCREEN_BLUE);

        draw_brightness_indicaor(LED_SCREEN_RED);
        draw_brightness_indicaor(LED_SCREEN_GREEN);
        draw_brightness_indicaor(LED_SCREEN_BLUE);
    }

    show_menu_icon();
    if (process_main_menu() == true)
    {
        stop_led_demo();
    }

    /* Reset alpha in case it was changed above */
    d2_setalpha(d2_handle, 0xFF);

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_led_screen
 *********************************************************************************************************************/

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

    /* RATE RGB */
    d2_setcolor(d2_handle, 0, 0x8f0000);
    d2_renderquad(d2_handle, (d2_point) (led_hb[BLINKING_INDEX].red.x_vert << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].red.y_hor) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].red.x_vert + BL_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].red.y_hor) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].red.x_vert + BL_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].red.y_hor + BL_WIDTH_SIZE) << 4),
                  (d2_point) (led_hb[BLINKING_INDEX].red.x_vert << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].red.y_hor + BL_WIDTH_SIZE) << 4), 0);

    d2_setcolor(d2_handle, 0, 0x008f00);
    d2_renderquad(d2_handle, (d2_point) (led_hb[BLINKING_INDEX].green.x_vert << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].green.y_hor) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].green.x_vert + BL_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].green.y_hor) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].green.x_vert + BL_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].green.y_hor + BL_WIDTH_SIZE) << 4),
                  (d2_point) (led_hb[BLINKING_INDEX].green.x_vert << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].green.y_hor + BL_WIDTH_SIZE) << 4), 0);

    d2_setcolor(d2_handle, 0, 0x00008f);
    d2_renderquad(d2_handle, (d2_point) (led_hb[BLINKING_INDEX].blue.x_vert << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].blue.y_hor) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].blue.x_vert + BL_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].blue.y_hor) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].blue.x_vert + BL_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].blue.y_hor + BL_WIDTH_SIZE) << 4),
                  (d2_point) (led_hb[BLINKING_INDEX].blue.x_vert << 4),
                  (d2_point) ((led_hb[BLINKING_INDEX].blue.y_hor + BL_WIDTH_SIZE) << 4), 0);

    /* Brightness RGB */
    d2_setcolor(d2_handle, 0, 0x8f0000);
    d2_renderquad(d2_handle, (d2_point) (led_hb[BRIGHTNESS_INDEX].red.x_vert << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].red.y_hor) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].red.x_vert + HB_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].red.y_hor) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].red.x_vert + HB_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].red.y_hor + HB_WIDTH_SIZE) << 4),
                  (d2_point) (led_hb[BRIGHTNESS_INDEX].red.x_vert << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].red.y_hor + HB_WIDTH_SIZE) << 4), 0);

    d2_setcolor(d2_handle, 0, 0x008f00);
    d2_renderquad(d2_handle, (d2_point) (led_hb[BRIGHTNESS_INDEX].green.x_vert << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].green.y_hor) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].green.x_vert + HB_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].green.y_hor) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].green.x_vert + HB_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].green.y_hor + HB_WIDTH_SIZE) << 4),
                  (d2_point) (led_hb[BRIGHTNESS_INDEX].green.x_vert << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].green.y_hor + HB_WIDTH_SIZE) << 4), 0);

    d2_setcolor(d2_handle, 0, 0x00008f);
    d2_renderquad(d2_handle, (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.x_vert + 0) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.y_hor) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.x_vert + 0 + HB_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.y_hor) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.x_vert + 0 + HB_HEIGHT_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.y_hor + HB_WIDTH_SIZE) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.x_vert + 0) << 4),
                  (d2_point) ((led_hb[BRIGHTNESS_INDEX].blue.y_hor + HB_WIDTH_SIZE) << 4), 0);
}

/**********************************************************************************************************************
 * End of function show_detection_boxes
 *********************************************************************************************************************/

static volatile uint32_t s_redled_flashing = OFF;
static volatile d2_width s_redled_intense  = 0;
static volatile d2_width s_redled_duty     = 99;

static volatile uint32_t s_greenled_flashing = OFF;
static volatile d2_width s_greenled_intense  = 0;
static volatile d2_width s_greenled_duty     = 99;

static volatile uint32_t s_blueled_flashing = OFF;
static volatile d2_width s_blueled_intense  = 0;
static volatile d2_width s_blueled_duty     = 99;

/**********************************************************************************************************************
 * Function Name: led_scr_red_blink_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void led_scr_red_blink_cb (timer_callback_args_t * p_args)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(p_args);

    if (OFF == s_redled_flashing)
    {
        s_redled_flashing = ON;
    }
    else
    {
        s_redled_flashing = OFF;
    }
}

/**********************************************************************************************************************
 * End of function led_scr_red_blink_cb
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_scr_green_blink_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void led_scr_green_blink_cb (timer_callback_args_t * p_args)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(p_args);

    if (OFF == s_greenled_flashing)
    {
        s_greenled_flashing = ON;
    }
    else
    {
        s_greenled_flashing = OFF;
    }
}

/**********************************************************************************************************************
 * End of function led_scr_green_blink_cb
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_scr_blue_blink_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void led_scr_blue_blink_cb (timer_callback_args_t * p_args)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(p_args);

    if (OFF == s_blueled_flashing)
    {
        s_blueled_flashing = ON;
    }
    else
    {
        s_blueled_flashing = OFF;
    }
}

/**********************************************************************************************************************
 * End of function led_scr_blue_blink_cb
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_scr_red_bright_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void led_scr_red_bright_cb (timer_callback_args_t * p_args)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(p_args);

    switch (s_redled_flashing)
    {
        case ON:
        {
            if ((s_redled_intense++) < s_redled_duty)
            {
                TURN_RED_ON
            }
            else
            {
                TURN_RED_OFF
            }

            if (s_redled_intense >= 100)
            {
                s_redled_intense = 0;
                s_redled_duty    = led_control[LED_SCREEN_RED].brightness_as_percent;
            }

            break;
        }

        default:
        {
            TURN_RED_OFF
                s_redled_intense = 0;
            s_redled_duty = led_control[LED_SCREEN_RED].brightness_as_percent;
        }
    }
}

/**********************************************************************************************************************
 * End of function led_scr_red_bright_cb
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_scr_green_bright_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void led_scr_green_bright_cb (timer_callback_args_t * p_args)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(p_args);

    switch (s_greenled_flashing)
    {
        case ON:
        {
            if ((s_greenled_intense++) < s_greenled_duty)
            {
                TURN_GREEN_ON
            }
            else
            {
                TURN_GREEN_OFF
            }

            if (s_greenled_intense >= 100)
            {
                s_greenled_intense = 0;
                s_greenled_duty    = led_control[LED_SCREEN_GREEN].brightness_as_percent;
            }

            break;
        }

        default:
        {
            TURN_GREEN_OFF
                s_greenled_intense = 0;
            s_greenled_duty = led_control[LED_SCREEN_GREEN].brightness_as_percent;
        }
    }
}

/**********************************************************************************************************************
 * End of function led_scr_green_bright_cb
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_scr_blue_bright_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void led_scr_blue_bright_cb (timer_callback_args_t * p_args)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(p_args);

    switch (s_blueled_flashing)
    {
        case ON:
        {
            if ((s_blueled_intense++) < s_blueled_duty)
            {
                TURN_BLUE_ON
            }
            else
            {
                TURN_BLUE_OFF
            }

            if (s_blueled_intense >= 100)
            {
                s_blueled_intense = 0;
                s_blueled_duty    = led_control[LED_SCREEN_BLUE].brightness_as_percent;
            }

            break;
        }

        default:
        {
            TURN_BLUE_OFF
                s_blueled_intense = 0;
            s_blueled_duty = led_control[LED_SCREEN_BLUE].brightness_as_percent;
        }
    }
}

/**********************************************************************************************************************
 * End of function led_scr_blue_bright_cb
 *********************************************************************************************************************/
