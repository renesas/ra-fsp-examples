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
#include "portable.h"

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

#include "touch_gt911.h"
#include "gimp.h"

#include "font_ai_face_digit.h"
#include "bg_font_18_full.h"
#include "fg_font_22_full.h"

/* RESOLUTION FROM CAMERA */
#define CAM_IMG_SIZE_X      320
#define CAM_IMG_SIZE_Y      240        /* Trim the Right Hand Edge hiding corruption */

/* normal screen */
#define CAM_LAYER_SIZE_X    476        /* 000 --> LCD_VPIX */
#define CAM_LAYER_SIZE_Y    360        /* 000 --> 480 */

#define MAX_TEXT_UPDATE     3;
#define MAX_STR_LEN         15

extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t   in_transition(void);

extern void   show_menu_icon(void);
extern bool_t process_main_menu(void);

extern uint8_t bsp_camera_out_rot_buffer565[BSP_CAM_WIDTH * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL];
extern st_ai_classification_point_t g_ai_classification[5];
extern const char ** getLabelPtr();
extern uint32_t      get_image_data(st_image_data_t ref);
extern uint32_t      get_sub_image_data(st_image_data_t ref, uint32_t sub_image);
extern bool_t        is_camera_mode(void);

static char local_str[5][32] = {0};
static char local_prob[5][8] = {0};
static display_runtime_cfg_t glcd_layer_change;
static uint32_t              total_number_of_menu_close_events = 0;
static d2_point              hor_offset = 20;
static d2_point              ver_offset = 80;

uint64_t image_classification_inference_time;
void do_object_detection_screen(void);
void process_str(const char * input, char * output, int max_len);

/**********************************************************************************************************************
 * Function Name: process_str
 * Description  : .
 * Arguments    : input
 *              : output
 *              : max_len
 * Return Value : .
 *********************************************************************************************************************/
void process_str (const char * input, char * output, int max_len)
{
    int i;
    for (i = 0; (input[i] != '\0') && (i < (max_len - 1)); i++)
    {
        if (input[i] == ',')
        {
            break;
        }

        output[i] = input[i];
    }

    for ( ; i < (max_len - 1); i++)
    {
        output[i] = ' ';
    }

    output[max_len - 1] = '\0';
}

/**********************************************************************************************************************
 * End of function process_str
 *********************************************************************************************************************/
uint32_t refresh_fg = 0;

uint32_t tm = 0;

static volatile d2_point xs_pos = 42;
static volatile d2_point xe_pos = 401;

static volatile d2_point ys_pos = 247;
static volatile d2_point ye_pos = 605;

static volatile d2_point x_pos = 42;
static volatile d2_point y_pos = 247;

static uint32_t frame_counter = 0;

/**********************************************************************************************************************
 * Function Name: do_object_detection_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_object_detection_screen (void)
{
    st_gimp_image_t * img = NULL;

    frame_counter++;

    /* Initialise camera edge */
    xs_pos = 42;
    xe_pos = 401;

    ys_pos = 247;
    ye_pos = 605;

    x_pos = 42;
    y_pos = 247;

    total_number_of_menu_close_events = 0;

    /* suspend AI operation */
    do_detection = false;
    vTaskDelay(0);

    if (is_camera_mode() == true)
    {
        total_number_of_menu_close_events += process_main_menu();
        bsp_camera_capture_image();
        total_number_of_menu_close_events += process_main_menu();

        /* resume AI operation */
        do_detection = false;
        vTaskDelay(0);
        do_classification = true;
        vTaskDelay(0);
    }

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_AI_OBJECT_DETECTION);
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

        print_bg_font_18(d2_handle, 220 + ver_offset, hor_offset, (char *) "MobileNet v1");
        print_bg_font_18(d2_handle, 142 + ver_offset, hor_offset, (char *) "224x224x3");
        print_bg_font_18(d2_handle, 64 + ver_offset, hor_offset, (char *) "-- ms ");

        /* foreground */
        show_menu_icon();

        if (is_camera_mode() == true)
        {
            // Move graphics on-screen
            glcd_layer_change.layer.coordinate.x = 76;
            glcd_layer_change.layer.coordinate.y = 520; // need to center horizontical;

            glcd_layer_change.input = g_display0.p_cfg->input[1];
            (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);
        }
    }
    else
    {
        if (is_camera_mode() == false)
        {
            total_number_of_menu_close_events += process_main_menu();

            d2_point vpos = 335;
            d2_point hpos = 272;
            total_number_of_menu_close_events += process_main_menu();

            print_bg_font_18(d2_handle, vpos -= 23, hpos - 32, (char *) "PROBLEM CONNECTING TO THE CAMERA");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "Power down the kit");
            print_bg_font_18(d2_handle, vpos, hpos - 32, (char *) "1.");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "Ensure that the Camera Expansion Board");
            print_bg_font_18(d2_handle, vpos, hpos - 32, (char *) "2.");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "is securely and correctly mounted ");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "on the Camera Expansion Port");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "Ensure that the configuration switches");
            print_bg_font_18(d2_handle, vpos, hpos - 32, (char *) "3.");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "(SW1) are set as 3 and 7 ON, all others OFF");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "Power up the kit");
            print_bg_font_18(d2_handle, vpos, hpos - 32, (char *) "4.");
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "Launch Image Classification application");
            print_bg_font_18(d2_handle, vpos, hpos - 32, (char *) "5.");
        }
        else
        {
            total_number_of_menu_close_events += process_main_menu();

            d2_setblitsrc(d2_handle,
                          bsp_camera_out_rot_buffer565,
                          CAM_IMG_SIZE_Y,
                          CAM_IMG_SIZE_Y,
                          CAM_IMG_SIZE_X,
                          d2_mode_rgb565);

            d2_blitcopy(d2_handle,
                        CAM_IMG_SIZE_Y,
                        CAM_IMG_SIZE_Y,                                        // Source width/height
                        (d2_blitpos) 0,
                        0,                                                     // Source position
                        (d2_width) ((CAM_LAYER_SIZE_Y) << 4),
                        (d2_width) ((CAM_LAYER_SIZE_Y) << 4),                  // Destination size width/height
                        (d2_width) ((((480 - 38) - CAM_LAYER_SIZE_Y) / 2) << 4),
                        (d2_width) (((LCD_VPIX - CAM_LAYER_SIZE_Y) / 2) << 4), // Destination offset position
                        d2_tm_filter);

// d2_setcolor(d2_handle, 0, 0xf7f7f7); white
            d2_setcolor(d2_handle, 0, 0x000000);

            y_pos = ys_pos;
            d2_renderline(d2_handle,
                          (((d2_point) xs_pos) << 4),
                          (((d2_point) y_pos) << 4),
                          (((d2_point) xe_pos) << 4),
                          (((d2_point) y_pos) << 4),
                          3 << 4,
            0);

            y_pos = ye_pos;
            d2_renderline(d2_handle,
                          (((d2_point) xs_pos) << 4),
                          (((d2_point) y_pos) << 4),
                          (((d2_point) xe_pos) << 4),
                          (((d2_point) y_pos) << 4),
                          3 << 4,
            0);

            x_pos = xs_pos;
            d2_renderline(d2_handle, (((d2_point) x_pos) << 4), (((d2_point) ys_pos) << 4), (((d2_point) x_pos) << 4),
                          (((d2_point) ye_pos) << 4), 3 << 4, 0);

            x_pos = xe_pos;
            d2_renderline(d2_handle, (((d2_point) x_pos) << 4), (((d2_point) ys_pos) << 4), (((d2_point) x_pos) << 4),
                          (((d2_point) ye_pos) << 4), 3 << 4, 0);

            total_number_of_menu_close_events += process_main_menu();

            const char ** labels = getLabelPtr(); // stored in ai_apps/img_class/Labels.c
            d2_point      vpos   = 180;
            d2_point      hpos   = 200;

            if (refresh_fg == 0)
            {
                refresh_fg = MAX_TEXT_UPDATE;
                for (int_t i = 0; i < 5; i++)
                {
                    char processed_str[MAX_STR_LEN] = {0};

                    process_str(labels[g_ai_classification[i].category], processed_str, MAX_STR_LEN);
                    sprintf(local_str[i], "%s            ", processed_str);
                    local_str[i][MAX_STR_LEN] = '\0';
                    memset(&local_str[i][strlen(processed_str)], ' ', strlen(local_str[i]) - 1);

                    sprintf(local_prob[i], "%02d%%  ", (size_t) (g_ai_classification[i].prob * 100.0));
                    local_prob[i][5] = '\0';

                    hpos -= 30;
                    print_fg_font_22((uint16_t) (vpos - 60), (uint16_t) hpos, (char *) local_prob[i]);
                    print_fg_font_22((uint16_t) vpos, (uint16_t) hpos, (char *) local_str[i]);
                }
            }

            refresh_fg--;

            uint32_t time       = (uint32_t) (image_classification_inference_time / 1000); // ms
            char_t   time_str[] = {'0', '0', ' ', 'm', 's', ' ', ' ', '\0'};

            time_str[0] += (char_t) (time / 10);
            time_str[1] += (char_t) (time % 10);

            print_bg_font_18(d2_handle, 64 + ver_offset, hor_offset, (char *) time_str);
        }
    }

    total_number_of_menu_close_events += process_main_menu();

    if (total_number_of_menu_close_events > 0)
    {
        /* This demo is closing so clear forground */
        uint8_t * buf_ptr = (uint8_t *) fb_foreground;
        display_input_cfg_t const * p_input = &g_display0.p_cfg->input[1]; // Layer 2

        glcd_layer_change.layer.coordinate.x = 76;
        glcd_layer_change.layer.coordinate.y = 900;

        glcd_layer_change.input = g_display0.p_cfg->input[1];
        (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);

        memset(buf_ptr, 0, p_input->hstride * p_input->vsize * 4);
    }

    /* Wait for q frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_object_detection_screen
 *********************************************************************************************************************/
