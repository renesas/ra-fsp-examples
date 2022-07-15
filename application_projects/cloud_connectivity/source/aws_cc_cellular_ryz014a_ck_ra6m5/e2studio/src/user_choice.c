/***********************************************************************************************************************
 * File Name    : user_choice.c
 * Description  : Contains data structures and functions used for I2C
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "user_choice.h"
#include "common_utils.h"
#include "usr_data.h"
#include "common_data.h"

char flt_oaq_str1[20] = { RESET_VALUE };
char flt_etoh_str1[20] = { RESET_VALUE };
char flt_eco2_str1[20] = { RESET_VALUE };
char flt_tvoc_str1[20] = { RESET_VALUE };
char flt_humi_str1[20] = { RESET_VALUE };
char flt_temp_str1[20] = { RESET_VALUE };
char Tempficp[20] = { RESET_VALUE };
char PressF[20] = { RESET_VALUE };

volatile uint16_t user_time = 0;

#define SUB_OPTIONS1T     "\r\n    Sensor temperature                : [GREEN]%3d.%d[WHITE]degC "
#define SUB_OPTIONS1H     "\r\n    Relative Humidity                 :  [GREEN]%2d.%d[WHITE]%%          "
int16_t hum_int;
int16_t hum_dec;
int16_t temp_int;
int16_t temp_dec;

/*******************************************************************************************************************//**
 * @brief       Timer call back after a interval of time set by user
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_user_timer_cb(timer_callback_args_t *p_args)
{
    (void) (p_args);
    user_time++;
    /* check for the user timer value */
    if (user_time > USER_TIMER_DATA_POST)
    {
        /* Fill latest sensor data into structure */
        updateData ();
        user_time = RESET_VALUE;
    }
}

/*******************************************************************************************************************//**
 * @brief       Start user timer
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void user_timer_start(void)
{
    g_timer1.p_api->open (g_timer1.p_ctrl, g_timer1.p_cfg);
    g_timer1.p_api->enable (g_timer1.p_ctrl);
    g_timer1.p_api->start (g_timer1.p_ctrl);
}

