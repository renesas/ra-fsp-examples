/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef MQTT_DEMO_HELPERS_H_
#define MQTT_DEMO_HELPERS_H_

#include "common_utils.h"

/* Include header for the subscription manager. */
#include "mqtt_subscription_manager.h"

/* MQTT API header. */
#include "core_mqtt.h"

/* Transport interface implementation include header for TLS. */
#include "transport_secure_sockets.h"

/* Include header for connection configurations. */
#include "aws_clientcredential.h"

#include "usr_config.h"

/*------------- Demo configurations -------------------------*/

/** Note: The device client certificate and private key credentials are
 * obtained by the transport interface implementation (with Secure Sockets)
 * from the demos/include/aws_clientcredential_keys.h file.
 *
 * The following macros SHOULD be defined for this demo which uses both server
 * and client authentications for TLS session:
 *   - keyCLIENT_CERTIFICATE_PEM for client certificate.
 *   - keyCLIENT_PRIVATE_KEY_PEM for client private key.
 */

#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT      USR_MQTT_ENDPOINT
#endif

#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER         USR_IOT_THING_NAME
#endif

#ifndef democonfigMQTT_BROKER_PORT

/**
 * @brief The port to use for the demo.
 */
    #define democonfigMQTT_BROKER_PORT          USR_MQTT_BROKER_PORT
#endif

/**
 * @brief The root CA certificate belonging to the broker.
 */
#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM               tlsATS1_ROOT_CERTIFICATE_PEM
#endif

/**
 * @brief The size to use for the network buffer.
 */
#ifndef mqttexampleNETWORK_BUFFER_SIZE
    #define mqttexampleNETWORK_BUFFER_SIZE      ( 2048U )
#endif

#undef LogInfo
#undef LogError
#undef LogWarn
#undef LogDebug
#undef LogErrorTrap

#define LogInfo              IotLogInfo
#define LogError             IotLogError
#define LogWarn              IotLogWarn
#define LogDebug             IotLogDebug
#define LogErrorTrap         IotLogErrorTrap

/* Macros used tunnel the IotLogging to RTT and SWO */
#define IotLogInfo              APP_INFO_PRINT
#define IotLogError             APP_ERR_PRINT
#define IotLogWarn              APP_WARN_PRINT
#define IotLogDebug             APP_DBG_PRINT
#define IotLogErrorTrap         APP_ERR_TRAP

/*-----------------------------------------------------------*/

typedef struct mqtt_status
{
    bool        mqtt_connect_status;
    BaseType_t  status;
} mqtt_status_t;

/*-----------------------------------------------------------*/

/**
 * @brief Establish a MQTT connection.
 *
 * @param[in] pxContext The memory for the MQTTContext_t that will be used for the
 * MQTT connection.
 * @param[in] pxNetContext The memory for the NetworkContext_t required for the
 * MQTT connection.
 * @param[in] pxNetworkBuffer The buffer space for initializing the @p pxContext MQTT
 * context used in the MQTT connection.
 * @param[in] appCallback The callback function used to receive incoming
 * publishes and incoming acks from MQTT library.
 *
 * @return The status of the final connection attempt.
 */
BaseType_t EstablishMqttSession( MQTTContext_t * pxMqttContext,
                                 NetworkContext_t * pxNetworkContext,
                                 MQTTFixedBuffer_t * pxNetworkBuffer,
                                 bool createCleanSession);

/**
 * @brief Process incoming Publish message.
 *
 * @param[in] pxPublishInfo is a pointer to structure containing deserialized
 * Publish message.
 */
void prvMQTTProcessIncomingPublish( MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Invoked by the event callback to handle SUBACK and UNSUBACK packets
 * from the broker.
 *
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] usPacketIdentifier Packet identifier of the incoming packet.
 */
void vHandleOtherIncomingPacket( MQTTPacketInfo_t * pxPacketInfo,
                                 uint16_t usPacketIdentifier );

/**
 * @brief Close the MQTT connection.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection to close.
 *
 * @return pdPASS if DISCONNECT was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t DisconnectMqttSession( MQTTContext_t * pxContext,
                                  NetworkContext_t * pxNetContext );

/**
 * @brief Subscribe to a MQTT topic filter.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection.
 * @param[in] pcTopicFilter Pointer to the shadow topic buffer.
 * @param[in] usTopicFilterLength Indicates the length of the shadow
 * topic buffer.
 *
 * @return pdPASS if SUBSCRIBE was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t SubscribeToTopic( MQTTContext_t * pxMqttContext,
                             const char * pcTopicFilter,
                             uint16_t usTopicFilterLength);

/**
 * @brief Sends an MQTT UNSUBSCRIBE to unsubscribe from the shadow
 * topic.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection.
 * @param[in] pcTopicFilter Pointer to the shadow topic buffer.
 * @param[in] usTopicFilterLength Indicates the length of the shadow
 * topic buffer.
 *
 * @return pdPASS if UNSUBSCRIBE was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t UnsubscribeFromTopicFilters( MQTTContext_t * pxMqttContext,
                                        const MQTTSubscribeInfo_t * pTopicFilters,
                                        size_t numOfTopicFilters );

/**
 * @brief Utility to subscribe to the passed topic filter and register
 * a callback for it in the subscription manager.
 *
 * The registered callback will be invoked by the subscription manager
 * when PUBLISH messages on topic(s) that match the registered topic filter
 * are received from the broker.
 *
 * @param[in] pContext The MQTT context representing the MQTT connection.
 * @param[in] pTopicFilter The topic filter to subscribe to and register a
 * callback for in the subscription manager.
 * @param[in] topicFilterLength The length of the topic filter, @p pTopicFilter.
 * @param[in] callback The callback to register for the topic filter with the
 * subscription manager.
 *
 * @return EXIT_SUCCESS if subscription and callback registration operations
 * for the topic filter were successfully; EXIT_FAILURE otherwise.
 */
BaseType_t subscribeToAndRegisterTopicFilter( MQTTContext_t * pContext,
                                              const char * pTopicFilter,
                                              uint16_t topicFilterLength,
                                              SubscriptionManagerCallback_t callback );

/**
 * @brief Publish a message to a MQTT topic.
 *
 * @param[in] pxContext The MQTT context for the MQTT connection.
 * @param[in] pcTopicFilter Points to the topic.
 * @param[in] topicFilterLength The length of the topic.
 * @param[in] pcPayload Points to the payload.
 * @param[in] payloadLength The length of the payload.
 *
 * @return pdPASS if PUBLISH was successfully sent;
 * pdFAIL otherwise.
 */
BaseType_t PublishToTopic( MQTTContext_t * pxMqttContext,
                           const char * pcTopicFilter,
                           uint16_t topicFilterLength,
                           const char * pMessage );

/**
 * @brief Invoke the core MQTT library's process loop function.
 *
 * @param[in] pxMqttContext The MQTT context for the MQTT connection.
 * @param[in] ulTimeoutMs Minimum time for the loop to run, if no error occurs.
 *
 * @return pdPASS if process loop was successful;
 * pdFAIL otherwise.
 */
BaseType_t ProcessLoop( MQTTContext_t * pxMqttContext,
                        uint32_t ulTimeoutMs );

BaseType_t subscribePublishLoop( MQTTContext_t * pxMqttContext, NetworkContext_t * pxNetworkContext );

#endif /* ifndef MQTT_DEMO_HELPERS_H_ */
