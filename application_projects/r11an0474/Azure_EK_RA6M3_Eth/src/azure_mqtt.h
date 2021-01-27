/***********************************************************************************************************************
 * File Name    : azure_mqtt.h
 * Description  : azure mqtt header file
 **********************************************************************************************************************/
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

#ifndef AZURE_MQTT_H_
#define AZURE_MQTT_H_
#include "user_cfg.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MQTT include. */
#include "iot_mqtt.h"

#include <azure/core/az_span.h>
#include <azure/core/az_result.h>
#include <main_thread.h>
/**
 * @brief The timeout for MQTT operations in this demo.
 */
#define MQTT_TIMEOUT_MS                          ( 5000 )

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define KEEP_ALIVE_SECONDS                       ( 60 )


/* Function Prototypes */
int azure_mqtt_init(void);
int azure_hub_connect(char *hostName,IotMqttConnection_t *pMqttConnection);
int azure_mqtt_subscribe(IotMqttConnection_t MqttConnection, const char *pSubTopics);
int azure_mqtt_unsubscribe(IotMqttConnection_t MqttConnection, const char *pSubTopics);
int azure_mqtt_publish(IotMqttConnection_t MqttConnection, const char *pPublishTopic, const char *msg);
az_result azure_iothub_read_configuration(char *hostName);
int send_telemetry(IotMqttConnection_t MqttConnection, const char *msg);
az_result read_configuration_entry(
        const char* env_name,
        char* default_value,
        az_span buffer,
        az_span* out_value);

#endif /* AZURE_MQTT_H_ */
