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

/**
 * @file mqtt_demo_helpers.c
 *
 * @brief This file provides helper functions used by the Shadow demo application to
 * do MQTT operations over a mutually authenticated TLS connection.
 *
 * A mutually authenticated TLS connection is used to connect to the AWS IoT
 * MQTT message broker in this example.  Define democonfigMQTT_BROKER_ENDPOINT
 * and DEMO_CONFIG_ROOT_CA_PEM, in shadow_demo_config.h, and the client
 * private key and certificate, in aws_clientcredential_keys.h, to establish a
 * mutually authenticated connection.
 */
#include "common_utils.h"
#include "common_data.h"
#include "flash/flash_hp.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "aws_dev_mode_key_provisioning.h"

#include "mqtt_demo_helpers.h"

/* Retry utilities include. */
#include "backoff_algorithm.h"

/* Include the TLS implementation of the transport interface. */
#include "transport_mbedtls_pkcs11.h"

/*-----------------------------------------------------------*/

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS                ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying connection to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS        ( 15000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS             ( 10000U )

/**
 * @brief The maximum number of retries for subscribing to topic filter when broker rejects an attempt.
 */
#define SUBSCRIBE_RETRY_MAX_ATTEMPTS                 ( 4U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying subscription to topic.
 */
#define SUBSCRIBE_RETRY_MAX_BACKOFF_DELAY_MS         ( 2500U )

/**
 * @brief The base back-off delay (in milliseconds) to use for subscription retry attempts.
 */
#define SUBSCRIBE_RETRY_BACKOFF_BASE_MS              ( 1000U )

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define CONNACK_RECV_TIMEOUT_MS                      ( 3000U )

/**
 * @brief Timeout for MQTT_ProcessLoop in milliseconds.
 */
#define PROCESS_LOOP_TIMEOUT_MS                      ( 200U )

/**
 * @brief Keep alive time reported to the broker while establishing an MQTT connection.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define KEEP_ALIVE_TIMEOUT_SECONDS                   ( 600U )

/**
 * @brief Maximum number of outgoing publishes maintained in the application
 * until an ack is received from the broker.
 */
#define MAX_OUTGOING_PUBLISHES                       ( 3U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND                      ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                        ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief Default socket send timeout.
 *
 * The user can change the send timeout for a socket using the SOCKETS_SetSockOpt API
 * with the SOCKETS_SO_SNDTIMEO option.
 */
#ifndef SOCKETS_CONFIG_DEFAULT_SEND_TIMEOUT
#define SOCKETS_CONFIG_DEFAULT_SEND_TIMEOUT            ( 5000U )
#endif

/**
 * @brief Default socket receive timeout.
 *
 * The user can change the receive timeout for a socket using the SOCKETS_SetSockOpt API
 * with the SOCKETS_SO_RCVTIMEO option.
 */
#ifndef SOCKETS_CONFIG_DEFAULT_RECV_TIMEOUT
#define SOCKETS_CONFIG_DEFAULT_RECV_TIMEOUT            ( 5000U )
#endif

/**
 * @brief ALPN protocol name for AWS IoT MQTT.
 *
 * This will be used if the DEMO_CONFIG_MQTT_BROKER_PORT is configured as 443 for AWS IoT MQTT
 * broker. Please see more details about the ALPN protocol for AWS IoT MQTT
 * endpoint in the link below.
 * https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/
 *
 * @note OpenSSL requires that the protocol string passed to it for configuration be encoded
 * with the prefix of 8-bit length information of the string. Thus, the 14 byte (0x0e) length
 * information is prefixed to the string.
 */
#define MQTTOP_ALPN_PROTOCOL_NAME                     ("\x0ex-amzn-mqtt-ca")

extern char g_mqtt_endpoint[128];
extern char g_iot_thing_name[128];
char democonfigMQTT_BROKER_ENDPOINT[128];
char democonfigCLIENT_IDENTIFIER[128];
/*-----------------------------------------------------------*/
/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in FreeRTOS-Plus/Source/Application-Protocols/network_transport.
 */
struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

/**
 * @brief Structure to keep the MQTT publish packets until an ack is received
 * for QoS1 publishes.
 */
typedef struct PublishPackets
{
    /**
     * @brief Packet identifier of the publish packet.
     */
    uint16_t packetId;

    /**
     * @brief Publish info of the publish packet.
     */
    MQTTPublishInfo_t pubInfo;
} PublishPackets_t;

/**
 * @brief A pair containing a topic filter and its SUBACK status.
 */
typedef struct topicFilterContext
{
    const char * pcTopicFilter;
    MQTTSubAckStatus_t xSubAckStatus;
} topicFilterContext_t;

/*-----------------------------------------------------------*/

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief A struct containing the context of a SUBACK; the SUBACK status
 * of a filter is updated when the event callback processes a SUBACK.
 */
static topicFilterContext_t xTopicFilterContext;

/**
 * @brief Array to keep the outgoing publish messages.
 * These stored outgoing publish messages are kept until a successful ack
 * is received.
 */
static PublishPackets_t outgoingPublishPackets[ MAX_OUTGOING_PUBLISHES ] = { 0 };

/**
 * @brief The flag to indicate the MQTT session changed.
 */
static BaseType_t xMqttSessionEstablished = pdFALSE;

/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted Subscribe packet.
 */
static uint16_t usSubscribePacketIdentifier = 0U;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
 * it is used to match received Unsubscribe response to the transmitted Unsubscribe
 * request.
 */
static uint16_t usUnsubscribePacketIdentifier = 0U;

static TlsTransportParams_t transport_params;

static volatile bool suback_is_received = false;
static volatile bool unsuback_is_received = false;
static volatile bool puback_is_received = false;
/*-----------------------------------------------------------*/

/**
 * @brief Calculate and perform an exponential backoff with jitter delay for
 * the next retry attempt of a failed network operation with the server.
 *
 * The function generates a random number, calculates the next backoff period
 * with the generated random number, and performs the backoff delay operation if the
 * number of retries have not exhausted.
 *
 * @note The PKCS11 module is used to generate the random number as it allows access
 * to a True Random Number Generator (TRNG) if the vendor platform supports it.
 * It is recommended to seed the random number generator with a device-specific entropy
 * source so that probability of collisions from devices in connection retries is mitigated.
 *
 * @note The backoff period is calculated using the backoffAlgorithm library.
 *
 * @param[in, out] pxRetryAttempts The context to use for backoff period calculation
 * with the backoffAlgorithm library.
 *
 * @return pdPASS if calculating the backoff period was successful; otherwise pdFAIL
 * if there was failure in random number generation OR all retry attempts had exhausted.
 */
static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams );

/**
 * @brief Connect to MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[out] pxNetworkContext The output parameter to return the created network context.
 *
 * @return The status of the final connection attempt.
 */
static TlsTransportStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext );

/**
 * @brief Function to get the free index at which an outgoing publish
 * can be stored.
 *
 * @param[out] pucIndex The output parameter to return the index at which an
 * outgoing publish message can be stored.
 *
 * @return pdFAIL if no more publishes can be stored;
 * pdPASS if an index to store the next outgoing publish is obtained.
 */
static BaseType_t prvGetNextFreeIndexForOutgoingPublishes( uint8_t * pucIndex );

/**
 * @brief Function to clean up an outgoing publish at given index from the
 * #outgoingPublishPackets array.
 *
 * @param[in] ucIndex The index at which a publish message has to be cleaned up.
 */
static void vCleanupOutgoingPublishAt( uint8_t ucIndex );

/**
 * @brief Function to clean up all the outgoing publishes maintained in the
 * array.
 */
static void vCleanupOutgoingPublishes( void );

/**
 * @brief Function to clean up the publish packet with the given packet id.
 *
 * @param[in] usPacketId Packet identifier of the packet to be cleaned up from
 * the array.
 */
static void vCleanupOutgoingPublishWithPacketID( uint16_t usPacketId );

/**
 * @brief Function to resend the publishes if a session is re-established with
 * the broker. This function handles the resending of the QoS1 publish packets,
 * which are maintained locally.
 *
 * @param[in] pxMqttContext MQTT context pointer.
 */
static BaseType_t handlePublishResend( MQTTContext_t * pxMqttContext );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

/**
 * @brief The application callback function for getting the incoming publishes,
 * incoming acks, and ping responses reported from the MQTT library.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pxDeserializedInfo Deserialized information from the incoming packet.
 */
static void commonEventHandler( MQTTContext_t * pMqttContext,
                                MQTTPacketInfo_t * pPacketInfo,
                                MQTTDeserializedInfo_t * pDeserializedInfo );

/* Declare macros and variables for initializing MQTT context with QoS > 0 */
#define MQTT_EXAMPLE_OUTGOING_PUBLISH_RECORD_LEN (8)
#define MQTT_EXAMPLE_INCOMING_PUBLISH_RECORD_LEN (9)
static MQTTPubAckInfo_t pOutgoingPublishRecords[ MQTT_EXAMPLE_OUTGOING_PUBLISH_RECORD_LEN ];
static MQTTPubAckInfo_t pIncomingPublishRecords[ MQTT_EXAMPLE_INCOMING_PUBLISH_RECORD_LEN ];

/*-----------------------------------------------------------*/

static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint16_t usNextRetryBackOff = 0U;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;

    /**
     * To calculate the backoff period for the next retry attempt, we will
     * generate a random number to provide to the backoffAlgorithm library.
     *
     * Note: The PKCS11 module is used to generate the random number as it allows access
     * to a True Random Number Generator (TRNG) if the vendor platform supports it.
     * It is recommended to use a random number generator seeded with a device-specific
     * entropy source so that probability of collisions from devices in connection retries
     * is mitigated.
     */
    uint32_t ulRandomNum = 0;

    if(FSP_SUCCESS == HW_SCE_RNG_Read(&ulRandomNum))
    {
        /* Get back-off value (in milliseconds) for the next retry attempt. */
        xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( pxRetryParams, ulRandomNum, &usNextRetryBackOff );

        if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
        {
            IotLogError("All retry attempts have exhausted. Operation will not be retried\r\n");
        }
        else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
        {
            /* Perform the backoff delay. */
            vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );

            xReturnStatus = pdPASS;
            IotLogInfo("Retry attempt %lu out of maximum retry attempts %lu.\r\n", ( pxRetryParams->attemptsDone + 1 ), pxRetryParams->maxRetryAttempts);
        }
    }
    else
    {
        IotLogError("Unable to retry operation with broker: Random number generation failed\r\n");
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static TlsTransportStatus_t prvConnectToServerWithBackoffRetries( NetworkContext_t * pxNetworkContext )
{
    NetworkCredentials_t credentials;
    TlsTransportStatus_t xNetworkStatus = TLS_TRANSPORT_SUCCESS;
    BackoffAlgorithmContext_t xReconnectParams = { 0 };
    BaseType_t xBackoffStatus = pdPASS;
    const char * pcAlpn[] = { MQTTOP_ALPN_PROTOCOL_NAME, NULL };

    configASSERT( pxNetworkContext != NULL );


    /* Initialize network context */
    pxNetworkContext->pParams = &transport_params;

    /* Setup network credentials */
    credentials.pRootCa = ( const unsigned char * ) DEMO_CONFIG_ROOT_CA_PEM;
    credentials.rootCaSize = sizeof( DEMO_CONFIG_ROOT_CA_PEM );
    credentials.pUserName = NULL;
    credentials.userNameSize = 0;
    credentials.pPassword = NULL;
    credentials.passwordSize = 0;
    credentials.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    credentials.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    credentials.disableSni = pdFALSE;
    if( DEMO_CONFIG_MQTT_BROKER_PORT == 443 )
    {
        /* Pass the ALPN protocol name depending on the port being used.
         * Please see more details about the ALPN protocol for AWS IoT MQTT endpoint
         * in the link below.
         * https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/
         */
        credentials.pAlpnProtos = pcAlpn;
    }
    else
    {
    	   credentials.pAlpnProtos = NULL;
    }

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       CONNECTION_RETRY_BACKOFF_BASE_MS,
                                       CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                       CONNECTION_RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after
     * a timeout. Timeout value will exponentially increase until maximum
     * attempts are reached.
     */
    do
    {
        /* Establish a TCP connection with the MQTT broker. This example connects to
         * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
         * DEMO_CONFIG_MQTT_BROKER_PORT at the top of this file. */
        IotLogInfo("Create a TCP connection to %s:%d.\r\n", democonfigMQTT_BROKER_ENDPOINT, DEMO_CONFIG_MQTT_BROKER_PORT);
        /* Connect to a MQTT host */
        xNetworkStatus = TLS_FreeRTOS_Connect(pxNetworkContext, democonfigMQTT_BROKER_ENDPOINT, DEMO_CONFIG_MQTT_BROKER_PORT, &credentials,
                SOCKETS_CONFIG_DEFAULT_RECV_TIMEOUT, SOCKETS_CONFIG_DEFAULT_SEND_TIMEOUT);

        if( xNetworkStatus != TLS_TRANSPORT_SUCCESS )
        {
            IotLogWarn("Connection to the broker failed. Attempting connection retry after backoff delay.");

            /* As the connection attempt failed, we will retry the connection after an
             * exponential backoff with jitter delay. */

            /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
            xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
        }
    } while( ( xNetworkStatus != TLS_TRANSPORT_SUCCESS ) && ( xBackoffStatus == pdPASS ) );

    return xNetworkStatus;
}

/*-----------------------------------------------------------*/

BaseType_t subscribeToAndRegisterTopicFilter( MQTTContext_t * pContext,
                                              const char * pTopicFilter,
                                              uint16_t topicFilterLength,
                                              SubscriptionManagerCallback_t callback )
{
    BaseType_t xReturnStatus = pdFAIL;
    SubscriptionManagerStatus_t managerStatus = 0u;

    /* Register the topic filter and its callback with subscription manager.
     * On an incoming PUBLISH message whose topic name that matches the topic filter
     * being registered, its callback will be invoked. */
    managerStatus = SubscriptionManager_RegisterCallback( pTopicFilter,
                                                          topicFilterLength,
                                                          callback );

    if( managerStatus == SUBSCRIPTION_MANAGER_SUCCESS )
    {
        IotLogInfo("Subscribing to the MQTT topic %s.\r\n", pTopicFilter);

        xReturnStatus = SubscribeToTopic( pContext,
                                         pTopicFilter,
                                         topicFilterLength);
    }

    if( xReturnStatus != pdPASS )
    {
        /* Remove the registered callback for the temperature topic filter as
        * the subscription operation for the topic filter did not succeed. */
        ( void ) SubscriptionManager_RemoveCallback( pTopicFilter,
                                                     topicFilterLength );
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t SubscribeToTopic( MQTTContext_t * pxMqttContext,
                             const char * pcTopicFilter,
                             uint16_t usTopicFilterLength)
{
    MQTTStatus_t                xMqttStatus = MQTTSuccess;
    BaseType_t                  xSubscribeStatus = pdFAIL;
    MQTTSubscribeInfo_t         pSubscriptionList[ 1 ];

    configASSERT( pxMqttContext != NULL );
    configASSERT( pcTopicFilter != NULL );
    configASSERT( usTopicFilterLength > 0 );

    /* Some fields not used so start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* Initialize the status of the subscription acknowledgement. */
    xTopicFilterContext.pcTopicFilter = pcTopicFilter;
    xTopicFilterContext.xSubAckStatus = MQTTSubAckFailure;

    /* Generate packet identifier for the SUBSCRIBE packet. */
    usSubscribePacketIdentifier = MQTT_GetPacketId( pxMqttContext );

    /* Subscribe to the #pcTopicFilter topic filter that is passed by a demo application. */
    pSubscriptionList[ 0 ].qos = MQTTQoS1;
    pSubscriptionList[ 0 ].pTopicFilter = pcTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = usTopicFilterLength;

    /* The client should now be connected to the broker. Subscribe to the topic
     * as specified in #pcTopicFilter by sending a subscribe packet. */
    IotLogInfo("Attempt to subscribe to the MQTT topic %s. Packet ID %d\r\n", pcTopicFilter, usSubscribePacketIdentifier);
    xMqttStatus = MQTT_Subscribe( pxMqttContext,
                                  pSubscriptionList,
                                  sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                  usSubscribePacketIdentifier );
    if( xMqttStatus != MQTTSuccess )
    {
        IotLogError("Failed to send SUBSCRIBE packet to broker with Status=%s.\r\n", MQTT_Status_strerror( xMqttStatus ));
    }
    else
    {
        IotLogInfo("SUBSCRIBE sent for topic %s to broker.\r\n", pcTopicFilter);

        /* Wait for acknowledgement packet (SUBACK) from the broker in response to the
         * subscribe request. */
        xMqttStatus = MQTT_ProcessLoop( pxMqttContext );
        if(suback_is_received)
        {
            xSubscribeStatus = pdPASS;
            suback_is_received = false;
        }
        else
        {
            if( xMqttStatus != MQTTSuccess )
            {
                IotLogError("MQTT_ProcessLoop failed: Status=%s.\r\n", MQTT_Status_strerror( xMqttStatus ));
            }
        }
    }

    return xSubscribeStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prvGetNextFreeIndexForOutgoingPublishes( uint8_t * pucIndex )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint8_t ucIndex = 0;

    assert( outgoingPublishPackets != NULL );
    assert( pucIndex != NULL );

    for( ucIndex = 0; ucIndex < MAX_OUTGOING_PUBLISHES; ucIndex++ )
    {
        /* A free ucIndex is marked by invalid packet id.
         * Check if the the ucIndex has a free slot. */
        if( outgoingPublishPackets[ ucIndex ].packetId == MQTT_PACKET_ID_INVALID )
        {
            xReturnStatus = pdPASS;
            break;
        }
    }

    /* Copy the available ucIndex into the output param. */
    *pucIndex = ucIndex;

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static void vCleanupOutgoingPublishAt( uint8_t ucIndex )
{
    assert( outgoingPublishPackets != NULL );
    assert( ucIndex < MAX_OUTGOING_PUBLISHES );

    /* Clear the outgoing publish packet. */
    ( void ) memset( &( outgoingPublishPackets[ ucIndex ] ),
                     0x00,
                     sizeof( outgoingPublishPackets[ ucIndex ] ) );
}

/*-----------------------------------------------------------*/

static void vCleanupOutgoingPublishes( void )
{
    assert( outgoingPublishPackets != NULL );

    /* Clean up all the outgoing publish packets. */
    ( void ) memset( outgoingPublishPackets, 0x00, sizeof( outgoingPublishPackets ) );
}

/*-----------------------------------------------------------*/

static void vCleanupOutgoingPublishWithPacketID( uint16_t usPacketId )
{
    uint8_t ucIndex = 0;

    assert( outgoingPublishPackets != NULL );
    assert( usPacketId != MQTT_PACKET_ID_INVALID );

    /* Clean up all the saved outgoing publishes. */
    for( ; ucIndex < MAX_OUTGOING_PUBLISHES; ucIndex++ )
    {
        if( outgoingPublishPackets[ ucIndex ].packetId == usPacketId )
        {
            vCleanupOutgoingPublishAt( ucIndex );
            IotLogInfo("Cleaned up outgoing publish packet with packet id %u.\r\n", usPacketId);
            break;
        }
    }
}

/*-----------------------------------------------------------*/

static BaseType_t handlePublishResend( MQTTContext_t * pxMqttContext )
{
    BaseType_t xReturnStatus = pdPASS;
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    uint8_t ucIndex = 0U;

    assert( outgoingPublishPackets != NULL );

    /* Resend all the QoS1 publishes still in the array. These are the
     * publishes that hasn't received a PUBACK. When a PUBACK is
     * received, the publish is removed from the array. */
    for( ucIndex = 0U; ucIndex < MAX_OUTGOING_PUBLISHES; ucIndex++ )
    {
        if( outgoingPublishPackets[ ucIndex ].packetId != MQTT_PACKET_ID_INVALID )
        {
            outgoingPublishPackets[ ucIndex ].pubInfo.dup = true;

            IotLogInfo("Sending duplicate PUBLISH with packet id %u.\r\n",
                       outgoingPublishPackets[ ucIndex ].packetId);
            eMqttStatus = MQTT_Publish( pxMqttContext,
                                        &outgoingPublishPackets[ ucIndex ].pubInfo,
                                        outgoingPublishPackets[ ucIndex ].packetId );

            if( eMqttStatus != MQTTSuccess )
            {
                IotLogError("Sending duplicate PUBLISH for packet id %u "
                            " failed with status %s.\r\n",
                            outgoingPublishPackets[ ucIndex ].packetId,
                            MQTT_Status_strerror( eMqttStatus ));
                xReturnStatus = pdFAIL;
                break;
            }
            else
            {
                IotLogInfo("Sent duplicate PUBLISH successfully for packet id %u.\r\n",
                           outgoingPublishPackets[ ucIndex ].packetId);
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static void commonEventHandler( MQTTContext_t * pMqttContext,
                                MQTTPacketInfo_t * pPacketInfo,
                                MQTTDeserializedInfo_t * pDeserializedInfo )
{
    MQTTStatus_t xResult = MQTTSuccess;
    uint8_t * pucPayload = NULL;
    size_t xSize = 0;

    configASSERT( pMqttContext != NULL );
    configASSERT( pPacketInfo != NULL );
    configASSERT( pDeserializedInfo != NULL );

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        configASSERT( pDeserializedInfo->pPublishInfo != NULL );
        /* Handle incoming publish. */
        SubscriptionManager_DispatchHandler( pMqttContext, pDeserializedInfo->pPublishInfo );
    }
    else
    {
        /* Handle other packets. */
        switch( pPacketInfo->type )
        {
            case MQTT_PACKET_TYPE_SUBACK:
                IotLogInfo("MQTT_PACKET_TYPE_SUBACK\r\n");
                /* A SUBACK from the broker, containing the server response to our subscription request, has been received.
                 * It contains the status code indicating server approval/rejection for the subscription to the single topic
                 * requested. The SUBACK will be parsed to obtain the status code, and this status code will be stored in global
                 * variable #xTopicFilterContext. */
                xResult = MQTT_GetSubAckStatusCodes( pPacketInfo, &pucPayload, &xSize );

                /* MQTT_GetSubAckStatusCodes always returns success if called with packet info
                 * from the event callback and non-NULL parameters. */
                configASSERT( xResult == MQTTSuccess );
                /* Only a single topic filter is expected for each SUBSCRIBE packet. */
                configASSERT( xSize == 1UL );
                xTopicFilterContext.xSubAckStatus = *pucPayload;

                if( xTopicFilterContext.xSubAckStatus != MQTTSubAckFailure )
                {
                    IotLogInfo("Subscribed to the topic %s with maximum QoS %u.\r\n",
                               xTopicFilterContext.pcTopicFilter,
                               xTopicFilterContext.xSubAckStatus);
                    IotLogInfo("ACK packet identifier %d.\r\n", pDeserializedInfo->packetIdentifier);
                }

                /* Make sure ACK packet identifier matches with Request packet identifier. */
                configASSERT ( usSubscribePacketIdentifier == pDeserializedInfo->packetIdentifier );
                suback_is_received = true;
                break;

            case MQTT_PACKET_TYPE_UNSUBACK:
                IotLogInfo("MQTT_PACKET_TYPE_UNSUBACK\r\n" );
                /* Make sure ACK packet identifier matches with Request packet identifier. */
                configASSERT ( usUnsubscribePacketIdentifier == pDeserializedInfo->packetIdentifier );
                unsuback_is_received = true;
                break;

            case MQTT_PACKET_TYPE_PINGRESP:

                /* Nothing to be done from application as library handles
                 * PINGRESP. */
                IotLogWarn("PINGRESP should not be handled by the application "
                        "callback when using MQTT_ProcessLoop.\r\n" );
                break;

            case MQTT_PACKET_TYPE_PUBACK:
                IotLogInfo("PUBACK received for packet id %u.\r\n", pDeserializedInfo->packetIdentifier);
                /* Cleanup publish packet when a PUBACK is received. */
                vCleanupOutgoingPublishWithPacketID( pDeserializedInfo->packetIdentifier );
                puback_is_received = true;
                break;

            /* Any other packet type is invalid. */
            default:
                IotLogError("Unknown packet type received:(%02x).\r\n", pPacketInfo->type);
        }
    }
}

/*-----------------------------------------------------------*/

BaseType_t EstablishMqttSession( MQTTContext_t * pxMqttContext,
                                 NetworkContext_t * pxNetworkContext,
                                 MQTTFixedBuffer_t * pxNetworkBuffer,
                                 bool createCleanSession)
{
    BaseType_t              xReturnStatus = pdPASS;
    MQTTStatus_t            eMqttStatus = MQTTSuccess;
    MQTTConnectInfo_t       xConnectInfo = { 0 };
    TransportInterface_t    xTransport;
    bool                    sessionPresent = false;

    assert( pxMqttContext != NULL );
    assert( pxNetworkContext != NULL );

    memcpy (democonfigMQTT_BROKER_ENDPOINT, g_mqtt_endpoint, strlen(g_mqtt_endpoint));
    memcpy (democonfigCLIENT_IDENTIFIER, g_iot_thing_name, strlen(g_iot_thing_name));
    /* Initialize the mqtt context and network context. */
    ( void ) memset( pxMqttContext, 0U, sizeof( MQTTContext_t ) );
    ( void ) memset( pxNetworkContext, 0U, sizeof( NetworkContext_t ) );

    /* Set the entry time of the demo application. This entry time will be used
     * to calculate relative time elapsed in the execution of the demo application,
     * by the timer utility function that is provided to the MQTT library.
     */
    ulGlobalEntryTimeMs = prvGetTimeMs();

    if( prvConnectToServerWithBackoffRetries( pxNetworkContext ) != TLS_TRANSPORT_SUCCESS )
    {
        /* Log error to indicate connection failure after all
         * reconnect attempts are over. */
        IotLogError("Failed to connect to MQTT broker %s.\r\n",
                    democonfigMQTT_BROKER_ENDPOINT);
        xReturnStatus = pdFAIL;
    }
    else
    {
        /* Fill in Transport Interface send and receive function pointers. */
        xTransport.pNetworkContext = pxNetworkContext;
        xTransport.send = TLS_FreeRTOS_send;
        xTransport.recv = TLS_FreeRTOS_recv;
        xTransport.writev = NULL;

        /* Initialize MQTT library. */
        eMqttStatus = MQTT_Init( pxMqttContext,
                                 &xTransport,
                                 prvGetTimeMs,
                                 commonEventHandler,
                                 pxNetworkBuffer );

        if( eMqttStatus != MQTTSuccess )
        {
            xReturnStatus = pdFAIL;
            IotLogError("MQTT init failed with status %s.\r\n",
                        MQTT_Status_strerror( eMqttStatus ));
        }
        else
        {
        	/* Initialize an MQTT context for QoS > 0 */
        	eMqttStatus = MQTT_InitStatefulQoS( pxMqttContext,
												pOutgoingPublishRecords,
												MQTT_EXAMPLE_OUTGOING_PUBLISH_RECORD_LEN,
												pIncomingPublishRecords,
												MQTT_EXAMPLE_INCOMING_PUBLISH_RECORD_LEN );

        	if( eMqttStatus != MQTTSuccess )
        	{
				xReturnStatus = pdFAIL;
				IotLogError("MQTT_InitStatefulQos failed with status %s.",
							MQTT_Status_strerror( eMqttStatus ));
        	}
            /* Establish MQTT session by sending a CONNECT packet. */
            /* Many fields not used in this demo so start with everything at 0. */
            ( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

            /* If #createCleanSession is true, start with a clean session
             * i.e. direct the MQTT broker to discard any previous session data.
             * If #createCleanSession is false, directs the broker to attempt to
             * reestablish a session which was already present. */
            xConnectInfo.cleanSession = createCleanSession;

            /* The client identifier is used to uniquely identify this MQTT client to
             * the MQTT broker. In a production device the identifier can be something
             * unique, such as a device serial number. */
            xConnectInfo.pClientIdentifier = democonfigCLIENT_IDENTIFIER;
            xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( democonfigCLIENT_IDENTIFIER );

            /* The maximum time interval in seconds which is allowed to elapse
             * between two Control Packets.
             * It is the responsibility of the Client to ensure that the interval between
             * Control Packets being sent does not exceed this Keep Alive value. In the
             * absence of sending any other Control Packets, the Client MUST send a
             * PINGREQ Packet. */
            xConnectInfo.keepAliveSeconds = KEEP_ALIVE_TIMEOUT_SECONDS;

            /* Username and password for authentication. Not used in this demo. */
#ifdef IO_USERNAME
            xConnectInfo.pUserName = IO_USERNAME;
            xConnectInfo.userNameLength = (uint16_t) strlen (IO_USERNAME);
            xConnectInfo.pPassword = IO_KEY;
            xConnectInfo.passwordLength = (uint16_t) strlen (IO_KEY);
#else
            xConnectInfo.pUserName = NULL;
			xConnectInfo.userNameLength = 0U;
			xConnectInfo.pPassword = NULL;
			xConnectInfo.passwordLength = 0U;
#endif /* IO_USERNAME */
            /* Send MQTT CONNECT packet to broker. */
            eMqttStatus = MQTT_Connect( pxMqttContext,
                                        &xConnectInfo,
                                        NULL,
                                        CONNACK_RECV_TIMEOUT_MS,
                                        &sessionPresent );

            if( eMqttStatus != MQTTSuccess )
            {
                xReturnStatus = pdFAIL;
                IotLogError("Connection with MQTT broker failed with status %s.\r\n",
                            MQTT_Status_strerror( eMqttStatus ));
            }
            else
            {
                IotLogInfo("MQTT connection successfully established with broker.\r\n");
            }
        }

        if( xReturnStatus == pdFAIL )
        {
            /* Keep a flag for indicating if MQTT session is established. This
             * flag will mark that an MQTT DISCONNECT has to be sent at the end
             * of the demo even if there are intermediate failures. */
            xMqttSessionEstablished = true;
        }

        if( xReturnStatus == pdFAIL )
        {
            /* Check if session is present and if there are any outgoing publishes
             * that need to resend. This is only valid if the broker is
             * re-establishing a session which was already present. */
            if( sessionPresent == true )
            {
                IotLogInfo("An MQTT session with broker is re-established.\r\n"
                           "Resending unacked publishes.\r\n");

                /* Handle all the resend of publish messages. */
                xReturnStatus = handlePublishResend( pxMqttContext );
            }
            else
            {
                IotLogInfo("A clean MQTT connection is established.\r\n"
                           "Cleaning up all the stored outgoing publishes.\r\n");

                /* Clean up the outgoing publishes waiting for ack as this new
                 * connection doesn't re-establish an existing session. */
                vCleanupOutgoingPublishes();
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t DisconnectMqttSession( MQTTContext_t * pxMqttContext,
                                  NetworkContext_t * pxNetworkContext )
{
    MQTTStatus_t eMqttStatus = MQTTSuccess;

    assert( pxMqttContext != NULL );
    assert( pxNetworkContext != NULL );

    if( xMqttSessionEstablished == true )
    {
        /* Send DISCONNECT. */
        eMqttStatus = MQTT_Disconnect( pxMqttContext );

        if( eMqttStatus != MQTTSuccess )
        {
            IotLogError("Sending MQTT DISCONNECT failed with status=%s.\r\n",
                        MQTT_Status_strerror( eMqttStatus ));
            return pdFAIL;
        }
    }

    /* Close the network connection.  */
    TLS_FreeRTOS_Disconnect( pxNetworkContext );
    return pdPASS;
}

/*-----------------------------------------------------------*/

BaseType_t UnsubscribeFromTopicFilters( MQTTContext_t * pxMqttContext,
                                        const MQTTSubscribeInfo_t * pTopicFilters,
                                        size_t numOfTopicFilters )
{
    BaseType_t      xReturnStatus = pdFAIL;
    MQTTStatus_t    xMqttStatus;
    size_t          index = 0U;

    configASSERT( pxMqttContext != NULL );
    configASSERT( pTopicFilters != NULL );
    configASSERT( numOfTopicFilters > 0 );

    /* Generate packet identifier for the UNSUBSCRIBE packet. */
    usUnsubscribePacketIdentifier = MQTT_GetPacketId( pxMqttContext );

    /* Send UNSUBSCRIBE packet. */
    xMqttStatus = MQTT_Unsubscribe( pxMqttContext,
                                    pTopicFilters,
                                    numOfTopicFilters,
                                    usUnsubscribePacketIdentifier  );

    if( xMqttStatus != MQTTSuccess )
    {
        IotLogError("Failed to send UNSUBSCRIBE packet to broker with error = %s.\r\n",
                    MQTT_Status_strerror( xMqttStatus ));
    }
    else
    {
        IotLogInfo("UNSUBSCRIBE sent for the following topic filter(s)\r\n");

        for( index = 0U; index < numOfTopicFilters; index++ )
        {
            IotLogInfo("%s\r\n", pTopicFilters[ index ].pTopicFilter);
        }

        xMqttStatus = MQTT_ProcessLoop( pxMqttContext );
        if(unsuback_is_received)
        {
            xReturnStatus = pdPASS;
            unsuback_is_received = false;
        }
        else
        {
            if( xMqttStatus != MQTTSuccess )
            {
                IotLogError("MQTT_ProcessLoop failed: Status=%s.\r\n", MQTT_Status_strerror( xMqttStatus ));
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t PublishToTopic( MQTTContext_t * pxMqttContext,
                           const char * pcTopicFilter,
                           uint16_t topicFilterLength,
                           const char * pMessage )
{
    BaseType_t xReturnStatus = pdFAIL;
    MQTTStatus_t xMqttStatus = MQTTSuccess;
    uint8_t ucPublishIndex = MAX_OUTGOING_PUBLISHES;

    assert( pxMqttContext != NULL );
    assert( pcTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    /* Get the next free index for the outgoing publish. All QoS1 outgoing
     * publishes are stored until a PUBACK is received. These messages are
     * stored for supporting a resend if a network connection is broken before
     * receiving a PUBACK. */
    xReturnStatus = prvGetNextFreeIndexForOutgoingPublishes( &ucPublishIndex );

    if( xReturnStatus == pdFAIL )
    {
        IotLogError("Unable to find a free spot for outgoing PUBLISH message.\r\n");
    }
    else
    {
        IotLogInfo("the published message:%s \r\n", pMessage);
        /* This example publishes to only one topic and uses QOS1. */
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.qos = MQTTQoS1;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.pTopicName = pcTopicFilter;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.topicNameLength = topicFilterLength;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.pPayload = pMessage;
        outgoingPublishPackets[ ucPublishIndex ].pubInfo.payloadLength = strlen( pMessage );

        /* Get a new packet id. */
        outgoingPublishPackets[ ucPublishIndex ].packetId = MQTT_GetPacketId( pxMqttContext );

        /* Send PUBLISH packet. */
        xMqttStatus = MQTT_Publish( pxMqttContext,
                                    &outgoingPublishPackets[ ucPublishIndex ].pubInfo,
                                    outgoingPublishPackets[ ucPublishIndex ].packetId );

        if( xMqttStatus != MQTTSuccess )
        {
            IotLogError("Failed to send PUBLISH packet to broker with error = %s.\r\n",
                        MQTT_Status_strerror( xMqttStatus ));
            vCleanupOutgoingPublishAt( ucPublishIndex );
        }
        else
        {
            IotLogInfo("PUBLISH sent for topic %s to broker with packet ID %u.\r\n",
                       pcTopicFilter,
                       outgoingPublishPackets[ ucPublishIndex ].packetId);

            /* Calling MQTT_ProcessLoop to process incoming publish echo, since
             * application subscribed to the same topic the broker will send
             * publish message back to the application. This function also
             * sends ping request to broker if MQTT_KEEP_ALIVE_INTERVAL_SECONDS
             * has expired since the last MQTT packet sent and receive
             * ping responses. */
            xMqttStatus = MQTT_ProcessLoop( pxMqttContext );
            if(puback_is_received)
            {
                xReturnStatus = pdPASS;
                puback_is_received = false;
            }
            else
            {
                if( xMqttStatus != MQTTSuccess )
                {
                    IotLogError("MQTT_ProcessLoop failed: Status=%s.\r\n", MQTT_Status_strerror( xMqttStatus ));
                }
            }
        }
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t ProcessLoop( MQTTContext_t * pxMqttContext )
{
    BaseType_t xReturnStatus = pdFAIL;
    MQTTStatus_t eMqttStatus = MQTTSuccess;

    eMqttStatus = MQTT_ProcessLoop( pxMqttContext );
    if( eMqttStatus == MQTTSuccess )
    {
        LogInfo("MQTT_ProcessLoop successful.");
        xReturnStatus = pdPASS;
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * MILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}

/*-----------------------------------------------------------*/

