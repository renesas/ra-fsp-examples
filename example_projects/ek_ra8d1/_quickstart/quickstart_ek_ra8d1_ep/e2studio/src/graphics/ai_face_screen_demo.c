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

/* RESOLUTION FROM CAMERA */
#define CAM_IMG_SIZE_X          320
#define CAM_IMG_SIZE_Y          240    /* Trim the Right Hand Edge hiding corruption */

/* normal screen */
#define CAM_LAYER_SIZE_X        476    /* 000 --> LCD_VPIX */
#define CAM_LAYER_SIZE_Y        360    /* 000 --> 480 */

/* AI model resolution */
#define DET_MODEL_IMG_SIZE_X    192
#define DET_MODEL_IMG_SIZE_Y    192

extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t   in_transition(void);
extern bool_t   is_camera_mode(void);

extern void   show_menu_icon(void);
extern bool_t process_main_menu(void);

extern uint8_t bsp_camera_out_rot_buffer565[BSP_CAM_WIDTH * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL];

extern st_ai_detection_point_t g_ai_detection[5];
extern uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);

uint64_t face_detection_inference_time;

void do_face_reconition_screen(void);

static volatile d2_point xs_pos = 42;
static volatile d2_point xe_pos = 401;

static volatile d2_point ys_pos = 247;
static volatile d2_point ye_pos = 605;

static volatile d2_point x_pos = 42;
static volatile d2_point y_pos = 247;

static st_gimp_countries_image_t gimp_spacer_text BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram");

static d2_point              hor_offset = 20;
static d2_point              ver_offset = 80;
static display_runtime_cfg_t glcd_layer_change;

static uint32_t total_number_of_menu_close_events = 0;

static uint32_t previous_face_count = 99;
static uint32_t frame_counter       = 0;

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
            buf_ptr[3] = object->pixel_data[v_offset + h + 3]; // A (Unused)
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
 * Function Name: do_face_reconition_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_face_reconition_screen (void)
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
    do_classification = false;
    vTaskDelay(0);

// do_detection = false;
    vTaskDelay(0);

    if (is_camera_mode() == true)
    {
        total_number_of_menu_close_events += process_main_menu();
        bsp_camera_capture_image();
        total_number_of_menu_close_events += process_main_menu();

        /* resume AI operation */
        do_classification = false;
        vTaskDelay(0);
        do_detection = true;
        vTaskDelay(0);
    }

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    /* Use background image */
    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_AI_FACE_RECONITION);
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

        /* show model information */
        print_bg_font_18(d2_handle, 220 + ver_offset, hor_offset, (char *) "Yolo-Fastest");
        print_bg_font_18(d2_handle, 142 + ver_offset, hor_offset, (char *) "192x192x1");
        print_bg_font_18(d2_handle, 64 + ver_offset, hor_offset, (char *) "--- ms ");

        if (is_camera_mode() == true)
        {
            /* Move graphics on-screen */
            glcd_layer_change.layer.coordinate.x = 186;
            glcd_layer_change.layer.coordinate.y = 652; // need to center horizontical;

            glcd_layer_change.input = g_display0.p_cfg->input[1];
            (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);
        }

        gimp_spacer_text.height          = 30;
        gimp_spacer_text.width           = 50;
        gimp_spacer_text.bytes_per_pixel = 4;
        memset(gimp_spacer_text.pixel_data, 0, ((50 * 30 * 4) + 1));

        previous_face_count = 99;

        /* foreground */
        show_menu_icon();
    }
    else
    {
        if (is_camera_mode() == false)
        {
            d2_point vpos = 346;
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
            print_bg_font_18(d2_handle, vpos -= 23, hpos, (char *) "Launch Face Detection application");
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
                        (d2_blitpos) 40,                                                     // Source position
                        (d2_width) ((CAM_LAYER_SIZE_Y) << 4),
                        (d2_width) ((CAM_LAYER_SIZE_Y) << 4),                  // Destination size width/height
                        (d2_width) ((((480 - 38) - CAM_LAYER_SIZE_Y) / 2) << 4),
                        (d2_width) (((LCD_VPIX - CAM_LAYER_SIZE_Y) / 2) << 4), // Destination offset position
                        d2_tm_filter);

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

            {
                uint32_t face_count = 0;
                d2_f32   scale_bb   = 360.0 / DET_MODEL_IMG_SIZE_X;

                for (int_t i = 0; i < 5; i++)
                {
                    d2_point x = g_ai_detection[i].m_x;
                    d2_point y = g_ai_detection[i].m_y;
                    d2_point w = g_ai_detection[i].m_w;
                    d2_point h = g_ai_detection[i].m_h;


                    x = (d2_point) (x * scale_bb);
                    y = (d2_point) (y * scale_bb);
                    w = (d2_point) (w * scale_bb);
                    h = (d2_point) (h * scale_bb);




					if((h != 0) && (w != 0))
					{
						face_count++;
						d2_point right_top_x = (d2_point)(xe_pos - y);
						d2_point right_top_y = (d2_point)(ys_pos + x);
						d2_point left_bottom_x = (d2_point)(xe_pos - y - h);
						d2_point left_bottom_y = (d2_point)(ys_pos + x + w);

                        d2_setcolor(d2_handle, 0, 0xFF0000);
                        d2_renderline(d2_handle, (d2_point) ((right_top_x) << 4), (d2_point) (right_top_y << 4),
                                      (d2_point) ((left_bottom_x) << 4), (d2_point) ((right_top_y) << 4),
                                      (d2_point) (2 << 4), 0);
                        d2_renderline(d2_handle, (d2_point) ((left_bottom_x) << 4), (d2_point) ((right_top_y) << 4),
                                      (d2_point) ((left_bottom_x) << 4), (d2_point) ((left_bottom_y) << 4),
                                      (d2_point) (2 << 4), 0);
                        d2_renderline(d2_handle, (d2_point) ((left_bottom_x) << 4), (d2_point) ((left_bottom_y) << 4),
                                      (d2_point) ((right_top_x) << 4), (d2_point) ((left_bottom_y) << 4),
                                      (d2_point) (2 << 4), 0);
                        d2_renderline(d2_handle, (d2_point) ((right_top_x) << 4), (d2_point) ((left_bottom_y) << 4),
                                      (d2_point) ((right_top_x) << 4), (d2_point) ((right_top_y) << 4),
                                      (d2_point) (2 << 4), 0);
                    }

                    memset(&g_ai_detection[i], 0, sizeof(g_ai_detection[i]));
                }

                if (face_count > 9)
                {
                    face_count = 9;
                }

                if (previous_face_count != face_count)
                {
                    previous_face_count = face_count;

                    /* Use biggest font available */
                    st_gimp_countries_image_t * simg = NULL;

                    draw_with_alpha(&gimp_spacer_text, 60, 24); // From bottom left of layer to bottom left of image

                    /* update string on display */
                    simg =
                        (st_gimp_countries_image_t *) get_sub_image_data(GI_CURRENCY_COUNTRIES,
                                                                         (GI_COUNTRIES_0 + face_count));
                    draw_with_alpha(simg, 60, 24);                                 // From bottom left of layer to bottom left of image
                }

                uint32_t time = (uint32_t) (face_detection_inference_time / 1000); // ms

                char_t time_str[] = {'0', '0', '0', ' ', 'm', 's', ' ', ' ', '\0'};
                time_str[0] += (char_t) (time / 100);
                time_str[1] += (char_t) ((time / 10) % 10);
                time_str[2] += (char_t) (time % 10);

                print_bg_font_18(d2_handle, 64 + (+ver_offset), hor_offset, (char *) time_str);
            }
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

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_face_reconition_screen
 *********************************************************************************************************************/
