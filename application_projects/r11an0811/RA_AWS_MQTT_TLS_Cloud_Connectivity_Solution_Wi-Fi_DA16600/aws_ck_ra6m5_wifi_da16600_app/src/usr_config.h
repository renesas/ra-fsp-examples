/***********************************************************************************************************************
 * File Name    : usr_config.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
