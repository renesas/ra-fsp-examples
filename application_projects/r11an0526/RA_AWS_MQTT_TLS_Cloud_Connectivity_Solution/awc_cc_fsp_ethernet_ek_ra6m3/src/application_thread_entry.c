/***********************************************************************************************************************
 * File Name    : application_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "aws_dev_mode_key_provisioning.h"
#include "common_utils.h"
#include "mqtt_demo_helpers.h"
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "usr_network.h"
#include "usr_hal.h"
#include "usr_config.h"
#include "usr_data.h"
#include "application_thread.h"

/*************************************************************************************
 * Macro definitions
 ************************************************************************************/

/* How many topic filters will be used in this Application Project. */
#define MAX_NUM_PUBLISH_TOPIC               ( 3 )

#define PUBLISH_PAYLOAD_FORMAT_PB_JSON "{\r\n" \
                                       "\"status\" : {\r\n" \
                                       "      \"%s\" :\"Pressed\"\r\n" \
                                       "\r\n         }\r\n" \
                                       "}\r\n"

#define PUBLISH_PAYLOAD_BUFFER_LENGTH_PB_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_TS_JSON ) + 2 )

#define PUBLISH_PAYLOAD_FORMAT_TS_JSON "{\r\n" \
                                       "\"status\" : {\r\n" \
                                       "       \"temperature(F)\" :\"%s\"\r\n" \
                                       "\r\n         }\r\n" \
                                       "}\r\n"

#define PUBLISH_PAYLOAD_BUFFER_LENGTH_TS_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_TS_JSON ) + 5 )


/*************************************************************************************
 * global functions
 ************************************************************************************/
void mqtt_cleanup_and_reinit(void);

/*************************************************************************************
 * global variables
 ************************************************************************************/
/* The MQTT context used for MQTT operation. */
MQTTContext_t xMqttContext;

/* The network context used for TLS operation. */
NetworkContext_t xNetworkContext;

mqtt_status_t g_mqtt_status =
{ .mqtt_connect_status = false, .status = pdFALSE};

char cBuffer[16] = { RESET_VALUE };

/* Topics used for the Publishing update in this Application Project. */
const char *pTopics_publish[MAX_NUM_PUBLISH_TOPIC] =
{
    "",
    "aws/topic/switch_status",
    "aws/topic/temperature",
};

/* Topics used for the LED actuation in this Application Project. This is Subsription Topic */
const char *pTopics_subscirbe[1] =
{
    "aws/topic/led",
};

/*************************************************************************************
 * Private global functions
 ************************************************************************************/
static fsp_err_t config_littlFs_flash(void);
static void RedLedDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static BaseType_t sendMessage(const char **pTopicNames, uint8_t Topics_index, char * message);
static void print_float(char *buffer, size_t buflen, double value);
static int parse_led_actuation_message(mqtt_rx_payload_t *lmq_data);
uint32_t get_mqtt_fail_count(void);
/*************************************************************************************
 * Private global variables
 ************************************************************************************/

static char CLIENT_CERTIFICATE_PEM[] = CLIENT_CERTIFICATE;
static char CLIENT_KEY_PEM[]         = CLIENT_KEY;

/* Static buffer used to hold MQTT messages being sent and received. */
static uint8_t ucSharedBuffer[mqttexampleNETWORK_BUFFER_SIZE];

/* Static buffer used to hold MQTT messages being sent and received. */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = mqttexampleNETWORK_BUFFER_SIZE
};

static uint32_t ulIPAddress = RESET_VALUE;
static mqtt_rx_payload_t mq_data = { RESET_VALUE };

static uint32_t mqtt_fail_count   = RESET_VALUE;
static double mcu_temp __attribute__ ((used)) = RESET_VALUE;
static char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_TS_JSON] = { RESET_VALUE };

/* Net Thread entry function */
/* pvParameters contains TaskHandle_t */
void application_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    fsp_pack_version_t      version = {RESET_VALUE};
    fsp_err_t               err = FSP_SUCCESS;
    BaseType_t              bt_status = pdFALSE;
    int                     ierr = FSP_SUCCESS;
    uint32_t                ip_status = RESET_VALUE;
    uint16_t                switch_num = RESET_VALUE;
    char                    temp_str[32] =  { RESET_VALUE };
    const char            * pcTopicFilter = NULL;
    uint16_t                topicFilterLength = RESET_VALUE;

    ProvisioningParams_t    params = {RESET_VALUE};

    params.pucClientPrivateKey       = (uint8_t *) CLIENT_KEY_PEM;
    params.pucClientCertificate      = (uint8_t *) CLIENT_CERTIFICATE_PEM;
    params.ulClientPrivateKeyLength  = 1 + strlen((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen((const char *) params.pucClientCertificate);

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT (BANNER_INFO, AP_VERSION, version.major, version.minor, version.patch);

    err = usr_hal_init();
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** HAL Initialization failed **\r\n");
        APP_ERR_TRAP(err);
    }

    err = config_littlFs_flash();
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** littleFs flash config failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize IOT FreeRTOS Libraries */
    bt_status = SYSTEM_Init();
    if (pdPASS != bt_status)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** Socket Init failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize the crypto hardware acceleration. */
    ierr = mbedtls_platform_setup(NULL);
    if (FSP_SUCCESS != ierr)
    {
        APP_ERR_PRINT("** HW SCE Init failed **\r\n");
        APP_ERR_TRAP(ierr);
    }

    /* Prints the Ethernet Configuration header prior to the IP Init. and prints the
    * Ethernet and IP configuration prior to IP Init. */
    APP_PRINT(ETH_PREINIT);
    print_ipconfig();

    /* Network initialization: FreeRTOS IP Initialization; This initializes the IP stack  */
    bt_status = network_ip_init();
    if(pdFALSE == bt_status)
    {
       FAILURE_INDICATION
       APP_ERR_PRINT("User Network Initialization Failed");
       APP_ERR_TRAP(bt_status);
    }

    APP_PRINT("waiting for network link up");
    bt_status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY);
    if (pdTRUE != bt_status)
    {
       FAILURE_INDICATION
       APP_ERR_PRINT("xTaskNotifyWait Failed");
       APP_ERR_TRAP(bt_status);
    }

    NETWORK_CONNECT_INDICATION
    /* Prints the Ethernet Configuration Header for Post IP Init*/
    APP_PRINT(ETH_POSTINIT);
    /* Get the IP address for the MQTT END POINT used for the application*/
    ulIPAddress = FreeRTOS_gethostbyname((char*)USR_MQTT_ENDPOINT);

    if( RESET_VALUE == ulIPAddress )
    {
        FAILURE_INDICATION
        IotLogError("FreeRTOS_gethostbyname  Failed to get the End point address for %s",USR_MQTT_ENDPOINT);
        IotLogErrorTrap(RESET_VALUE);
    }

    /* Convert the IP address to a string to print on to the console. */
    FreeRTOS_inet_ntoa( ulIPAddress, ( char * ) cBuffer);
    /* Prints the Ethernet and IP Configuration post IP Init */
    print_ipconfig();

    APP_PRINT("\r\nDNS Lookup for \"%s\" is      : %s  \r\n", USR_MQTT_ENDPOINT,  cBuffer);

    /* A simple example to demonstrate key and certificate provisioning in code flash using PKCS#11 interface.
     * This should be replaced by production ready key provisioning mechanism. */
    err = vAlternateKeyProvisioning (&params);
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** Alternate Key Provisioning failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /******** Attempt to establish TLS session with MQTT broker. **********/
    bt_status = EstablishMqttSession(&xMqttContext, &xNetworkContext, &xBuffer, true);
    if( bt_status == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("Failed to connect to MQTT broker."));
        APP_ERR_TRAP(bt_status);
    }

    g_mqtt_status.mqtt_connect_status = true;

    pcTopicFilter = pTopics_subscirbe[0];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[0]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   RedLedDataCallback );
    if( bt_status == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    g_mqtt_status.status = pdTRUE;
    AWS_CONNECT_INDICATION

    APP_PRINT("Successful MQTT Connection to the end point %s\r\n", USR_MQTT_ENDPOINT);
    APP_PRINT("Device is Ready for Publishing and Subscription of Messages \r\n\r\n");

    while (1)
    {
        bt_status = xQueueReceive (g_topic_queue, &mq_data, QUEUE_WAIT);
        if(bt_status == pdTRUE)
        {
            switch (mq_data.id)
            {
                case ID_PB:
                {
                    APP_PRINT("Push Button S%d Pressed\r\n", mq_data.value.pb_data.pb_num);
                    switch_num = mq_data.value.pb_data.pb_num;

                    if(pdTRUE == g_mqtt_status.status)
                    {
                        if(switch_num == PBS1)
                        {
                            g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_PB, "switch-1");
                        }
                        else if(switch_num == PBS2)
                        {
                            g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_PB, "switch-2");
                        }

                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** Push Button Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION
                        }
                    }
                    else
                    {
                        APP_PRINT("MQTT Cleanup and reinit in Progress. Messages will be published once Up and Running \r\n");
                    }
                }
                break;

                case ID_TEMPERATURE:
                {
                    mcu_temp = ((((float) mq_data.value.adc_data.adc_value) * 0.353793f) - 467.39f);
                    print_float (temp_str, sizeof(temp_str), mcu_temp);
                    APP_PRINT("MCU Temperature  %s \r\n",temp_str);

                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_TEMPERATURE, temp_str);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** Temperature Sensor Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION
                        }
                    }
                    else
                    {
                        APP_PRINT("MQTT Cleanup and reinit in Progress. Messages will be published once Up and Running \r\n");
                    }
                }
                break;

                case ID_LED_ACT:
                {
                    APP_PRINT("Topic Received from Cloud %s \r\n",mq_data.value.led_data.led_act_topic_msg);
                    if (FSP_SUCCESS != parse_led_actuation_message (&mq_data))
                    {
                        APP_ERR_PRINT("** LED Topic Actuation returned  Error **\r\n");
                        mqtt_fail_count++;
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION
                    }
                }
                break;

                default:
                {
                    APP_ERR_PRINT("** Error in MQTT Messages.. MQTT Client Cleanup started  **\r\n");
                    mqtt_fail_count++;
                    mqtt_cleanup_and_reinit ();
                }
                break;
            }
        }
        else
        {
            if(g_mqtt_status.mqtt_connect_status)
            {
                bt_status = ProcessLoop( &xMqttContext, PROCESS_LOOP_TIMEOUT);
                if( bt_status != pdPASS )
                {
                    APP_ERR_PRINT("MQTT normal process fail\r\n");
                }
            }
        }
    }
}

/*********************************************************************************************************************
 * @brief   configures the littleFS Flash.
 *
 * This function sets up the littleFS Flash for Data storage.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 *********************************************************************************************************************/
fsp_err_t config_littlFs_flash(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if(FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** littleFs Initialization failed **\r\n");
    }

    err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if(FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** littleFs Flash Format failed **\r\n");
    }

    err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if(FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** littleFs Mount failed **\r\n");
    }

    return err;
}

/*-----------------------------------------------------------*/

static void RedLedDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t led_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    led_data.id = ID_LED_ACT;
    strncpy ((char *) &led_data.value.led_data.led_act_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &led_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}

/*-----------------------------------------------------------*/
/*******************************************************************************************************************************
 * This function creates the payload for the User Push button messages in UTF8 of JSON format and Publishes the Message
 * called by the Application Code.
 *
 * @param[in] uint16_t switch_num indicating the User Push button S1 or S2.
 * @return `EXIT_SUCCESS` if the Application Project completes successfully; `EXIT_FAILURE` otherwise.
 ******************************************************************************************************************************/
static BaseType_t sendMessage(const char **pTopicNames, uint8_t Topics_index, char * message)
{
    BaseType_t      xReturnStatus = pdFALSE;
    const char    * pcTopicFilter = NULL;
    uint16_t        topicFilterLength = 0;
    int             payloadlength = 0;

    pcTopicFilter = pTopicNames[Topics_index];
    topicFilterLength = (uint16_t) strlen (pTopicNames[Topics_index]);
    memset(pubPayload, 0x00, sizeof(pubPayload));
    /* Generate the payload for the PUBLISH. */
    if(Topics_index == (uint8_t)ID_PB)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_PB_JSON, message);
    }
    else if(Topics_index == (uint8_t)ID_TEMPERATURE)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_TS_JSON, message);
    }
    else
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), message);
    }

    /* Check for errors from snprintf. */
    if (payloadlength < 0)
    {
        IotLogError("** Failed to generate MQTT PUBLISH payload for PUBLISH %s. **\r\n", (char *) pcTopicFilter);
        return EXIT_FAILURE;
    }

    /* PUBLISH  messages. */
    xReturnStatus = PublishToTopic( &xMqttContext,
                                    pcTopicFilter,
                                    topicFilterLength,
                                    pubPayload);
    return xReturnStatus;
}

/*-----------------------------------------------------------*/

/*********************************************************************************************************************
 * convert float to string format and store it in buffer
 *********************************************************************************************************************/
static void print_float(char *buffer, size_t buflen, double value)
{
    char sign = RESET_VALUE;
    int integer = (int) value;
    int fraction = (int) ((value - integer) * 100.0);

    if (NULL == buffer)
    {
        return;
    }
    if (value < 0.0)
    {
        sign = '-';
        snprintf (buffer, buflen, "%c%02d.%02d", sign, abs (integer), abs (fraction));
    }
    else
    {
        snprintf (buffer, buflen, "%02d.%02d", abs (integer), abs (fraction));
    }
}

/*-----------------------------------------------------------*/

/*********************************************************************************************************************
 * @brief   parse_message function
 *
 * This function parse the incoming MQTT message and actuates the LED on the Board.
 *********************************************************************************************************************/
static int parse_led_actuation_message(mqtt_rx_payload_t *lmq_data)
{
    int status = RESET_VALUE;

    const char msg_red_led_on[] = "{\"Red_LED\":\"ON\"}";
    const char msg_red_led_off[] = "{\"Red_LED\":\"OFF\"}";
    const char msg_blue_led_on[] = "{\"Blue_LED\":\"ON\"}";
    const char msg_blue_led_off[] = "{\"Blue_LED\":\"OFF\"}";
    const char msg_green_led_on[] = "{\"Green_LED\":\"ON\"}";
    const char msg_green_led_off[] = "{\"Green_LED\":\"OFF\"}";

    if (NULL == lmq_data)
    {
        IotLogErrorTrap("\r\n NULL pointer in parse_message \r\n");
        status = 0xFF;
    }
    else
    {
        /* Red LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_red_led_on))
        {
            led_on_off (LED_RED, LED_ON);
            IotLogInfo("\r\nRed LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_red_led_off))
        {
            led_on_off (LED_RED, LED_OFF);
            IotLogInfo("\r\nRed LED OFF \r\n");
        }

        /* Blue LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_blue_led_on))
        {
            led_on_off (LED_BLUE, LED_ON);
            IotLogInfo("\r\nBlue LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_blue_led_off))
        {
            led_on_off (LED_BLUE, LED_OFF);
            IotLogInfo("\r\nBlue LED OFF \r\n");
        }

        /* Green LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_green_led_on))
        {
            led_on_off (LED_GREEN, LED_ON);
            IotLogInfo("\r\nGreen LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_green_led_off))
        {
            led_on_off (LED_GREEN, LED_OFF);
            IotLogInfo("\r\nGreen LED OFF \r\n");
        }
    }
    return status;
}

uint32_t get_mqtt_fail_count(void)
{
    return mqtt_fail_count;
}

/*-----------------------------------------------------------*/

/*********************************************************************************************************************
 * @brief   cleans up the mqtt by deinitializing and reiniting for cleaner connectivity.
 *
 *  This is called by the application when error in sending or receiving the MQTT messages are encountered at the
 *  Application level.
 *********************************************************************************************************************/
void mqtt_cleanup_and_reinit(void)
{
    BaseType_t      xReturnStatus = pdFALSE;
    const char    * pcTopicFilter = NULL;
    uint16_t        topicFilterLength = 0;
    MQTTSubscribeInfo_t pSubscriptionList[1];

    if(!g_mqtt_status.mqtt_connect_status)
        return;

    g_mqtt_status.mqtt_connect_status = false;
    IotLogInfo("MQTT Client Cleaning up \r\n");

    pcTopicFilter = pTopics_subscirbe[0];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[0]);
    /* Remove the callback for subscribed topics. */
    SubscriptionManager_RemoveCallback( pcTopicFilter, topicFilterLength );

    /* Set the subscription list memory to zero . */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* Populate the array with topic filters to unsubscribe from.
     * We will use a single UNSUBSCRIBE packet to unsubscribe from all
     * topic filters. */
    pSubscriptionList[ 0 ].pTopicFilter = pcTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = topicFilterLength;
    /* Unsubscribe from all topic filters of temperature, humidity and
     * precipitation data. */
    xReturnStatus = UnsubscribeFromTopicFilters( &xMqttContext,
                                                 pSubscriptionList,
                                                 sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ) );

    /* Send an MQTT Disconnect packet over the already connected TCP socket.
     * There is no corresponding response for the disconnect packet. After sending
     * disconnect, client must close the network connection. */
    IotLogInfo("Disconnecting the MQTT connection with %s.\r\n", democonfigMQTT_BROKER_ENDPOINT);

    if( xReturnStatus == pdFAIL )
    {
        /* Returned status is not used to update the local status as there
         * were failures in demo execution. */
        ( void ) DisconnectMqttSession( &xMqttContext, &xNetworkContext );
    }
    else
    {
        xReturnStatus = DisconnectMqttSession( &xMqttContext, &xNetworkContext );
    }

    AWS_ACTIVITY_INDICATION
    NETWORK_ACTIVITY_INDICATION

    do
    {
        vTaskDelay (1);
    }while(FSP_SUCCESS != isNetworkUp());

    NETWORK_ACTIVITY_INDICATION

    xReturnStatus = EstablishMqttSession(&xMqttContext, &xNetworkContext, &xBuffer, true);
    if( xReturnStatus == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        IotLogError(("Failed to connect to MQTT broker."));
        IotLogErrorTrap(xReturnStatus);
    }

    xReturnStatus = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                 pcTopicFilter,
                                                 topicFilterLength,
                                                 RedLedDataCallback );
    if( xReturnStatus == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        IotLogError(("subscribeToAndRegisterTopicFilter fail"));
        IotLogErrorTrap(xReturnStatus);
    }

    g_mqtt_status.status = pdTRUE;
    g_mqtt_status.mqtt_connect_status = true;
    AWS_CONNECT_INDICATION
}
