/***********************************************************************************************************************
 * File Name    : app_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <console_thread.h>
#include "app_thread.h"
#include "aws_dev_mode_key_provisioning.h"
#include "common_utils.h"
#include "mqtt_demo_helpers.h"
#include "usr_hal.h"
#include "usr_config.h"
#include "usr_data.h"
#include "flash/flash_hp.h"
#include "transport_mbedtls_pkcs11.h"
#include "user_choice.h"

/*************************************************************************************
 * Macro definitions
 ************************************************************************************/
#define DISABLE_SNI                         (true)
#define MQTT_RECEIVE_TIMEOUT                (5000)
#define MQTT_SEND_TIMEOUT                   (5000)
#define MILLISECONDS_PER_SECOND             (1000U)
#define MILLISECONDS_PER_TICK               ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )
#define APP_DEBUG_MSG_SEQ_CNTRL             (1)
#define APP_DEBUG_MSG_CNTRL                 (1)

/* Declare variables for receiving data from queue */
volatile usr_iaq_data_t g_iaq_data_rcv;
volatile usr_oaq_data_t g_oaq_data_rcv;
volatile usr_hs3001_data_t g_hs3001_data_rcv;
volatile usr_icp_data_t g_icp_data_rcv;
volatile usr_ob1203_data_t g_ob1203_data_rcv;
volatile usr_icm_data_t g_icm_data_rcv;

/* How many topic filters will be used in this Application Project. */
#define MAX_NUM_PUBLISH_TOPIC               (8)

#define MAX_NUM_SUBSCRIBE_TOPIC             (9)

/* PUBLISH Topic Payload in the JSON format used in this Application Project. */
#define PUBLISH_PAYLOAD_FORMAT_IAQ_JSON     "{\r\n"\
                                            "\"IAQ\" : {\r\n"\
                                            "      \"TVOC (mg/m^3)\" :\"%f\",\r\n"\
											"      \"Etoh (ppm)\" :\"%f\",\r\n"\
											"      \"eco2 (ppm)\" :\"%f\"\r\n"\
                                            "     }\r\n"\
                                            "}\r\n"

/* Json format for the OAQ sensor data */
#define PUBLISH_PAYLOAD_FORMAT_OAQ_JSON     "{\r\n"\
                                            "\"OAQ\" : {\r\n"\
                                            "      \"air quality (Index)\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"

/* Json format for the HS3001 sensor data */
#define PUBLISH_PAYLOAD_FORMAT_HS3001_JSON  "{\r\n"\
                                            "\"HS3001\" : {\r\n"\
                                            "      \"Humidity ()\" :\"%f\",\r\n"\
                                            "      \"Temperature (F)\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"

/* Json format for the ICM sensor data */
#define PUBLISH_PAYLOAD_FORMAT_ICM_JSON     "{\r\n"\
                                            "\"ICM\" : {\r\n"\
                                            "   \"acc\" : {\r\n"\
                                            "      \"x \" :\"%f\",\r\n"\
                                            "      \"y \" :\"%f\",\r\n"\
                                            "      \"z \" :\"%f\"\r\n"\
                                            "   \r\n      },\r\n"\
                                            "   \"mag\" : {\r\n"\
                                            "      \"x \" :\"%f\",\r\n"\
                                            "      \"y \" :\"%f\",\r\n"\
                                            "      \"z \" :\"%f\"\r\n"\
                                            "   \r\n      },\r\n"\
                                            "   \"gyr\" : {\r\n"\
                                            "      \"x \" :\"%f\",\r\n"\
                                            "      \"y \" :\"%f\",\r\n"\
                                            "      \"z \" :\"%f\"\r\n"\
                                            "   \r\n      }\r\n"\
                                            "\r\n      }\r\n"\
                                            "}\r\n"

/* Json format for the ICP sensor data */
#define PUBLISH_PAYLOAD_FORMAT_ICP_JSON     "{\r\n"\
                                            "\"ICP\" : {\r\n"\
                                            "      \"Temperature (F)\" :\"%f\",\r\n"\
                                            "      \"Pressure (Pa)\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"

/* Json format for the OB1203 sensor data */
#define PUBLISH_PAYLOAD_FORMAT_OB1203_JSON  "{\r\n"\
                                            "\"OB1203\" : {\r\n"\
                                            "      \"spo2 ()\" :\"%f\",\r\n"\
                                            "      \"Heart Rate ()\" :\"%f\",\r\n"\
                                            "      \"Breath rate ()\" :\"%f\",\r\n"\
                                            "      \"P2P ()\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"


#define PUBLISH_PAYLOAD_FORMAT_BULK_JSON     "{\r\n"\
                                             "\"IAQ\" : {\r\n"\
                                             "      \"TVOC (mg/m^3)\" :\"%f\",\r\n"\
                                             "      \"Etoh (ppm)\" :\"%f\",\r\n"\
                                             "      \"eco2 (ppm)\" :\"%f\"\r\n"\
                                             "          },\r\n"\
                                             "\"OAQ\" : {\r\n"\
	                                         "      \"air quality (Index)\" :\"%f\"\r\n"\
	                                         "          },\r\n"\
	                                         "\"HS3001\" : {\r\n"\
	                                         "      \"Humidity ()\" :\"%f\",\r\n"\
	                                         "      \"Temperature (F)\" :\"%f\"\r\n"\
	                                         "             },\r\n"\
                                             "\"ICM\" : {\r\n"\
                                             "   \"acc\" : {\r\n"\
                                             "      \"x \" :\"%f\",\r\n"\
                                             "      \"y \" :\"%f\",\r\n"\
                                             "      \"z \" :\"%f\"\r\n"\
                                             "             },\r\n"\
                                             "   \"mag\" : {\r\n"\
                                             "      \"x \" :\"%f\",\r\n"\
                                             "      \"y \" :\"%f\",\r\n"\
                                             "      \"z \" :\"%f\"\r\n"\
                                             "             },\r\n"\
                                             "   \"gyr\" : {\r\n"\
                                             "      \"x \" :\"%f\",\r\n"\
                                             "      \"y \" :\"%f\",\r\n"\
                                             "      \"z \" :\"%f\"\r\n"\
                                             "             }\r\n"\
                                             "         },\r\n"\
                                             "\"ICP\" : {\r\n"\
                                             "      \"Temperature (F)\" :\"%f\",\r\n"\
                                             "      \"Pressure (Pa)\" :\"%f\"\r\n"\
                                             "            },\r\n"\
	                                         "\"OB1203\" : {\r\n"\
	                                         "      \"spo2 ()\" :\"%f\",\r\n"\
	                                         "      \"Heart Rate ()\" :\"%f\",\r\n"\
	                                         "      \"Breath rate ()\" :\"%f\",\r\n"\
	                                         "      \"P2P ()\" :\"%f\"\r\n"\
	                                         "              }\r\n"\
                                             "}\r\n"
/* The length of the payload format for bulk data */
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_BULK_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_BULK_JSON ) + 315 )

extern char* getDNS();
extern bool setupCellular( void );
extern TaskHandle_t sensors_thread;
extern char g_certificate[2048];
extern char g_private_key[2048];
extern char g_mqtt_endpoint[128];

/*************************************************************************************
 * Private functions
 ************************************************************************************/
static int config_littlFs_flash(void);
static BaseType_t sendMessage(const char **pTopicNames, uint8_t Topics_index, double *message);

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

/*************************************************************************************
 * global variables
 ************************************************************************************/
ProvisioningParams_t params;
/* The MQTT context used for MQTT operation. */
MQTTContext_t xMqttContext;
/* The network context used for TLS operation. */
NetworkContext_t xNetworkContext;

mqtt_status_t g_mqtt_status = { .mqtt_connect_status = false, .status = pdFALSE};

char cBuffer[16] = { RESET_VALUE };
double   temp_db[21] =  { RESET_VALUE };

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

extern uint32_t  g_console_status;

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

/*************************************************************************************
 * Private variables
 ************************************************************************************/
char CLIENT_CERTIFICATE_PEM[2048];
char CLIENT_KEY_PEM[2048];
static char USER_MQTT_ENDPOINT[128];
/* Static buffer used to hold MQTT messages being sent and received. */
static uint8_t ucSharedBuffer[MQTT_EXAMPLE_NETWORK_BUFFER_SIZE];
/* Static buffer used to hold MQTT messages being sent and received. */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = MQTT_EXAMPLE_NETWORK_BUFFER_SIZE
};

static mqtt_rx_payload_t mq_data = { RESET_VALUE };
static uint32_t mqtt_fail_count   = RESET_VALUE;
static char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_BULK_JSON] = { RESET_VALUE };

/* The network context used for TLS operation. */
NetworkContext_t networkContext;

char * pHostResolvedAddress;
TlsTransportStatus_t    tls_status;

/*******************************************************************************************************************//**
 * @brief      Application Thread entry function
 * @param[in]   pvParameters     contains TaskHandle_t
 * @retval
 * @retval
 ***********************************************************************************************************************/
void app_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_pack_version_t      version = {RESET_VALUE};
    fsp_err_t               err = FSP_SUCCESS;
    BaseType_t              bt_status = pdFALSE;
    int                     lfs_err = LFS_ERR_OK;
    int                     ierr = FSP_SUCCESS;
    const char              * pcTopicFilter = NULL;
    uint16_t                topicFilterLength = RESET_VALUE;
    NetworkCredentials_t    credentials;
    ProvisioningParams_t    params = {RESET_VALUE};
    TlsTransportParams_t    transport_params;
    bool retCellular = true;

    static uint32_t   sequenceNumber = RESET_VALUE;

    bt_status = xTaskNotifyWait(pdFALSE, pdFALSE, &g_console_status, portMAX_DELAY);
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Copy AWS client certificate, private key and MQTT end point */
    memcpy (CLIENT_CERTIFICATE_PEM, g_certificate, strlen(g_certificate));
    memcpy (CLIENT_KEY_PEM, g_private_key, strlen(g_private_key));
    memcpy (USER_MQTT_ENDPOINT, g_mqtt_endpoint, strlen(g_mqtt_endpoint));

    params.pucClientPrivateKey       = (uint8_t *) CLIENT_KEY_PEM;
    params.pucClientCertificate      = (uint8_t *) CLIENT_CERTIFICATE_PEM;
    params.ulClientPrivateKeyLength  = 1 + strlen((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen((const char *) params.pucClientCertificate);
    params.pucJITPCertificate        = NULL;
    params.ulJITPCertificateLength   = 0;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT (BANNER_INFO, AP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);


    /* Initialize network context */
    networkContext.pParams = &transport_params;

    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_01, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_01, BSP_IO_LEVEL_HIGH);
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
    lfs_err = config_littlFs_flash();
    if (LFS_ERR_OK != lfs_err)
    {
        FAILURE_INDICATION;
        APP_ERR_PRINT("** littleFs flash config failed **\r\n");
        APP_ERR_TRAP(lfs_err);
    }
#if 1

    /* Initialize the crypto hardware acceleration. */
     ierr = mbedtls_platform_setup(NULL);
    if (FSP_SUCCESS != ierr)
    {
        APP_ERR_PRINT("** HW SCE Init failed **\r\n");
        APP_ERR_TRAP(ierr);
    }


    /* A simple example to demonstrate key and certificate provisioning in code flash using PKCS#11 interface.
     * This should be replaced by production ready key provisioning mechanism. */

    err = vAlternateKeyProvisioning (&params);
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION;
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
        APP_ERR_TRAP(tls_status);
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
#endif
    xTaskNotifyFromISR(sensors_thread, 1, 1, NULL);

    while (1)
    {
        bt_status = xQueueReceive (g_topic_queue, &mq_data, QUEUE_WAIT);
        if( pdTRUE == bt_status )
        {
            if(APP_DEBUG_MSG_SEQ_CNTRL)
            {
                APP_PRINT("\r\nMsg Sequence Number = %d \r\n",sequenceNumber++);
            }
			
            switch (mq_data.id)
            {
				
                case ID_IAQ_DATA_PULL:
                case ID_IAQ_DATA_PUSH:
                {
                    /* receive data from queue of IAQ sensor*/
                    bt_status = xQueuePeek (g_iaq_queue, &g_iaq_data_rcv, 0);
                    if( pdTRUE == bt_status )
                    {
                        APP_PRINT("Msg received from IAQ sensor \r\n");
                        /* store value to temp_db array */
                        temp_db[0] = g_iaq_data_rcv.gs_tvoc;
                        temp_db[1] = g_iaq_data_rcv.gs_etoh;
                        temp_db[2] = g_iaq_data_rcv.gs_eco2;
                    }
                    else
                    {
                        memset(temp_db, 0, sizeof(temp_db[0])*3);
                        #if _ZMOD4410_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of IAQ sensor \r\n");
                        #endif
                    }
                    AWS_ACTIVITY_INDICATION;

                    if(APP_DEBUG_MSG_CNTRL)
                    {
                        APP_PRINT("IAQ data tvoc  %f \r\n",temp_db[0]);
                        APP_PRINT("IAQ data etoh  %f \r\n",temp_db[1]);
                        APP_PRINT("IAQ data eco2  %f \r\n",temp_db[2]);
                    }
                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_IAQ_DATA_PULL, temp_db);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                                APP_ERR_PRINT("** IAQ Sensor Message Publish failed **\r\n");
                                mqtt_fail_count++;
                                mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
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
                    /* receive data from queue of OAQ sensor */
                    bt_status = xQueuePeek (g_oaq_queue, &g_oaq_data_rcv, 0);
                    if( pdTRUE == bt_status )
                    {
                        APP_PRINT("Msg received from OAQ sensor\r\n");
                        /* store value to temp_db array */
                        temp_db[0] = g_oaq_data_rcv.gs_air_quality;
                    }
                    else
                    {
                        memset(temp_db, 0, sizeof(temp_db[0]));
                        #if  _ZMOD4510_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of OAQ sensor \r\n");
                        #endif
                    }
                    if(APP_DEBUG_MSG_CNTRL)
                    {
                        APP_PRINT("OAQ data  %f \r\n",temp_db[0]);
                    }

                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_OAQ_DATA_PULL, temp_db);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** OAQ Sensor Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
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
                    /* receive data from queue of HS3001 sensor*/
                    bt_status = xQueuePeek (g_hs3001_queue, &g_hs3001_data_rcv, 0);
                    if( pdTRUE == bt_status )
                    {
                        APP_PRINT("Msg data received from HS3001 sensor \r\n");
                        /* store value to temp_db array */
                        temp_db[0] = g_hs3001_data_rcv.gs_humidity;
                        temp_db[1] = (g_hs3001_data_rcv.gs_temperature * (float) 1.8) + (float) 32.0;
                    }
                    else
                    {
                        memset(temp_db, 0, sizeof(temp_db[0])*2);
                        #if _HS3001_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of HS3001 sensor \r\n");
                        #endif
                    }

                    if(APP_DEBUG_MSG_CNTRL){
                        APP_PRINT("HS3001 data - humidity: %f \r\n",temp_db[0]);
                        APP_PRINT("HS3001 data - temperature: %f \r\n",temp_db[1]);
                    }
                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_HS_DATA_PULL, temp_db);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** HS3001 Sensor Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
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
                    /* receive data from queue of ICM sensor*/
                    bt_status = xQueuePeek (g_icm_queue, &g_icm_data_rcv, 0);
                    if( pdTRUE == bt_status )
                    {
                        APP_PRINT("Msg data received from ICM sensor \r\n");
                         /* store value to temp_db array */
                        temp_db[0] = g_icm_data_rcv.acc_data.x;
                        temp_db[1] = g_icm_data_rcv.acc_data.y;
                        temp_db[2] = g_icm_data_rcv.acc_data.z;
                        temp_db[3] = g_icm_data_rcv.mag_data.x;
                        temp_db[4] = g_icm_data_rcv.mag_data.y;
                        temp_db[5] = g_icm_data_rcv.mag_data.z;
                        temp_db[6] = g_icm_data_rcv.gyr_data.x;
                        temp_db[7] = g_icm_data_rcv.gyr_data.y;
                        temp_db[8] = g_icm_data_rcv.gyr_data.z;
                    }
                    else
                    {
                        memset(temp_db, 0, sizeof(temp_db[0])*9);
                        #if _ICM42605_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of ICM sensor \r\n");
                        #endif
                    }
                    if(APP_DEBUG_MSG_CNTRL)
                    {
                        APP_PRINT("ICM data acce x %f \r\n",temp_db[0]);
                        APP_PRINT("ICM data acce y %f \r\n",temp_db[1]);
                        APP_PRINT("ICM data acce z %f \r\n",temp_db[2]);

                        APP_PRINT("ICM data mag x %f \r\n",temp_db[3]);
                        APP_PRINT("ICM data mag y %f \r\n",temp_db[4]);
                        APP_PRINT("ICM data mag z %f \r\n",temp_db[5]);

                        APP_PRINT("ICM data gyr x %f \r\n",temp_db[6]);
                        APP_PRINT("ICM data gyr y %f \r\n",temp_db[7]);
                        APP_PRINT("ICM data gyr z %f \r\n",temp_db[8]);
                    }

                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_ICM_DATA_PULL,temp_db);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
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
                    /* receive data from queue of ICP sensor*/
                    bt_status = xQueuePeek (g_icp_queue, &g_icp_data_rcv, 0);
                    if( pdTRUE == bt_status )
                    {
                        APP_PRINT("Msg data received from ICP sensor \r\n");
                        /* store value to temp_db array */
                        temp_db[0] = (g_icp_data_rcv.temperature_C * (float) 1.8) + (float) 32.0;
                        temp_db[1] = g_icp_data_rcv.pressure_Pa;

                    }
                    else
                    {
                        memset(temp_db, 0, sizeof(temp_db[0])*2);
                        #if _ICP20100_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of ICP sensor \r\n");
                        #endif
                    }
                    if(APP_DEBUG_MSG_CNTRL)
                    {
                        APP_PRINT("ICP data - temperature:  %f \r\n",temp_db[0]);
                        APP_PRINT("ICP data - pressure:  %f \r\n",temp_db[1]);
                    }

                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_ICP_DATA_PULL, temp_db);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
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
                    /* receive data from queue of OB1203 sensor*/
                    bt_status = xQueuePeek (g_ob1203_queue, &g_ob1203_data_rcv, 0);
                    if( pdTRUE == bt_status )
                    {
                        APP_PRINT("Msg received from OB1203 sensor \r\n");
                        /* store value to temp_db array */
                        temp_db[0] = g_ob1203_data_rcv.spo2_val;
                        temp_db[1] = g_ob1203_data_rcv.heart_rate_Val;
                        temp_db[2] = g_ob1203_data_rcv.breathing_rate;
                        temp_db[3] = g_ob1203_data_rcv.r_p2p;
                    }
                    else
                    {
                        memset(temp_db, 0, sizeof(temp_db[0])*4);
                        #if _OB1203_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of OB1203 sensor \r\n");
                        #endif
                    }

                    if(APP_DEBUG_MSG_CNTRL)
                    {
                        APP_PRINT("OB1203 data - spo2:  %f \r\n",temp_db[0]);
                        APP_PRINT("OB1203 data - Heart Rate:  %f \r\n",temp_db[1]);
                        APP_PRINT("OB1203 data - Breath rate:  %f \r\n",temp_db[2]);
                        APP_PRINT("OB1203 data - P2P:  %f \r\n",temp_db[3]);
                    }

                    if(pdTRUE == g_mqtt_status.status)
                    {
                        g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_OB1203_DATA_PULL, temp_db);
                        if ((pdTRUE != g_mqtt_status.status))
                        {
                            APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
                            mqtt_fail_count++;
                            mqtt_cleanup_and_reinit ();
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
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
                    /* get value of IAQ sensor */
                    bt_status = xQueuePeek (g_iaq_queue, &app_iaq_data, 0);
                    if( pdTRUE == bt_status )
                    {
                        temp_db[0] = app_iaq_data.gs_tvoc;
                        temp_db[1] = app_iaq_data.gs_etoh;
                        temp_db[2] = app_iaq_data.gs_eco2;
                    }
                    else
                    {
                        temp_db[0] = 0;
                        temp_db[1] = 0;
                        temp_db[2] = 0;
                        #if _ZMOD4410_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of IAQ sensor \r\n");
                        #endif
                    }

                    /* get value of OAQ sensor */
                    bt_status = xQueuePeek (g_oaq_queue, &app_oaq_data, 0);
                    if( pdTRUE == bt_status )
                    {
                        temp_db[3] = app_oaq_data.gs_air_quality;
                    }
                    else
                    {
                        temp_db[3] = 0;
                        #if  _ZMOD4510_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of OAQ sensor \r\n");
                        #endif
                    }

                    /* get value of HS3001 sensor */
                    bt_status = xQueuePeek (g_hs3001_queue, &app_hs3001_data, 0);
                    if( pdTRUE == bt_status )
                    {
                        temp_db[4] = app_hs3001_data.gs_humidity;
                        temp_db[5] = (app_hs3001_data.gs_temperature * (float) 1.8) + (float) 32.0;
                    }
                    else
                    {
                        temp_db[4] = 0;
                        temp_db[5] = 0;
                        #if _HS3001_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of HS3001 sensor \r\n");
                        #endif
                    }

                    /* get value of ICM sensor */
                    bt_status = xQueuePeek (g_icm_queue, &app_icm_data, 0);
                    if( pdTRUE == bt_status )
                    {
                        temp_db[6] = app_icm_data.acc_data.x;
                        temp_db[7] = app_icm_data.acc_data.y;
                        temp_db[8] = app_icm_data.acc_data.z;
                        temp_db[9] = app_icm_data.mag_data.x;
                        temp_db[10] = app_icm_data.mag_data.y;
                        temp_db[11] = app_icm_data.mag_data.z;
                        temp_db[12] = app_icm_data.gyr_data.x;
                        temp_db[13] = app_icm_data.gyr_data.y;
                        temp_db[14] = app_icm_data.gyr_data.z;
                    }
                    else
                    {
                        temp_db[6] = 0;
                        temp_db[7] = 0;
                        temp_db[8] = 0;
                        temp_db[9] = 0;
                        temp_db[10] = 0;
                        temp_db[11] = 0;
                        temp_db[12] = 0;
                        temp_db[13] = 0;
                        temp_db[14] = 0;
                        #if _ICM42605_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of ICM sensor \r\n");
                        #endif
                    }

                    /* get value of ICP sensor */
                    bt_status = xQueuePeek (g_icp_queue, &app_icp_data, 0);
                    if( pdTRUE == bt_status )
                    {
                        temp_db[15] = (app_icp_data.temperature_C * (float) 1.8) + (float) 32.0;
                        temp_db[16] = app_icp_data.pressure_Pa;
                    }
                    else
                    {
                        temp_db[15] = 0;
                        temp_db[16] = 0;
                        #if _ICP20100_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of ICP sensor \r\n");
                        #endif
                    }

                    /* get value of OB1203 sensor */
                    bt_status = xQueuePeek (g_ob1203_queue, &app_ob1203_data, 0);
                    if( pdTRUE == bt_status )
                    {
                        temp_db[17] = app_ob1203_data.spo2_val;
                        temp_db[18] = app_ob1203_data.heart_rate_Val;
                        temp_db[19] = app_ob1203_data.breathing_rate;
                        temp_db[20] = app_ob1203_data.r_p2p;
                    }
                    else
                    {
                        temp_db[17] = 0;
                        temp_db[18] = 0;
                        temp_db[19] = 0;
                        temp_db[20] = 0;
                        #if _OB1203_SENSOR_ENABLE_
                            APP_PRINT("Can not get value of OB1203 sensor \r\n");
                        #endif
                    }

                    APP_PRINT("Msg received for  Bulk Sensor Data \r\n");

                     if(APP_DEBUG_MSG_CNTRL)
                     {
                         APP_PRINT("IAQ tvoc  %f \r\n",temp_db[0]);
                         APP_PRINT("IAQ etoh %f \r\n",temp_db[1]);
                         APP_PRINT("IAQ eco2 %f \r\n",temp_db[2]);

                         APP_PRINT("OAQ air_quality  %f \r\n",temp_db[3]);

                         APP_PRINT("HS3001 Humidity  %f \r\n",temp_db[4]);
                         APP_PRINT("HS3001 temperature  %f \r\n",temp_db[5]);

                         APP_PRINT("ICM data acce x  %f \r\n",temp_db[6]);
                         APP_PRINT("ICM data acce y  %f \r\n",temp_db[7]);
                         APP_PRINT("ICM data acce z  %f \r\n",temp_db[8]);
                         APP_PRINT("ICM data mag x  %f \r\n",temp_db[9]);
                         APP_PRINT("ICM data mag y  %f \r\n",temp_db[10]);
                         APP_PRINT("ICM data mag z  %f \r\n",temp_db[11]);
                         APP_PRINT("ICM data gyr x  %f \r\n",temp_db[12]);
                         APP_PRINT("ICM data gyr y  %f \r\n",temp_db[13]);
                         APP_PRINT("ICM data gyr z  %f \r\n",temp_db[14]);

                         APP_PRINT("ICP data - temperature %f \r\n",temp_db[15]);
                         APP_PRINT("ICP data - pressure %f \r\n",temp_db[16]);

                         APP_PRINT("OB1203 data - spo2 %f \r\n",temp_db[17]);
                         APP_PRINT("OB1203 data - Heart rate %f \r\n",temp_db[18]);
                         APP_PRINT("OB1203 data - Breath rate %f \r\n",temp_db[19]);
                         APP_PRINT("OB1203 data - P2P %f \r\n",temp_db[20]);
                     }

                     if(pdTRUE == g_mqtt_status.status)
                     {
                         g_mqtt_status.status = sendMessage (pTopics_publish, (uint8_t)ID_BULK_SENS_DATA_PULL, temp_db);
                         if ((pdTRUE != g_mqtt_status.status))
                         {
                              APP_ERR_PRINT("** Bulk Sensor Message Publish failed **\r\n");
                              mqtt_fail_count++;
                              mqtt_cleanup_and_reinit ();
                         }
                         else
                         {
                             AWS_ACTIVITY_INDICATION;
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
                    APP_PRINT("Topic Received from Cloud %s \r\n",mq_data.led_data.led_act_topic_msg);
                    if (FSP_SUCCESS != parse_temperature_led_actuation_message (&mq_data))
                    {
                        APP_ERR_PRINT("** LED Topic Actuation returned  Error **\r\n");
                        mqtt_fail_count++;
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
                    }
                }
                break;
                case ID_SPO2_DATA_PUSH:
                {
                    APP_PRINT("Topic Received from Cloud %s \r\n",mq_data.led_data.led_act_topic_msg);
                    if (FSP_SUCCESS != parse_spo2_led_actuation_message (&mq_data))
                    {
                        APP_ERR_PRINT("** LED Topic Actuation returned  Error **\r\n");
                        mqtt_fail_count++;
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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
                bt_status = ProcessLoop( &xMqttContext );
                if( bt_status != pdPASS )
                {
                    APP_ERR_PRINT("MQTT normal process fail\r\n");
                }
            }
        }
    }
}
/*********************************************************************************************************************//**
 * @brief   configures the littleFS Flash.
 *
 * This function sets up the littleFS Flash for Data storage.
 * @param[in]   None
 * @retval      LFS_ERR_OK              If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 *********************************************************************************************************************/
int config_littlFs_flash(void)
{
    int lfs_err = LFS_ERR_OK;
    fsp_err_t err = FSP_SUCCESS;

    err = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if(FSP_SUCCESS != err)
    {
        FAILURE_INDICATION;
        APP_ERR_PRINT("** littleFs Initialization failed **\r\n");
    }

    /* mount the filesystem */
    lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if(LFS_ERR_OK != lfs_err)
    {
        /* reformat if we can't mount the filesystem
         * this should only happen on the first boot
         */
        lfs_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
        if(LFS_ERR_OK != lfs_err)
        {
            FAILURE_INDICATION;
            APP_ERR_PRINT("** littleFs Flash Format failed **\r\n");
        }
        lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
        if(LFS_ERR_OK != lfs_err)
        {
            FAILURE_INDICATION;
            APP_ERR_PRINT("** littleFs Mount failed **\r\n");
        }
    }

    return lfs_err;
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
    strncpy ((char *) &led_data.led_data.led_act_topic_msg, (const char *) pPublishInfo->pPayload,
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
    strncpy ((char *) &led_data.led_data.led_act_topic_msg, (const char *) pPublishInfo->pPayload,
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
    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      HS3001 data publish Callback function
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
    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      ICMD data publish Callback function
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
    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      ICPD data publish Callback function
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
    xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);

    /* Print information about the incoming PUBLISH message. */
    IotLogInfo("Incoming QOS : %d.\r\n", pPublishInfo->qos);
    IotLogInfo("Retain flag : %d.\r\n", pPublishInfo->retain);

    strncpy(strlog, pPublishInfo->pPayload, pPublishInfo->payloadLength);
    IotLogInfo("Incoming Publish Topic matches subscribed topic.\r\n"
            "Incoming Publish Message : %s.\r\n", strlog);
}


/*******************************************************************************************************************//**
 * @brief      BulkData publish Callback function
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
static BaseType_t sendMessage(const char **pTopicNames, uint8_t Topics_index, double *message)
{
    BaseType_t      xReturnStatus = pdFALSE;
    const char    * pcTopicFilter = NULL;
    uint16_t        topicFilterLength = 0;
    int             payloadlength = 0;

    pcTopicFilter = pTopicNames[Topics_index];
    topicFilterLength = (uint16_t) strlen (pTopicNames[Topics_index]);
    memset(pubPayload, 0x00, sizeof(pubPayload));
    /* Generate the payload for the PUBLISH. */
    switch (Topics_index)
        {
            case ID_IAQ_DATA_PULL:
                payloadlength =
                        snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_IAQ_JSON,
                                message[0], message[1], message[2]);
                break;
            case ID_OAQ_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_OAQ_JSON, message[0]);
                break;
            case ID_HS_DATA_PULL:
                payloadlength =
                        snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_HS3001_JSON, message[0],message[1]);
                break;
            case ID_ICM_DATA_PULL:
                payloadlength =
                        snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_ICM_JSON,
                                message[0], message[1], message[2], message[3], message[4], message[5], message[6], message[7],
                                message[8]);
                break;
            case ID_ICP_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_ICP_JSON, message[0], message[1]);
                break;
            case ID_OB1203_DATA_PULL:
                payloadlength =
                        snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_OB1203_JSON, message[0], message[1],
                                message[2], message[3]);
                break;
            case ID_BULK_SENS_DATA_PULL:
                payloadlength =
                        snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_BULK_JSON, message[0], message[1],
                                message[2], message[3], message[4], message[5], message[6], message[7], message[8], message[9],
                                message[10], message[11], message[12], message[13], message[14], message[15], message[16],
                                message[17], message[18], message[19], message[20]);
                break;
            default:
                break;
        }

    /* Check for errors from snprintf. */
    if (payloadlength <= 0)
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
   //     ( void ) DisconnectMqttSession( &xMqttContext, &networkContext );
    }
    else
    {
  //      xReturnStatus = DisconnectMqttSession( &xMqttContext, &networkContext );
    }


//    xReturnStatus = EstablishMqttSession(&xMqttContext, &networkContext, &xBuffer, true);
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

}

/*********************************************************************************************************************
 * @brief   Temperature Led Actuation Message Parse function
 *
 * This function parse the incoming MQTT message and actuates the LED on the Board.
 * Note: For the RGB LED, LED_ON turns OFF the LED and Vice verse
 * @param[in] lmq_data Pointer to Message payload
 * @return   RESET_VALUE if parsed successfully ,0xFF otherwise
 *********************************************************************************************************************/
static int parse_temperature_led_actuation_message(mqtt_rx_payload_t *lmq_data)
{
    int status = RESET_VALUE;

    const char msg_cold_led_on[] = "{\"Temperature_LED\": \"COLD\"}";
    const char msg_warm_led_on[] = "{\"Temperature_LED\": \"WARM\"}";
    const char msg_hot_led_on[] = "{\"Temperature_LED\": \"HOT\"}";
    const char msg_temperature_led_off[] = "{\"Temperature_LED\": \"OFF\"}";

    if (NULL == lmq_data)
    {
        IotLogErrorTrap("\r\n NULL pointer in parse_message \r\n");
        status = 0xFF;
    }
    else
    {
        /* BLUE LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_cold_led_on))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_OFF);
            APP_PRINT("\r\nCOLD LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            APP_PRINT("\r\nCOLD LED OFF \r\n");
        }

        /* GREEN LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_warm_led_on))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_OFF);
            led_on_off (RGB_LED_BLUE, LED_ON);
            APP_PRINT("\r\nWARM LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);

            APP_PRINT("\r\nWARM LED OFF \r\n");
        }

        /* RED LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_hot_led_on))
        {
            led_on_off (RGB_LED_RED, LED_OFF);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            APP_PRINT("\r\nHOT LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            APP_PRINT("\r\nHOT LED OFF \r\n");
        }
    }
    return status;
}

/*********************************************************************************************************************
 * @brief   Parse spo2 led actuation message function
 *
 * This function parse the incoming MQTT message and actuates the LED on the Board.
 * @param[in] lmq_data   Pointer to Message payload
 * @return   RESET_VALUE if parsed successfully ,0xFF otherwise
 *********************************************************************************************************************/
static int parse_spo2_led_actuation_message(mqtt_rx_payload_t *lmq_data)
{
    int status = RESET_VALUE;

    const char msg_spo2_led_on[] = "{\"Spo_LED\": \"ON\"}";
    const char msg_spo2_led_off[] = "{\"Spo_LED\": \"OFF\"}";

    if (NULL == lmq_data)
    {
        IotLogErrorTrap("\r\n NULL pointer in parse_message \r\n");
        status = 0xFF;
    }
    else
    {
        /* BLUE LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_spo2_led_on))
        {
            led_on_off (LED_BLUE, LED_ON);
            APP_PRINT("\r\nSPO2 LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_spo2_led_off))
        {
            led_on_off (LED_BLUE, LED_OFF);
            APP_PRINT("\r\nSPO2 LED OFF \r\n");
        }
     }
    return status;
}
