/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : user_font_title_if.h
 * Version      : 1.0
 * Description  : Interface for title font allowing the system to access the font plotting primitives.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2018 1.00     First Release
 *********************************************************************************************************************/

#include "r_typedefs.h"

#ifndef USER_FONT_TITLE_IF_H_
#define USER_FONT_TITLE_IF_H_

extern const st_font_title_rgb565_t g_user_font_title_uc_a;
extern const st_font_title_rgb565_t g_user_font_title_uc_b;
extern const st_font_title_rgb565_t g_user_font_title_uc_c;
extern const st_font_title_rgb565_t g_user_font_title_uc_d;
extern const st_font_title_rgb565_t g_user_font_title_uc_e;
extern const st_font_title_rgb565_t g_user_font_title_uc_f;
extern const st_font_title_rgb565_t g_user_font_title_uc_g;
extern const st_font_title_rgb565_t g_user_font_title_uc_h;
extern const st_font_title_rgb565_t g_user_font_title_uc_i;
extern const st_font_title_rgb565_t g_user_font_title_uc_j;
extern const st_font_title_rgb565_t g_user_font_title_uc_k;
extern const st_font_title_rgb565_t g_user_font_title_uc_l;
extern const st_font_title_rgb565_t g_user_font_title_uc_m;
extern const st_font_title_rgb565_t g_user_font_title_uc_n;
extern const st_font_title_rgb565_t g_user_font_title_uc_o;
extern const st_font_title_rgb565_t g_user_font_title_uc_p;
extern const st_font_title_rgb565_t g_user_font_title_uc_q;
extern const st_font_title_rgb565_t g_user_font_title_uc_r;
extern const st_font_title_rgb565_t g_user_font_title_uc_s;
extern const st_font_title_rgb565_t g_user_font_title_uc_t;
extern const st_font_title_rgb565_t g_user_font_title_uc_u;
extern const st_font_title_rgb565_t g_user_font_title_uc_v;
extern const st_font_title_rgb565_t g_user_font_title_uc_w;
extern const st_font_title_rgb565_t g_user_font_title_uc_x;
extern const st_font_title_rgb565_t g_user_font_title_uc_y;
extern const st_font_title_rgb565_t g_user_font_title_uc_z;

extern const st_font_title_rgb565_t g_user_font_title_minus;
extern const st_font_title_rgb565_t g_user_font_title_space;
extern const st_font_title_rgb565_t g_user_font_title_underscore;

extern void user_font_title_draw_line(/* d2_device *d2_handle, */ lv_point_t *cursor_pos, char_t *line);


#endif /* USER_FONT_TITLE_IF_H_ */
