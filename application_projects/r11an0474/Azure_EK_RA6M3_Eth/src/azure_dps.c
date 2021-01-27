/***********************************************************************************************************************
 * File Name    : azure_dps.c
 * Description  : Azure DPS Source file
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

#include "user_cfg.h"
#include "azure.h"
#include <ctype.h>
#include <azure/iot/az_iot_provisioning_client.h>
#include <azure/core/az_result.h>
#include <azure/core/az_span.h>
#include <main_thread.h>

#include "azure_mqtt.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"


// DO NOT MODIFY: the path to a PEM file containing the device certificate and
// key as well as any intermediate certificates chaining to an uploaded group certificate.
#define ENV_DEVICE_X509_CERT_PEM_FILE       "AZ_IOT_DEVICE_X509_CERT_PEM_FILE"

// DO NOT MODIFY: the path to a PEM file containing the server trusted CA
// This is usually not needed on Linux or Mac but needs to be set on Windows.
#define ENV_DEVICE_X509_TRUST_PEM_FILE      "AZ_IOT_DEVICE_X509_TRUST_PEM_FILE"
// Service information
#define GLOBAL_PROVISIONING_ENDPOINT_ENV        "AZ_IOT_GLOBAL_PROVISIONING_ENDPOINT"
#define ID_SCOPE_ENV                            "AZ_IOT_ID_SCOPE"
// Device information
#define DEVICE_ID_ENV                           "AZ_IOT_REGISTRATION_ID"

/* Global declarations */
static char global_provisioning_endpoint[256] = {0};
static char id_scope[WIDTH_16] = {0};
static char registration_id[WIDTH_32] = {0};
static char dps_client_id[WIDTH_128] = {0};
static char dps_username[WIDTH_128] = {0};
static char register_publish_topic[WIDTH_128];
static char query_topic[WIDTH_256];
static az_iot_provisioning_client provisioning_client;

/* Function Prototypes */
static az_result azure_dps_read_configuration(void);
static int azure_dps_connect(IotMqttConnection_t * pMqttConnection, char *username, char* password);
static int azure_dps_register_device(IotMqttConnection_t MqttConnection);
static void print_az_span(az_span span);
int azure_dps_get_operation_status(IotMqttConnection_t MqttConnection, char *hostName);
int getAzureHubInfo(char *hostName);

/* Function to retrieve the hostname from DPS */
int getAzureHubInfo(char *hostName)
{
    int rc = pdFAIL;
    IotMqttConnection_t dpsConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    if(az_result_failed(rc = azure_dps_read_configuration()))
    {
        printf("Failed to read configuration from environment variables, return code %d\n", rc);
        return rc;
    }

    // Get the MQTT client id used for the MQTT connection
    if(az_result_failed(rc = az_iot_provisioning_client_get_client_id(
            &provisioning_client, dps_client_id, sizeof(dps_client_id), NULL)))
    {
        printf("Failed to get MQTT clientId, return code %d\n", rc);
        return rc;
    }

    //Get the MQTT user name to connect to the provisioning service
    if(az_result_failed(
            rc = az_iot_provisioning_client_get_user_name(
                    &provisioning_client, dps_username, WIDTH_128, NULL)))
    {
        printf("Failed to get MQTT username, return code %d\n", rc);
        return rc;
    }

    printf("dps_username: %s\r\n", dps_username);

    //connect to the provisioning service
    if(IOT_MQTT_SUCCESS != (rc = azure_dps_connect(&dpsConnection, dps_username, NULL )))
    {
        printf("Connection to Azure DPS Failed!!!\r\n");
        return rc;
    }

    printf("Connected to %s.\n", AZURE_DPS_DEFAULT_ENDPOINT);

    //subscribe to the necessary provisioning topic to receive provisioning responses
    if((rc = azure_mqtt_subscribe(dpsConnection, AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC)) != IOT_MQTT_SUCCESS)
    {
        printf("Failed to subscribe, return code %d\r\n", rc);
        return rc;
    }

    printf("Subscribed to %s\r\n",AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC);

    //Begin the registration process by sending a registration request message
    rc = azure_dps_register_device(dpsConnection);
    if(rc)
    {
        printf("azure_dps_register_device failed!!!\r\n");
        return rc;
    }

    rc = azure_dps_get_operation_status(dpsConnection, hostName);
    if(rc)
    {
        printf("azure_dps_get_operation_status failed!!!\r\n");
        return rc;
    }

    return pdPASS;
}

// Print an az_span to the console
static void print_az_span(az_span span)
{
  char* buffer = (char*)az_span_ptr(span);
  for (int32_t i = 0; i < az_span_size(span); i++)
  {
    putchar(*buffer++);
  }

  puts("\r\n");
}

//read the user environment variables used to connect to the provisioning service
static az_result azure_dps_read_configuration(void)
{
    az_span endpoint_span = AZ_SPAN_FROM_BUFFER(global_provisioning_endpoint);
    az_span id_scope_span = AZ_SPAN_FROM_BUFFER(id_scope);
    az_span registration_id_span = AZ_SPAN_FROM_BUFFER(registration_id);

    AZURE_RETURN_IF_FAILED(read_configuration_entry(GLOBAL_PROVISIONING_ENDPOINT_ENV,
                                                 AZURE_DPS_DEFAULT_ENDPOINT,
                                                 endpoint_span,
                                                 &endpoint_span));

    AZURE_RETURN_IF_FAILED(read_configuration_entry(
          ID_SCOPE_ENV, AZURE_SCOPE_ID, id_scope_span, &id_scope_span));


    AZURE_RETURN_IF_FAILED(read_configuration_entry(
          DEVICE_ID_ENV,
          AZURE_DEVICE_ID,
          registration_id_span,
          &registration_id_span));

    //Initialize the provisioning client with the provisioning endpoint
    AZURE_RETURN_IF_FAILED(az_iot_provisioning_client_init(
          &provisioning_client, endpoint_span, id_scope_span, registration_id_span, NULL));

    return AZ_OK;
}

static int azure_dps_connect(IotMqttConnection_t * pMqttConnection, char *username, char* password)
{
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    IotNetworkServerInfo_t xServerInfo = { 0 };
    IotMqttNetworkInfo_t xNetworkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotNetworkCredentials_t xCredentials = {0};
    IotMqttConnectInfo_t xMqttConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

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
    xServerInfo.pHostName = AZURE_DPS_DEFAULT_ENDPOINT;
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
        printf("MQTT connection status %x \r\n", xMqttStatus);
    }

    return xMqttStatus;
}

//send the register pub message to the provisioning service
static int azure_dps_register_device(IotMqttConnection_t MqttConnection)
{
    int rc;

    //Get the topic to send the provisioning request
    if(az_result_failed(
            rc = az_iot_provisioning_client_register_get_publish_topic(
                    &provisioning_client, register_publish_topic, sizeof(register_publish_topic), NULL)))
    {
        printf("Failed to get MQTT PUB register topic, return code %d\r\n", rc);
        return rc;
    }

    char message[64] = {0};
    sprintf(message, "{\"registrationId\": \"%s\"}",AZURE_DEVICE_ID);
    printf("Sending register pub message to DPS..\r\n");
    azure_mqtt_publish(MqttConnection, register_publish_topic, message);
    return 0;
}

//Receive the status of the registration request
int azure_dps_get_operation_status(IotMqttConnection_t MqttConnection, char *hostName)
{
    int rc;
    bool is_operation_complete = false;
    az_iot_provisioning_client_register_response response;
    IotMqttPublishInfo_t PublishInfo;

    /* Continue to parse incoming responses from the provisioning service until
       the device has been successfully provisioned or an error occurs
    */
    while(!is_operation_complete)
    {
        //Wait for data
        xQueueReceive(g_dps_queue, &PublishInfo, portMAX_DELAY );

        //parse the incoming message and payload
        if(az_result_failed(
                rc = az_iot_provisioning_client_parse_received_topic_and_payload(
                        &provisioning_client,
                        az_span_create((uint8_t*)PublishInfo.pTopicName, PublishInfo.topicNameLength),
                        az_span_create((uint8_t*)PublishInfo.pPayload, (int32_t)PublishInfo.payloadLength),
                        &response)))
        {
            printf("Message from unknown topic, return code %d\r\n", rc);
            return rc;
        }

        //Check whether or not the operation is complete
        is_operation_complete = az_iot_provisioning_client_operation_complete(response.operation_status);

        if(!is_operation_complete)
        {
            // In case the operation is not complete, issue a new query to the service
            // Get the topic to send the query message
            if(az_result_failed(rc = az_iot_provisioning_client_query_status_get_publish_topic(
                                    &provisioning_client, response.operation_id, query_topic, sizeof(query_topic),NULL)))
            {
                printf("Failed to get operation status topic, return code %d\n", rc);
                return rc;
            }

            //wait the recommended retry after number of seconds before query
            printf("Querying after %u seconds...\r\n", (unsigned int)response.retry_after_seconds);

            vTaskDelay(response.retry_after_seconds * 1000);

            //publish the query message
            rc = azure_mqtt_publish(MqttConnection, query_topic,"");
        }
        else
        {
            //Successful assignment - print out the assigned hostname and device id
            if(response.operation_status == AZ_IOT_PROVISIONING_STATUS_ASSIGNED)
            {
                printf("\r\n");
                printf("SUCCESS - Device provisioned:\r\n");
                printf("\r\nAzure Hub Hostname: ");
                print_az_span(response.registration_state.assigned_hub_hostname);
                printf("Device Id: ");
                print_az_span(response.registration_state.device_id);

                if(hostName)
                {
                    strncpy(hostName, (char*)az_span_ptr(response.registration_state.assigned_hub_hostname),(size_t)az_span_size(response.registration_state.assigned_hub_hostname));
                }
            }
            else    //unassigned, failed or disabled states
            {
                printf("ERROR - Device Provisioning failed:\n");
                printf("\tRegistration state: %d\r\n", response.operation_status);
                printf("\tLast operation status: %d\n", response.status);
                printf("\tOperation ID: ");
                print_az_span(response.operation_id);
                printf("\tError code: %u\n", (unsigned int)response.registration_state.extended_error_code);
                printf("\tError message: ");
                print_az_span(response.registration_state.error_message);
                printf("\tError timestamp: ");
                print_az_span(response.registration_state.error_timestamp);
                printf("\tError tracking ID: ");
                print_az_span(response.registration_state.error_tracking_id);
                if (response.retry_after_seconds > 0)
                {
                  printf("\tRetry-after: %u seconds.", (unsigned int)response.retry_after_seconds);
                }

                hostName = NULL;
            }

            //unsubscribe DPS topics
            if((rc = azure_mqtt_unsubscribe(MqttConnection, AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC)) != IOT_MQTT_SUCCESS)
            {
                printf("Failed to unsubscribe, return code %d\r\n", rc);
                return rc;
            }

            //Disconnect from DPS
            IotMqtt_Disconnect(MqttConnection, 0);

            printf("\r\n***** Detached from DPS *****\r\n");
        }
    }

    return 0;
}

#pragma GCC diagnostic pop
