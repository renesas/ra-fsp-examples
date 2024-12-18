/*
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 */

/*
 * This demo creates multiple tasks, all of which use the MQTT agent API to
 * communicate with an MQTT broker through the same MQTT connection.
 *
 * This file contains the initial task created after the TCP/IP stack connects
 * to the network.  The task:
 *
 * 1) Connects to the MQTT broker.
 * 2) Creates the other demo tasks, in accordance with the #defines set in
 *    demo_config.h.  For example, if demo_config.h contains the following
 *    settings:
 *
 *    #define democonfigCREATE_LARGE_MESSAGE_SUB_PUB_TASK     1
 *    #define democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE 3
 *
 *    then the initial task will create the task implemented in
 *    large_message_sub_pub_demo.c and three instances of the task
 *    implemented in simple_sub_pub_demo.c.  See the comments at the top
 *    of those files for more information.
 *
 * 3) After creating the demo tasks the initial task could create the MQTT
 *    agent task.  However, as it has no other operations to perform, rather
 *    than create the MQTT agent as a separate task the initial task just calls
 *    the agent's implementing function - effectively turning itself into the
 *    MQTT agent.
 */
#include "common_utils.h"
#include <usr_config.h>
/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"

/* Demo Specific configs. */
#include "demo_config.h"
#include "core_pkcs11_config.h"
#include "core_pkcs11_config_defaults.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* Transport interface implementation include header for TLS. */
#include "transport_mbedtls_pkcs11.h"
#include "aws_clientcredential.h"
#include "iot_default_root_certificates.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* MQTT agent include. */
#include "core_mqtt_agent.h"

/* MQTT Agent ports. */
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"

/* Exponential backoff retry include. */
#include "backoff_algorithm.h"

/* Transport interface header file. */
#include "transport_mbedtls_pkcs11.h"

/* Includes MQTT Agent Task management APIs. */
#include "mqtt_agent_task.h"
#include "backoff_algorithm.h"

#include "pkcs11_helpers.h"

/**
 * @brief The root CA certificate belonging to the broker.
 */
#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif

#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

#ifndef democonfigMQTT_BROKER_PORT

/**
 * @brief The port to use for the demo.
 */
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/**
 * @brief The maximum number of times to run the subscribe publish loop in this
 * demo.
 */
#ifndef democonfigMQTT_MAX_DEMO_COUNT
    #define democonfigMQTT_MAX_DEMO_COUNT    ( 3 )
#endif

/**
 * @brief Dimensions the buffer used to serialize and deserialize MQTT packets.
 * @note Specified in bytes.  Must be large enough to hold the maximum
 * anticipated MQTT payload.
 */
#ifndef MQTT_AGENT_NETWORK_BUFFER_SIZE
#define MQTT_AGENT_NETWORK_BUFFER_SIZE    ( 5000 )
#endif

/**
 * @brief Maximum number of subscriptions maintained by the MQTT agent in the subscription store.
 */
#ifndef MQTT_AGENT_MAX_SUBSCRIPTIONS
#define MQTT_AGENT_MAX_SUBSCRIPTIONS    15U
#endif

/**
 * @brief Timeout for receiving CONNACK after sending an MQTT CONNECT packet.
 * Defined in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS           ( 2000U )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                           ( BACKOFF_ALGORITHM_RETRY_FOREVER )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed operation
 *  with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS                   ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS                        ( 500U )

/**
 * @brief The maximum time interval in seconds which is allowed to elapse
 *  between two Control Packets.
 *
 *  It is the responsibility of the Client to ensure that the interval between
 *  Control Packets being sent does not exceed the this Keep Alive value. In the
 *  absence of sending any other Control Packets, the Client MUST send a
 *  PINGREQ Packet.
 *//*_RB_ Move to be the responsibility of the agent. */
#define mqttexampleKEEP_ALIVE_INTERVAL_SECONDS       ( 60U )

/**
 * @brief Socket send and receive timeouts to use.  Specified in milliseconds.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 1000 )

/**
 * @brief Configuration is used to turn on or off persistent sessions with MQTT broker.
 * If the flag is set to true, MQTT broker will remember the previous session so that a re
 * subscription to the topics are not required. Also any incoming publishes to subscriptions
 * will be stored by the broker and resend to device, when it comes back online.
 *
 */
#define mqttexamplePERSISTENT_SESSION_REQUIRED       ( 1 )

/**
 * @brief Used to convert times to/from ticks and milliseconds.
 */
#define mqttexampleMILLISECONDS_PER_SECOND           ( 1000U )
#define mqttexampleMILLISECONDS_PER_TICK             ( mqttexampleMILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief The MQTT agent manages the MQTT contexts.  This set the handle to the
 * context used by this demo.
 */
#define mqttexampleMQTT_CONTEXT_HANDLE               ( ( MQTTContextHandle_t ) 0 ) ''

/**
 * @brief Event Bit corresponding to an MQTT agent state.
 * The event bit is used to set the state bit in event group so that application tasks can
 * wait on a state transition.
 */
#define mqttexampleEVENT_BIT( xState )    ( ( EventBits_t ) ( 1UL << xState ) )

/**
 * @brief Mask to clear all set event bits for the MQTT agent state event group.
 * State event group is always cleared before setting the next state event bit so that only
 * state is set at anytime.
 */
#define mqttexampleEVENT_BITS_ALL    ( ( EventBits_t ) ( ( 1ULL << MQTT_AGENT_NUM_STATES ) - 1U ) )

/**
 * @brief ThingName which is used as the client identifier for MQTT connection.
 * Thing name is retrieved  at runtime from a key value store.
 */
static char * pcThingName = NULL;

/**
 * @brief Broker endpoint name for the MQTT connection.
 * Broker endpoint name is retrieved at runtime from a key value store.
 */
static char * pcBrokerEndpoint = NULL;

/**
 * @brief Root CA
 */
static char * pcRootCA = NULL;
/*-----------------------------------------------------------*/

/**
 * @brief An element in the list of topic filter subscriptions.
 */
typedef struct TopicFilterSubscription
{
    IncomingPubCallback_t pxIncomingPublishCallback;
    void * pvIncomingPublishCallbackContext;
    uint16_t usTopicFilterLength;
    const char * pcTopicFilter;
    BaseType_t xManageResubscription;
} TopicFilterSubscription_t;

/*-----------------------------------------------------------*/

static TlsTransportParams_t xTlsTransportParams;

/**
 * @brief Initializes an MQTT context, including transport interface and
 * network buffer.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`.
 */
static MQTTStatus_t prvMQTTInit( void );

/**
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param[in] xIsReconnect Boolean flag to indicate if this is a reconnection.
 * @return `MQTTSuccess` if connection succeeds, else appropriate error code
 * from MQTT_Connect.
 */
static MQTTStatus_t prvCreateMQTTConnection( bool xIsReconnect );

/**
 * @brief Connect a TCP socket to the MQTT broker.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if connection succeeds, else `pdFAIL`.
 */
static BaseType_t prvCreateTLSConnection( NetworkContext_t * pxNetworkContext );

/**
 * @brief Disconnect a TCP connection.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if disconnect succeeds, else `pdFAIL`.
 */
static BaseType_t prvDisconnectTLS( NetworkContext_t * pxNetworkContext );

/**
 * @brief Function to attempt to resubscribe to the topics already present in the
 * subscription list.
 *
 * This function will be invoked when this demo requests the broker to
 * reestablish the session and the broker cannot do so. This function will
 * enqueue commands to the MQTT Agent queue and will be processed once the
 * command loop starts.
 *
 * @return `MQTTSuccess` if adding subscribes to the command queue succeeds, else
 * appropriate error code from MQTTAgent_Subscribe.
 */
static MQTTStatus_t prvHandleResubscribe( void );

/**
 * @brief The callback invoked by MQTT agent for a response to SUBSCRIBE request.
 * Parameter indicates whether the request was successful or not. If subscribe was not successful
 * then callback removes the topic from the subscription store and displays a warning log.
 *
 *
 * @param pxCommandContext Pointer to the command context passed from caller
 * @param pxReturnInfo Return Info containing the result of the subscribe command.
 */
static void prvSubscriptionCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                            MQTTAgentReturnInfo_t * pxReturnInfo );

/**
 * @brief Fan out the incoming publishes to the callbacks registered by different
 * tasks. If there are no callbacks registered for the incoming publish, it will be
 * passed to the unsolicited publish handler.
 *
 * @param[in] pMqttAgentContext Agent context.
 * @param[in] packetId Packet ID of publish.
 * @param[in] pxPublishInfo Info of incoming publish.
 */
static void prvIncomingPublishCallback( MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo );


static bool prvMatchTopicFilterSubscriptions( MQTTPublishInfo_t * pxPublishInfo );


static void prvSetMQTTAgentState( MQTTAgentState_t xAgentState );

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

/**
 * @brief Connects a TCP socket to the MQTT broker, then creates and MQTT
 * connection to the same.
 * @param[in] xIsReconnect Boolean flag to indicate if its a reconnection.
 * @return MQTTConnected if connection was successful, MQTTNotConnected if MQTT connection
 *         failed and all retries exhausted.
 */
static MQTTConnectionStatus_t prvConnectToMQTTBroker( bool xIsReconnect );

/**
 * @brief Get the string value for a key from the KV store.
 * Memory allocated for the string should be freed by calling vPortFree.
 *
 * @return NULL if value not found, pointer to the NULL terminated string value
 *         if found.
 */
//static char * prvKVStoreGetString( KVStoreKey_t xKey );


static void prvMQTTAgentTask( void * pvParameters );
/*-----------------------------------------------------------*/

/**
 * @brief The network context used by the MQTT library transport interface.
 * See https://www.freertos.org/network-interface.html
 */
struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

static NetworkContext_t xNetworkContext;

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

MQTTAgentContext_t xGlobalMqttAgentContext;

static uint8_t xNetworkBuffer[ MQTT_AGENT_NETWORK_BUFFER_SIZE ];

static MQTTAgentMessageContext_t xCommandQueue;

static TopicFilterSubscription_t xTopicFilterSubscriptions[ MQTT_AGENT_MAX_SUBSCRIPTIONS ];

static SemaphoreHandle_t xSubscriptionsMutex;

/**
 * @brief Holds the current state of the MQTT agent.
 */
static MQTTAgentState_t xState = MQTT_AGENT_STATE_NONE;


/**
 * @brief Event group used by other tasks to synchronize with the MQTT agent states.
 */
static EventGroupHandle_t xStateEventGrp;

extern char g_mqtt_endpoint[128];
extern char g_iot_thing_name[128];

/*-----------------------------------------------------------*/

static MQTTStatus_t prvMQTTInit( void )
{
	TransportInterface_t xTransport;
    MQTTStatus_t xReturn;
    MQTTFixedBuffer_t xFixedBuffer = { .pBuffer = xNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE };
    static uint8_t staticQueueStorageArea[ MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof( MQTTAgentCommand_t * ) ];
    static StaticQueue_t staticQueueStructure;
    MQTTAgentMessageInterface_t messageInterface =
    {
        .pMsgCtx        = NULL,
        .send           = Agent_MessageSend,
        .recv           = Agent_MessageReceive,
        .getCommand     = Agent_GetCommand,
        .releaseCommand = Agent_ReleaseCommand
    };

    APP_DBG_PRINT( ( "Creating command queue. \r\n" ) );
    xCommandQueue.queue = xQueueCreateStatic( MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                              sizeof( MQTTAgentCommand_t * ),
                                              staticQueueStorageArea,
                                              &staticQueueStructure );
    configASSERT( xCommandQueue.queue );
    messageInterface.pMsgCtx = &xCommandQueue;

    /* Initialize the command pool. */
    Agent_InitializePool();

    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = &xNetworkContext;
    xTransport.send = TLS_FreeRTOS_send;
    xTransport.recv = TLS_FreeRTOS_recv;
    xTransport.writev = NULL;

    /* Initialize MQTT library. */
    xReturn = MQTTAgent_Init( &xGlobalMqttAgentContext,
                              &messageInterface,
                              &xFixedBuffer,
                              &xTransport,
                              prvGetTimeMs,
                              prvIncomingPublishCallback,
                              /* Context to pass into the callback. Passing the pointer to subscription array. */
                              NULL );

    return xReturn;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t prvCreateMQTTConnection( bool xIsReconnect )
{
    MQTTStatus_t xResult;
    MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent = false;

    /* Many fields are not used in this demo so start with everything at 0. */
    memset( &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

    /* Start with a clean session i.e. direct the MQTT broker to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
#if ( mqttexamplePERSISTENT_SESSION_REQUIRED == 1 )
    {
        xConnectInfo.cleanSession = false;
    }
#else
    {
        xConnectInfo.cleanSession = true;
    }
#endif

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier = pcThingName;
    xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( pcThingName );

    /* Set MQTT keep-alive period. It is the responsibility of the application
     * to ensure that the interval between Control Packets being sent does not
     * exceed the Keep Alive value. In the absence of sending any other Control
     * Packets, the Client MUST send a PINGREQ Packet.  This responsibility will
     * be moved inside the agent. */
    xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_INTERVAL_SECONDS;

    /* Append metrics when connecting to the AWS IoT Core broker. */
#ifdef democonfigUSE_AWS_IOT_CORE_BROKER
#ifdef democonfigCLIENT_USERNAME
    xConnectInfo.pUserName = CLIENT_USERNAME_WITH_METRICS;
    xConnectInfo.userNameLength = ( uint16_t ) strlen( CLIENT_USERNAME_WITH_METRICS );
    xConnectInfo.pPassword = democonfigCLIENT_PASSWORD;
    xConnectInfo.passwordLength = ( uint16_t ) strlen( democonfigCLIENT_PASSWORD );
#else
    xConnectInfo.pUserName = AWS_IOT_METRICS_STRING;
    xConnectInfo.userNameLength = AWS_IOT_METRICS_STRING_LENGTH;
    /* Password for authentication is not used. */
    xConnectInfo.pPassword = NULL;
    xConnectInfo.passwordLength = 0U;
#endif
#else /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */
#ifdef democonfigCLIENT_USERNAME
    xConnectInfo.pUserName = democonfigCLIENT_USERNAME;
    xConnectInfo.userNameLength = ( uint16_t ) strlen( democonfigCLIENT_USERNAME );
    xConnectInfo.pPassword = democonfigCLIENT_PASSWORD;
    xConnectInfo.passwordLength = ( uint16_t ) strlen( democonfigCLIENT_PASSWORD );
#endif /* ifdef democonfigCLIENT_USERNAME */
#endif /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */

    APP_PRINT( "Creating an MQTT connection to the broker. \r\n" );

    /* Send MQTT CONNECT packet to broker. MQTT's Last Will and Testament feature
     * is not used in this demo, so it is passed as NULL. */
    xResult = MQTT_Connect( &( xGlobalMqttAgentContext.mqttContext ),
                            &xConnectInfo,
                            NULL,
                            mqttexampleCONNACK_RECV_TIMEOUT_MS,
                            &xSessionPresent );

    if( ( xResult == MQTTSuccess ) && ( xIsReconnect == true ) )
    {
        APP_PRINT( "Resuming previous MQTT session with broker. \r\n" );
        xResult = MQTTAgent_ResumeSession( &xGlobalMqttAgentContext, xSessionPresent );

        if( ( xResult == MQTTSuccess ) && ( xSessionPresent == false ) )
        {
            /* Resubscribe to all the subscribed topics. */
            xResult = prvHandleResubscribe();
        }
    }

    return xResult;
}
/*-----------------------------------------------------------*/

static BaseType_t prvCreateTLSConnection( NetworkContext_t * pxNetworkContext )
{
    BaseType_t xConnected = pdFAIL;

    TlsTransportStatus_t xNetworkStatus = TLS_TRANSPORT_CONNECT_FAILURE;
    NetworkCredentials_t xNetworkCredentials = { 0 };
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams = { 0 };
    uint16_t usNextRetryBackOff = 0U;

#ifdef democonfigUSE_AWS_IOT_CORE_BROKER

    /* ALPN protocols must be a NULL-terminated list of strings. Therefore,
     * the first entry will contain the actual ALPN protocol string while the
     * second entry must remain NULL. */
    const char * pcAlpnProtocols[] = { NULL, NULL };

    /* The ALPN string changes depending on whether username/password authentication is used. */
#ifdef democonfigCLIENT_USERNAME
    pcAlpnProtocols[ 0 ] = AWS_IOT_CUSTOM_AUTH_ALPN;
#else
    pcAlpnProtocols[ 0 ] = AWS_IOT_MQTT_ALPN;
#endif
    xNetworkCredentials.pAlpnProtos = pcAlpnProtocols;
#endif /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */

    /* Set the credentials for establishing a TLS connection. */
    xNetworkCredentials.pRootCa = ( const unsigned char * ) pcRootCA;
    xNetworkCredentials.rootCaSize = strlen( pcRootCA ) + 1;
    xNetworkCredentials.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    xNetworkCredentials.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;


    xNetworkCredentials.disableSni = democonfigDISABLE_SNI;
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                        RETRY_BACKOFF_BASE_MS,
                                        RETRY_MAX_BACKOFF_DELAY_MS,
                                        RETRY_MAX_ATTEMPTS );

    /* Establish a TCP connection with the MQTT broker. This example connects to
     * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
     * democonfigMQTT_BROKER_PORT at the top of this file. */

    uint32_t ulRandomNum = 0;
	do
	{
	APP_PRINT( "Creating a TLS connection to %s:%u.\r\n",
			pcBrokerEndpoint,
			democonfigMQTT_BROKER_PORT);
	xNetworkStatus = TLS_FreeRTOS_Connect( pxNetworkContext,
                                               pcBrokerEndpoint,
                                               democonfigMQTT_BROKER_PORT,
                                               &xNetworkCredentials,
                                           mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS,
                                           mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS );

    xConnected = ( xNetworkStatus == TLS_TRANSPORT_SUCCESS ) ? pdPASS : pdFAIL;

		if( !xConnected )
		{
			/* Get back-off value (in milliseconds) for the next connection retry. */
			if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
												 sizeof( ulRandomNum ) ) == pdPASS )
			{
			xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, ulRandomNum, &usNextRetryBackOff );
			}

			if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
			{
				APP_WARN_PRINT( "Connection to the broker failed. "
						   "Retrying connection in %hu ms. \r\n",
						   usNextRetryBackOff );
				vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );
			}
		}

		if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
		{
			APP_ERR_PRINT( "Connection to the broker failed, all attempts exhausted. \r\n" );
		}
	} while( ( xConnected != pdPASS ) && ( xBackoffAlgStatus == BackoffAlgorithmSuccess ) );

    return xConnected;
}

/*-----------------------------------------------------------*/

static BaseType_t prvDisconnectTLS( NetworkContext_t * pxNetworkContext )
{
    APP_PRINT( "Disconnecting TLS connection.\n" );
    TLS_FreeRTOS_Disconnect( pxNetworkContext );
    return pdPASS;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t prvHandleResubscribe( void )
{
    MQTTStatus_t xResult = MQTTBadParameter;
    uint32_t ulIndex = 0U;
    uint16_t usNumSubscriptions = 0U;

    /* These variables need to stay in scope until command completes. */
    static MQTTAgentSubscribeArgs_t xSubArgs = { 0 };
    static MQTTSubscribeInfo_t xSubInfo[ MQTT_AGENT_MAX_SUBSCRIPTIONS ] = { 0 };
    static MQTTAgentCommandInfo_t xCommandParams = { 0 };

    /* Loop through each subscription in the subscription list and add a subscribe
     * command to the command queue. */
    xSemaphoreTake( xSubscriptionsMutex, portMAX_DELAY );
    {
        for( ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++ )
        {
            /* Check if there is a subscription in the subscription list. This demo
             * doesn't check for duplicate subscriptions. */
            if( ( xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength > 0 ) &&
                ( xTopicFilterSubscriptions[ ulIndex ].xManageResubscription == pdTRUE ) )
            {
                xSubInfo[ usNumSubscriptions ].pTopicFilter = xTopicFilterSubscriptions[ ulIndex ].pcTopicFilter;
                xSubInfo[ usNumSubscriptions ].topicFilterLength = xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength;

                /* QoS1 is used for all the subscriptions in this demo. */
                xSubInfo[ usNumSubscriptions ].qos = MQTTQoS1;

                APP_PRINT( "Resubscribe to the topic %s will be attempted. \r\n",
                           xSubInfo[ usNumSubscriptions ].pTopicFilter );

                usNumSubscriptions++;
            }
        }
    }
    xSemaphoreGive( xSubscriptionsMutex );

    if( usNumSubscriptions > 0U )
    {
        xSubArgs.pSubscribeInfo = xSubInfo;
        xSubArgs.numSubscriptions = usNumSubscriptions;

        /* The block time can be 0 as the command loop is not running at this point. */
        xCommandParams.blockTimeMs = 0U;
        xCommandParams.cmdCompleteCallback = prvSubscriptionCommandCallback;
        xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xSubArgs;

        /* Enqueue subscribe to the command queue. These commands will be processed only
         * when command loop starts. */
        xResult = MQTTAgent_Subscribe( &xGlobalMqttAgentContext, &xSubArgs, &xCommandParams );
    }
    else
    {
        /* Mark the resubscribe as success if there is nothing to be subscribed. */
        xResult = MQTTSuccess;
    }

    if( xResult != MQTTSuccess )
    {
        APP_ERR_PRINT( "Failed to enqueue the MQTT subscribe command. xResult=%s \r\n",
                    MQTT_Status_strerror( xResult ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static void prvSubscriptionCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                            MQTTAgentReturnInfo_t * pxReturnInfo )
{
    uint32_t ulIndex = 0;
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs = ( MQTTAgentSubscribeArgs_t * ) pxCommandContext;

    /* If the return code is success, no further action is required as all the topic filters
     * are already part of the subscription list. */
    if( pxReturnInfo->returnCode != MQTTSuccess )
    {
        /* Check through each of the suback codes and determine if there are any failures. */
        for( ulIndex = 0; ulIndex < pxSubscribeArgs->numSubscriptions; ulIndex++ )
        {
            /* This demo doesn't attempt to resubscribe in the event that a SUBACK failed. */
            if( pxReturnInfo->pSubackCodes[ ulIndex ] == MQTTSubAckFailure )
            {
                APP_ERR_PRINT( "Failed to resubscribe to topic %s \r\n",pxSubscribeArgs->pSubscribeInfo[ ulIndex ].pTopicFilter);
            }
        }

        /* Hit an assert as some of the tasks won't be able to proceed correctly without
         * the subscriptions. This logic will be updated with exponential backoff and retry.  */
        configASSERT( pdTRUE );
    }
}

/*-----------------------------------------------------------*/

static void prvIncomingPublishCallback( MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo )
{
    bool xPublishHandled = false;
    char cOriginalChar;
    char * pcLocation;
    ( void ) pMqttAgentContext;
    ( void ) packetId;

    /* Fan out the incoming publishes to the callbacks registered using
     * subscription manager. */
    xPublishHandled = prvMatchTopicFilterSubscriptions( pxPublishInfo );

    /* If there are no callbacks to handle the incoming publishes,
     * handle it as an unsolicited publish. */
    if( xPublishHandled != true )
    {
        /* Ensure the topic string is terminated for printing.  This will over-
         * write the message ID, which is restored afterwards. */
        pcLocation = ( char * ) &( pxPublishInfo->pTopicName[ pxPublishInfo->topicNameLength ] );
        cOriginalChar = *pcLocation;
        *pcLocation = 0x00;
        APP_WARN_PRINT( "WARN:  Received an unsolicited publish from topic %s \r\n", pxPublishInfo->pTopicName );
        *pcLocation = cOriginalChar;
    }
}

/*-----------------------------------------------------------*/
void vStartMQTTAgent( configSTACK_DEPTH_TYPE uxStackSize,
        UBaseType_t uxPriority  )
{
    xTaskCreate( prvMQTTAgentTask,
                  "MQTT",
                  uxStackSize,
                  NULL,
                  uxPriority,
                  NULL );
}


void prvMQTTAgentTask( void * pvParameters )
{
    BaseType_t xStatus = pdPASS;
    MQTTStatus_t xMQTTStatus = MQTTBadParameter;
    MQTTContext_t * pMqttContext = &( xGlobalMqttAgentContext.mqttContext );
    ( void ) pvParameters;

    ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_INITIALIZED, portMAX_DELAY );
    APP_PRINT( "---------Start MQTT Agent Task---------\r\n" );

    /* Initialization of timestamp for MQTT. */
    ulGlobalEntryTimeMs = prvGetTimeMs();

    /* Save the thing name, broker endpoint, and root CA to the corresponding variables */
    pcThingName = g_iot_thing_name;
    pcBrokerEndpoint = g_mqtt_endpoint;
    pcRootCA = democonfigROOT_CA_PEM;

    /* Initialize the MQTT context with the buffer and transport interface. */
    if( xStatus == pdPASS )
    {
        xMQTTStatus = prvMQTTInit();

        if( xMQTTStatus != MQTTSuccess )
        {
            APP_ERR_PRINT("Failed to initialize MQTT with error %d. \r\n", xMQTTStatus);
        }
    }

    if( xMQTTStatus == MQTTSuccess )
    {
        pMqttContext->connectStatus = prvConnectToMQTTBroker( false );

        while( pMqttContext->connectStatus == MQTTConnected )
        {
            /* MQTTAgent_CommandLoop() is effectively the agent implementation.  It
             * will manage the MQTT protocol until such time that an error occurs,
             * which could be a disconnect.  If an error occurs the MQTT context on
             * which the error happened is returned so there is an attempt to
             * clean up and reconnect. */
            prvSetMQTTAgentState( MQTT_AGENT_STATE_CONNECTED );

            xMQTTStatus = MQTTAgent_CommandLoop( &xGlobalMqttAgentContext );

            pMqttContext->connectStatus = MQTTNotConnected;
            prvSetMQTTAgentState( MQTT_AGENT_STATE_DISCONNECTED );

            if( xMQTTStatus == MQTTSuccess )
            {
                /*
                 * On a graceful termination, MQTT agent loop returns success.
                 * Cancel all pending MQTT agent requests.
                 * Disconnect the socket and terminate MQTT agent loop.
                 */
                APP_PRINT( "MQTT Agent loop terminated due to a graceful disconnect. \r\n" );
                ( void ) MQTTAgent_CancelAll( &xGlobalMqttAgentContext );
                ( void ) prvDisconnectTLS( &xNetworkContext );
            }
            else
            {
                APP_PRINT( "MQTT Agent loop terminated due to abrupt disconnect. Retrying MQTT connection.. \r\n" );
                /* MQTT agent returned due to an underlying error, reconnect to the loop. */
                ( void ) prvDisconnectTLS( &xNetworkContext );
                pMqttContext->connectStatus = prvConnectToMQTTBroker( true );
            }
        }
    }

    prvSetMQTTAgentState( MQTT_AGENT_STATE_TERMINATED );
    if( pcThingName != NULL )
   {
	   vPortFree( pcThingName );
	   pcThingName = NULL;
   }

   if( pcBrokerEndpoint != NULL )
   {
	   vPortFree( pcBrokerEndpoint );
	   pcBrokerEndpoint = NULL;
   }

    if (pcRootCA != NULL)
    {
        vPortFree( pcRootCA );
        pcRootCA = NULL;
    }

   APP_PRINT( "---------MQTT Agent Task Finished---------\r\n");
   vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

static MQTTConnectionStatus_t prvConnectToMQTTBroker( bool xIsReconnect )
{
    BaseType_t xStatus = pdFAIL;
    MQTTStatus_t xMQTTStatus;
    MQTTConnectionStatus_t xConnectionStatus = MQTTNotConnected;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams = { 0 };
    uint16_t usNextRetryBackOff = 0U;
/* Initialize network context. */
    xNetworkContext.pParams = &xTlsTransportParams;

    /* We will use a retry mechanism with an exponential backoff mechanism and
     * jitter.  That is done to prevent a fleet of IoT devices all trying to
     * reconnect at exactly the same time should they become disconnected at
     * the same time. We initialize reconnect attempts and interval here. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       RETRY_BACKOFF_BASE_MS,
                                       RETRY_MAX_BACKOFF_DELAY_MS,
                                       RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to MQTT broker. If connection fails, retry after a
     * timeout. Timeout value will exponentially increase until the maximum
     * number of attempts are reached.
     */
    do
    {
        /* Create a TLS connection to broker */
        xStatus = prvCreateTLSConnection( &xNetworkContext );

        if( xStatus == pdPASS )
        {
            xMQTTStatus = prvCreateMQTTConnection( xIsReconnect );

            if( xMQTTStatus != MQTTSuccess )
            {
                APP_ERR_PRINT( "Failed to connect to MQTT broker, error = %u \r\n", xMQTTStatus );
                prvDisconnectTLS( &xNetworkContext );
                xStatus = pdFAIL;
            }
            else
            {
                APP_PRINT( "Successfully connected to MQTT broker. \r\n" );
                xConnectionStatus = MQTTConnected;
            }
        }

        if( xStatus == pdFAIL )
        {
            /* Get back-off value (in milliseconds) for the next connection retry. */
            xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, xTaskGetTickCount(), &usNextRetryBackOff );

            if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
            {
                APP_WARN_PRINT("Connection to the broker failed. "
                           "Retrying connection in %hu ms. \r\n",
                           usNextRetryBackOff);
                vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );
            }
            else if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                APP_ERR_PRINT( ( "Connection to the broker failed, all attempts exhausted." ) );
            }
            else
            {
                /* Empty Else. */
            }
        }
    } while( ( xConnectionStatus == MQTTNotConnected ) && ( xBackoffAlgStatus == BackoffAlgorithmSuccess ) );

    return xConnectionStatus;
}
/*-----------------------------------------------------------*/

static uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * mqttexampleMILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}
/*-----------------------------------------------------------*/

static bool prvMatchTopicFilterSubscriptions( MQTTPublishInfo_t * pxPublishInfo )
{
    uint32_t ulIndex = 0;
    bool isMatched = false;
    bool publishHandled = false;

    xSemaphoreTake( xSubscriptionsMutex, portMAX_DELAY );
    {
        for( ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++ )
        {
            if( xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength > 0 )
            {
                MQTT_MatchTopic( pxPublishInfo->pTopicName,
                                 pxPublishInfo->topicNameLength,
                                 xTopicFilterSubscriptions[ ulIndex ].pcTopicFilter,
                                 xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength,
                                 &isMatched );

                if( isMatched == true )
                {
                    xTopicFilterSubscriptions[ ulIndex ].pxIncomingPublishCallback( xTopicFilterSubscriptions[ ulIndex ].pvIncomingPublishCallbackContext,
                                                                                    pxPublishInfo );
                    publishHandled = true;
                }
            }
        }
    }
    xSemaphoreGive( xSubscriptionsMutex );
    return publishHandled;
}

/*-----------------------------------------------------------*/

static void prvSetMQTTAgentState( MQTTAgentState_t xAgentState )
{
    xState = xAgentState;
    ( void ) xEventGroupClearBits( xStateEventGrp, mqttexampleEVENT_BITS_ALL );
    ( void ) xEventGroupSetBits( xStateEventGrp, mqttexampleEVENT_BIT( xAgentState ) );
}

/*-----------------------------------------------------------*/

BaseType_t xMQTTAgentInit( void )
{
    BaseType_t xResult = pdFAIL;

    if( xState == MQTT_AGENT_STATE_NONE )
    {
        xSubscriptionsMutex = xSemaphoreCreateMutex();

        if( xSubscriptionsMutex != NULL )
        {
            xResult = pdPASS;
        }

        if( xResult == pdPASS )
        {
            xStateEventGrp = xEventGroupCreate();

            if( xStateEventGrp == NULL )
            {
                xResult = pdFAIL;
            }
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

MQTTAgentState_t xGetMQTTAgentState( void )
{
    return xState;
}

void xSetMQTTAgentState( MQTTAgentState_t xAgentState )
{
    prvSetMQTTAgentState(xAgentState);
}

/*-----------------------------------------------------------*/

BaseType_t xWaitForMQTTAgentState( MQTTAgentState_t xState_s,
                                   TickType_t xTicksToWait )
{
    EventBits_t xBitsSet;
    EventBits_t xBitsToWaitFor;
    BaseType_t xResult = pdFAIL;

    if( xState_s != MQTT_AGENT_STATE_NONE )
    {
        xBitsToWaitFor = mqttexampleEVENT_BIT( xState_s );
        xBitsSet = xEventGroupWaitBits( xStateEventGrp, xBitsToWaitFor, pdFALSE, pdFALSE, xTicksToWait );

        if( ( xBitsSet & xBitsToWaitFor ) != 0 )
        {
            xResult = pdTRUE;
        }
    }

    return xResult;
}

/*-----------------------------------------------------------------*/

BaseType_t xAddMQTTTopicFilterCallback( const char * pcTopicFilter,
                                        uint16_t usTopicFilterLength,
                                        IncomingPubCallback_t pxCallback,
                                        void * pvCallbackContext,
                                        BaseType_t xManageResubscription )
{
    BaseType_t xResult = pdFAIL;
    uint32_t ulIndex = 0U;
    uint32_t ulAvailableIndex = MQTT_AGENT_MAX_SUBSCRIPTIONS;

    xSemaphoreTake( xSubscriptionsMutex, portMAX_DELAY );
    {
        /**
         * If this is a duplicate subscription for same topic filter do nothing and return a failure.
         * Else insert at the first available index;
         */
        for( ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++ )
        {
            if( ( xTopicFilterSubscriptions[ ulIndex ].pcTopicFilter == NULL ) &&
                ( ulAvailableIndex == MQTT_AGENT_MAX_SUBSCRIPTIONS ) )
            {
                ulAvailableIndex = ulIndex;
            }
            else if( ( xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength == usTopicFilterLength ) &&
                     ( strncmp( pcTopicFilter, xTopicFilterSubscriptions[ ulIndex ].pcTopicFilter, ( size_t ) usTopicFilterLength ) == 0 ) )
            {
                /* If a subscription already exists, don't do anything. */
                if( ( xTopicFilterSubscriptions[ ulAvailableIndex ].pxIncomingPublishCallback == pxCallback ) &&
                    ( xTopicFilterSubscriptions[ ulAvailableIndex ].pvIncomingPublishCallbackContext == pvCallbackContext ) )
                {
                    ulAvailableIndex = MQTT_AGENT_MAX_SUBSCRIPTIONS;
                    xResult = pdPASS;
                    break;
                }
            }
        }

        if( ulAvailableIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS )
        {
            xTopicFilterSubscriptions[ ulAvailableIndex ].pcTopicFilter = pcTopicFilter;
            xTopicFilterSubscriptions[ ulAvailableIndex ].usTopicFilterLength = usTopicFilterLength;
            xTopicFilterSubscriptions[ ulAvailableIndex ].pxIncomingPublishCallback = pxCallback;
            xTopicFilterSubscriptions[ ulAvailableIndex ].pvIncomingPublishCallbackContext = pvCallbackContext;
            xTopicFilterSubscriptions[ ulAvailableIndex ].xManageResubscription = xManageResubscription;
            xResult = pdPASS;
        }
    }
    xSemaphoreGive( xSubscriptionsMutex );

    return xResult;
}
/*-----------------------------------------------------------------*/

void vRemoveMQTTTopicFilterCallback( const char * pcTopicFilter,
                                     uint16_t usTopicFilterLength )
{
    uint32_t ulIndex;

    xSemaphoreTake( xSubscriptionsMutex, portMAX_DELAY );
    {
        for( ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++ )
        {
            if( xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength == usTopicFilterLength )
            {
                if( strncmp( xTopicFilterSubscriptions[ ulIndex ].pcTopicFilter, pcTopicFilter, usTopicFilterLength ) == 0 )
                {
                    memset( &( xTopicFilterSubscriptions[ ulIndex ] ), 0x00, sizeof( TopicFilterSubscription_t ) );
                }
            }
        }
    }
    xSemaphoreGive( xSubscriptionsMutex );
}
