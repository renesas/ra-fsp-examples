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
 * File Name    : font_ai_face_digit.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "r_typedefs.h"
#include "dave_driver.h"
#include "gimp.h"

void        get_ai_face_font(uint32_t index, d2_color col, st_gimp_ai_face_num_image_t * result);
extern void get_ai_face_font(uint32_t index, d2_color col, st_gimp_ai_face_num_image_t * result);

/*
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_0;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_1;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_2;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_3;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_4;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_5;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_6;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_7;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_8;
 * extern  const st_gimp_ai_face_num_image_t ai_face_digit_9;
 * extern  const st_gimp_ai_face_num_image_t ai_face_percent;
 */
