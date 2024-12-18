/*
 * FreeRTOS V202112.00
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
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/**
 * @file OtaOverMqttDemoExample.c
 * @brief Over The Air Update demo using coreMQTT Agent.
 *
 * The file demonstrates how to perform Over The Air update using OTA agent and coreMQTT Agent
 * library. It creates an OTA agent task which manages the OTA firmware update
 * for the device. The example also provides implementations to subscribe, publish,
 * and receive data from an MQTT broker. The implementation uses coreMQTT agent which manages
 * thread safety of the MQTT operations and allows OTA agent to share the same MQTT
 * broker connection with other tasks. OTA agent invokes the callback implementations to
 * publish job related control information, as well as receive chunks
 * of pre-signed firmware image from the MQTT broker.
 *
 * See https://freertos.org/mqtt/mqtt-agent-demo.html
 * See https://freertos.org/ota/ota-mqtt-agent-demo.html
 */

/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "common_utils.h"
#include <usr_config.h>


/* Demo config includes. */
#include "demo_config.h"

/* Library config includes. */
#include "ota_config.h"
#include "demo_config.h"

/* MQTT library includes. */
#include "core_mqtt_agent.h"

/* OTA Library include. */
#include "ota.h"

/* OTA Library Interface include. */
#include "ota_os_freertos.h"
#include "ota_mqtt_interface.h"

/* Include firmware version struct definition. */
#include "ota_appversion32.h"

/* Include platform abstraction header. */
#include "ota_pal.h"

#include "mqtt_agent_task.h"

/*------------- Demo configurations -------------------------*/

/**
 * @brief The maximum size of the file paths used in the demo.
 */
#define otaexampleMAX_FILE_PATH_SIZE                     ( 260 )

/**
 * @brief The maximum size of the stream name required for downloading update file
 * from streaming service.
 */
#define otaexampleMAX_STREAM_NAME_SIZE                   ( 128 )

/**
 * @brief The delay used in the OTA demo task to periodically output the OTA
 * statistics like number of packets received, dropped, processed and queued per connection.
 */
#define otaexampleTASK_DELAY_MS                          ( 1000U )
/**
 * @brief The maximum amount of time in milliseconds to wait for the commands
 * to be posted to the MQTT agent should the MQTT agent's command queue be full.
 * Tasks wait in the Blocked state, so don't use any CPU time.
 */
#define MQTT_AGENT_SEND_BLOCK_TIME_MS               	 ( 200U )

/**
 * @brief The common prefix for all OTA topics.
 *
 * Thing name is substituted with a wildcard symbol `+`. OTA agent
 * registers with MQTT broker with the thing name in the topic. This topic
 * filter is used to match incoming packet received and route them to OTA.
 * Thing name is not needed for this matching.
 */
#define OTA_TOPIC_PREFIX                                 "$aws/things/+/"

/**
 * @brief Wildcard topic filter for job notification.
 * The filter is used to match the constructed job notify topic filter from OTA agent and register
 * appropriate callback for it.
 */
#define OTA_JOB_NOTIFY_TOPIC_FILTER                      OTA_TOPIC_PREFIX "jobs/notify-next"

/**
 * @brief Length of job notification topic filter.
 */
#define OTA_JOB_NOTIFY_TOPIC_FILTER_LENGTH               ( ( uint16_t ) ( sizeof( OTA_JOB_NOTIFY_TOPIC_FILTER ) - 1 ) )

/**
 * @brief Wildcard topic filter for matching job response messages.
 * This topic filter is used to match the responses from OTA service for OTA agent job requests. THe
 * topic filter is a reserved topic which is not subscribed with MQTT broker.
 *
 */
#define OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER           OTA_TOPIC_PREFIX "jobs/$next/get/accepted"

/**
 * @brief Length of job accepted response topic filter.
 */
#define OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Wildcard topic filter for matching OTA data packets.
 *  The filter is used to match the constructed data stream topic filter from OTA agent and register
 * appropriate callback for it.
 */
#define OTA_DATA_STREAM_TOPIC_FILTER           OTA_TOPIC_PREFIX  "streams/#"

/**
 * @brief Length of data stream topic filter.
 */
#define OTA_DATA_STREAM_TOPIC_FILTER_LENGTH    ( ( uint16_t ) ( sizeof( OTA_DATA_STREAM_TOPIC_FILTER ) - 1 ) )


/**
 * @brief Starting index of client identifier within OTA topic.
 */
#define OTA_TOPIC_CLIENT_IDENTIFIER_START_IDX    ( 12U )

/**
 * @brief Used to clear bits in a task's notification value.
 */
#define otaexampleMAX_UINT32                     ( 0xffffffff )


/**
 * @brief This demo uses task notifications to signal tasks from MQTT callback
 * functions.  mqttexampleMS_TO_WAIT_FOR_NOTIFICATION defines the time, in ticks,
 * to wait for such a callback.
 */
#define MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION      ( 5000U )

/**
 * @brief Stack size required for OTA agent task.
 */
#define OTA_AGENT_TASK_STACK_SIZE                   ( 5000U )

/**
 * @brief Priority required for OTA agent task.
 */
#define OTA_AGENT_TASK_PRIORITY                     ( tskIDLE_PRIORITY )

/**
 * @brief The timeout for waiting for the agent to get suspended after closing the
 * connection.
 *
 * Timeout value should be large enough for OTA agent to finish any pending MQTT operations
 * and suspend itself.
 *
 */
#define OTA_SUSPEND_TIMEOUT_MS                      ( 10000U )

/**
 * @brief ThingName which is used as the client identifier for MQTT connection.
 * Thing name is retrieved  at runtime from a key value store.
 */
static char * pcThingName = NULL;
static size_t xThingNameLength = 0U;

/**
 * @brief Defines the structure to use as the command callback context in this
 * demo.
 */
struct MQTTAgentCommandContext
{
    TaskHandle_t xTaskToNotify;
    void * pArgs;
};


/**
 * @brief Buffer used to store the firmware image file path.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t updateFilePath[ otaexampleMAX_FILE_PATH_SIZE ];

/**
 * @brief Buffer used to store the code signing certificate file path.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t certFilePath[ otaexampleMAX_FILE_PATH_SIZE ];

/**
 * @brief Buffer used to store the name of the data stream.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t streamName[ otaexampleMAX_STREAM_NAME_SIZE ];

/**
 * @brief Buffer used decode the CBOR message from the MQTT payload.
 * Buffer is passed to the OTA agent during initialization.
 */
static uint8_t decodeMem[ ( 1U << otaconfigLOG2_FILE_BLOCK_SIZE ) ];

/**
 * @brief Application buffer used to store the bitmap for requesting firmware image
 * chunks from MQTT broker. Buffer is passed to the OTA agent during initialization.
 */
static uint8_t bitmap[ OTA_MAX_BLOCK_BITMAP_SIZE ];

/**
 * @brief A statically allocated array of event buffers used by the OTA agent.
 * Maximum number of buffers are determined by how many chunks are requested
 * by OTA agent at a time along with an extra buffer to handle control message.
 * The size of each buffer is determined by the maximum size of firmware image
 * chunk, and other metadata send along with the chunk.
 */
static OtaEventData_t eventBuffer[ otaconfigMAX_NUM_OTA_DATA_BUFFERS ] = { 0 };

/*
 * @brief Mutex used to manage thread safe access of OTA event buffers.
 */
static SemaphoreHandle_t xBufferSemaphore;

/**
 * @brief Static handle used for MQTT agent context.
 */
extern MQTTAgentContext_t xGlobalMqttAgentContext;
extern char g_iot_thing_name[128];
/*---------------------------------------------------------*/

/**
 * @brief Function used by OTA agent to publish control messages to the MQTT broker.
 *
 * The implementation uses MQTT agent to queue a publish request. It then waits
 * for the request complete notification from the agent. The notification along with result of the
 * operation is sent back to the caller task using xTaksNotify API. For publishes involving QOS 1 and
 * QOS2 the operation is complete once an acknowledgment (PUBACK) is received. OTA agent uses this function
 * to fetch new job, provide status update and send other control related messages to the MQTT broker.
 *
 * @param[in] pacTopic Topic to publish the control packet to.
 * @param[in] topicLen Length of the topic string.
 * @param[in] pMsg Message to publish.
 * @param[in] msgSize Size of the message to publish.
 * @param[in] qos Qos for the publish.
 * @return OtaMqttSuccess if successful. Appropriate error code otherwise.
 */
static OtaMqttStatus_t prvMQTTPublish( const char * const pacTopic,
                                       uint16_t topicLen,
                                       const char * pMsg,
                                       uint32_t msgSize,
                                       uint8_t qos );

/**
 * @brief The callback invoked by the MQTT agent on a successful subscription of a topic filter
 * with broker.
 * The implementation adds a local subscription for the topic filter with the MQTT agent.
 *
 * @param pCommandContext Pointer to the command context passed from the caller.
 * @param pxReturnInfo Pointer to the return status of the subscribe command.
 */
static void prvMQTTSubscribeCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                         MQTTAgentReturnInfo_t * pxReturnInfo );

/**
 * @brief The callback invoked by the MQTT agent on unsubscribing a topic filter
 * with broker.
 * The implementation removes the local subscription for the topic filter with the MQTT agent.
 *
 * @param pCommandContext Pointer to the command context passed from the caller.
 * @param pxReturnInfo Pointer to the return status of the unsubscribe command.
 */
static void prvMQTTUnsubscribeCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                           MQTTAgentReturnInfo_t * pxReturnInfo );

/**
 * @brief Matches a client identifier within an OTA topic.
 * This function is used to validate that topic is valid and intended for this device thing name.
 *
 * @param[in] pTopic Pointer to the topic
 * @param[in] topicNameLength length of the topic
 * @param[in] pClientIdentifier Client identifier, should be null terminated.
 * @param[in] clientIdentifierLength Length of the client identifier.
 * @return pdTRUE if client identifier is found within the topic at the right index.
 */
static BaseType_t prvMatchClientIdentifierInTopic( const char * pTopic,
                                                   size_t topicNameLength,
                                                   const char * pClientIdentifier,
                                                   size_t clientIdentifierLength );

/**
 * @brief Function used by OTA agent to subscribe for a control or data packet from the MQTT broker.
 *
 * The implementation queues a SUBSCRIBE request for the topic filter with the MQTT agent. It then waits for
 * a notification of the request completion. Notification will be sent back to caller task,
 * using xTaskNotify APIs. MQTT agent also stores a callback provided by this function with
 * the associated topic filter. The callback will be used to
 * route any data received on the matching topic to the OTA agent. OTA agent uses this function
 * to subscribe to all topic filters necessary for receiving job related control messages as
 * well as firmware image chunks from MQTT broker.
 *
 * @param[in] pTopicFilter The topic filter used to subscribe for packets.
 * @param[in] topicFilterLength Length of the topic filter string.
 * @param[in] ucQoS Intended qos value for the messages received on this topic.
 * @return OtaMqttSuccess if successful. Appropriate error code otherwise.
 */
static OtaMqttStatus_t prvMQTTSubscribe( const char * pTopicFilter,
                                         uint16_t topicFilterLength,
                                         uint8_t ucQoS );

/**
 * @brief Function is used by OTA agent to unsubscribe a topicfilter from MQTT broker.
 *
 * The implementation queues an UNSUBSCRIBE request for the topic filter with the MQTT agent. It then waits
 * for a successful completion of the request from the agent. Notification along with results of
 * operation is sent using xTaskNotify API to the caller task. MQTT agent also removes the topic filter
 * subscription from its memory so any future
 * packets on this topic will not be routed to the OTA agent.
 *
 * @param[in] pTopicFilter Topic filter to be unsubscribed.
 * @param[in] topicFilterLength Length of the topic filter.
 * @param[in] ucQos Qos value for the topic.
 * @return OtaMqttSuccess if successful. Appropriate error code otherwise.
 *
 */
static OtaMqttStatus_t prvMQTTUnsubscribe( const char * pTopicFilter,
                                           uint16_t topicFilterLength,
                                           uint8_t ucQoS );

/**
 * @brief Fetch an unused OTA event buffer from the pool.
 *
 * Demo uses a simple statically allocated array of fixed size event buffers. The
 * number of event buffers is configured by the param otaconfigMAX_NUM_OTA_DATA_BUFFERS
 * within ota_config.h. This function is used to fetch a free buffer from the pool for processing
 * by the OTA agent task. It uses a mutex for thread safe access to the pool.
 *
 * @return A pointer to an unusued buffer. NULL if there are no buffers available.
 */
static OtaEventData_t * prvOTAEventBufferGet( void );

/**
 * @brief Free an event buffer back to pool
 *
 * OTA demo uses a statically allocated array of fixed size event buffers . The
 * number of event buffers is configured by the param otaconfigMAX_NUM_OTA_DATA_BUFFERS
 * within ota_config.h. The function is used by the OTA application callback to free a buffer,
 * after OTA agent has completed processing with the event. The access to the pool is made thread safe
 * using a mutex.
 *
 * @param[in] pxBuffer Pointer to the buffer to be freed.
 */
static void prvOTAEventBufferFree( OtaEventData_t * const pxBuffer );

/**
 * @brief The function which runs the OTA agent task.
 *
 * The function runs the OTA Agent Event processing loop, which waits for
 * any events for OTA agent and process them. The loop never returns until the OTA agent
 * is shutdown. The tasks exits gracefully by freeing up all resources in the event of an
 *  OTA agent shutdown.
 *
 * @param[in] pvParam Any parameters to be passed to OTA agent task.
 */
static void prvOTAAgentTask( void * pvParam );


/**
 * @brief The function which runs the OTA demo task.
 *
 * The demo task initializes the OTA agent an loops until OTA agent is shutdown.
 * It reports OTA update statistics (which includes number of blocks received, processed and dropped),
 * at regular intervals.
 *
 * @param[in] pvParam Any parameters to be passed to OTA Demo task.
 */
static void vOtaDemoTask( void * pvParam );

/**
 * @brief The function which implements the flow for OTA demo.
 *
 * @return pdPASS if success or pdFAIL.
 */
static BaseType_t prvRunOTADemo( void );

/**
 * @brief Callback registered with the OTA library that notifies the OTA agent
 * of an incoming PUBLISH containing a job document.
 *
 * @param[in] pContext MQTT context which stores the connection.
 * @param[in] pPublishInfo MQTT packet information which stores details of the
 * job document.
 */
static void prvMqttJobCallback( void * pContext,
                                MQTTPublishInfo_t * pPublish );


/**
 * @brief Callback that notifies the OTA library when a data block is received.
 *
 * @param[in] pContext MQTT context which stores the connection.
 * @param[in] pPublishInfo MQTT packet that stores the information of the file block.
 */
static void prvMqttDataCallback( void * pContext,
                                 MQTTPublishInfo_t * pPublish );

/**
 * @brief Default callback used to receive unsolicited messages for OTA.
 *
 * The callback is not subscribed with MQTT broker, but only with local subscription manager.
 * A wildcard OTA job topic is used for subscription so that all unsolicited messages related to OTA is
 * forwarded to this callback for filteration. Right now the callback is used to filter responses to job requests
 * from the OTA service.
 *
 * @param[in] pvIncomingPublishCallbackContext MQTT context which stores the connection.
 * @param[in] pPublishInfo MQTT packet that stores the information of the file block.
 */
static void prvMqttDefaultCallback( void * pvIncomingPublishCallbackContext,
                                    MQTTPublishInfo_t * pxPublishInfo );

void vStartOtaDemo( void );



/**
 * @brief Structure containing all application allocated buffers used by the OTA agent.
 * Structure is passed to the OTA agent during initialization.
 */
static OtaAppBuffer_t otaBuffer =
{
    .pUpdateFilePath    = updateFilePath,
    .updateFilePathsize = otaexampleMAX_FILE_PATH_SIZE,
    .pCertFilePath      = certFilePath,
    .certFilePathSize   = otaexampleMAX_FILE_PATH_SIZE,
    .pStreamName        = streamName,
    .streamNameSize     = otaexampleMAX_STREAM_NAME_SIZE,
    .pDecodeMemory      = decodeMem,
    .decodeMemorySize   = ( 1U << otaconfigLOG2_FILE_BLOCK_SIZE ),
    .pFileBitmap        = bitmap,
    .fileBitmapSize     = OTA_MAX_BLOCK_BITMAP_SIZE
};

static void prvOTAEventBufferFree( OtaEventData_t * const pxBuffer )
{
    if( xSemaphoreTake( xBufferSemaphore, portMAX_DELAY ) == pdTRUE )
    {
        pxBuffer->bufferUsed = false;
        ( void ) xSemaphoreGive( xBufferSemaphore );
    }
}

/*-----------------------------------------------------------*/

static OtaEventData_t * prvOTAEventBufferGet( void )
{
    uint32_t ulIndex = 0;
    OtaEventData_t * pFreeBuffer = NULL;

    if( xSemaphoreTake( xBufferSemaphore, portMAX_DELAY ) == pdTRUE )
    {
        for( ulIndex = 0; ulIndex < otaconfigMAX_NUM_OTA_DATA_BUFFERS; ulIndex++ )
        {
            if( eventBuffer[ ulIndex ].bufferUsed == false )
            {
                eventBuffer[ ulIndex ].bufferUsed = true;
                pFreeBuffer = &eventBuffer[ ulIndex ];
                break;
            }
        }

        ( void ) xSemaphoreGive( xBufferSemaphore );
    }

    return pFreeBuffer;
}

/*-----------------------------------------------------------*/

/**
 * @brief The OTA agent has completed the update job or it is in
 * self test mode. If it was accepted, we want to activate the new image.
 * This typically means we should reset the device to run the new firmware.
 * If now is not a good time to reset the device, it may be activated later
 * by your user code. If the update was rejected, just return without doing
 * anything and we will wait for another job. If it reported that we should
 * start test mode, normally we would perform some kind of system checks to
 * make sure our new firmware does the basic things we think it should do
 * but we will just go ahead and set the image as accepted for demo purposes.
 * The accept function varies depending on your platform. Refer to the OTA
 * PAL implementation for your platform in ota_pal.c to see what it
 * does for you.
 *
 * @param[in] event Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pData Data associated with the event.
 * @return None.
 */
static void otaAppCallback( OtaJobEvent_t event,
                            const void * pData )
{
    OtaErr_t err = OtaErrUninitialized;

    switch( event )
    {
        case OtaJobEventActivate:
            IotLogInfo( "Received OtaJobEventActivate callback from OTA Agent. \r\n" );
            IotLogInfo( "[GREEN]New image is ready to boot... \r\n[WHITE]" );

            /**
             * Activate the new firmware image immediately. Applications can choose to postpone
             * the activation to a later stage if needed.
             */
            err = OTA_ActivateNewImage();

            /**
             * Activation of the new image failed. This indicates an error that requires a follow
             * up through manual activation by resetting the device. The demo reports the error
             * and shuts down the OTA agent.
             */
            IotLogError( "New image activation failed.\r\n" );

            /* Shutdown OTA Agent, if it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1. */
            OTA_Shutdown( 0, 1 );


            break;

        case OtaJobEventFail:

            /**
             * No user action is needed here. OTA agent handles the job failure event.
             */
            IotLogInfo( "Received an OtaJobEventFail notification from OTA Agent. \r\n" );

            break;

        case OtaJobEventStartTest:

            /* This demo just accepts the image since it was a good OTA update and networking
             * and services are all working (or we would not have made it this far). If this
             * were some custom device that wants to test other things before validating new
             * image, this would be the place to kick off those tests before calling
             * OTA_SetImageState() with the final result of either accepted or rejected. */

            IotLogInfo( "Received OtaJobEventStartTest callback from OTA Agent. \r\n");

            err = OTA_SetImageState( OtaImageStateAccepted );

            if( err == OtaErrNone )
            {
                IotLogInfo( "New image validation succeeded in self test mode. \r\n");
            }
            else
            {
                IotLogError( "Failed to set image state as accepted with error %d. \r\n", err);
            }

            break;

        case OtaJobEventProcessed:

            IotLogDebug("OTA Event processing completed. Freeing the event buffer to pool.\r\n" );
            configASSERT( pData != NULL );
            prvOTAEventBufferFree( ( OtaEventData_t * ) pData );

            break;

        case OtaJobEventSelfTestFailed:
            IotLogDebug( "Received OtaJobEventSelfTestFailed callback from OTA Agent.\r\n" );

            /* Requires manual activation of previous image as self-test for
             * new image downloaded failed.*/
            IotLogError( "[RED]OTA Self-test failed for new image. shutting down OTA Agent.\r\n[WHITE]" );

            /* Shutdown OTA Agent, if it is required that the unsubscribe operations are not
             * performed while shutting down please set the second parameter to 0 instead of 1. */
            OTA_Shutdown( 0, 1 );

            break;
        default:
            IotLogWarn( "Received an unhandled callback event from OTA Agent, event = %d \r\n", event );

            break;
    }
}

static void prvMqttJobCallback( void * pvIncomingPublishCallbackContext,
                                MQTTPublishInfo_t * pxPublishInfo )
{
    OtaEventData_t * pData;
    OtaEventMsg_t eventMsg = { 0 };
    BaseType_t isMatch = pdFALSE;

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    isMatch = prvMatchClientIdentifierInTopic( pxPublishInfo->pTopicName,
                                               pxPublishInfo->topicNameLength,
                                               pcThingName,
                                               xThingNameLength );

    if( isMatch == pdFALSE )
    {
        IotLogWarn( "Received a job message on an unsolicited topic, thing name does not match. topic: %s \r\n",pxPublishInfo->pTopicName);
        return;
    }

    if( pxPublishInfo->payloadLength > OTA_DATA_BLOCK_SIZE )
    {
        IotLogError( "Received OTA job message size (%d) is larger than the OTA maximum size (%d) defined.\n\n", pxPublishInfo->payloadLength, OTA_DATA_BLOCK_SIZE );
        return;
    }
    IotLogInfo( "Received OTA job message, size: %d.\r\n", pxPublishInfo->payloadLength );
    pData = prvOTAEventBufferGet();

    if( pData != NULL )
    {
        memcpy( pData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pData->dataLength = pxPublishInfo->payloadLength;
        eventMsg.eventId = OtaAgentEventReceivedJobDocument;
        eventMsg.pEventData = pData;

        /* Send job document received event. */
        OTA_SignalEvent( &eventMsg );
    }
    else
    {
        IotLogError( "Error: No OTA data buffers available.\r\n" );
    }
}

/*-----------------------------------------------------------*/
static void prvMqttDefaultCallback( void * pvIncomingPublishCallbackContext,
                                    MQTTPublishInfo_t * pxPublishInfo )
{
    bool isMatch = false;

    ( void ) MQTT_MatchTopic( pxPublishInfo->pTopicName,
                              pxPublishInfo->topicNameLength,
                              OTA_JOB_NOTIFY_TOPIC_FILTER,
                              OTA_JOB_NOTIFY_TOPIC_FILTER_LENGTH,
                              &isMatch );

    if( isMatch == true )
    {
        prvMqttJobCallback( pvIncomingPublishCallbackContext, pxPublishInfo );
    }

    if( isMatch == false )
    {
        ( void ) MQTT_MatchTopic( pxPublishInfo->pTopicName,
                                  pxPublishInfo->topicNameLength,
                                  OTA_DATA_STREAM_TOPIC_FILTER,
                                  OTA_DATA_STREAM_TOPIC_FILTER_LENGTH,
                                  &isMatch );

        if( isMatch == true )
        {
            prvMqttDataCallback( pvIncomingPublishCallbackContext, pxPublishInfo );
        }
    }
}

/*-----------------------------------------------------------*/
static void prvMqttDataCallback( void * pvIncomingPublishCallbackContext,
                                 MQTTPublishInfo_t * pxPublishInfo )
{
    BaseType_t isMatch = pdFALSE;
    OtaEventData_t * pxData;
    OtaEventMsg_t eventMsg = { 0 };

    configASSERT( pxPublishInfo != NULL );
    ( void ) pvIncomingPublishCallbackContext;

    isMatch = prvMatchClientIdentifierInTopic( pxPublishInfo->pTopicName,
                                               pxPublishInfo->topicNameLength,
                                               pcThingName,
                                               xThingNameLength );

    if( isMatch == pdFALSE )
    {
        IotLogWarn("Received data block on an unsolicited topic, thing name does not match. topic: %s \r\n",pxPublishInfo->pTopicName );
        return;
    }

    if( pxPublishInfo->payloadLength > OTA_DATA_BLOCK_SIZE )
    {
        IotLogError( "Received OTA data block of size (%d) larger than maximum size(%d) defined. \r\n", pxPublishInfo->payloadLength, OTA_DATA_BLOCK_SIZE  );
        return;
    }

    IotLogInfo( "Received data message callback, size %u.\r\n", pxPublishInfo->payloadLength );

    pxData = prvOTAEventBufferGet();

    if( pxData != NULL )
    {
        memcpy( pxData->data, pxPublishInfo->pPayload, pxPublishInfo->payloadLength );
        pxData->dataLength = pxPublishInfo->payloadLength;
        eventMsg.eventId = OtaAgentEventReceivedFileBlock;
        eventMsg.pEventData = pxData;

        /* Send job document received event. */
        OTA_SignalEvent( &eventMsg );
    }
    else
    {
        IotLogError( ( "Error: No OTA data buffers available.\r\n" ) );
    }
}

/*-----------------------------------------------------------*/

static void prvCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                MQTTAgentReturnInfo_t * pxReturnInfo )
{

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        xTaskNotify( pxCommandContext->xTaskToNotify, ( uint32_t ) ( pxReturnInfo->returnCode ), eSetValueWithOverwrite );
    }
}

static void prvMQTTSubscribeCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                              MQTTAgentReturnInfo_t * pxReturnInfo )
{
    MQTTAgentSubscribeArgs_t * pSubsribeArgs;

    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        pSubsribeArgs = ( MQTTAgentSubscribeArgs_t * ) ( pxCommandContext->pArgs );
        xAddMQTTTopicFilterCallback (pSubsribeArgs->pSubscribeInfo[0].pTopicFilter,
                                     pSubsribeArgs->pSubscribeInfo[0].topicFilterLength,
                                     prvMqttDefaultCallback,
                                     NULL,
                                     pdFALSE);
    }

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        /* Send the context's ulNotificationValue as the notification value so
         * the receiving task can check the value it set in the context matches
         * the value it receives in the notification. */
        xTaskNotify( pxCommandContext->xTaskToNotify, ( uint32_t ) ( pxReturnInfo->returnCode ), eSetValueWithOverwrite );
    }
}

/*-----------------------------------------------------------*/

static void prvMQTTUnsubscribeCompleteCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                                MQTTAgentReturnInfo_t * pxReturnInfo )
{
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs = ( MQTTAgentSubscribeArgs_t * ) pxCommandContext->pArgs;

    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        vRemoveMQTTTopicFilterCallback( pxSubscribeArgs->pSubscribeInfo[ 0 ].pTopicFilter,
                                        pxSubscribeArgs->pSubscribeInfo[ 0 ].topicFilterLength );
    }

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        /* Send the context's ulNotificationValue as the notification value so
         * the receiving task can check the value it set in the context matches
         * the value it receives in the notification. */
        xTaskNotify( pxCommandContext->xTaskToNotify, ( uint32_t ) ( pxReturnInfo->returnCode ), eSetValueWithOverwrite );
    }
}

/*-----------------------------------------------------------*/

static OtaMqttStatus_t prvMQTTSubscribe( const char * pTopicFilter,
                                         uint16_t topicFilterLength,
                                         uint8_t ucQoS )
{
    MQTTStatus_t mqttStatus;
    uint32_t ulNotifiedValue;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };
    MQTTSubscribeInfo_t xSubscribeInfo = { 0 };
    BaseType_t result;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xApplicationDefinedContext = { 0 };
    OtaMqttStatus_t otaRet = OtaMqttSuccess;

    configASSERT( pTopicFilter != NULL );
    configASSERT( topicFilterLength > 0 );

    xSubscribeInfo.pTopicFilter = pTopicFilter;
    xSubscribeInfo.topicFilterLength = topicFilterLength;
    xSubscribeInfo.qos = ucQoS;
    xSubscribeArgs.pSubscribeInfo = &xSubscribeInfo;
    xSubscribeArgs.numSubscriptions = 1;

    xApplicationDefinedContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xApplicationDefinedContext.pArgs = &xSubscribeArgs;

    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTSubscribeCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xApplicationDefinedContext;

    /*
     * Wait for Agent to be connected before sending a subscribe message.
     */
    if( xGetMQTTAgentState() != MQTT_AGENT_STATE_CONNECTED )
    {
        ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
    }

    xTaskNotifyStateClear( NULL );

    mqttStatus = MQTTAgent_Subscribe( &xGlobalMqttAgentContext,
                                      &xSubscribeArgs,
                                      &xCommandParams );

    /* Wait for command to complete so MQTTSubscribeInfo_t remains in scope for the
     * duration of the command. */
    if( mqttStatus == MQTTSuccess )
    {
        result = xTaskNotifyWait( 0, otaexampleMAX_UINT32, &ulNotifiedValue, pdMS_TO_TICKS( MQTT_AGENT_MS_TO_WAIT_FOR_NOTIFICATION ) );

        if( result == pdTRUE )
        {
            mqttStatus = ( MQTTStatus_t ) ( ulNotifiedValue );
        }
        else
        {
            mqttStatus = MQTTRecvFailed;
        }
    }

    if( mqttStatus != MQTTSuccess )
    {
        IotLogError( "Failed to SUBSCRIBE to topic with error = %u. \r\n", mqttStatus );

        otaRet = OtaMqttSubscribeFailed;
    }
    else
    {
        IotLogInfo( "Subscribed to topic %s.\r\n", pTopicFilter );

        otaRet = OtaMqttSuccess;
    }

    return otaRet;
}

static OtaMqttStatus_t prvMQTTPublish( const char * const pacTopic,
                                       uint16_t topicLen,
                                       const char * pMsg,
                                       uint32_t msgSize,
                                       uint8_t qos )
{
    OtaMqttStatus_t otaRet = OtaMqttSuccess;
    BaseType_t result;
    MQTTStatus_t mqttStatus = MQTTBadParameter;
    MQTTPublishInfo_t publishInfo = { 0 };
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xCommandContext = { 0 };
    uint32_t ulNotifiedValue;

    publishInfo.pTopicName = pacTopic;
    publishInfo.topicNameLength = topicLen;
    publishInfo.qos = qos;
    publishInfo.pPayload = pMsg;
    publishInfo.payloadLength = msgSize;

    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();

    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xCommandContext;

    /*
     * Wait for Agent to be connected before sending a publish message.
     */
    if( xGetMQTTAgentState() != MQTT_AGENT_STATE_CONNECTED )
    {
        ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
    }

    xTaskNotifyStateClear( NULL );

    mqttStatus = MQTTAgent_Publish( &xGlobalMqttAgentContext,
                                    &publishInfo,
                                    &xCommandParams );

    /* Block for command to complete so MQTTPublishInfo_t remains in scope for the
     * duration of the command. */
    if( mqttStatus == MQTTSuccess )
    {
        result = xTaskNotifyWait( 0, otaexampleMAX_UINT32, &ulNotifiedValue, portMAX_DELAY );

        if( result != pdTRUE )
        {
            mqttStatus = MQTTSendFailed;
        }
        else
        {
            mqttStatus = ( MQTTStatus_t ) ( ulNotifiedValue );
        }
    }

    if( mqttStatus != MQTTSuccess )
    {
        IotLogError("Failed to send PUBLISH packet to broker with error = %u.\r\n", mqttStatus);
        otaRet = OtaMqttPublishFailed;
    }
    else
    {
        IotLogDebug( "Sent PUBLISH packet to broker %s to broker.\n\n",pacTopic);

        otaRet = OtaMqttSuccess;
    }

    return otaRet;
}

static OtaMqttStatus_t prvMQTTUnsubscribe( const char * pTopicFilter,
                                           uint16_t topicFilterLength,
                                           uint8_t ucQoS )
{
    MQTTStatus_t mqttStatus;
    uint32_t ulNotifiedValue;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };
    MQTTSubscribeInfo_t xSubscribeInfo = { 0 };
    BaseType_t result;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTAgentCommandContext_t xApplicationDefinedContext = { 0 };
    OtaMqttStatus_t otaRet = OtaMqttSuccess;

    configASSERT( pTopicFilter != NULL );
    configASSERT( topicFilterLength > 0 );

    xSubscribeInfo.pTopicFilter = pTopicFilter;
    xSubscribeInfo.topicFilterLength = topicFilterLength;
    xSubscribeInfo.qos = ucQoS;
    xSubscribeArgs.pSubscribeInfo = &xSubscribeInfo;
    xSubscribeArgs.numSubscriptions = 1;


    xApplicationDefinedContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xApplicationDefinedContext.pArgs = ( void * ) ( &xSubscribeArgs );

    xCommandParams.blockTimeMs = MQTT_AGENT_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvMQTTUnsubscribeCompleteCallback;
    xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xApplicationDefinedContext;

    IotLogInfo( " Unsubscribing to topic filter: %s", pTopicFilter );
    xTaskNotifyStateClear( NULL );


    mqttStatus = MQTTAgent_Unsubscribe( &xGlobalMqttAgentContext,
                                        &xSubscribeArgs,
                                        &xCommandParams );

    /* Wait for command to complete so MQTTSubscribeInfo_t remains in scope for the
     * duration of the command. */
    if( mqttStatus == MQTTSuccess )
    {
        result = xTaskNotifyWait( 0, otaexampleMAX_UINT32, &ulNotifiedValue, portMAX_DELAY );

        if( result == pdTRUE )
        {
            mqttStatus = ( MQTTStatus_t ) ( ulNotifiedValue );
        }
        else
        {
            mqttStatus = MQTTRecvFailed;
        }
    }

    if( mqttStatus != MQTTSuccess )
    {
        IotLogError( "Failed to UNSUBSCRIBE from topic %s with error = %u. \r\n",pTopicFilter, mqttStatus );

        otaRet = OtaMqttUnsubscribeFailed;
    }
    else
    {
        IotLogInfo( "UNSUBSCRIBED from topic %s.\r\n", pTopicFilter );

        otaRet = OtaMqttSuccess;
    }

    return otaRet;
}

/*-----------------------------------------------------------*/

static void setOtaInterfaces( OtaInterfaces_t * pOtaInterfaces )
{
    configASSERT( pOtaInterfaces != NULL );

    /* Initialize OTA library OS Interface. */
    pOtaInterfaces->os.event.init = OtaInitEvent_FreeRTOS;
    pOtaInterfaces->os.event.send = OtaSendEvent_FreeRTOS;
    pOtaInterfaces->os.event.recv = OtaReceiveEvent_FreeRTOS;
    pOtaInterfaces->os.event.deinit = OtaDeinitEvent_FreeRTOS;
    pOtaInterfaces->os.timer.start = OtaStartTimer_FreeRTOS;
    pOtaInterfaces->os.timer.stop = OtaStopTimer_FreeRTOS;
    pOtaInterfaces->os.timer.delete = OtaDeleteTimer_FreeRTOS;
    pOtaInterfaces->os.mem.malloc = Malloc_FreeRTOS;
    pOtaInterfaces->os.mem.free = Free_FreeRTOS;

    /* Initialize the OTA library MQTT Interface.*/
    pOtaInterfaces->mqtt.subscribe = prvMQTTSubscribe;
    pOtaInterfaces->mqtt.publish = prvMQTTPublish;
    pOtaInterfaces->mqtt.unsubscribe = prvMQTTUnsubscribe;

    /* Initialize the OTA library PAL Interface.*/
    pOtaInterfaces->pal.getPlatformImageState = otaPal_GetPlatformImageState;
    pOtaInterfaces->pal.setPlatformImageState = otaPal_SetPlatformImageState;
    pOtaInterfaces->pal.writeBlock = otaPal_WriteBlock;
    pOtaInterfaces->pal.activate = otaPal_ActivateNewImage;
    pOtaInterfaces->pal.closeFile = otaPal_CloseFile;
    pOtaInterfaces->pal.reset = otaPal_ResetDevice;
    pOtaInterfaces->pal.abort = otaPal_Abort;
    pOtaInterfaces->pal.createFile = otaPal_CreateFileForRx;
}

/*-----------------------------------------------------------*/

static void prvOTAAgentTask( void * pParam )
{
    /* Calling OTA agent task. */
    OTA_EventProcessingTask( pParam );
    IotLogInfo( "OTA Agent stopped. \r\n" );

    vTaskDelete( NULL );
}

static BaseType_t prvSuspendOTA( void )
{
    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;
    BaseType_t status = pdPASS;
    uint32_t suspendTimeout;

    IotLogInfo( "\r\n[GREEN]Suspend OTA... [WHITE]\r\n" );

    otaRet = OTA_Suspend();

    if( otaRet == OtaErrNone )
    {
        suspendTimeout = OTA_SUSPEND_TIMEOUT_MS;

        while( ( OTA_GetState() != OtaAgentStateSuspended ) && ( suspendTimeout > 0 ) )
        {
            /* Wait for OTA Library state to suspend */
            vTaskDelay( pdMS_TO_TICKS( otaexampleTASK_DELAY_MS ) );
            suspendTimeout -= otaexampleTASK_DELAY_MS;
        }

        if( OTA_GetState() != OtaAgentStateSuspended )
        {
            IotLogError( "Failed to suspend OTA. \r\n" );
            status = pdFAIL;
        }
    }
    else
    {
        IotLogError("Error while trying to suspend OTA agent %d \r\n", otaRet);
        status = pdFAIL;
    }

    return status;
}

static BaseType_t prvResumeOTA( void )
{
    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;
    BaseType_t status = pdPASS;
    uint32_t suspendTimeout;
    
    IotLogInfo( "\r\n[GREEN]Resume OTA... [WHITE]\r\n" );
    
    otaRet = OTA_Resume();

    if( otaRet == OtaErrNone )
    {
        suspendTimeout = OTA_SUSPEND_TIMEOUT_MS;

        while( ( OTA_GetState() == OtaAgentStateSuspended ) && ( suspendTimeout > 0 ) )
        {
            /* Wait for OTA Library state to suspend */
            vTaskDelay( pdMS_TO_TICKS( otaexampleTASK_DELAY_MS ) );
            suspendTimeout -= otaexampleTASK_DELAY_MS;
        }

        if( OTA_GetState() == OtaAgentStateSuspended )
        {
            IotLogError( "Failed to resume OTA. \r\n" );
            status = pdFAIL;
        }
    }
    else
    {
        IotLogError( "Error while trying to resume OTA agent %d \r\n", otaRet );
        status = pdFAIL;
    }

    return status;
}

static BaseType_t prvRunOTADemo( void )
{
    /* Status indicating a successful demo or not. */
    BaseType_t xStatus = pdPASS;

    /* OTA library return status. */
    OtaErr_t otaRet = OtaErrNone;

    /* OTA event message used for sending event to OTA Agent.*/
    OtaEventMsg_t eventMsg = { 0 };

    /* OTA interface context required for library interface functions.*/
    OtaInterfaces_t otaInterfaces;

    /* OTA library packet statistics per job.*/
    OtaAgentStatistics_t otaStatistics = { 0 };

    /* OTA Agent state returned from calling OTA_GetState.*/
    OtaState_t state = OtaAgentStateStopped;

    /* Set OTA Library interfaces.*/
    setOtaInterfaces( &otaInterfaces );

    /****************************** Init OTA Library. ******************************/

    if( xStatus == pdPASS )
    {
        memset( eventBuffer, 0x00, sizeof( eventBuffer ) );

        if( ( otaRet = OTA_Init( &otaBuffer,
                                 &otaInterfaces,
                                 ( const uint8_t * ) ( pcThingName ),
                                 otaAppCallback ) ) != OtaErrNone )
        {
            IotLogError( "Failed to initialize OTA Agent, exiting = %u.\r\n", otaRet );

            xStatus = pdFAIL;
        }
    }

    /****************************** Create OTA Agent Task. ******************************/

    if( xStatus == pdPASS )
    {
        xStatus = xTaskCreate( prvOTAAgentTask,
                               "OTA Agent Task",
                               OTA_AGENT_TASK_STACK_SIZE,
                               NULL,
                               OTA_AGENT_TASK_PRIORITY,
                               NULL );

        if( xStatus != pdPASS )
        {
            IotLogError( "Failed to create OTA agent task: \r\n"  );
        }
    }

    if( xStatus == pdPASS )
    {
        /**
         * Register a callback for receiving messages intended for OTA agent from broker,
         * for which the topic has not been subscribed for.
         */
        xStatus = xAddMQTTTopicFilterCallback ( OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER,
                                                OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH,
                                               prvMqttJobCallback,
                                               NULL,
                                               pdFALSE);
    }

    /****************************** Start OTA ******************************/

    if( xStatus == pdPASS )
    {
        /* Send start event to OTA Agent.*/
        eventMsg.eventId = OtaAgentEventStart;
        OTA_SignalEvent( &eventMsg );
    }

    /****************************** Loop and display OTA statistics ******************************/

    if( xStatus == pdPASS )
    {
        while( ( state = OTA_GetState() ) != OtaAgentStateStopped )
        {
            /* Get OTA statistics for currently executing job. */
            if( state != OtaAgentStateSuspended )
            {
                OTA_GetStatistics( &otaStatistics );

                IotLogInfo( " Received: %u   Queued: %u   Processed: %u   Dropped: %u \r\n",
                           otaStatistics.otaPacketsReceived,
                           otaStatistics.otaPacketsQueued,
                           otaStatistics.otaPacketsProcessed,
                           otaStatistics.otaPacketsDropped );

                if (xWaitForMQTTAgentState (MQTT_AGENT_STATE_DISCONNECTED,
                                            pdMS_TO_TICKS(otaexampleTASK_DELAY_MS)) == pdTRUE)
                {
                    /* Suspend ongoing OTA job if any until MQTT agent is reconnected. */
                    prvSuspendOTA ();

                    (void ) xWaitForMQTTAgentState (MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY);

                    /* Resume OTA Update so that agent checks for any new jobs during a lost connection. */
                    prvResumeOTA ();
                }
            }

            vTaskDelay( pdMS_TO_TICKS( otaexampleTASK_DELAY_MS ) );
        }
    }

    IotLogInfo( "---------OTA agent task stopped. Exiting OTA demo.---------" );

    /**
     * Remvove callback for receiving messages intended for OTA agent from broker,
     * for which the topic has not been subscribed for.
     */
    vRemoveMQTTTopicFilterCallback( OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER,
                                    OTA_JOB_ACCEPTED_RESPONSE_TOPIC_FILTER_LENGTH );

    return xStatus;
}

/**
 * @brief Entry point of Ota demo task.
 *
 * This example initializes the OTA library to enable OTA updates via the
 * MQTT broker. It simply connects to the MQTT broker with the users
 * credentials and spins in an indefinite loop to allow MQTT messages to be
 * forwarded to the OTA agent for possible processing. The OTA agent does all
 * of the real work; checking to see if the message topic is one destined for
 * the OTA agent. If not, it is simply ignored.
 *
 */
static void vOtaDemoTask( void * pvParam )
{
    /* Return error status. */
    BaseType_t xReturnStatus = pdPASS;

    if (xGetMQTTAgentState () != MQTT_AGENT_STATE_CONNECTED)
    {
        (void ) xWaitForMQTTAgentState (MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY);
    }
    IotLogInfo("---------Start OTA Task---------\r\n");
    ( void ) pvParam;

    pcThingName = g_iot_thing_name;
    xThingNameLength = strlen(g_iot_thing_name);

    IotLogInfo( "[GREEN]OTA over MQTT demo, Application version %u.%u.%u [WHITE]\r\n",
               appFirmwareVersion.u.x.major,
               appFirmwareVersion.u.x.minor,
               appFirmwareVersion.u.x.build );

    /* Initialize semaphore for buffer operations. */
    xBufferSemaphore = xSemaphoreCreateMutex();

    if( xBufferSemaphore == NULL )
    {
        IotLogError( "Failed to initialize buffer semaphore. \r\n" );
        xReturnStatus = pdFAIL;
    }


    /****************************** Start OTA Demo. ******************************/

    if( xReturnStatus == pdPASS )
    {
        /* Start OTA demo. The function returns only if OTA completes successfully and a
         * shutdown of OTA is triggered for a manual restart of the device.*/
        if( prvRunOTADemo() != pdPASS )
        {
            xReturnStatus = pdFAIL;
        }
    }

    /****************************** Cleanup ******************************/

    if( xBufferSemaphore != NULL )
    {
        /* Cleanup semaphore created for buffer operations. */
        vSemaphoreDelete( xBufferSemaphore );
    }

    if( pcThingName != NULL )
	{
		vPortFree( pcThingName );
		pcThingName = NULL;
	}

    vTaskDelete( NULL );
}

/*
 * @brief Create the task that demonstrates the Ota demo.
 */
void vStartOtaDemo( void )
{
    /*
     * vOtaDemoTask() connects to the MQTT broker, creates the
     * MQTT Agent task and calls the Ota demo loop prvRunOTADemo()
     * which creates the OTA Agent task.
     */

    xTaskCreate( vOtaDemoTask,             /* Function that implements the task. */
                 "OTA Demo Task",          /* Text name for the task - only used for debugging. */
                 democonfigDEMO_STACKSIZE, /* Size of stack (in words, not bytes) to allocate for the task. */
                 NULL,                     /* Optional - task parameter - not used in this case. */
                 tskIDLE_PRIORITY + 1,     /* Task priority, must be between 0 and configMAX_PRIORITIES - 1. */
                 NULL );                   /* Optional - used to pass out a handle to the created task. */
}

/*-----------------------------------------------------------*/


static BaseType_t prvMatchClientIdentifierInTopic( const char * pTopic,
                                                   size_t topicNameLength,
                                                   const char * pClientIdentifier,
                                                   size_t clientIdentifierLength )
{
    BaseType_t isMatch = pdFALSE;
    size_t idx;
    size_t matchIdx = 0;

    for( idx = OTA_TOPIC_CLIENT_IDENTIFIER_START_IDX; idx < topicNameLength; idx++ )
    {
        if( matchIdx == clientIdentifierLength )
        {
            if( pTopic[ idx ] == '/' )
            {
                isMatch = pdTRUE;
            }

            break;
        }
        else
        {
            if( pClientIdentifier[ matchIdx ] != pTopic[ idx ] )
            {
                break;
            }
        }

        matchIdx++;
    }

    return isMatch;
}
