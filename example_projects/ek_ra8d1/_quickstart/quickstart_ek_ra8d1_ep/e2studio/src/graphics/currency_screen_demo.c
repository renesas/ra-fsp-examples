/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER Escreen_offset_xpRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE Escreen_offset_xpRESSLY DISCLAIMED. TO THE MAXIMUM
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

#include "graphics\graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "touch_gt911.h"
#include "gimp.h"

#include "currency_table.h"

static st_gimp_currency_image_t gimp_spacer_text BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");

#define CURRENCY_HB_WIDTH_SIZE     (150)
#define CURRENCY_HB_HEIGHT_SIZE    (120)

extern bool_t   https_currency_update(void);
extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t   in_transition(void);

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

extern void   show_menu_icon();
extern bool_t process_main_menu(void);
extern void   reset_transition(void);

extern touch_data_t g_touch_data;

typedef enum e_currency_indictor
{
    LED_SCREEN_RED,
    LED_SCREEN_GREEN,
    LED_SCREEN_BLUE,
} e_currency_indictor_t;

typedef struct st_currency_screen_indicator_def
{
    e_currency_indictor_t id;
    char_t              * p_name_str;
    char_t              * p_sname_str;
    st_lcd_point_def_t    center;
    bool_t                in_use;      // blink indicator position in radians
} st_currency_screen_indicator_def_t;

st_currency_screen_indicator_def_t currency_screen_control[] =
{
    {0, "Austrailian Dollar",    "AUD",                     {31,  280  }, false},
    {1, "British Pound Sterlin", "GBP",                     {191, 280  }, false},
    {2, "Canadian Dollar",       "CAD",                     {352, 280  }, false},
    {3, "Chinese Yen",           "CNY",                     {514, 280  }, false},
    {4, "European Union Euro",   "EUR",                     {677, 280  }, false},
    {5, "Hong Kong Dollar",      "HKD",                     {31,  154  }, false},
    {6, "Indian Rupee",          "INR",                     {191, 154  }, false},
    {7, "Jananese Yen",          "JPY",                     {352, 154  }, false},
    {8, "Singaphorean Dollar",   "SGD",                     {514, 154  }, false},
    {9, "United States Dollar",  "USD",                     {677, 154  }, false},
};

// Simulated / live data if available
extern char_t * gp_currency_full_table[10];

st_lcd_point_def_t refpoint     = {89, 26};
bool_t             s_show_reset = false;
static void show_detection_boxes(void);
static void select_detection_box(int32_t * _chosen);
static void draw_with_alpha(st_gimp_countries_image_t * object, uint16_t hoffset, uint16_t voffset);

void do_currency_screen(void);
void reset_selection(void);

extern const st_gimp_cc_buttons_image_t cc_reset;

static uint32_t progress_state = 0;

static int32_t first_chosen  = -1;
static int32_t second_chosen = -1;

static display_runtime_cfg_t glcd_layer_change;

/**********************************************************************************************************************
 * Function Name: reset_selection
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void reset_selection (void)
{
    first_chosen  = -1;
    second_chosen = -1;

    progress_state = 0;
    s_show_reset   = false;

    /* clear screen */
    taskENTER_CRITICAL();
    display_input_cfg_t const * p_input = &g_display0.p_cfg->input[1]; // Layer 2
    uint8_t * buf_ptr = (uint8_t *) fb_foreground;

    /* ver basic clear screen */
    gimp_spacer_text.height          = 290;
    gimp_spacer_text.width           = 70;
    gimp_spacer_text.bytes_per_pixel = 4;
    memset(gimp_spacer_text.pixel_data, 0, ((50 * 405 * 4) + 1));

    // Set to off screen;

    glcd_layer_change.layer.coordinate.x = 80;
    glcd_layer_change.layer.coordinate.y = 900;
    glcd_layer_change.input              = g_display0.p_cfg->input[1];

    memset(buf_ptr, 0, p_input->hstride * p_input->vsize * 4);

    taskEXIT_CRITICAL();

    (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);
}

/**********************************************************************************************************************
 * End of function reset_selection
 *********************************************************************************************************************/

extern uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);

/**********************************************************************************************************************
 * Function Name: draw_with_alpha
 * Description  : .
 * Arguments    : object
 *              : hoffset
 *              : voffset
 * Return Value : .
 *********************************************************************************************************************/
static void draw_with_alpha (st_gimp_countries_image_t * object, uint16_t hoffset, uint16_t voffset)
{
    /* GLCDC Graphics Layer 1 size must be 182 x 200 */
    uint8_t * buf_ptr = (uint8_t *) fb_foreground;
    display_input_cfg_t const * p_input = &g_display0.p_cfg->input[1]; // Layer 2

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
 * Function Name: do_currency_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_currency_screen (void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        reset_selection();

        img = (st_gimp_image_t *) get_image_data(GI_CURRENCY_SCREEN);

        d2_setblitsrc(d2_handle,
                      img->pixel_data,
                      (d2_s32) img->width,
                      (d2_s32) img->width,
                      (d2_s32) img->height,
                      EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    (d2_s32) img->width,
                    (d2_s32) img->height,
                    (d2_blitpos) 0,
                    0,                               // Source position
                    (d2_width) ((img->width) << 4),
                    (d2_width) ((img->height) << 4), // Destination width/height
                    (d2_width) ((0) << 4),
                    (d2_width) ((0) << 4),           // Destination position
                    d2_tm_filter);

        img = (st_gimp_image_t *) get_sub_image_data(GI_CURRENCY_INSTRUCTIONS, GI_CURRENCY_LINE_0);

        d2_setblitsrc(d2_handle,
                      img->pixel_data,
                      (d2_s32) img->width,
                      (d2_s32) img->width,
                      (d2_s32) img->height,
                      EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    (d2_s32) img->width,
                    (d2_s32) img->height,
                    (d2_blitpos) 0,
                    0,                               // Source position
                    (d2_width) ((img->width) << 4),
                    (d2_width) ((img->height) << 4), // Destination width/height
                    (d2_width) ((0) << 4),
                    (d2_width) ((0) << 4),           // Destination position
                    d2_tm_filter);

        // foreground
        show_menu_icon();
    }
    else
    {
        int32_t chosen           = -1;
        bool_t  show_calculation = false;

        https_currency_update();

        select_detection_box(&chosen);

        switch (progress_state)
        {
            case 0:
            {
                img = (st_gimp_image_t *) get_sub_image_data(GI_CURRENCY_INSTRUCTIONS, GI_CURRENCY_LINE_0);
                break;
            }

            case 1:
            {
                img = (st_gimp_image_t *) get_sub_image_data(GI_CURRENCY_INSTRUCTIONS, GI_CURRENCY_LINE_1);
                break;
            }

            case 2:
            {
                img = (st_gimp_image_t *) get_sub_image_data(GI_CURRENCY_INSTRUCTIONS, GI_CURRENCY_LINE_2);
                break;
            }

            default:
            {
                img            = (st_gimp_image_t *) get_sub_image_data(GI_CURRENCY_INSTRUCTIONS, GI_CURRENCY_LINE_0);
                progress_state = 0;
                break;
            }
        }

        d2_setblitsrc(d2_handle,
                      img->pixel_data,
                      (d2_s32) img->width,
                      (d2_s32) img->width,
                      (d2_s32) img->height,
                      EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    (d2_s32) img->width,
                    (d2_s32) img->height,
                    (d2_blitpos) 0,
                    0,                               // Source position
                    (d2_width) ((img->width) << 4),
                    (d2_width) ((img->height) << 4), // Destination width/height
                    (d2_width) ((00) << 4),
                    (d2_width) ((00) << 4),          // Destination position
                    d2_tm_filter);

        if (s_show_reset)
        {
            img = (st_gimp_image_t *) &cc_reset;
            d2_setblitsrc(d2_handle,
                          img->pixel_data,
                          (d2_s32) cc_reset.width,
                          (d2_s32) cc_reset.width,
                          (d2_s32) cc_reset.height,
                          EP_SCREEN_MODE);

            d2_blitcopy(d2_handle,
                        (d2_s32) cc_reset.width,
                        (d2_s32) cc_reset.height,
                        (d2_blitpos) 0,
                        0,                                     // Source position
                        (d2_width) ((img->width) << 4),
                        (d2_width) ((img->height) << 4),       // Destination width/height
                        (d2_width) ((refpoint.horizontal) << 4),
                        (d2_width) ((refpoint.verticle) << 4), // Destination position
                        d2_tm_filter);
        }

        /* if user has chosen country AND the selected country isn't already selected as 1st or 2nd choice AND user is touching screen (debounce) */
        if ((chosen >= 0) && (first_chosen != chosen) && (second_chosen != chosen) && (g_touch_data.num_points != 0))
        {
            /* try to assign chosen country to first choice */
            if (first_chosen == (-1))
            {
                first_chosen     = chosen;
                progress_state   = 1;
                show_calculation = false;
                s_show_reset     = true;
            }
            // try to assign chosen country to 2nd choice
            else if ((first_chosen >= 0) && (second_chosen == (-1)))
            {
                second_chosen    = chosen;
                show_calculation = true;
                progress_state   = 2;
                s_show_reset     = true;
            }
            else
            {
                /* try to replace chosen country as 2nd choice */
                if ((first_chosen >= 0) && (first_chosen != chosen) && (second_chosen >= 0) &&
                    (second_chosen != chosen))
                {
                    second_chosen    = chosen;
                    show_calculation = true;
                    progress_state   = 2;
                    s_show_reset     = true;
                }
            }

// if ((first_chosen >= 0) && (show_calculation == false))
            if (first_chosen >= 0)
            {
                st_gimp_countries_image_t * simg = NULL;

                simg = (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES, (GI_COUNTRIES_1));
                draw_with_alpha(simg, 10, 24); // From bottom left of layer to bottom left of image

                simg =
                    (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES,
                                                                     currency_screen_control[first_chosen].id);
                draw_with_alpha(simg, 50, 24);              // From bottom left of layer to bottom left of image

                glcd_layer_change.layer.coordinate.x = 60;
                glcd_layer_change.layer.coordinate.y = 220; // need to center horizontical;

                glcd_layer_change.input = g_display0.p_cfg->input[1];
                (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);
            }

            // only implement aus // need live data
            if (show_calculation == true)
            {
                char_t result_str[32]            = "";
                st_gimp_countries_image_t * simg = NULL;

                currency_get_rate((char_t *) gp_currency_full_table[first_chosen],
                                  currency_screen_control[second_chosen].p_sname_str,
                                  (char *) result_str);

                simg = (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES, (GI_COUNTRIES_1));
                draw_with_alpha(simg, 10, 24); // From bottom left of layer to bottom left of image

                simg =
                    (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES,
                                                                     currency_screen_control[first_chosen].id);
                draw_with_alpha(simg, 50, 24);              // From bottom left of layer to bottom left of image

                glcd_layer_change.layer.coordinate.x = 60;
                glcd_layer_change.layer.coordinate.y = 220; // need to center horizontical;

                glcd_layer_change.input = g_display0.p_cfg->input[1];
                (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);

                uint16_t txt_off = 160;
                uint16_t std_off = 28;

                simg = (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES, (GI_COUNTRIES_EQUALS));
                draw_with_alpha(simg, txt_off, 24);                                            // From bottom left of layer to bottom left of image
                txt_off += 54;

                draw_with_alpha((st_gimp_countries_image_t *) &gimp_spacer_text, txt_off, 24); // From bottom left of layer to bottom left of image

                for (size_t rc = 0; rc < 6; rc++)
                {
                    if (result_str[rc] == '.')
                    {
                        simg = (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES,
                                                                                GI_COUNTRIES_DECIMAL);
                        draw_with_alpha(simg, txt_off, 24); // From bottom left of layer to bottom left of image
                        txt_off += (uint16_t) simg->height;
                    }
                    else
                    {
                        int32_t as_num_index = 0;
                        as_num_index = (result_str[rc] - '0');

                        if ((as_num_index >= 0) && (as_num_index <= 9))
                        {
                            simg =
                                (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES,
                                                                                 (uint32_t) (GI_COUNTRIES_0 +
                                                                                             as_num_index));
                            draw_with_alpha(simg, txt_off, 24); // From bottom left of layer to bottom left of image
                            txt_off += (uint16_t) simg->height;
                        }
                    }
                }

                txt_off += std_off;
                simg     = (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES,
                                                                            currency_screen_control[second_chosen].id);
                draw_with_alpha(simg, txt_off, 24); // From bottom left of layer to bottom left of image
            }

            chosen = -1;
        }

        show_detection_boxes();
        bool_t close_demo = process_main_menu();

        if (close_demo == true)
        {
            reset_selection();
        }
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_currency_screen
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
    int32_t                limit = (sizeof(currency_screen_control)) / sizeof(st_currency_screen_indicator_def_t);

    /* Store cp locally so it won't change during processing */
    cp.x = g_touch_data.point[0].x;
    cp.y = g_touch_data.point[0].y;

    for (int32_t sel = 0; sel < limit; sel++)
    {
        d2_point min_horizontal = currency_screen_control[sel].center.horizontal;
        d2_point max_horizontal = currency_screen_control[sel].center.horizontal + CURRENCY_HB_WIDTH_SIZE;

        d2_point min_verticle = currency_screen_control[sel].center.verticle;
        d2_point max_verticle = (currency_screen_control[sel].center.verticle + CURRENCY_HB_HEIGHT_SIZE);

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

    if (s_show_reset && (g_touch_data.num_points > 0))
    {
        d2_point min_horizontal = 26;
        d2_point max_horizontal = 26 + 159;

        d2_point min_verticle = 89;
        d2_point max_verticle = 89 + 37;

        /* Check for button activation */
        if (
            (cp.x > min_horizontal) && (cp.x <= max_horizontal) &&
            (cp.y > min_verticle) && (cp.y <= max_verticle))
        {
            reset_transition();
        }
    }
}

/**********************************************************************************************************************
 * End of function select_detection_box
 *********************************************************************************************************************/
static bool_t shown = false;

/* DEBUG FUNCTIONS */

/**********************************************************************************************************************
 * Function Name: show_detection_boxes
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void show_detection_boxes (void)
{

    /* For Debug use only, show the detection boxes as solid colour blocks */
    return;

    uint32_t sel   = 0;
    uint32_t limit = (sizeof(currency_screen_control)) / sizeof(st_currency_screen_indicator_def_t);

    FSP_PARAMETER_NOT_USED(sel);
    FSP_PARAMETER_NOT_USED(limit);

    for (sel = 0; sel < limit; sel++)
    {
        d2_renderquad(d2_handle, (d2_point) (currency_screen_control[sel].center.verticle << 4),
                      (d2_point) ((currency_screen_control[sel].center.horizontal) << 4),
                      (d2_point) ((currency_screen_control[sel].center.verticle + CURRENCY_HB_HEIGHT_SIZE) << 4),
                      (d2_point) ((currency_screen_control[sel].center.horizontal) << 4),
                      (d2_point) ((currency_screen_control[sel].center.verticle + CURRENCY_HB_HEIGHT_SIZE) << 4),
                      (d2_point) ((currency_screen_control[sel].center.horizontal + CURRENCY_HB_WIDTH_SIZE) << 4),
                      (d2_point) (currency_screen_control[sel].center.verticle << 4),
                      (d2_point) ((currency_screen_control[sel].center.horizontal + CURRENCY_HB_WIDTH_SIZE) << 4), 0);

        if (!shown)
        {
            sprintf(s_print_buffer,
                    "%s\tHITBOX POS [%03d, %03d]\r\n",
                    currency_screen_control[sel].p_name_str,
                    currency_screen_control[sel].center.horizontal,
                    currency_screen_control[sel].center.verticle);
            print_to_console(s_print_buffer);
        }
    }

    shown = true;
}

/**********************************************************************************************************************
 * End of function show_detection_boxes
 *********************************************************************************************************************/
