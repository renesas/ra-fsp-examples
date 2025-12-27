/***********************************************************************************************************************
 * File Name    : user_choice.c
 * Description  : Contains data structures and functions used for I2C
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "user_choice.h"
#include "common_utils.h"
#include "usr_data.h"
#include "common_data.h"

extern void updateData(void);

volatile uint16_t user_time = 0;

#define SUB_OPTIONS1T     "\r\n    Sensor temperature                : [GREEN]%3d.%d[WHITE]degC "
#define SUB_OPTIONS1H     "\r\n    Relative Humidity                 : [GREEN]%2d.%d[WHITE]%%          "
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

