/***********************************************************************************************************************
 * File Name    : azure_mqtt.h
 * Description  : azure mqtt header file
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
