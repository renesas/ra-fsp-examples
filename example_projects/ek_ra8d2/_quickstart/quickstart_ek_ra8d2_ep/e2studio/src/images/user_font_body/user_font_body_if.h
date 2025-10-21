/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : user_font_body_if.h
 * Version      : 1.0
 * Description  : Interface for body font allowing the system to access the font plotting primitives.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

#include "r_typedefs.h"

#ifndef _USER_FONT_BODY_IF_H_
#define _USER_FONT_BODY_IF_H_

extern const st_font_body_rgb565_t g_user_font_body_uc_a;
extern const st_font_body_rgb565_t g_user_font_body_uc_b;
extern const st_font_body_rgb565_t g_user_font_body_uc_c;
extern const st_font_body_rgb565_t g_user_font_body_uc_d;
extern const st_font_body_rgb565_t g_user_font_body_uc_e;
extern const st_font_body_rgb565_t g_user_font_body_uc_f;
extern const st_font_body_rgb565_t g_user_font_body_uc_g;
extern const st_font_body_rgb565_t g_user_font_body_uc_h;
extern const st_font_body_rgb565_t g_user_font_body_uc_i;
extern const st_font_body_rgb565_t g_user_font_body_uc_j;
extern const st_font_body_rgb565_t g_user_font_body_uc_k;
extern const st_font_body_rgb565_t g_user_font_body_uc_l;
extern const st_font_body_rgb565_t g_user_font_body_uc_m;
extern const st_font_body_rgb565_t g_user_font_body_uc_n;
extern const st_font_body_rgb565_t g_user_font_body_uc_o;
extern const st_font_body_rgb565_t g_user_font_body_uc_p;
extern const st_font_body_rgb565_t g_user_font_body_uc_q;
extern const st_font_body_rgb565_t g_user_font_body_uc_r;
extern const st_font_body_rgb565_t g_user_font_body_uc_s;
extern const st_font_body_rgb565_t g_user_font_body_uc_t;
extern const st_font_body_rgb565_t g_user_font_body_uc_u;
extern const st_font_body_rgb565_t g_user_font_body_uc_v;
extern const st_font_body_rgb565_t g_user_font_body_uc_w;
extern const st_font_body_rgb565_t g_user_font_body_uc_x;
extern const st_font_body_rgb565_t g_user_font_body_uc_y;
extern const st_font_body_rgb565_t g_user_font_body_uc_z;

extern const st_font_body_rgb565_t g_user_font_body_lc_a;
extern const st_font_body_rgb565_t g_user_font_body_lc_b;
extern const st_font_body_rgb565_t g_user_font_body_lc_c;
extern const st_font_body_rgb565_t g_user_font_body_lc_d;
extern const st_font_body_rgb565_t g_user_font_body_lc_e;
extern const st_font_body_rgb565_t g_user_font_body_lc_f;
extern const st_font_body_rgb565_t g_user_font_body_lc_g;
extern const st_font_body_rgb565_t g_user_font_body_lc_h;
extern const st_font_body_rgb565_t g_user_font_body_lc_i;
extern const st_font_body_rgb565_t g_user_font_body_lc_j;
extern const st_font_body_rgb565_t g_user_font_body_lc_k;
extern const st_font_body_rgb565_t g_user_font_body_lc_l;
extern const st_font_body_rgb565_t g_user_font_body_lc_m;
extern const st_font_body_rgb565_t g_user_font_body_lc_n;
extern const st_font_body_rgb565_t g_user_font_body_lc_o;
extern const st_font_body_rgb565_t g_user_font_body_lc_p;
extern const st_font_body_rgb565_t g_user_font_body_lc_q;
extern const st_font_body_rgb565_t g_user_font_body_lc_r;
extern const st_font_body_rgb565_t g_user_font_body_lc_s;
extern const st_font_body_rgb565_t g_user_font_body_lc_t;
extern const st_font_body_rgb565_t g_user_font_body_lc_u;
extern const st_font_body_rgb565_t g_user_font_body_lc_v;
extern const st_font_body_rgb565_t g_user_font_body_lc_w;
extern const st_font_body_rgb565_t g_user_font_body_lc_x;
extern const st_font_body_rgb565_t g_user_font_body_lc_y;
extern const st_font_body_rgb565_t g_user_font_body_lc_z;

extern const st_font_body_rgb565_t g_user_font_body_0;
extern const st_font_body_rgb565_t g_user_font_body_1;
extern const st_font_body_rgb565_t g_user_font_body_2;
extern const st_font_body_rgb565_t g_user_font_body_3;
extern const st_font_body_rgb565_t g_user_font_body_4;
extern const st_font_body_rgb565_t g_user_font_body_5;
extern const st_font_body_rgb565_t g_user_font_body_6;
extern const st_font_body_rgb565_t g_user_font_body_7;
extern const st_font_body_rgb565_t g_user_font_body_8;
extern const st_font_body_rgb565_t g_user_font_body_9;

extern const st_font_body_rgb565_t g_user_font_body_space;
extern const st_font_body_rgb565_t g_user_font_body_minus;
extern const st_font_body_rgb565_t g_user_font_body_and;
extern const st_font_body_rgb565_t g_user_font_body_open_curved;
extern const st_font_body_rgb565_t g_user_font_body_closed_curved;
extern const st_font_body_rgb565_t g_user_font_body_open_square;
extern const st_font_body_rgb565_t g_user_font_body_closed_square;
extern const st_font_body_rgb565_t g_user_font_body_degrees;
extern const st_font_body_rgb565_t g_user_font_body_colon;
extern const st_font_body_rgb565_t g_user_font_body_comma;
extern const st_font_body_rgb565_t g_user_font_body_percent;
extern const st_font_body_rgb565_t g_user_font_body_slash;
extern const st_font_body_rgb565_t g_user_font_body_full_stop;
extern const st_font_body_rgb565_t g_user_font_body_underscore;

extern void user_font_body_draw_line(/* d2_device *d2_handle, */ lv_point_t *cursor_pos, char_t *line);
#endif // _USER_FONT_BODY_IF_H_
