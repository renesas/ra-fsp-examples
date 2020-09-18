/*********************************************************************************************************************
 * File Name	: mqtt_interface.c
 * Description	: Contains data structures and functions used in mqtt interface for Cloud Connectivity application
 *********************************************************************************************************************
 */
/*********************************************************************************************************************
 **
 * Copyright [2015-2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 *
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any 
 * name or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 
 * 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE 
 * PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * SPECIAL, OR CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 *********************************************************************************************************************
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set up logging for this Application Project. */
#include "iot_logging.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* MQTT include. */
#include "iot_mqtt.h"
#include "common_data.h"
#include "mqtt_interface.h"


#ifdef MQTT_REQUIRED

/* Unique identifier for the device. */
static char deviceIdentifier[AWS_IOT_DEVICE_IDENTIFIER_LENGTH + 1] = {0};

/* Device metrics sent to the cloud. */
static char deviceMetrics[AWS_IOT_METRICS_NAME_LENGTH + 1] = {0};

#endif

IotMqttConnection_t mqttConn = IOT_MQTT_CONNECTION_INITIALIZER;


/* Topics used for the push button status update in this Application Project. This Publish Topic */
const char *pTopics_pb[TOPIC_FILTER_COUNT] =
{
    MQTT_TOPIC_PREFIX "/topic/switch_status", 
};
	
/* Topics used for the temperature update in this Application Project. This Publish Topic */
const char *pTopics_ts[TOPIC_FILTER_COUNT] =
{
    MQTT_TOPIC_PREFIX "/topic/temperature",
};

/* Topics used for the LED actuation  in this Application Project. This is Subsription Topic */
const char *psTopics[TOPIC_FILTER_COUNT] =
{
    MQTT_TOPIC_PREFIX "/topic/led",
};

/***************************************************************************************************************************
 * Called by the MQTT library when an PUBLISH operation completes.
 *
 * The Appication uses this callback to determine the result of PUBLISH operations.
 * @param[in] param1 The number of the PUBLISH that completed, passed as an intptr_t.
 * @param[in] pOperation Information about the completed operation passed by the
 * MQTT library.
 **************************************************************************************************************************/
static void publish_completion_cb(void *param1, IotMqttCallbackParam_t * const pOperation)
{
    intptr_t publishCount = (intptr_t) param1;

    /* Silence warnings about unused variables. publishCount will not be used if logging is disabled. */
    (void) publishCount;

    /* Print the status of the completed operation. A PUBLISH operation is successful when transmitted over the network.*/
    if (IOT_MQTT_SUCCESS == pOperation->u.operation.result)
    {
        IotLogInfo("MQTT %s successfully sent.\r\n", IotMqtt_OperationType (pOperation->u.operation.type));
    }
    else
    {
        IotLogError("** MQTT %s could not be sent. Error %s.**\r\n", IotMqtt_OperationType (pOperation->u.operation.type),
                    IotMqtt_strerror (pOperation->u.operation.result));
    }
}

/*************************************************************************************************************************
 * Called by the MQTT library when an incoming PUBLISH message is received.
 *
 * The Application Project uses this callback to handle incoming PUBLISH messages. This callback
 * prints the contents of an incoming message and publishes an acknowledgement
 * to the MQTT server.
 * @param[in] param1 Counts the total number of received PUBLISH messages. This
 * callback will increment this counter.
 * @param[in] pPublish Information about the incoming PUBLISH message passed by
 * the MQTT library.
 *************************************************************************************************************************/
static void mqtt_subscription_cb(void *param1, IotMqttCallbackParam_t * const pPublish)
{
    int acknowledgementLength = 0;
    size_t messageNumberLength = 1;
    const char *pPayload = pPublish->u.message.info.pPayload;
    char pAcknowledgementMessage[ACK_MESSAGE_BUFFER_LENGTH] =   { 0 };
    IotMqttPublishInfo_t acknowledgementInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;

    (void ) param1;
    mqtt_rx_payload_t led_data =    {'\0'};
    led_data.id = ID_LED_ACT;
    strncpy ((char *) &led_data.value.led_data.led_act_topic_msg, pPayload, pPublish->u.message.info.payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &led_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogDebug("Incoming PUBLISH received:\r\n"
            "Subscription topic filter: %.*s\r\n"
            "Publish topic name: %.*s\r\n"
            "Publish retain flag: %d\r\n"
            "Publish QoS: %d\r\n"
            "Publish payload: %.*s\r\n",
            pPublish->u.message.topicFilterLength,
            pPublish->u.message.pTopicFilter,
            pPublish->u.message.info.topicNameLength,
            pPublish->u.message.info.pTopicName,
            pPublish->u.message.info.retain,
            pPublish->u.message.info.qos,
            pPublish->u.message.info.payloadLength,
            pPayload );

    messageNumberLength = pPublish->u.message.info.payloadLength;

    /* Generate an acknowledgement message. */
    acknowledgementLength = snprintf (pAcknowledgementMessage,
                                      ACK_MESSAGE_BUFFER_LENGTH,
                                      ACK_MESSAGE_FORMAT, (int) messageNumberLength, pPayload);

    /* Check for errors from snprintf. */
    if (acknowledgementLength < 0)
    {
         IotLogWarn("Failed to generate acknowledgement message for PUBLISH %.*s.", (int ) messageNumberLength,
                     pPayload);
    }
    else
    {
        /* Set the members of the publish info for the acknowledgment message. */
        acknowledgementInfo.qos = IOT_MQTT_QOS_1;
        acknowledgementInfo.pTopicName = ACK_TOPIC_NAME;
        acknowledgementInfo.topicNameLength = ACK_TOPIC_NAME_LENGTH;
        acknowledgementInfo.pPayload = pAcknowledgementMessage;
        acknowledgementInfo.payloadLength = (size_t) acknowledgementLength;
        acknowledgementInfo.retryMs = PUBLISH_RETRY_MS;
        acknowledgementInfo.retryLimit = PUBLISH_RETRY_LIMIT;

        /* Send the acknowledgement for the received message. This Application Project program
         * will not be notified on the status of the acknowledgement because
         * neither a callback nor IOT_MQTT_FLAG_WAITABLE is set. However,
         * the MQTT library will still guarantee at-least-once delivery (subject
         * to the retransmission strategy) because the acknowledgement message is
         * sent at QoS 1. */
        if (IOT_MQTT_STATUS_PENDING == IotMqtt_Publish (pPublish->mqttConnection, &acknowledgementInfo, 0, NULL, NULL))
        {
            IotLogInfo("Acknowledgment message for PUBLISH %.*s will be sent.\r\n", (int ) messageNumberLength,
                        pPayload);
        }
        else
        {
            IotLogWarn("Acknowledgment message for PUBLISH %.*s will NOT be sent.\r\n", (int ) messageNumberLength,
                        pPayload);
        }
    }
}

/*****************************************************************************************************************************
 * Initialize the MQTT library.
 *
 *	Return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise, failed to initialize MQTT library.
 ****************************************************************************************************************************/
static int mqtt_preinit(void)
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;

    mqttInitStatus = IotMqtt_Init ();

    if (IOT_MQTT_SUCCESS != mqttInitStatus)
    {
        /* Failed to initialize MQTT library. */
        status = EXIT_FAILURE;
    }

    return status;
}

/*************************************************************************************************************************
 * Clean up the MQTT library.
 ************************************************************************************************************************/
void mqtt_cleanup(void)
{
    IotMqtt_Cleanup ();
}

/************************************************************************************************************************
 * Establish a new connection to the MQTT server.
 *
 * @param[in] awsIotMqttMode Specify if this Application Project is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this Application Project.
 * @param[out] pMqttConnection Set to the handle to the new MQTT connection.
 *
 * @return `EXIT_SUCCESS` if the connection is successfully established; `EXIT_FAILURE`
 * otherwise.
 *************************************************************************************************************************/
static int mqtt_connect(mqtt_cfg_t *ptr, IotMqttConnection_t *pMqttConnection)
{
    int status = EXIT_SUCCESS;
    IotMqttError_t connectStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t nwkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t conInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    char pClientIdentifierBuffer[CLIENT_IDENTIFIER_MAX_LENGTH] =  { 0 };

    /* Set the members of the network info not set by the initializer. This
     * struct provided information on the transport layer to the MQTT connection.
     */
    nwkInfo.createNetworkConnection = true;
    nwkInfo.u.setup.pNetworkServerInfo = (void *) ptr->p_nwk_srvr_info;
    nwkInfo.u.setup.pNetworkCredentialInfo = (void *) ptr->p_nwk_credn_info;
    nwkInfo.pNetworkInterface = ptr->p_nwk_intf;

#if (1 == IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES ) && defined( IOT_DEMO_MQTT_SERIALIZER )
    nwkInfo.pMqttSerializer = IOT_DEMO_MQTT_SERIALIZER;
#endif

    /* Set the members of the connection info not set by the initializer. */
    conInfo.awsIotMqttMode = ptr->aws_mqtt_mode;
    conInfo.cleanSession = true;
    conInfo.keepAliveSeconds = KEEP_ALIVE_SECONDS;
    conInfo.pWillInfo = &willInfo;

    /* Set the members of the Last Will and Testament (LWT) message info. The
     * MQTT server will publish the LWT message if this client disconnects
     * unexpectedly. */
    willInfo.pTopicName = WILL_TOPIC_NAME;
    willInfo.topicNameLength = WILL_TOPIC_NAME_LENGTH;
    willInfo.pPayload = WILL_MESSAGE;
    willInfo.payloadLength = WILL_MESSAGE_LENGTH;

    /* Use the parameter client identifier if provided. Otherwise, generate a
     * unique client identifier. */
    if ((NULL != ptr->p_identifier) && ('\0' != ptr->p_identifier[0]))
    {
        conInfo.pClientIdentifier = ptr->p_identifier;
        conInfo.clientIdentifierLength = (uint16_t) strlen (ptr->p_identifier);
    }
    else
    {
        /* Every active MQTT connection must have a unique client identifier. The Application Projects
         * generate this unique client identifier by appending a timestamp to a common
         * prefix. 
         */
        status = snprintf (pClientIdentifierBuffer, CLIENT_IDENTIFIER_MAX_LENGTH,
                           CLIENT_IDENTIFIER_PREFIX "%lu", (long unsigned int) IotClock_GetTimeMs ());

        /* Check for errors from snprintf. */
        if (status < 0)
        {
            IotLogError ("** Failed to generate unique client identifier for Application Project. **\r\n");
            status = EXIT_FAILURE;
        }
        else
        {
            /* Set the client identifier buffer and length. */
            conInfo.pClientIdentifier = pClientIdentifierBuffer;
            conInfo.clientIdentifierLength = (uint16_t) status;
            status = EXIT_SUCCESS;
        }
    }

    /* Establish the MQTT connection. */
    if (EXIT_SUCCESS == status)
    {

        IotLogInfo("MQTT Application Project client identifier is %s \r\n",(char *)conInfo.pClientIdentifier);

        connectStatus = IotMqtt_Connect (&nwkInfo, &conInfo,
                                         MQTT_TIMEOUT_MS,
                                         pMqttConnection);

        if (IOT_MQTT_SUCCESS != connectStatus)
        {
            IotLogError ("** MQTT CONNECT returned error %s. **\r\n", IotMqtt_strerror (connectStatus));
            status = EXIT_FAILURE;
        }
    }
    return status;
}

/*******************************************************************************************************************************
 * Add or remove subscriptions by either subscribing or unsubscribing.
 *
 * @param[in] mqttConnection The MQTT connection to use for subscriptions.
 * @param[in] operation Either #IOT_MQTT_SUBSCRIBE or #IOT_MQTT_UNSUBSCRIBE.
 * @param[in] pTopicFilters Array of topic filters for subscriptions.
 * @param[in] pCallbackParameter The parameter to pass to the subscription
 * callback.
 *
 * return `EXIT_SUCCESS` if the subscription operation succeeded; `EXIT_FAILURE`
 * otherwise.
 ******************************************************************************************************************************/
static int mqtt_modify_subscriptions(IotMqttConnection_t lmqttConn, IotMqttOperationType_t operation,
                                     const char **pTopicFilters)
{
    int status = EXIT_SUCCESS;
    int32_t i = 0;
    IotMqttError_t subscriptionStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t pSubscriptions[TOPIC_FILTER_COUNT] =  { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Set the members of the subscription list. */
    for (i = 0; i < TOPIC_FILTER_COUNT; i++)
    {
        pSubscriptions[i].qos = IOT_MQTT_QOS_1;
        pSubscriptions[i].pTopicFilter = pTopicFilters[i];
        pSubscriptions[i].topicFilterLength = (uint16_t) strlen (pTopicFilters[i]);
        pSubscriptions[i].callback.function = mqtt_subscription_cb;
    }

    /* Modify subscriptions by either subscribing or unsubscribing. */
    if (IOT_MQTT_SUBSCRIBE == operation)
    {
        subscriptionStatus = IotMqtt_TimedSubscribe (lmqttConn, pSubscriptions,
                                                     TOPIC_FILTER_COUNT,
                                                     0,
                                                     MQTT_TIMEOUT_MS);

        /* Check the status of SUBSCRIBE. */
        switch (subscriptionStatus)
        {
            case IOT_MQTT_SUCCESS:
                 IotLogInfo ("All Application Project topic filter subscriptions accepted.\r\n");
            break;

            case IOT_MQTT_SERVER_REFUSED:

                /* Check which subscriptions were rejected before exiting the Application Project. */
                for (i = 0; i < TOPIC_FILTER_COUNT; i++)
                {
                    if (IotMqtt_IsSubscribed (lmqttConn, pSubscriptions[i].pTopicFilter,
                                              pSubscriptions[i].topicFilterLength,
                                              NULL) == true)
                    {
                        IotLogInfo ("Topic filter %.*s was accepted.",
                                     pSubscriptions[i].topicFilterLength,
                                     pSubscriptions[i].pTopicFilter);
                    }
                    else
                    {
                        IotLogError ("Topic filter %.*s was rejected.",
                                      pSubscriptions[i].topicFilterLength,
                                      pSubscriptions[i].pTopicFilter);
                    }
                }
                status = EXIT_FAILURE;
            break;

            default:
                status = EXIT_FAILURE;
            break;
        }
    }
    else if (IOT_MQTT_UNSUBSCRIBE == operation)
    {
        subscriptionStatus = IotMqtt_TimedUnsubscribe (lmqttConn, pSubscriptions,
                                                       TOPIC_FILTER_COUNT,
                                                       0,
                                                       MQTT_TIMEOUT_MS);

        /* Check the status of UNSUBSCRIBE. */
        if (IOT_MQTT_SUCCESS != subscriptionStatus)
        {
            status = EXIT_FAILURE;
        }
    }
    else
    {
        /* Only SUBSCRIBE and UNSUBSCRIBE are valid for modifying subscriptions. */
        IotLogError ("** MQTT operation %s is not valid for modifying subscriptions. ** \r\n",IotMqtt_OperationType (operation));
        status = EXIT_FAILURE;
    }

    return status;
}

/**********************************************************************************************************************************
 * Transmit Temperature Sensor messages to the mqtt server.
 *
 * @param[in] lmqttConn, The MQTT connection to use for publishing.
 * @param[in] pTopicNames topic names for publishing. 
 *
 * return `EXIT_SUCCESS` if all messages are published and received; `EXIT_FAILURE` otherwise.
 *********************************************************************************************************************************/
static int mqtt_publish_messages(IotMqttConnection_t lmqttConn, const char **pTopicNames, void *pPublishPayload,
                                 int payloadLen)
{
    int status = EXIT_SUCCESS;
    static intptr_t publishCount = 0;
    IotMqttError_t publishStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttCallbackInfo_t publishComplete = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* The MQTT library should invoke this callback when a PUBLISH message
     * is successfully transmitted. */
    publishComplete.function = publish_completion_cb;

    /* Set the common members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.topicNameLength = (uint16_t) strlen (*pTopicNames);
    publishInfo.pPayload = pPublishPayload;
    publishInfo.retryMs = PUBLISH_RETRY_MS;
    publishInfo.retryLimit = PUBLISH_RETRY_LIMIT;
    publishInfo.pTopicName = pTopicNames[0];
    publishInfo.payloadLength = (size_t) payloadLen;

    publishCount++;

    /* Pass the PUBLISH number to the operation complete callback. */
    publishComplete.pCallbackContext = (void *) publishCount;

    /* PUBLISH a message. This is an asynchronous function that notifies of completion through a callback. */
    publishStatus = IotMqtt_Publish (lmqttConn, &publishInfo, 0, &publishComplete, NULL);

    if (IOT_MQTT_STATUS_PENDING != publishStatus)
    {
        IotLogError("** MQTT PUBLISH Count %d returned error %s. **\r\n",(int) publishCount, IotMqtt_strerror(publishStatus));
        status = EXIT_FAILURE;
    }
    return status;
}

/*******************************************************************************************************************************
 * This function initializes, connects and adds the topic Subscription list and prepares the 
 * application Project for running, called by the Application Code.
 *
 * @param[in] mqtt_cfg structure which holds the configuration required for mqtt connectivity 
 * to MQTT server. 
 * @return `EXIT_SUCCESS` if the Application Project completes successfully; `EXIT_FAILURE` otherwise.
 ******************************************************************************************************************************/
int mqtt_init(mqtt_cfg_t *ptr)
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;

    /* Initialize the libraries required for this Application Project. */
    status = mqtt_preinit ();

    if (EXIT_SUCCESS == status)
    {
        /* Update the status of mqtt libraries as initialized. */
        ptr->mqtt_lib_init_status = true;

        /* Establish a new MQTT connection. */
        status = mqtt_connect (ptr, &mqttConn);
    }

    if (EXIT_SUCCESS == status)
    {
        /* Update the status of the MQTT connection as established. */
        ptr->mqtt_connect_status = true;

        /* Add the topic filter subscriptions used in this Application Project. */
        status = mqtt_modify_subscriptions (mqttConn, IOT_MQTT_SUBSCRIBE, psTopics);
    }

    return status;
}
/*******************************************************************************************************************************
 * This function deinitializes, disconnects and removes the topic Subscription list and prepares the 
 * application Project for termination, called by the Application Code.
 *
 * @param[in] mqtt_cfg structure which holds the configuration required for mqtt connectivity 
 * to MQTT server. 
 * @return `EXIT_SUCCESS` if the Application Project completes successfully; `EXIT_FAILURE` otherwise.
 ******************************************************************************************************************************/
int mqtt_deinit(mqtt_cfg_t *ptr)
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;

	/* Remove the topic filter subscriptions used in this Application Project. */
    //status = mqtt_modify_subscriptions (mqttConn, IOT_MQTT_UNSUBSCRIBE, psTopics);

    if (EXIT_SUCCESS != status)
    {
        status = EXIT_FAILURE; 
    }
    else if (true == ptr->mqtt_lib_init_status)
    {
        /* Establish a new MQTT connection. */
        IotMqtt_Disconnect (mqttConn, IOT_MQTT_FLAG_CLEANUP_ONLY);
		
        /* Initialize the libraries required for this Application Project. */
        mqtt_cleanup ();
		ptr->mqtt_lib_init_status = false;
		
        /* Update the status of the MQTT connection as disconnected. */
        ptr->mqtt_connect_status = false;
    }

    return status;
}

/*******************************************************************************************************************************
 * This function creates the payload for the User Push button messages in UTF8 of JSON format and Publishes the Message 
 * called by the Application Code.
 *
 * @param[in] uint16_t switch_num indcating the User Push button S1 or S2. 
 * @return `EXIT_SUCCESS` if the Application Project completes successfully; `EXIT_FAILURE` otherwise.
 ******************************************************************************************************************************/
int sendMessage_pb(uint16_t lswitch_num)
{
    int status = EXIT_SUCCESS;
#if USR_MQTT_DATA_FORMAT == UTF8
    char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_PB] = { RESET_VALUE };
	int  payloadlength = 0;

    /* Generate the payload for the PUBLISH. */
    payloadlength = snprintf (pubPayload,
                              PUBLISH_PAYLOAD_BUFFER_LENGTH_PB,
                              PUBLISH_PAYLOAD_FORMAT_PB, (int) lswitch_num);

#elif USR_MQTT_DATA_FORMAT == JSON

    char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_PB_JSON] = { RESET_VALUE };
    int payloadlength = RESET_VALUE;

	payloadlength += snprintf((char *)(pubPayload), PUBLISH_PAYLOAD_BUFFER_LENGTH_TS_JSON,
									   PUBLISH_PAYLOAD_FORMAT_PB_JSON,lswitch_num);
#endif

    /* Check for errors from snprintf. */
    if (payloadlength < 0)
    {
        IotLogError("** Failed to generate MQTT PUBLISH payload for PUBLISH %s. **\r\n", (char *) PUBLISH_PAYLOAD_FORMAT_PB);
        return EXIT_FAILURE;
    }

    /* PUBLISH  messages. */
    status = mqtt_publish_messages (mqttConn, pTopics_pb, &pubPayload, payloadlength);
    return status;
}

/*******************************************************************************************************************************
 * This function creates the payload for the temperature sensor data messages in UTF8 of JSON format and Publishes the Message 
 * called by the Application Code.
 *
 * @param[in]  Formatted string of the Temperature  indcating the MCU Temperature in fahrenheit
 * @return `EXIT_SUCCESS` if the Application Project completes successfully; `EXIT_FAILURE` otherwise.
 ******************************************************************************************************************************/
int sendMessage_ts(char *ltemp_str)
{
    int status = EXIT_SUCCESS;
#if USR_MQTT_DATA_FORMAT == UTF8
    char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_TS] = { RESET_VALUE };
	int  payloadlength = RESET_VALUE;
	
    /* Generate the payload for the PUBLISH. */
	payloadlength = snprintf (pubPayload,
                              PUBLISH_PAYLOAD_BUFFER_LENGTH_TS,
                              PUBLISH_PAYLOAD_FORMAT_TS, (char *) ltemp_str);

#elif USR_MQTT_DATA_FORMAT == JSON
    char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_TS_JSON] = { RESET_VALUE };
	int  payloadlength = RESET_VALUE;

	payloadlength += snprintf((char *)(pubPayload), PUBLISH_PAYLOAD_BUFFER_LENGTH_TS_JSON,
	                            	   PUBLISH_PAYLOAD_FORMAT_TS_JSON,ltemp_str);
	
#endif

    /* Check for errors from snprintf. */
    if (payloadlength < 0)
    {
        IotLogError("** Failed to generate MQTT PUBLISH payload for PUBLISH %s. **\r\n", (char *) PUBLISH_PAYLOAD_FORMAT_TS);
        return EXIT_FAILURE;
    }

    /* PUBLISH  messages. */
    status = mqtt_publish_messages(mqttConn, pTopics_ts,&pubPayload,payloadlength);
    return status;
}



#ifdef MQTT_REQUIRED

//! [MQTT-required-functions]

/********************************************************************************************************************************
 * Retrieves the unique identifier for the device.
 ********************************************************************************************************************************/
const char *getDeviceIdentifier(void)
{
    return deviceIdentifier;
}

/********************************************************************************************************************************
 * Retrieves the device metrics to be sent to cloud.
 ********************************************************************************************************************************/
const char *getDeviceMetrics(void)
{
    return deviceMetrics;
}

/********************************************************************************************************************************
 * Get the device metrics length.
 ********************************************************************************************************************************/
uint16_t getDeviceMetricsLength(void)
{
    return (uint16_t) (strlen (deviceMetrics));
}

#endif
