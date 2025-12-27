/***********************************************************************************************************************
 * File Name    : user_choice.h
 * Description  : Contains data structures, macros and functions definitions for user choice
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USER_CHOICE_H_
#define USER_CHOICE_H_

#include <sensors_thread.h>
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
#define USER_TIMER_DATA_POST  10

/* Function declaration */
void user_timer_start(void);
void user_timer_reset(void);

#endif /* USER_CHOICE_H_ */
