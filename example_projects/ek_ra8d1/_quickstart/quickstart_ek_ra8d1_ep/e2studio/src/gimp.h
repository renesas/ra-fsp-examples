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
 * File Name    : gimp.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * gimp.h
 *
 *  Created on: 19 Jun 2023
 *      Author: b3800117
 */

#include "dave_driver.h"

#ifndef GRAPHICS_GIMP_H_
 #define GRAPHICS_GIMP_H_

typedef unsigned int  guint;
typedef unsigned char guint8;
typedef          char gchar;

typedef struct st_gimp_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(480 * 854 * 2) + 1];
} st_gimp_image_t;

typedef struct st_gimp_countries_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(50 * 132 * 4) + 1];
} st_gimp_countries_image_t;

typedef struct st_gimp_weather_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(182 * 200 * 4) + 1];
} st_gimp_weather_image_t;

typedef struct st_gimp_mab_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(192 * 192 * 2) + 1];
} st_gimp_mab_image_t;

typedef struct st_gimp_currency_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(50 * 405 * 4) + 1];
} st_gimp_currency_image_t;

typedef struct st_gimp_led_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(332 * 240 * 2) + 1];
} st_gimp_led_image_t;

typedef struct st_gimp_ai_face_num_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(14 * 9 * 2) + 1];
} st_gimp_ai_face_num_image_t;

typedef struct st_gimp_font_22_full_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(30 * 345 * 4) + 1];
} st_gimp_font_22_full_image_t;

typedef struct st_gimp_font_full_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(28 * 691 * 2) + 1];
} st_gimp_font_full_image_t;

typedef struct st_gimp_bg_font_18_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(22 * 19 * 2) + 1];
} st_gimp_bg_font_18_image_t;

typedef struct st_gimp_cc_instruct_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(59 * 854 * 2) + 1];
} st_gimp_cc_instruct_image_t;

typedef struct st_gimp_cc_buttons_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(59 * 854 * 2) + 1];
} st_gimp_cc_buttons_image_t;

typedef struct st_gimp_city_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(66 * 133 * 2) + 1];
} st_gimp_city_image_t;

typedef struct st_gimp_fg_font_22_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(30 * 25 * 4) + 1];
} st_gimp_fg_font_22_image_t;

typedef struct st_gimp_fg_icon_image
{
    guint  width;
    guint  height;
    guint  bytes_per_pixel;            /* 2:RGB16, 3:RGB, 4:RGBA */
    guint8 pixel_data[(113 * 126 * 4) + 1];
} st_gimp_fg_icon_image_t;

typedef struct st_lcd_point_def
{
    d2_point horizontal;
    d2_point verticle;
} st_lcd_point_def_t;

typedef enum image_data
{
    GI_SPLASH_SCREEN = 0,              // 0x90040000 M1
    GI_MAIN_SCREEN,                    // 0x90140000
    GI_MAIN_BLANK,                     // 0x90240000 M2
    GI_MAIN_MENU,                      // 0x90340010
    GI_TIMEZONES_LOCALE,               // 0x90440010 M3
    GI_CURRENCY_COUNTRIES,             // 0x90540010
    GI_LED_GRAPHICS_SCREEN,            // 0x90640010 M4
    GI_NOT_IN_USE_1,                   // 0x90740010  OLDE ICONS I THINK
    GI_WEATHER_ICON_SCREEN,            // 0x90840010 MS2
    GI_CURRENCY_INSTRUCTIONS,          // 0x90940010
    GI_KIS_SCREEN,                     // 0x90A40010 MS3
    GI_HELP_SCREEN,                    // 0x90B40010
    GI_CREDITS,                        // 0x90C40010 MS4
    GI_LED_GRAPHICS_SCREEN_2,          // 0x90D40010
    DATA_WEATHER_SIMULATED,            // 0x90E40010 MS5
    GI_WORLD_SCREEN,                   // 0x90F40010
    GI_WEATHER_HONG_KONG,              // 0x91040010 MS6
    GI_WEATHER_KYOTO,                  // 0x91140010
    GI_WEATHER_LONDON,                 // 0x91240010
    GI_WEATHER_MIAMI,                  // 0x91340010
    GI_WEATHER_MUNICH,                 // 0x91440010
    GI_WEATHER_NEW_YORK,               // 0x91540010
    GI_WEATHER_PARIS,                  // 0x91640010
    GI_WEATHER_PRAGUE,                 // 0x91740010
    GI_WEATHER_QUEENSTOWN,             // 0x91840010
    GI_WEATHER_RIO_DE_JANERIO,         // 0x91940010
    GI_WEATHER_ROME,                   // 0x91A40010
    GI_WEATHER_SAN_FRANCISCO,          // 0x91B40010
    GI_WEATHER_SHANGHAI,               // 0x91C40010
    GI_WEATHER_SINGAPORE,              // 0x91D40010
    GI_WEATHER_SYDNEY,                 // 0x91E40010
    GI_WEATHER_TORONTO,                // 0x91F40010
    GI_AI_OBJECT_DETECTION,            // 0x92040010
    GI_AI_FACE_RECONITION,             // 0x92140010
    GI_CURRENCY_SCREEN,                // 0x92240010
    MODE_SLIDE_30,                     // 0x92340010
    MODE_SLIDE_31,                     // 0x92440010
} st_image_data_t;

typedef enum image_currency_instructions
{
    GI_CURRENCY_LINE_0 = 0,
    GI_CURRENCY_LINE_1,
    GI_CURRENCY_LINE_2
} st_image_currency_instructions_t;

typedef enum image_weather_icons
{
    GI_ICON_RAIN = 0,
    GI_ICON_SNOW,
    GI_ICON_SUN,
    GI_ICON_WIND
} st_image_weather_icons_t;

typedef enum image_timezone_locale
{
    GI_SAN_FRANSICSCO = 0,
    GI_MEXICO_CITY,
    GI_TORONTO,
    GI_SANTIAGO,
    GI_SAO_PAULO,
    GI_LONDON,
    GI_BERLIN,
    GI_CAPE_TOWN,
    GI_MOSCOW,
    GI_NEW_DELHI,
    GI_SINGAPHORE,
    GI_BEIJING,
    GI_TOKYO,
    GI_SYDNEY,
    GI_AUCKLAND
} st_image_timezone_locale_t;

typedef enum image_currency_countries
{
    GI_COUNTRIES_AUS = 0,
    GI_COUNTRIES_GBP,
    GI_COUNTRIES_CAN,
    GI_COUNTRIES_CNY,
    GI_COUNTRIES_EUR,
    GI_COUNTRIES_HKD,
    GI_COUNTRIES_INR,
    GI_COUNTRIES_JPY,
    GI_COUNTRIES_SGD,
    GI_COUNTRIES_USD,

    GI_COUNTRIES_EQUALS,
    GI_COUNTRIES_DECIMAL,

// GI_COUNTRIES_COLON,
    GI_COUNTRIES_0,
    GI_COUNTRIES_1,
    GI_COUNTRIES_2,
    GI_COUNTRIES_3,
    GI_COUNTRIES_4,
    GI_COUNTRIES_5,
    GI_COUNTRIES_6,
    GI_COUNTRIES_7,
    GI_COUNTRIES_8,
    GI_COUNTRIES_9,
} st_image_currency_countries_t;

#endif                                 /* GRAPHICS_GIMP_H_ */
