/***********************************************************************************************************************
 * File Name    : app_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2022] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include <console_thread.h>
#include "app_thread.h"
#include "aws_dev_mode_key_provisioning.h"
#include "common_utils.h"
#include "mqtt_demo_helpers.h"
#include "iot_logging_task.h"
#include "usr_hal.h"
#include "usr_config.h"
#include "usr_data.h"
#include "flash/flash_hp.h"

/*************************************************************************************
 * Macro definitions
 ************************************************************************************/
#define DISABLE_SNI                         (true)
#define MQTT_RECEIVE_TIMEOUT                (500)
#define MQTT_SEND_TIMEOUT                   (500)
#define MILLISECONDS_PER_SECOND             (1000U)
#define MILLISECONDS_PER_TICK               ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )
#define APP_DEBUG_MSG_CNTRL                 (1)
#define APP_DEBUG_MSG_SEQ_CNTRL             (1)

/* How many topic filters will be used in this Application Project. */
#define MAX_NUM_PUBLISH_TOPIC               (8)

#define MAX_NUM_SUBSCRIBE_TOPIC             (9)


#define CERTIFICATE_SIZE                    (2048)
#define KEY_SIZE                            (2048)
#define ENDPOINT_SIZE                       (128)

/* PUBLISH Topic Payload in the JSON format used in this Application Project. */

#define PUBLISH_PAYLOAD_FORMAT_IAQ_JSON     "{\r\n"\
                                            "\"IAQ\" : {\r\n"\
                                            "      \"TVOC (mg/m^3)\" :\"%s\",\r\n"\
											"      \"Etoh (ppm)\" :\"%s\",\r\n"\
											"      \"eco2 (ppm)\" :\"%s\"\r\n"\
                                            "     }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_IAQ_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_IAQ_JSON ) + 15 )


#define PUBLISH_PAYLOAD_FORMAT_OAQ_JSON     "{\r\n"\
                                            "\"OAQ\" : {\r\n"\
                                            "      \"air quality (Index)\" :\"%s\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_OAQ_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_OAQ_JSON ) + 5 )


#define PUBLISH_PAYLOAD_FORMAT_HS3001_JSON  "{\r\n"\
                                            "\"HS3001\" : {\r\n"\
                                            "      \"Humidity ()\" :\"%s\",\r\n"\
                                            "      \"Temperature (F)\" :\"%s\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_HS3001_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_HS3001_JSON ) + 10 )

#define PUBLISH_PAYLOAD_FORMAT_ICM_JSON     "{\r\n"\
                                            "\"ICM\" : {\r\n"\
                                            "   \"acc\" : {\r\n"\
                                            "      \"x \" :\"%s\",\r\n"\
                                            "      \"y \" :\"%s\",\r\n"\
                                            "      \"z \" :\"%s\"\r\n"\
                                            "   \r\n      },\r\n"\
                                            "   \"mag\" : {\r\n"\
                                            "      \"x \" :\"%s\",\r\n"\
                                            "      \"y \" :\"%s\",\r\n"\
                                            "      \"z \" :\"%s\"\r\n"\
                                            "   \r\n      },\r\n"\
                                            "   \"gyr\" : {\r\n"\
                                            "      \"x \" :\"%s\",\r\n"\
                                            "      \"y \" :\"%s\",\r\n"\
                                            "      \"z \" :\"%s\"\r\n"\
                                            "   \r\n      }\r\n"\
                                            "\r\n      }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_ICM_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_ICM_JSON ) + 45 )

#define PUBLISH_PAYLOAD_FORMAT_ICP_JSON     "{\r\n"\
                                            "\"ICP\" : {\r\n"\
                                            "      \"Temperature (F)\" :\"%s\",\r\n"\
                                            "      \"Pressure (Pa)\" :\"%s\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_ICP_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_ICP_JSON ) + 10 )

#define PUBLISH_PAYLOAD_FORMAT_OB1203_JSON  "{\r\n"\
                                            "\"OB1203\" : {\r\n"\
                                            "      \"spo2 ()\" :\"%s\",\r\n"\
                                            "      \"Heart Rate ()\" :\"%s\",\r\n"\
                                            "      \"Breath rate ()\" :\"%s\",\r\n"\
                                            "      \"P2P ()\" :\"%s\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_OB1203_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_OB1203_JSON ) + 20 )


#define PUBLISH_PAYLOAD_FORMAT_BULK_JSON     "{\r\n"\
                                             "\"IAQ\" : {\r\n"\
                                             "      \"TVOC (mg/m^3)\" :\"%s\",\r\n"\
                                             "      \"Etoh (ppm)\" :\"%s\",\r\n"\
                                             "      \"eco2 (ppm)\" :\"%s\"\r\n"\
                                             "          },\r\n"\
                                             "\"OAQ\" : {\r\n"\
	                                         "      \"air quality (Index)\" :\"%s\"\r\n"\
	                                         "          },\r\n"\
	                                         "\"HS3001\" : {\r\n"\
	                                         "      \"Humidity ()\" :\"%s\",\r\n"\
	                                         "      \"Temperature (F)\" :\"%s\"\r\n"\
	                                         "             },\r\n"\
                                             "\"ICM\" : {\r\n"\
                                             "   \"acc\" : {\r\n"\
                                             "      \"x \" :\"%s\",\r\n"\
                                             "      \"y \" :\"%s\",\r\n"\
                                             "      \"z \" :\"%s\"\r\n"\
                                             "             },\r\n"\
                                             "   \"mag\" : {\r\n"\
                                             "      \"x \" :\"%s\",\r\n"\
                                             "      \"y \" :\"%s\",\r\n"\
                                             "      \"z \" :\"%s\"\r\n"\
                                             "             },\r\n"\
                                             "   \"gyr\" : {\r\n"\
                                             "      \"x \" :\"%s\",\r\n"\
                                             "      \"y \" :\"%s\",\r\n"\
                                             "      \"z \" :\"%s\"\r\n"\
                                             "             }\r\n"\
                                             "         },\r\n"\
                                             "\"ICP\" : {\r\n"\
                                             "      \"Temperature (F)\" :\"%s\",\r\n"\
                                             "      \"Pressure (Pa)\" :\"%s\"\r\n"\
                                             "            },\r\n"\
	                                         "\"OB1203\" : {\r\n"\
	                                         "      \"spo2 ()\" :\"%s\",\r\n"\
	                                         "      \"Heart Rate ()\" :\"%s\",\r\n"\
	                                         "      \"Breath rate ()\" :\"%s\",\r\n"\
	                                         "      \"P2P ()\" :\"%s\"\r\n"\
	                                         "              }\r\n"\
                                             "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_BULK_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_BULK_JSON ) + 130 )

extern void get_time();
extern char* getDNS();
extern bool setupCellular( void );
extern void CRYPTO_Init( void );


/*************************************************************************************
 * Private functions
 ************************************************************************************/
static fsp_err_t config_littlFs_flash(void);
static BaseType_t sendMessage(const char **pTopicNames, uint8_t Topics_index, char *message, uint8_t sizeIdx);

static void IAQDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void OAQDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void HS3001DataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void ICMDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void ICPDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void OB1203DataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void BulkDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void temperatureLedDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void spo2LedDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );

static int parse_temperature_led_actuation_message(mqtt_rx_payload_t *lmq_data);
static int parse_spo2_led_actuation_message(mqtt_rx_payload_t *lmq_data);
/*************************************************************************************
 * global functions
 ************************************************************************************/
void mqtt_cleanup_and_reinit(void);
uint32_t get_mqtt_fail_count(void);

extern void print_float(char *buffer, size_t buflen, double value);

extern TaskHandle_t sensor_thread;
extern char g_certificate[CERTIFICATE_SIZE];
extern char g_private_key[KEY_SIZE];
extern char g_mqtt_endpoint[ENDPOINT_SIZE];
extern uint32_t  console_status;

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

/* For testing purpose the initial values are random, later needs to be moved to the right location */
usr_iaq_data_t app_iaq_data = {RESET_VALUE};
usr_oaq_data_t app_oaq_data = {RESET_VALUE};
usr_hs3001_data_t app_hs3001_data = {RESET_VALUE};
icm_acc_data_t app_acc_data =     {RESET_VALUE};
icm_mag_data_t app_icm_mag_data = {RESET_VALUE};
icm_gyr_data_t app_icm_gyr_data = {RESET_VALUE};
usr_icm_data_t app_icm_data = {RESET_VALUE};
usr_icp_data_t app_icp_data = {RESET_VALUE};
usr_ob1203_data_t app_ob1203_data = {RESET_VALUE};
bulk_sens_data_t app_bulk_sens_data = {RESET_VALUE};


/* Topics used for the Publishing update in this Application Project. */
const char *pTopics_publish[MAX_NUM_PUBLISH_TOPIC] =
{
    "",
    "aws/topic/iaq_sensor_data",
    "aws/topic/oaq_sensor_data",
    "aws/topic/hs3001_sensor_data",
    "aws/topic/icm_sensor_data",
    "aws/topic/icp_sensor_data",
    "aws/topic/ob1203_sensor_data",
    "aws/topic/bulk_sensor_data",
};

/* Topics used for the Data Pull and LED actuation in this Application Project. This is Subscription Topic */
const char *pTopics_subscirbe[MAX_NUM_SUBSCRIBE_TOPIC] =
{
    "aws/topic/get_iaq_sensor_data",
    "aws/topic/get_oaq_sensor_data",
    "aws/topic/get_hs3001_sensor_data",
    "aws/topic/get_icm_sensor_data",
    "aws/topic/get_icp_sensor_data",
    "aws/topic/get_ob1203_sensor_data",
    "aws/topic/get_bulk_sensor_data",
    "aws/topic/set_temperature_led_data",
    "aws/topic/set_spo2_led_data",
};

struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

/*************************************************************************************
 * Private variables
 ************************************************************************************/
static char CLIENT_CERTIFICATE_PEM[2048];
static char CLIENT_KEY_PEM[2048];
static char USER_MQTT_ENDPOINT[128];
/* Static buffer used to hold MQTT messages being sent and received. */
static uint8_t ucSharedBuffer[mqttexampleNETWORK_BUFFER_SIZE];
/* Static buffer used to hold MQTT messages being sent and received. */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = mqttexampleNETWORK_BUFFER_SIZE
};

static mqtt_rx_payload_t mq_data = { RESET_VALUE };
static uint32_t mqtt_fail_count   = RESET_VALUE;
static char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_BULK_JSON] = { RESET_VALUE };


/* The network context used for TLS operation. */
NetworkContext_t networkContext;

char * pHostResolvedAddress;
TlsTransportStatus_t    tls_status;
char                    temp_str[21][32] =  { RESET_VALUE };



/* Application Thread entry function */
/* pvParameters contains TaskHandle_t */
void app_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_pack_version_t      version = {RESET_VALUE};
    fsp_err_t               err = FSP_SUCCESS;
    BaseType_t              bt_status = pdFALSE;
    int                     ierr = FSP_SUCCESS;
    const char            * pcTopicFilter = NULL;
    uint16_t                topicFilterLength = RESET_VALUE;
    NetworkCredentials_t    credentials;
    ProvisioningParams_t    params = {RESET_VALUE};
    TlsTransportParams_t    transport_params;
    bool retCellular = true;
    static uint32_t   sequenceNumber = RESET_VALUE;

    bt_status = xTaskNotifyWait(pdFALSE, pdFALSE, &console_status, portMAX_DELAY);
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Copy AWS client certificate, private key and MQTT end point */
    memcpy (CLIENT_CERTIFICATE_PEM, g_certificate, strlen(g_certificate));
    memcpy (CLIENT_KEY_PEM, g_private_key, strlen(g_private_key));
    memcpy (USER_MQTT_ENDPOINT, g_mqtt_endpoint, strlen(g_mqtt_endpoint));

    params.pucClientPrivateKey       = (uint8_t *) CLIENT_KEY_PEM;
    params.pucClientCertificate      = (uint8_t *) CLIENT_CERTIFICATE_PEM;
    params.ulClientPrivateKeyLength  = 1 + strlen((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen((const char *) params.pucClientCertificate);

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT (BANNER_INFO, AP_VERSION, version.major, version.minor, version.patch);

    err = config_littlFs_flash();
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION
        APP_ERR_PRINT("** littleFs flash config failed **\r\n");
        APP_ERR_TRAP(err);
    }

    CRYPTO_Init();

    /* Initialize the crypto hardware acceleration. */
    ierr = mbedtls_platform_setup(NULL);
    if (FSP_SUCCESS != ierr)
    {
        APP_ERR_PRINT("** HW SCE Init failed **\r\n");
        APP_ERR_TRAP(ierr);
    }

    /* Start the logging task */
    xLoggingTaskInitialize(256, 1, 10); // NOLINT(readability-magic-numbers)

    /* Initialize network context */
    networkContext.pParams = &transport_params;

    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_04_PIN_09, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_04_PIN_09, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(3000, BSP_DELAY_UNITS_MILLISECONDS); // NOLINT(readability-magic-numbers)

    /* Setup cellular. */
    retCellular = setupCellular();

    if( false == retCellular )
    {
        APP_ERR_PRINT("Cellular failed to initialize.\r\n" );
        APP_ERR_TRAP(retCellular);
    }
    APP_PRINT ("Cellular Setup Done \r\n");

    pHostResolvedAddress = getDNS(USER_MQTT_ENDPOINT);

    /* A simple example to demonstrate key and certificate provisioning in code flash using PKCS#11 interface.
     * This should be replaced by production ready key provisioning mechanism. */

    err = vAlternateKeyProvisioning (&params);
    if (FSP_SUCCESS != err)
    {
	    FAILURE_INDICATION
	    APP_ERR_PRINT("** Alternate Key Provisioning failed **\r\n");
	    APP_ERR_TRAP(err);
    }

    /* Setup network credentials */
    credentials.pAlpnProtos = NULL;
    credentials.disableSni = DISABLE_SNI;
    credentials.pRootCa = ( const unsigned char * ) SERVER_CERTIFICATE_PEM;
    credentials.rootCaSize = sizeof( SERVER_CERTIFICATE_PEM );
    credentials.pUserName = NULL;
    credentials.userNameSize = 0;
    credentials.pPassword = NULL;
    credentials.passwordSize = 0;
    credentials.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    credentials.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    /* TLS Connect - to a MQTT host */
    tls_status = TLS_FreeRTOS_Connect(&networkContext, pHostResolvedAddress, 8883, &credentials, MQTT_RECEIVE_TIMEOUT, MQTT_SEND_TIMEOUT);

    if(TLS_TRANSPORT_SUCCESS == tls_status )
    {
        APP_PRINT("TLS Connect Success %d  \r\n",tls_status);
    }
    else
    {
        APP_PRINT("TLS Connect Failure %d \r\n",tls_status);
    }


    /******** Attempt to establish TLS session with MQTT broker. **********/
    bt_status = EstablishMqttSession(&xMqttContext, &networkContext, &xBuffer, true);
    if( bt_status == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("Failed to connect to MQTT broker."));
        APP_ERR_TRAP(bt_status);
    }

    g_mqtt_status.mqtt_connect_status = true;

	/*****************************************************************/
	/* Before starting the Data transfer, make sure the Sensor Thread is up and running and */

    pcTopicFilter = pTopics_subscirbe[0];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[0]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   IAQDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[1];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[1]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   OAQDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[2];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[2]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   HS3001DataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[3];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[3]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   ICMDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[4];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[4]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   ICPDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[5];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[5]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   OB1203DataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[6];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[6]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   BulkDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[7];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[7]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   temperatureLedDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    pcTopicFilter = pTopics_subscirbe[8];
    topicFilterLength = (uint16_t) strlen (pTopics_subscirbe[8]);
    bt_status = subscribeToAndRegisterTopicFilter( &xMqttContext,
                                                   pcTopicFilter,
                                                   topicFilterLength,
                                                   spo2LedDataCallback );
    if( pdFAIL == bt_status )
    {
        /* Log error to indicate connection failure. */
        APP_ERR_PRINT(("subscribeToAndRegisterTopicFilter fail"));
        APP_ERR_TRAP(bt_status);
    }

    g_mqtt_status.status = pdTRUE;

    APP_PRINT("Successful MQTT Connection to the end point %s\r\n", USER_MQTT_ENDPOINT);
    APP_PRINT("Device is Ready for Publishing and Subscription of Messages \r\n\r\n");

    xTaskNotifyFromISR(sensor_thread, 1, 1, NULL);
    while (1)
    {
        bt_status = xQueueReceive (g_topic_queue, &mq_data, QUEUE_WAIT);
        if( pdTRUE == bt_status )
        {
            switch (mq_data.id)
            {
                case ID_IAQ_DATA_PULL:
                case ID_IAQ_DATA_PUSH:
                {
                    APP_PRINT("Topic Received from Cloud for IAQ Sensor Data \r\n");
                    AWS_ACTIVITY_INDICATION

                    print_float (temp_str[0], sizeof(temp_str[0]), app_iaq_data.gs_tvoc);
                    print_float (temp_str[1], sizeof(temp_str[1]), app_iaq_data.gs_etoh);
                    print_float (temp_str[2], sizeof(temp_str[2]), app_iaq_data.gs_eco2);

                    if(APP_DEBUG_MSG_CNTRL)
                    {
                        APP_PRINT("IAQ data tvoc  %s \r\n",temp_str[0]);
                        APP_PRINT("IAQ data etoh  %s \r\n",temp_str[1]);
                        APP_PRINT("IAQ data eco2  %s \r\n",temp_str[2]);
                    }
                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_IAQ_DATA_PULL, (char *)temp_str,32);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                             APP_ERR_PRINT("** IAQ Sensor Message Publish failed **\r\n");
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

                case ID_OAQ_DATA_PULL:
                case ID_OAQ_DATA_PUSH:
                {
                     APP_PRINT("Topic Received from Cloud for OAQ Sensor Data \r\n");
                     print_float (temp_str[0], sizeof(temp_str[0]), app_oaq_data.gs_air_quality);

                     if(APP_DEBUG_MSG_CNTRL)
                     {
                         APP_PRINT("OAQ data  %s \r\n",temp_str[0]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_OAQ_DATA_PULL, (char *)temp_str,32);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** IAQ Sensor Message Publish failed **\r\n");
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

                case ID_HS_DATA_PULL:
                case ID_HS_DATA_PUSH:
                {
                     APP_PRINT("Topic Received from Cloud for HS3001 Sensor Data \r\n");
                     print_float (temp_str[0], sizeof(temp_str[0]), app_hs3001_data.gs_humidity);
                     print_float (temp_str[1], sizeof(temp_str[1]), app_hs3001_data.gs_temperature);

                     if(APP_DEBUG_MSG_CNTRL){
                         APP_PRINT("HS3001 data  %s \r\n",temp_str[0]);
                         APP_PRINT("HS3001 data  %s \r\n",temp_str[1]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_HS_DATA_PULL, (char *)temp_str,32);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** HS3001 Sensor Message Publish failed **\r\n");
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

                case ID_ICM_DATA_PULL:
                case ID_ICM_DATA_PUSH:
                {
                     APP_PRINT("Topic Received from Cloud for ICM Sensor Data \r\n");
                     print_float (temp_str[0], sizeof(temp_str[0]), app_icm_data.acc_data.x);
                     print_float (temp_str[1], sizeof(temp_str[1]), app_icm_data.acc_data.y);
                     print_float (temp_str[2], sizeof(temp_str[2]), app_icm_data.acc_data.z);

                     print_float (temp_str[3], sizeof(temp_str[3]), app_icm_data.mag_data.x);
                     print_float (temp_str[4], sizeof(temp_str[4]), app_icm_data.mag_data.y);
                     print_float (temp_str[5], sizeof(temp_str[5]), app_icm_data.mag_data.z);

                     print_float (temp_str[6], sizeof(temp_str[6]), app_icm_data.gyr_data.x);
                     print_float (temp_str[7], sizeof(temp_str[7]), app_icm_data.gyr_data.y);
                     print_float (temp_str[8], sizeof(temp_str[8]), app_icm_data.gyr_data.z);

                     if(APP_DEBUG_MSG_CNTRL)
                     {
                         APP_PRINT("ICM data acce %s \r\n",temp_str[0]);
                         APP_PRINT("ICM data acce %s \r\n",temp_str[1]);
                         APP_PRINT("ICM data acce %s \r\n",temp_str[2]);

                         APP_PRINT("ICM data mag %s \r\n",temp_str[3]);
                         APP_PRINT("ICM data mag %s \r\n",temp_str[4]);
                         APP_PRINT("ICM data mag %s \r\n",temp_str[5]);

                         APP_PRINT("ICM data gyr %s \r\n",temp_str[6]);
                         APP_PRINT("ICM data gyr %s \r\n",temp_str[7]);
                         APP_PRINT("ICM data gyr %s \r\n",temp_str[8]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_ICM_DATA_PULL, (char *)temp_str,32);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
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

                case ID_ICP_DATA_PULL:
                case ID_ICP_DATA_PUSH:
                {
                     APP_PRINT("Topic Received from Cloud for ICP Sensor Data \r\n");
                     print_float (temp_str[0], sizeof(temp_str[0]), app_icp_data.temperature_C);
                     print_float (temp_str[1], sizeof(temp_str[1]), app_icp_data.pressure_Pa);

                     if(APP_DEBUG_MSG_CNTRL)
                     {
                         APP_PRINT("ICP data  %s \r\n",temp_str[0]);
                         APP_PRINT("ICP data  %s \r\n",temp_str[1]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_ICP_DATA_PULL, (char *)temp_str,32);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
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

                case ID_OB1203_DATA_PULL:
                case ID_OB1203_DATA_PUSH:
                {

                     APP_PRINT("Topic Received from Cloud for OB1203 Sensor Data \r\n");
                     print_float (temp_str[0], sizeof(temp_str[0]), app_ob1203_data.spo2_val);
                     print_float (temp_str[1], sizeof(temp_str[1]), app_ob1203_data.heart_rate_Val);
                     print_float (temp_str[2], sizeof(temp_str[2]), app_ob1203_data.breathing_rate);
                     print_float (temp_str[3], sizeof(temp_str[3]), app_ob1203_data.r_p2p);

                     if(APP_DEBUG_MSG_CNTRL)
                     {
                         APP_PRINT("OB1203 data  %s \r\n",temp_str[0]);
                         APP_PRINT("OB1203 data  %s \r\n",temp_str[1]);
                         APP_PRINT("OB1203 data  %s \r\n",temp_str[2]);
                         APP_PRINT("OB1203 data  %s \r\n",temp_str[3]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_OB1203_DATA_PULL, (char *)temp_str,32);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
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
                case ID_BULK_SENS_DATA_PULL:
                case ID_BULK_SENS_DATA_PUSH:
                {
                     APP_PRINT("Topic Received from Cloud for Bulk Sensor Data \r\n");
                     print_float (temp_str[0], sizeof(temp_str[0]), app_iaq_data.gs_etoh);
                     print_float (temp_str[1], sizeof(temp_str[1]), app_iaq_data.gs_eco2);
                     print_float (temp_str[2], sizeof(temp_str[2]), app_iaq_data.gs_tvoc);

                     print_float (temp_str[3], sizeof(temp_str[3]), app_oaq_data.gs_air_quality);

                     print_float (temp_str[4], sizeof(temp_str[4]), app_hs3001_data.gs_humidity);
                     print_float (temp_str[5], sizeof(temp_str[5]), app_hs3001_data.gs_temperature);

                     print_float (temp_str[6], sizeof(temp_str[6]), app_icm_data.acc_data.x);
                     print_float (temp_str[7], sizeof(temp_str[7]), app_icm_data.acc_data.y);
                     print_float (temp_str[8], sizeof(temp_str[8]), app_icm_data.acc_data.z);

                     print_float (temp_str[9], sizeof(temp_str[9]), app_icm_data.mag_data.x);
                     print_float (temp_str[10], sizeof(temp_str[10]), app_icm_data.mag_data.y);
                     print_float (temp_str[11], sizeof(temp_str[11]), app_icm_data.mag_data.z);

                     print_float (temp_str[12], sizeof(temp_str[12]), app_icm_data.gyr_data.x);
                     print_float (temp_str[13], sizeof(temp_str[13]), app_icm_data.gyr_data.y);
                     print_float (temp_str[14], sizeof(temp_str[14]), app_icm_data.gyr_data.z);

                     print_float (temp_str[15], sizeof(temp_str[15]), app_icp_data.temperature_C);
                     print_float (temp_str[16], sizeof(temp_str[16]), app_icp_data.pressure_Pa);

                     print_float (temp_str[17], sizeof(temp_str[17]), app_ob1203_data.spo2_val);
                     print_float (temp_str[18], sizeof(temp_str[18]), app_ob1203_data.heart_rate_Val);
                     print_float (temp_str[19], sizeof(temp_str[19]), app_ob1203_data.breathing_rate);
                     print_float (temp_str[20], sizeof(temp_str[20]), app_ob1203_data.r_p2p);

                     if(APP_DEBUG_MSG_CNTRL)
                     {
                         APP_PRINT("IAQ etoh  %s \r\n",temp_str[0]);
                         APP_PRINT("IAQ eco2  %s \r\n",temp_str[1]);
                         APP_PRINT("IAQ tvoc  %s \r\n",temp_str[2]);

                         APP_PRINT("OAQ air_quality  %s \r\n",temp_str[3]);

                         APP_PRINT("HS3001 Humidity  %s \r\n",temp_str[4]);
                         APP_PRINT("HS3001 temperature  %s \r\n",temp_str[5]);

                         APP_PRINT("ICM data  %s \r\n",temp_str[6]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[7]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[8]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[9]);

                         APP_PRINT("ICM data  %s \r\n",temp_str[10]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[11]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[12]);

                         APP_PRINT("ICM data  %s \r\n",temp_str[13]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[14]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[15]);

                         APP_PRINT("ICM data  %s \r\n",temp_str[16]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[17]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[18]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[19]);
                         APP_PRINT("ICM data  %s \r\n",temp_str[20]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_BULK_SENS_DATA_PULL, (char *)temp_str,32);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** Bulk Sensor Message Publish failed **\r\n");
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
                case ID_TEMPERATURE_DATA_PUSH:
                {
                    APP_PRINT("Topic Received from Cloud %s \r\n",mq_data.value.led_data.led_act_topic_msg);
                    if (FSP_SUCCESS != parse_temperature_led_actuation_message (&mq_data))
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
                case ID_SPO2_DATA_PUSH:
                {
                    APP_PRINT("Topic Received from Cloud %s \r\n",mq_data.value.led_data.led_act_topic_msg);
                    if (FSP_SUCCESS != parse_spo2_led_actuation_message (&mq_data))
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
            if(APP_DEBUG_MSG_SEQ_CNTRL)
            {
                APP_PRINT("Msg Sequence Number = %d \r\n",sequenceNumber++);
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
        vTaskDelay(100);
    }

}

/*********************************************************************************************************************//**
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

/*******************************************************************************************************************//**
 * @brief      Temperature data publish Callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void temperatureLedDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t led_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    led_data.id = ID_TEMPERATURE_DATA_PUSH;
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


/*******************************************************************************************************************//**
 * @brief      spo2 data publish callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void spo2LedDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t led_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    led_data.id = ID_SPO2_DATA_PUSH;
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


/*******************************************************************************************************************//**
 * @brief      IAQ data publish Callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void IAQDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_IAQ_DATA_PULL;
     strncpy ((char *) &payload_data.value.iaq_data.iaq_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      OAQ data publish Callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void OAQDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_OAQ_DATA_PULL;
    strncpy ((char *) &payload_data.value.oaq_data.oaq_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      HS3001 data publish Callback fucntion
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void HS3001DataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_HS_DATA_PULL;
    strncpy ((char *) &payload_data.value.hs3001_data.hs3001_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      ICMD data publish Callback fucntion
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void ICMDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_ICM_DATA_PULL;
    strncpy ((char *) &payload_data.value.icm_data.icm_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      ICPD data publish Callback fucntion
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void ICPDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_ICP_DATA_PULL;
    strncpy ((char *) &payload_data.value.icp_data.icp_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      OB1203 data publish Callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void OB1203DataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_OB1203_DATA_PULL;
    strncpy ((char *) &payload_data.value.ob1203_data.ob1203_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      BulkData publish Callback fucntion
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void BulkDataCallback( MQTTContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
    assert( pPublishInfo != NULL );
    assert( pContext != NULL );
    mqtt_rx_payload_t payload_data = {'\0'};
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;
    char strlog[64] = {'\0'};

    /* Suppress unused parameter warning when asserts are disabled in build. */
    ( void ) pContext;
    IotLogInfo( ( "Invoked led callback.\r\n" ) );

    payload_data.id = ID_BULK_SENS_DATA_PULL;
    strncpy ((char *) &payload_data.value.bulk_data.bulk_topic_msg, (const char *) pPublishInfo->pPayload,
            pPublishInfo->payloadLength);

    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************************
 * @brief This function creates the payload for the User Push button messages in UTF8 of JSON format and Publishes the Message
 * called by the Application Code.
 * @param[in] pTopicNames   pointer to Topic Names
 * @param[in] Topics_index  Topic Index
 * @param[in] message       Message Payload
 * @param[in] sizeIdx       Message index
 * @return    EXIT_FAILURE  if Failed to generate MQTT PUBLISH payload for PUBLISH
 * @return    EXIT_SUCCESS  if Published to Topic Successfully
 ******************************************************************************************************************************/
static BaseType_t sendMessage(const char **pTopicNames, uint8_t Topics_index, char *message,uint8_t sizeIdx)
{
    BaseType_t      xReturnStatus = pdFALSE;
    const char    * pcTopicFilter = NULL;
    uint16_t        topicFilterLength = 0;
    int             payloadlength = 0;

    pcTopicFilter = pTopicNames[Topics_index];
    topicFilterLength = (uint16_t) strlen (pTopicNames[Topics_index]);
    memset(pubPayload, 0x00, sizeof(pubPayload));
    /* Generate the payload for the PUBLISH. */
    if(Topics_index == (uint8_t)ID_IAQ_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_IAQ_JSON, &message[sizeIdx*0],&message[sizeIdx*1],&message[sizeIdx*2]);
    }
    else if(Topics_index == (uint8_t)ID_OAQ_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_OAQ_JSON, &message[sizeIdx*0]);
    }
    else if(Topics_index == (uint8_t)ID_HS_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_HS3001_JSON, &message[sizeIdx*0],&message[sizeIdx*1]);
    }
    else if(Topics_index == (uint8_t)ID_ICM_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_ICM_JSON, &message[sizeIdx*0],&message[sizeIdx*1],&message[sizeIdx*2],
                                  &message[sizeIdx*3],&message[sizeIdx*4],&message[sizeIdx*5],&message[sizeIdx*6],&message[sizeIdx*7],&message[sizeIdx*8]);
    }
    else if(Topics_index == (uint8_t)ID_ICP_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_ICP_JSON, &message[sizeIdx*0],&message[sizeIdx*1]);
    }
    else if(Topics_index == (uint8_t)ID_OB1203_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_OB1203_JSON, &message[sizeIdx*0],&message[sizeIdx*1],&message[sizeIdx*2],&message[sizeIdx*3]);
    }
    else if(Topics_index == (uint8_t)ID_BULK_SENS_DATA_PULL)
    {
        payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_BULK_JSON, &message[sizeIdx*0],&message[sizeIdx*1],&message[sizeIdx*2],&message[sizeIdx*3],
                                  &message[sizeIdx*4],&message[sizeIdx*5],&message[sizeIdx*6],&message[sizeIdx*7],&message[sizeIdx*8],&message[sizeIdx*9],&message[sizeIdx*10],&message[sizeIdx*11],&message[sizeIdx*12],&message[sizeIdx*13],
                                  &message[sizeIdx*14],&message[sizeIdx*15],&message[sizeIdx*16],&message[sizeIdx*17],&message[sizeIdx*18],&message[sizeIdx*19],&message[sizeIdx*20],&message[sizeIdx*21]);
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


/*********************************************************************************************************************
 * @brief   Cleans up the mqtt by deinitializing and reiniting for cleaner connectivity.
 *  This is called by the application when error in sending or receiving the MQTT messages are encountered at the
 *  Application level.
 *@param[in]
 *@return
 *********************************************************************************************************************/
void mqtt_cleanup_and_reinit(void)
{
#ifdef TODO
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
    IotLogInfo("Disconnecting the MQTT connection with %s.\r\n", USER_MQTT_ENDPOINT);

    if( xReturnStatus == pdFAIL )
    {
        /* Returned status is not used to update the local status as there
         * were failures in demo execution. */
        ( void ) DisconnectMqttSession( &xMqttContext, &networkContext );
    }
    else
    {
        xReturnStatus = DisconnectMqttSession( &xMqttContext, &networkContext );
    }


    xReturnStatus = EstablishMqttSession(&xMqttContext, &networkContext, &xBuffer, true);
    if( xReturnStatus == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        IotLogError(("Failed to connect to MQTT broker."));
        IotLogErrorTrap(xReturnStatus);
    }
#if 0
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
#endif
    g_mqtt_status.status = pdTRUE;
    g_mqtt_status.mqtt_connect_status = true;
#endif
}

/*********************************************************************************************************************
 * @brief   Temperature Led Actuation Message Parse function
 *
 * This function parse the incoming MQTT message and actuates the LED on the Board.
 * Note: For the RGB LED, LED_ON turns OFF the LED and Vice verse
 * @param[in] lmq_data Pointer to Message paylod
 * @return   RESET_VALUE if parsed successfully ,0xFF otherwise
 *********************************************************************************************************************/
static int parse_temperature_led_actuation_message(mqtt_rx_payload_t *lmq_data)
{
    int status = RESET_VALUE;

    const char msg_cold_led_on[] = "{\"Temperature_LED\":\"COLD\"}";
    const char msg_warm_led_on[] = "{\"Temperature_LED\":\"WARM\"}";
    const char msg_hot_led_on[] = "{\"Temperature_LED\":\"HOT\"}";
    const char msg_temperature_led_off[] = "{\"Temperature_LED\":\"OFF\"}";

    if (NULL == lmq_data)
    {
        IotLogErrorTrap("\r\n NULL pointer in parse_message \r\n");
        status = 0xFF;
    }
    else
    {
        /* BLUE LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_cold_led_on))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_OFF);
            IotLogInfo("\r\nCOLD LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nCOLD LED OFF \r\n");
        }

        /* GREEN LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_warm_led_on))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_OFF);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nWARM LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);

            IotLogInfo("\r\nWARM LED OFF \r\n");
        }

        /* RED LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_hot_led_on))
        {
            led_on_off (RGB_LED_RED, LED_OFF);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nHOT LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nHOT LED OFF \r\n");
        }
    }
    return status;
}

/*********************************************************************************************************************
 * @brief   Parse spo2 led actuation message fucntion
 *
 * This function parse the incoming MQTT message and actuates the LED on the Board.
 * @param[in] lmq_data   Pointer to Message paylod
 * @return   RESET_VALUE if parsed successfully ,0xFF otherwise
 *********************************************************************************************************************/
static int parse_spo2_led_actuation_message(mqtt_rx_payload_t *lmq_data)
{
    int status = RESET_VALUE;

    const char msg_spo2_led_on[] = "{\"Spo2_LED\":\"ON\"}";
    const char msg_spo2_led_off[] = "{\"Spo2_LED\":\"OFF\"}";

    if (NULL == lmq_data)
    {
        IotLogErrorTrap("\r\n NULL pointer in parse_message \r\n");
        status = 0xFF;
    }
    else
    {
        /* BLUE LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_spo2_led_on))
        {
            led_on_off (LED_BLUE, LED_ON);
            IotLogInfo("\r\nSPO2 LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->value.led_data.led_act_topic_msg, msg_spo2_led_off))
        {
            led_on_off (LED_BLUE, LED_OFF);
            IotLogInfo("\r\nSPO2 LED OFF \r\n");
        }
     }
    return status;
}
