/*
 * coreMQTT Agent v1.2.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Modifications Copyright (C) 2023 Renesas Electronics Corporation. or its affiliates.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file core_mqtt_agent_config.h
 * @brief This represents the default values for the configuration macros
 * for the MQTT-Agent library.
 */

#ifndef CORE_MQTT_AGENT_CONFIG_H_
#define CORE_MQTT_AGENT_CONFIG_H_

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for MQTT Agent.
 * 3. Include the header file "logging_stack.h", if logging is enabled for MQTT.
 */

#include "logging_levels.h"

/* Logging configuration for the MQTT library. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "MQTT_Agent"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

/**
 * @brief Maximum time MQTT agent waits in the queue for any pending MQTT
 * operations.
 *
 * The wait time is kept smallest possible to increase the responsiveness of
 * MQTT agent while processing  pending MQTT operations as well as receive
 * packets from network.
 */
#define MQTT_AGENT_MAX_EVENT_QUEUE_WAIT_TIME    ( 50U )

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* ifndef CORE_MQTT_AGENT_CONFIG_H_ */
