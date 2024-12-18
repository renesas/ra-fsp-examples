/***********************************************************************************************************************
 * File Name    : user_choice.h
 * Description  : Contains data structures, macros and functions definitions for user choice
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef USER_CHOICE_H_
#define USER_CHOICE_H_

#include <stdint.h>

#define _OB1203_SENSOR_ENABLE_    1
#define _HS3001_SENSOR_ENABLE_    1
#define _ZMOD4410_SENSOR_ENABLE_  1
#define _ZMOD4510_SENSOR_ENABLE_  1
#define _ICP20100_SENSOR_ENABLE_  1
#define _ICM42605_SENSOR_ENABLE_  1

/*MACRO BASED DATA POSTING ON TIME BASIS
 Here, USER_TIMER_DATA_POST macro can be set by user based on which the data can be
 posted on interval of seconds. For e.g. if User sets it to 10 then the sensor data will
 be posted at an interval of every 10 sec*/

/* User timer delay in seconds */
#define USER_TIMER_DATA_POST_INTERVAL  10

/* Function declaration */
void user_timer_start(void);
void user_timer_reset(void);

#endif /* USER_CHOICE_H_ */
