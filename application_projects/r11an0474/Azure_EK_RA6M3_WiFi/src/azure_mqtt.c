/***********************************************************************************************************************
 * File Name    : azure_mqtt.c
 * Description  : Azure MQTT API Source file
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

#include "azure_mqtt.h"
#include "azure.h"
#include <azure/iot/az_iot_provisioning_client.h>
#include <azure/iot/az_iot_hub_client.h>
#include <azure/core/az_result.h>
#include <azure/core/az_span.h>
#include <azure/core/_az_cfg.h>
#include <azure/core/az_json.h>




// DO NOT MODIFY: Device ID Environment Variable Name
#define ENV_DEVICE_ID "AZ_IOT_DEVICE_ID"
// DO NOT MODIFY: IoT Hub Hostname Environment Variable Name
#define ENV_IOT_HUB_HOSTNAME "AZ_IOT_HUB_HOSTNAME"

#define AZ_RETURN_IF_NOT_ENOUGH_SIZE(span, required_size)          \
  do                                                                       \
  {                                                                        \
    int32_t _iot_sample_req_sz = (required_size);                          \
    if (az_span_size(span) < _iot_sample_req_sz || _iot_sample_req_sz < 0) \
    {                                                                      \
      return AZ_ERROR_NOT_ENOUGH_SPACE;                                    \
    }                                                                      \
  } while (0)

/* Global declarations */
static az_iot_hub_client mqtt_client;
static char telemetry_topic[WIDTH_128]          = { 0 };
static char mqtt_username[WIDTH_256]            = { 0 };
static char mqtt_client_id[WIDTH_128]           = { 0 };
static char device_id[WIDTH_64]                 = { 0 };
static char iot_hub_hostname[WIDTH_128]         = { 0 };
static char methods_response_topic[ WIDTH_128 ] = { 0 };

/* Function prototypes */
static int azure_iot_hub_connect(char *hostName, char *username, char *password, IotMqttConnection_t * pMqttConnection);
int send_method_response(char* topic);
static int azure_report_property(IotMqttConnection_t MqttConnection);


int azure_mqtt_init(void)
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;

    mqttInitStatus = IotMqtt_Init();

    if(mqttInitStatus != IOT_MQTT_SUCCESS)
    {
        status = EXIT_FAILURE;
    }

    return status;
}

az_result read_configuration_entry(
        const char* env_name,
        char* default_value,
        az_span buffer,
        az_span* out_value)
{
      char* env_value = getenv(env_name);

      if (env_value != NULL)
      {
        az_span env_span = az_span_create_from_str(env_value);
        AZ_RETURN_IF_NOT_ENOUGH_SIZE(buffer, az_span_size(env_span));
        az_span_copy(buffer, env_span);
        *out_value = az_span_slice(buffer, 0, az_span_size(env_span));
      }
      else if (default_value != NULL)
      {
        az_span default_span = az_span_create_from_str(default_value);
        AZ_RETURN_IF_NOT_ENOUGH_SIZE(buffer, az_span_size(default_span));
        az_span_copy(buffer, default_span);
        *out_value = az_span_slice(buffer, 0, az_span_size(default_span));
      }
      else
      {
        printf("(missing) Please set the %s environment variable.\r\n", env_name);
        return AZ_ERROR_ARG;
      }

      return AZ_OK;
}

int send_telemetry(IotMqttConnection_t MqttConnection, const char *msg)
{
    int rc;

    if(az_result_failed(
            rc = az_iot_hub_client_telemetry_get_publish_topic(
                    &mqtt_client, NULL, telemetry_topic, sizeof(telemetry_topic), NULL)))
    {
        return rc;
    }

    azure_mqtt_publish(MqttConnection, telemetry_topic, msg);

    return 0;
}

int azure_hub_connect(char *hostName, IotMqttConnection_t *pMqttConnection)
{
    int rc;

    if(az_result_failed(rc = azure_iothub_read_configuration(hostName)))
    {
        printf("Failed to read configuration from environment variables, return code %d\n", rc);
        return rc;
    }

    // Get the MQTT client id used for the MQTT connection
    if (az_result_failed(
            rc = az_iot_hub_client_get_client_id(
                &mqtt_client, mqtt_client_id, sizeof(mqtt_client_id), NULL)))
    {
        printf("Failed to get MQTT clientId, return code %d\n", rc);
        return rc;
    }

    printf("mqtt_client_id: %s\r\n\r\n", mqtt_client_id);

    // Get the MQTT user name used to connect to IoT Hub
    if (az_result_failed(rc = az_iot_hub_client_get_user_name(&mqtt_client, mqtt_username, sizeof(mqtt_username), NULL)))
    {
        printf("Failed to get MQTT username, return code %d\n", rc);
        return rc;
    }

    printf("mqtt_username: %s\r\n\r\n", mqtt_username);

    //connect to Azure IoT Hub
    if(IOT_MQTT_SUCCESS != (rc = azure_iot_hub_connect(hostName, mqtt_username, NULL, pMqttConnection)))
    {
        printf("Connection to Azure IoT Hub Failed!!!\r\n");
        return rc;
    }

    printf("\r\nConnected to %s.\r\n", hostName);

    //subscribe to topics to get incoming data from Azure IoT Hub
    if((rc = azure_mqtt_subscribe(*pMqttConnection, AZ_IOT_HUB_CLIENT_METHODS_SUBSCRIBE_TOPIC)) != IOT_MQTT_SUCCESS)
    {
        printf("Failed to subscribe, return code %d\r\n", rc);
        return rc;
    }

    printf("Subscribed to %s\r\n", AZ_IOT_HUB_CLIENT_METHODS_SUBSCRIBE_TOPIC);

    //update properties
    azure_report_property(*pMqttConnection);

    return pdPASS;
}

//read the user environment variables used to connect to IoT Hub
az_result azure_iothub_read_configuration(char *hostName)
{
    az_span device_id_span = AZ_SPAN_FROM_BUFFER(device_id);

    AZURE_RETURN_IF_FAILED(read_configuration_entry(
          ENV_DEVICE_ID , AZURE_DEVICE_ID, device_id_span, &device_id_span));

    az_span iot_hub_hostname_span = AZ_SPAN_FROM_BUFFER(iot_hub_hostname);
    AZURE_RETURN_IF_FAILED(read_configuration_entry(
        ENV_IOT_HUB_HOSTNAME ,
        hostName,
        iot_hub_hostname_span,
        &iot_hub_hostname_span));

    //Initialize the hub client with the hub host endpoint
    AZURE_RETURN_IF_FAILED(az_iot_hub_client_init(
            &mqtt_client,
            az_span_slice(iot_hub_hostname_span, 0, (int32_t)strlen(iot_hub_hostname)),
            az_span_slice(device_id_span, 0, (int32_t)strlen(device_id)),
            NULL));

    return AZ_OK;
}


static int azure_iot_hub_connect(char *hostName, char *username, char *password, IotMqttConnection_t * pMqttConnection)
{
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    IotNetworkServerInfo_t xServerInfo = { 0 };
    IotMqttNetworkInfo_t xNetworkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotNetworkCredentials_t xCredentials = {0};
    IotMqttConnectInfo_t xMqttConnectInfo = {0};

    /* Set the members of the connection info not set by the initializer. */
    xMqttConnectInfo.awsIotMqttMode = false;  //Not connecting to AWS
    xMqttConnectInfo.cleanSession = true;
    xMqttConnectInfo.keepAliveSeconds = KEEP_ALIVE_SECONDS;
    xMqttConnectInfo.pWillInfo = NULL;
    xMqttConnectInfo.pUserName = username;
    xMqttConnectInfo.userNameLength = (uint16_t)strlen(username);
    xMqttConnectInfo.pPassword = password;

    if(xMqttConnectInfo.pPassword == NULL)
        xMqttConnectInfo.passwordLength = 0;
    else
        xMqttConnectInfo.passwordLength = (uint16_t)strlen(password);

    xMqttConnectInfo.pClientIdentifier = AZURE_DEVICE_ID;
    xMqttConnectInfo.clientIdentifierLength = (uint16_t)strlen(AZURE_DEVICE_ID);

    xCredentials.pRootCa = ROOTCA_X509_CERT_PEM_FILE;
    xCredentials.rootCaSize = strlen(ROOTCA_X509_CERT_PEM_FILE) + 1;
    xCredentials.pPrivateKey = DEVICE_PRIVATE_KEY_FILE;
    xCredentials.privateKeySize = strlen(DEVICE_PRIVATE_KEY_FILE) + 1;
    xCredentials.pClientCert = DEVICE_X509_TRUST_PEM_FILE;
    xCredentials.clientCertSize = strlen(DEVICE_X509_TRUST_PEM_FILE) + 1;

    /* Set the server info. */
    xServerInfo.pHostName = hostName;
    xServerInfo.port = AZURE_MQTT_SERVER_PORT;

    /* Set the members of the network info. */
    xNetworkInfo.createNetworkConnection = true;
    xNetworkInfo.u.setup.pNetworkServerInfo = &xServerInfo;
    xNetworkInfo.u.setup.pNetworkCredentialInfo = &xCredentials;
    xNetworkInfo.pNetworkInterface = IOT_NETWORK_INTERFACE_AFR;

    xMqttStatus = IotMqtt_Connect( &xNetworkInfo,
                                             &xMqttConnectInfo,
                                             MQTT_TIMEOUT_MS,
                                             pMqttConnection );
    if(IOT_MQTT_SUCCESS !=  xMqttStatus)
    {
        printf("MQTT connection status %d \r\n", xMqttStatus);
    }

    return xMqttStatus;
}




static char reported_property_topic[WIDTH_128]  = { 0 };
static char reported_prop_payload_buf[WIDTH_128]= { 0 };
static az_span reported_property_topic_request_id = AZ_SPAN_LITERAL_FROM_STR("reported_prop");
static az_span reported_property_name = AZ_SPAN_LITERAL_FROM_STR("manufacturer");
static az_span reported_property_value = AZ_SPAN_LITERAL_FROM_STR(MANUFACTURER_STRING);

static az_result build_reported_property(az_span json_payload, az_span *out_json_payload)
{
    az_json_writer jw;

    AZURE_RETURN_IF_FAILED(az_json_writer_init(&jw, json_payload, NULL));
    AZURE_RETURN_IF_FAILED(az_json_writer_append_begin_object(&jw));
    AZURE_RETURN_IF_FAILED(az_json_writer_append_property_name(&jw, reported_property_name));
    AZURE_RETURN_IF_FAILED(az_json_writer_append_string(&jw, reported_property_value));
    AZURE_RETURN_IF_FAILED(az_json_writer_append_end_object(&jw));

    *out_json_payload = az_json_writer_get_bytes_used_in_destination(&jw);

    return AZ_OK;
}

static int azure_report_property(IotMqttConnection_t MqttConnection)
{
    int rc = 0;

    // Get the topic used to send a reported property update
    if (az_result_failed(
              rc = az_iot_hub_client_twin_patch_get_publish_topic(
                  &mqtt_client,
                  reported_property_topic_request_id,
                  reported_property_topic,
                  sizeof(reported_property_topic),
                  NULL)))
    {
        printf("Unable to get twin document publish topic, return code %d\n", rc);
        return rc;
    }

    // Twin reported properties must be in JSON format. The payload is constructed here.
    az_span json_payload = AZ_SPAN_FROM_BUFFER(reported_prop_payload_buf);
    if (az_result_failed(rc = build_reported_property(json_payload, &json_payload)))
    {
        return rc;
    }

    printf("Payload: %.*s\n", (int) az_span_size(json_payload), (char*)az_span_ptr(json_payload));

    // Publish the reported property payload to IoT Hub
    azure_mqtt_publish(MqttConnection, reported_property_topic, (char*)az_span_ptr(json_payload));

    return 0;
}


static void _mqttSubscriptionCallback(void * param1, IotMqttCallbackParam_t * const pPublish)
{
    ( void )param1;
    int rc = 0;

    printf("\r\n#############################\r\n");
    printf( "Incoming message:\r\n\r\n"
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
                    (char *) pPublish->u.message.info.pPayload );

    printf("\r\n#############################\r\n");

    if(strcmp(AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, pPublish->u.message.pTopicFilter) == 0)
    {
        xQueueSend(g_dps_queue, &pPublish->u.message.info, portMAX_DELAY );
    }
    else
    {
        // Parse the incoming message topic and check to make sure it is a methods message
        az_iot_hub_client_method_request method_request;

        if (!az_result_failed(rc = az_iot_hub_client_methods_parse_received_topic(
                  &mqtt_client, az_span_create((uint8_t*)pPublish->u.message.info.pTopicName, pPublish->u.message.info.topicNameLength), &method_request)))

        {
            // Get the response topic to publish the method response
            if (az_result_failed(rc = az_iot_hub_client_methods_response_get_publish_topic(
                &mqtt_client,
                method_request.request_id,
                200,
                methods_response_topic,
                sizeof(methods_response_topic),
                NULL)))
            {
                printf("Unable to get response publish topic");
                return;
            }

            printf("Response: %s\r\n", methods_response_topic);

            if((rc = send_method_response(methods_response_topic)) != 0)
            {
                printf("Unable to send response, status %d\r\n", rc);
            }
        }
    }
}

static void _operationCompleteCallback(void * param1, IotMqttCallbackParam_t * const pOperation)
{
    ( void )param1;

    /* Print the status of the completed operation. A PUBLISH operation is
     * successful when transmitted over the network. */
    if( pOperation->u.operation.result == IOT_MQTT_SUCCESS )
    {
        printf( "MQTT %s successfully sent.\r\n",
                    IotMqtt_OperationType( pOperation->u.operation.type ));
    }
    else
    {
        printf( "MQTT %s could not be sent. Error %s.\r\n",
                     IotMqtt_OperationType( pOperation->u.operation.type ),
                     IotMqtt_strerror( pOperation->u.operation.result ) );
    }
}


int azure_mqtt_publish(IotMqttConnection_t MqttConnection, const char *pPublishTopic, const char *msg)
{
    IotMqttError_t publishStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttCallbackInfo_t publishComplete = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* The MQTT library should invoke this callback when a PUBLISH message
     * is successfully transmitted. */
    publishComplete.function = _operationCompleteCallback;


    /* Set the common members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = pPublishTopic;
    publishInfo.topicNameLength = (uint16_t)strlen(pPublishTopic);
    publishInfo.pPayload = msg;
    publishInfo.payloadLength = strlen(msg);
    publishInfo.retryLimit = PUBLISH_RETRY_LIMIT;
    publishInfo.retryMs = PUBLISH_RETRY_MS;

    /* PUBLISH a message. This is an asynchronous function that notifies of
     * completion through a callback. */
    publishStatus = IotMqtt_Publish(MqttConnection,
                                    &publishInfo,
                                    0,
                                    &publishComplete,
                                    NULL);
    if( publishStatus != IOT_MQTT_STATUS_PENDING )
    {
        printf( "MQTT PUBLISH returned error %s.\r\n",
                                 IotMqtt_strerror( publishStatus ) );
    }

    return publishStatus;
}


int azure_mqtt_subscribe(IotMqttConnection_t MqttConnection, const char *pSubTopics)
{
    IotMqttError_t subscriptionStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t Subscriptions = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    Subscriptions.qos = IOT_MQTT_QOS_1;
    Subscriptions.pTopicFilter = pSubTopics;
    Subscriptions.topicFilterLength = (uint16_t)strlen(pSubTopics);
    Subscriptions.callback.pCallbackContext = NULL;
    Subscriptions.callback.function = _mqttSubscriptionCallback;

    subscriptionStatus = IotMqtt_TimedSubscribe( MqttConnection,
                                                 &Subscriptions,
                                                 1,
                                                 0,
                                                 MQTT_TIMEOUT_MS );

    /* Check the status of SUBSCRIBE. */
    switch( subscriptionStatus )
    {
        case IOT_MQTT_SUCCESS:
            printf( "\r\nAll demo topic filter subscriptions accepted.\r\n" );
            break;

        case IOT_MQTT_SERVER_REFUSED:
            /* Check which subscriptions were rejected before exiting the demo. */
            if( IotMqtt_IsSubscribed( MqttConnection,
                                      Subscriptions.pTopicFilter,
                                      Subscriptions.topicFilterLength,
                                      NULL ) == true )
            {
                printf( "Topic filter %.*s was accepted.\r\n",
                            Subscriptions.topicFilterLength,
                            Subscriptions.pTopicFilter );
            }
            else
            {
                printf( "Topic filter %.*s was rejected.\r\n",
                             Subscriptions.topicFilterLength,
                             Subscriptions.pTopicFilter );
            }

            break;

        default:
            break;
    }

    return subscriptionStatus;
}

int azure_mqtt_unsubscribe(IotMqttConnection_t MqttConnection, const char *pSubTopics)
{
    IotMqttError_t subscriptionStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t Subscriptions = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    Subscriptions.qos = IOT_MQTT_QOS_1;
    Subscriptions.pTopicFilter = pSubTopics;
    Subscriptions.topicFilterLength = (uint16_t)strlen(pSubTopics);
    Subscriptions.callback.pCallbackContext = NULL;
    Subscriptions.callback.function = _mqttSubscriptionCallback;

    subscriptionStatus = IotMqtt_TimedUnsubscribe(MqttConnection,
                                                  &Subscriptions,
                                                  1,
                                                  0,
                                                  MQTT_TIMEOUT_MS);

    return subscriptionStatus;
}



