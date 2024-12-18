/***********************************************************************************************************************
 * File Name    : usr_config.h
 * Description  : Contains macros, data structures and functions used  in the Application
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
#ifndef USR_CONFIG_H_
#define USR_CONFIG_H_

#include "common_utils.h"
#define USR_LOG_LVL          (LOG_ERROR)     /* User Options are:  LOG_ERROR, LOG_WARN, LOG_INFO, , LOG_DEBUG */

#if 0
    #define USR_LOG_TERMINAL     (RTT_TERMINAL)  /* User Options are:  RTT_TERMINAL */
#else
    #define USR_LOG_TERMINAL     (UART_TERMINAL)  /* User Options are:  RTT_TERMINAL */
#endif

#define USR_MQTT_DATA_FORMAT (JSON)          /* JSON, UTF8  */

#define LOGGING_TASK_STACK_SIZE         (1 * 1024)
#define LOGGING_TASK_STACK_PRIORITY     (6)
#define LOGGING_TASK_QUEUE_SIZE         (1 * 1024)

#define DEBUG                           (1)

#define USR_MQTT_BROKER_PORT            (8883)

#endif /* USR_CONFIG_H_ */
