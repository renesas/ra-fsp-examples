/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : user_font_title_if.c
 * Version      : 1.0
 * Description  : Title font functions allowing the system to access the font plotting primitives.
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

#include "user_font_title_if.h"


static const st_font_title_rgb565_t *user_font_title_tbl[32] =
{
 &g_user_font_title_uc_a, // A
 &g_user_font_title_uc_b, // B
 &g_user_font_title_uc_c, // C
 &g_user_font_title_uc_d, // D
 &g_user_font_title_uc_e, // E
 &g_user_font_title_uc_f, // F
 &g_user_font_title_uc_g, // G
 &g_user_font_title_uc_h, // H
 &g_user_font_title_uc_i, // I
 &g_user_font_title_uc_j, // J
 &g_user_font_title_uc_k, // K
 &g_user_font_title_uc_l, // L
 &g_user_font_title_uc_m, // M
 &g_user_font_title_uc_n, // N
 &g_user_font_title_uc_o, // O
 &g_user_font_title_uc_p, // P
 &g_user_font_title_uc_q, // Q
 &g_user_font_title_uc_r, // R
 &g_user_font_title_uc_s, // S
 &g_user_font_title_uc_t, // T
 &g_user_font_title_uc_u, // U
 &g_user_font_title_uc_v, // V
 &g_user_font_title_uc_w, // W
 &g_user_font_title_uc_x, // X
 &g_user_font_title_uc_y, // Y
 &g_user_font_title_uc_z, // Z
};

static uint16_t s_bg_height;
static uint16_t s_bg_width;

static void user_font_title_load (void);
static const st_font_title_rgb565_t * select_bitmap(char_t character);
static void draw_bitmap(/* d2_device *gp_d2_handle, */ lv_point_t *cursor_pos, const st_font_title_rgb565_t *image);

void user_font_title_draw_line(/* d2_device *gp_d2_handle, */ lv_point_t *cursor_pos, char_t *line);

/**********************************************************************************************************************
 * Function Name: user_font_title_load
 * Description  : Local helper function, to map screen size to this font file (screen size configured in r_typedefs.h)
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
static void user_font_title_load (void)
{
    s_bg_height = FULL_IMAGE_HEIGHT;
    s_bg_width  = FULL_IMAGE_WIDTH;
}
/**********************************************************************************************************************
 End of user_font_title_load select_bitmap
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: select_bitmap
 * Description  : Local helper function, to map available characters images to target ASCII char_t
 *                If an appropriate image is not available default to underscore
 * Argument     : character - target ASCII char_t, note only a subset of characters are available.
 * Return Value : point to the image file describing selected character.
 *********************************************************************************************************************/
static const st_font_title_rgb565_t * select_bitmap(char_t character)
{
    const st_font_title_rgb565_t *t = NULL;
    char_t target;

    target = (char_t)toupper(character);

    /* Select bitmap */
    if((target >= 'A') && (target <= 'Z'))
    {
        target = target - 'A';

        if(NULL != user_font_title_tbl[(size_t)target])
        {
            t = user_font_title_tbl[(size_t)target];
        }
    }

    if (character == ' ')
    {
        t = &g_user_font_title_space;
    }

    if (character == '-')
    {
        t = &g_user_font_title_minus;
    }

    if (character == '_')
    {
        t = &g_user_font_title_underscore;
    }

    if (t == NULL)
    {
        t = &g_user_font_title_underscore;
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
static void draw_bitmap(/* d2_device *gp_d2_handle, */ lv_point_t *cursor_pos, const st_font_title_rgb565_t *image)
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
 End of draw_bitmap
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: user_font_title_draw_line
 * Description  : Local helper function, decode a character string into a series of calls to draw_bitmap
 * Argument     : cursor_pos - location on screen in full pixels (not fractions)
 *                line       - NULL terminated string
 * Return Value : point to the image file describing selected character.
 *********************************************************************************************************************/
void user_font_title_draw_line(/* d2_device *gp_d2_handle, */ lv_point_t *cursor_pos, char_t *line)
{
    size_t total_characters = strlen(line);
    const st_font_title_rgb565_t *image = NULL;

    user_font_title_load();

    for(size_t next_character = 0; next_character < total_characters; next_character++)
    {
        image = select_bitmap(line[next_character]);

        draw_bitmap(/* d2_device *gp_d2_handle, */ cursor_pos, image);
        vTaskDelay(1);
    }
}
/**********************************************************************************************************************
 End of user_font_title_draw_line
 *********************************************************************************************************************/
