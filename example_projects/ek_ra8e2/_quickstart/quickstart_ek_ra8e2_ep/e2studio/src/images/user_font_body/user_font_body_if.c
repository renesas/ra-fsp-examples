/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : user_font_body_if.c
 * Version      : 1.0
 * Description  : Body font functions allowing the system to access the font plotting primitives.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "r_typedefs.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "jlink_console.h"

#include "images/user_font_body/user_font_body_if.h"

static const st_font_body_rgb565_t *g_user_font_body_uc_tbl[32] =
{
     &g_user_font_body_uc_a, // A
     &g_user_font_body_uc_b, // B
     &g_user_font_body_uc_c, // C
     &g_user_font_body_uc_d, // D
     &g_user_font_body_uc_e, // E
     &g_user_font_body_uc_f, // F
     &g_user_font_body_uc_g, // G
     &g_user_font_body_uc_h, // H
     &g_user_font_body_uc_i, // I
     &g_user_font_body_uc_j, // J
     &g_user_font_body_uc_k, // K
     &g_user_font_body_uc_l, // L
     &g_user_font_body_uc_m, // M
     &g_user_font_body_uc_n, // N
     &g_user_font_body_uc_o, // O
     &g_user_font_body_uc_p, // P
     &g_user_font_body_uc_q, // Q
     &g_user_font_body_uc_r, // R
     &g_user_font_body_uc_s, // S
     &g_user_font_body_uc_t, // T
     &g_user_font_body_uc_u, // U
     &g_user_font_body_uc_v, // V
     &g_user_font_body_uc_w, // W
     &g_user_font_body_uc_x, // X
     &g_user_font_body_uc_y, // Y
     &g_user_font_body_uc_z, // Z
};

static const st_font_body_rgb565_t *g_user_font_body_lc_tbl[32] =
{
     &g_user_font_body_lc_a, // a
     &g_user_font_body_lc_b, // b
     &g_user_font_body_lc_c, // c
     &g_user_font_body_lc_d, // d
     &g_user_font_body_lc_e, // e
     &g_user_font_body_lc_f, // f
     &g_user_font_body_lc_g, // g
     &g_user_font_body_lc_h, // h
     &g_user_font_body_lc_i, // i
     &g_user_font_body_lc_j, // j
     &g_user_font_body_lc_k, // k
     &g_user_font_body_lc_l, // l
     &g_user_font_body_lc_m, // m
     &g_user_font_body_lc_n, // n
     &g_user_font_body_lc_o, // o
     &g_user_font_body_lc_p, // p
     &g_user_font_body_lc_q, // q
     &g_user_font_body_lc_r, // r
     &g_user_font_body_lc_s, // s
     &g_user_font_body_lc_t, // t
     &g_user_font_body_lc_u, // u
     &g_user_font_body_lc_v, // v
     &g_user_font_body_lc_w, // w
     &g_user_font_body_lc_x, // x
     &g_user_font_body_lc_y, // y
     &g_user_font_body_lc_z, // z
};

static const st_font_body_rgb565_t *g_user_font_body_num_tbl[32] =
{
     &g_user_font_body_0, // 0
     &g_user_font_body_1, // 1
     &g_user_font_body_2, // 2
     &g_user_font_body_3, // 3
     &g_user_font_body_4, // 4
     &g_user_font_body_5, // 5
     &g_user_font_body_6, // 6
     &g_user_font_body_7, // 7
     &g_user_font_body_8, // 8
     &g_user_font_body_9, // 9
};

static uint16_t s_bg_height;
static uint16_t s_bg_width;

static void user_font_body_load (void);
static const st_font_body_rgb565_t * select_bitmap(char_t character);
static void draw_bitmap(/* d2_device *draw_bitma, */ lv_point_t *cursor_pos, const st_font_body_rgb565_t *image);

void g_user_font_body_draw_line(/* d2_device *draw_bitma, */ lv_point_t *cursor_pos, char_t *line);

/**********************************************************************************************************************
 * Function Name: user_font_body_load
 * Description  : Local helper function, to map screen size to this font file (screen size configured in r_typedefs.h)
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
static void user_font_body_load (void)
{
    s_bg_height = 600;
    s_bg_width  = 1024;
}
/**********************************************************************************************************************
 End of function user_font_body_load
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: select_bitmap
 * Description  : Local helper function, to map available characters images to target ASCII char_t
 *                If an appropriate image is not available default to underscore
 * Argument     : character - target ASCII char_t, note only a subset of characters are available.
 * Return Value : point to the image file describing selected character.
 *********************************************************************************************************************/
static const st_font_body_rgb565_t * select_bitmap(char_t character)
{
    const st_font_body_rgb565_t *t = NULL;
    char_t target = character;

    /* Select bitmap */
    if((target >= 'A') && (target <= 'Z'))
    {
        target = target - 'A';

        if(NULL != g_user_font_body_uc_tbl[(size_t)target])
        {
            t = g_user_font_body_uc_tbl[(size_t)target];
        }
    }

    if((target >= 'a') && (target <= 'z'))
    {
        target = target - 'a';

        if(NULL != g_user_font_body_lc_tbl[(size_t)target])
        {
            t = g_user_font_body_lc_tbl[(size_t)target];
        }
    }

    if((target >= '0') && (target <= '9'))
    {
        target = target - '0';

        if(NULL != g_user_font_body_num_tbl[(size_t)target])
        {
            t = g_user_font_body_num_tbl[(size_t)target];
        }
    }

    if (character == ' ')
    {
        t = &g_user_font_body_space;
    }

    if (character == '-')
    {
        t = &g_user_font_body_minus;
    }

    if (character == '&')
    {
        t = &g_user_font_body_and;
    }

    if (character == '(')
    {
        t = &g_user_font_body_open_curved;
    }

    if (character == ')')
    {
        t = &g_user_font_body_closed_curved;
    }

    if (character == '[')
    {
        t = &g_user_font_body_open_square;
    }

    if (character == ']')
    {
        t = &g_user_font_body_closed_square;
    }

    if (character == '\'')
    {
        t = &g_user_font_body_degrees;
    }

    if (character == ':')
    {
        t = &g_user_font_body_colon;
    }

    if (character == ',')
    {
        t = &g_user_font_body_comma;
    }

    if (character == '%')
    {
        t = &g_user_font_body_percent;
    }

    if (character == '/')
    {
        t = &g_user_font_body_slash;
    }

    if (character == '.')
    {
        t = &g_user_font_body_full_stop;
    }

    if (character == '_')
    {
        t = &g_user_font_body_underscore;
    }

    if (t == NULL)
    {
        t = &g_user_font_body_underscore;
    }

    return (t);
}
/**********************************************************************************************************************
 End of function select_bitmap
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: draw_bitmap
 * Description  : Local helper function, to plot image to the draw layer
 *                After character is drawn, increment the cursor_pos to horizontal position to draw the next character.
 *                Note auto incrementing does not allow next start point to be off screen.
 * Argument     : cursor_pos - location on screen in full pixels (not fractions)
 *                image      - selected image file (see select_bitmap)
 * Return Value : point to the image file describing selected character.
 *********************************************************************************************************************/
static void draw_bitmap(/* d2_device *gp_d2_handle, */ lv_point_t *cursor_pos, const st_font_body_rgb565_t *image)
{
     d2_setblitsrc(gp_d2_handle, (void *)image->pixel_data, image->width, image->width, image->height, d2_mode_rgb565);

     /* display as VGA 640x480 on mipi lcd */
     // Param gp_d2_handle, Source width/height    Source position, Destination scale , Destination position on lcd
     d2_blitcopy(gp_d2_handle,
             (d2_s32) image->width, (d2_s32) image->height,
             (d2_blitpos) 0, (d2_blitpos) 0,
             (d2_width) ((uint32_t)(image->width) << 4), (d2_width) ((uint32_t)(image->height) << 4),
             (d2_point) (cursor_pos->x << 4), (d2_point) (cursor_pos->y << 4),
             d2_tm_filter);

    if (cursor_pos->x < (s_bg_width - (uint32_t)image->offset))
    {
        cursor_pos->x += image->offset;
    }
}
/**********************************************************************************************************************
 End of function draw_bitmap
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: user_font_body_draw_line
 * Description  : Local helper function, decode a character string into a series of calls to draw_bitmap
 * Argument     : cursor_pos - location on screen in full pixels (not fractions)
 *                line       - NULL terminated string
 * Return Value : point to the image file describing selected character.
 *********************************************************************************************************************/
void user_font_body_draw_line(/* d2_device *gp_d2_handle, */ lv_point_t *cursor_pos, char_t *line)
{
    size_t total_characters = strlen(line);
    const st_font_body_rgb565_t *image = NULL;

    user_font_body_load();

    for(size_t next_character = 0; next_character < total_characters; next_character++)
    {
        image = select_bitmap(line[next_character]);

        draw_bitmap(/* d2_device *gp_d2_handle, */ cursor_pos, image);
        vTaskDelay(1);
    }
}
/**********************************************************************************************************************
 End of function user_font_body_draw_line
 *********************************************************************************************************************/

