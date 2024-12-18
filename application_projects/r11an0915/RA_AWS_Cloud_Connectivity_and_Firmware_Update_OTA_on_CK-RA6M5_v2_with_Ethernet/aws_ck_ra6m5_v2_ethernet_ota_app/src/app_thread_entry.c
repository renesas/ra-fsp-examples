/***********************************************************************************************************************
 * File Name    : app_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2024] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
#include "demo_config.h"
#include "mqtt_agent_task.h"
#include "core_mqtt_agent.h"
#include "usr_network.h"
#include "usr_hal.h"
#include "usr_config.h"
#include "usr_data.h"
#include "flash/flash_hp.h"
#include "transport_mbedtls_pkcs11.h"
#include "user_choice.h"

#include "ota.h"
/*************************************************************************************
 * Macro definitions
 ************************************************************************************/
#define MILLISECONDS_PER_SECOND             (1000U)
#define MILLISECONDS_PER_TICK               ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )
#define APP_DEBUG_MSG_SEQ_CNTRL             (1)
#define APP_DEBUG_MSG_CNTRL                 (1)

/* Declare variables for receiving data from queue */
usr_iaq_data_t g_iaq_data_rcv;
usr_oaq_data_t g_oaq_data_rcv;
usr_hs3001_data_t g_hs3001_data_rcv;
usr_icp_data_t g_icp_data_rcv;
usr_ob1203_data_t g_ob1203_data_rcv;
usr_icm_data_t g_icm_data_rcv;

/* How many topic filters will be used in this Application Project. */
#define MAX_NUM_PUBLISH_TOPIC               (8)

#define MAX_NUM_SUBSCRIBE_TOPIC             (10)

#define APP_MAIN_MQTT_AGENT_TASK_STACK_SIZE         ( 6144 )
#define APP_MAIN_MQTT_AGENT_TASK_PRIORITY           ( tskIDLE_PRIORITY + 4 )

#define MQTT_EXAMPLE_MAX_COMMAND_SEND_BLOCK_TIME_MS         ( 1000 )
#define MQTT_EXAMPLE_NUM_PUBLISH_RETRIES                    ( 3 )

/* PUBLISH Topic Payload in the JSON format used in this Application Project. */
#define PUBLISH_PAYLOAD_FORMAT_IAQ_JSON     "{\r\n"\
                                            "\"IAQ\" : {\r\n"\
                                            "      \"TVOC (mg/m^3)\" :\"%f\",\r\n"\
											"      \"Etoh (ppm)\" :\"%f\",\r\n"\
											"      \"eco2 (ppm)\" :\"%f\"\r\n"\
                                            "     }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_IAQ_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_IAQ_JSON ) + 15 )


#define PUBLISH_PAYLOAD_FORMAT_OAQ_JSON     "{\r\n"\
                                            "\"OAQ\" : {\r\n"\
                                            "      \"air quality (Index)\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_OAQ_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_OAQ_JSON ) + 5 )


#define PUBLISH_PAYLOAD_FORMAT_HS3001_JSON  "{\r\n"\
                                            "\"HS3001\" : {\r\n"\
                                            "      \"Humidity ()\" :\"%f\",\r\n"\
                                            "      \"Temperature (F)\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_HS3001_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_HS3001_JSON ) + 10 )

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
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_ICM_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_ICM_JSON ) + 45 )

#define PUBLISH_PAYLOAD_FORMAT_ICP_JSON     "{\r\n"\
                                            "\"ICP\" : {\r\n"\
                                            "      \"Temperature (F)\" :\"%f\",\r\n"\
                                            "      \"Pressure (Pa)\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_ICP_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_ICP_JSON ) + 10 )

#define PUBLISH_PAYLOAD_FORMAT_OB1203_JSON  "{\r\n"\
                                            "\"OB1203\" : {\r\n"\
                                            "      \"spo2 ()\" :\"%f\",\r\n"\
                                            "      \"Heart Rate ()\" :\"%f\",\r\n"\
                                            "      \"Breath rate ()\" :\"%f\",\r\n"\
                                            "      \"P2P ()\" :\"%f\"\r\n"\
                                            "\r\n         }\r\n"\
                                            "}\r\n"
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_OB1203_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_OB1203_JSON ) + 20 )


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
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_BULK_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_BULK_JSON ) + 130 )

extern TaskHandle_t sensors_thread;
extern char g_certificate[2048];
extern char g_private_key[2048];
extern char g_mqtt_endpoint[128];
extern char g_code_sign_pkey[256];

/*************************************************************************************
 * Private functions
 ************************************************************************************/
static int config_littlFs_flash(void);
static MQTTStatus_t sendMessage(const char **pTopicNames, uint8_t Topics_index, double *message);

static void IAQDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void OAQDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void HS3001DataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void ICMDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void ICPDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void OB1203DataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void BulkDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void temperatureLedDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static void spo2LedDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo );
static MQTTStatus_t prvSubscribeToTopic( MQTTQoS_t xQoS,char * pcTopicFilter, size_t xTopicFilterLength);
static void prvSubscribeCommandCallback( MQTTAgentCommandContext_t * pxCommandContext, MQTTAgentReturnInfo_t * pxReturnInfo );
static void prvIncomingPublishCallback( void * pvIncomingPublishCallbackContext, MQTTPublishInfo_t * pxPublishInfo );
static MQTTStatus_t prvPublishToTopic( MQTTQoS_t xQoS,char * pcTopic,size_t xTopicLength,uint8_t * pucPayload,size_t xPayloadLength,int32_t lNumTries );
static void prvPublishCommandCallback( MQTTAgentCommandContext_t * pxCommandContext, MQTTAgentReturnInfo_t * pxReturnInfo );
static CK_RV storePublicKey(void);
static int parse_temperature_led_actuation_message(mqtt_rx_payload_t *lmq_data);
static int parse_spo2_led_actuation_message(mqtt_rx_payload_t *lmq_data);
/*************************************************************************************
 * global functions
 ************************************************************************************/
extern MQTTAgentContext_t xGlobalMqttAgentContext;
extern void vStartOtaDemo( void );

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
struct MQTTAgentCommandContext
{
    TaskHandle_t xTaskToNotify;
    void * pArgs;
};
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
const char *pTopics_subscribe[MAX_NUM_SUBSCRIBE_TOPIC] =
{
    "",
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
typedef void ( * callback_fn)(MQTTAgentCommandContext_t *, MQTTPublishInfo_t *);
typedef struct callback_subscribe
{
    callback_fn ( * p_func)(MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo); /*<! Pointer to Test Function */
} st_callback_subscribe_t;
static st_callback_subscribe_t s_callback_subscribe[] =
{
     {},
     {(void *) IAQDataCallback},
     {(void *)OAQDataCallback},
     {(void *)HS3001DataCallback},
     {(void *)ICMDataCallback},
     {(void *)ICPDataCallback},
     {(void *)OB1203DataCallback},
     {(void *)BulkDataCallback},
     {(void *)temperatureLedDataCallback},
     {(void *)spo2LedDataCallback},
};

/*************************************************************************************
 * Private variables
 ************************************************************************************/
char CLIENT_CERTIFICATE_PEM[2048];
char CLIENT_KEY_PEM[2048];
static char USER_MQTT_ENDPOINT[128];

static uint32_t ulIPAddress = RESET_VALUE;
static mqtt_rx_payload_t mq_data = { RESET_VALUE };
static char pubPayload[PUBLISH_PAYLOAD_BUFFER_LENGTH_BULK_JSON] = { RESET_VALUE };

static char * public_key = NULL;
/*******************************************************************************************************************//**
 * @brief      Application Thread entry function
 * @param[in]   pvParameters     contains TaskHandle_t
 * @retval
 * @retval
 ***********************************************************************************************************************/
void app_thread_entry(void *pvParameters)
{
    fsp_pack_version_t      version = {RESET_VALUE};
    fsp_err_t               err = FSP_SUCCESS;
    BaseType_t              bt_status = pdFALSE;
    int                     lfs_err = LFS_ERR_OK;
    int                     ierr = FSP_SUCCESS;
    uint32_t                ip_status = RESET_VALUE;
    MQTTStatus_t            xMQTTStatus;
    CK_RV                   str_status = CKR_OK;

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

    lfs_err = config_littlFs_flash();
    if (LFS_ERR_OK != lfs_err)
    {
        FAILURE_INDICATION;
        APP_ERR_PRINT("** littleFs flash config failed **\r\n");
        APP_ERR_TRAP(lfs_err);
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
       FAILURE_INDICATION;
       APP_ERR_PRINT("User Network Initialization Failed");
       APP_ERR_TRAP(bt_status);
    }

    APP_PRINT("waiting for network link up");
    bt_status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY);
    if (pdTRUE != bt_status)
    {
       FAILURE_INDICATION;
       APP_ERR_PRINT("xTaskNotifyWait Failed");
       APP_ERR_TRAP(bt_status);
    }

    NETWORK_CONNECT_INDICATION;
    /* Prints the Ethernet Configuration Header for Post IP Init*/
    APP_PRINT(ETH_POSTINIT);
    /* Get the IP address for the MQTT END POINT used for the application*/
    ulIPAddress = FreeRTOS_gethostbyname((char*)USER_MQTT_ENDPOINT);

    if( RESET_VALUE == ulIPAddress )
    {
        FAILURE_INDICATION;
        IotLogError("FreeRTOS_gethostbyname  Failed to get the End point address for %s",USER_MQTT_ENDPOINT);
        APP_ERR_TRAP(RESET_VALUE);
    }

    /* Convert the IP address to a string to print on to the console. */
    if(NULL == FreeRTOS_inet_ntoa( ulIPAddress, ( char * ) cBuffer))
    {
        APP_PRINT("\r\nDNS Lookup for \"%s\" is      : %s  \r\n", USER_MQTT_ENDPOINT,  cBuffer);
        APP_ERR_TRAP(FSP_ERR_TIMEOUT);
    }
    /* Prints the Ethernet and IP Configuration post IP Init */
    print_ipconfig();

    APP_PRINT("\r\nDNS Lookup for \"%s\" is      : %s  \r\n", USER_MQTT_ENDPOINT,  cBuffer);

    err = vAlternateKeyProvisioning (&params);
    if (FSP_SUCCESS != err)
    {
          FAILURE_INDICATION;
          APP_ERR_PRINT("** Alternate Key Provisioning failed **\r\n");
          APP_ERR_TRAP(err);
    }

#if ENABLE_OTA_UPDATE_DEMO
    /* Store public key */
    str_status = storePublicKey();
    if (CKR_OK != str_status)
    {
        APP_ERR_PRINT("** Store Code Signing Public Key key failed **\r\n");
        APP_ERR_TRAP(err);
    }
#endif

    bt_status = xMQTTAgentInit();
    if (pdTRUE != bt_status)
    {
       FAILURE_INDICATION;
       APP_ERR_PRINT("xMQTTAgentInit Failed");
       APP_ERR_TRAP(bt_status);
    }
    xSetMQTTAgentState( MQTT_AGENT_STATE_INITIALIZED );
    vStartMQTTAgent (APP_MAIN_MQTT_AGENT_TASK_STACK_SIZE, APP_MAIN_MQTT_AGENT_TASK_PRIORITY);
#if ENABLE_OTA_UPDATE_DEMO
    /* Start OTA */
    vStartOtaDemo();
#endif

    /******** Attempt to establish TLS session with MQTT broker. **********/
    if( xGetMQTTAgentState() != MQTT_AGENT_STATE_CONNECTED )
    {
        ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
    }

    /* Subscribe to topics for the Data Pull and LED actuation */
    const char **pTopicnames = pTopics_subscribe;
    const char * pcTopic;
    uint8_t index;
    for (index = 1; index < MAX_NUM_SUBSCRIBE_TOPIC; index++)
    {
        pcTopic=pTopicnames[index];
        xMQTTStatus = prvSubscribeToTopic( MQTTQoS1, (char *)pcTopic, strlen(pcTopic));
        if( MQTTSuccess != xMQTTStatus )
        {
            APP_ERR_PRINT( "Failed to subscribe to topic: %s \r\n", pcTopic);
            APP_ERR_TRAP(MQTTSuccess == xMQTTStatus);
        }
        else
        {
            APP_PRINT( "Successfully subscribed to topic: %s \r\n",pcTopic );
        }
    }

    APP_PRINT("Device is Ready for Publishing and Subscription of Messages \r\n\r\n");

    xTaskNotifyFromISR(sensors_thread, 1, 1, NULL);

    while (1)
    {
        if( xGetMQTTAgentState() == MQTT_AGENT_STATE_DISCONNECTED )
        {
            ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
        }
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

                    xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_IAQ_DATA_PULL, temp_db);
                    if ((MQTTSuccess != xMQTTStatus))
                    {
                            APP_ERR_PRINT("** IAQ Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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

                    xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_OAQ_DATA_PULL, temp_db);
                    if ((MQTTSuccess != xMQTTStatus))
                    {
                        APP_ERR_PRINT("** OAQ Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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
                    xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_HS_DATA_PULL, temp_db);
                    if ((MQTTSuccess != xMQTTStatus))
                    {
                        APP_ERR_PRINT("** HS3001 Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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

                    xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_ICM_DATA_PULL,temp_db);
                    if ((MQTTSuccess != xMQTTStatus))
                    {
                        APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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

                    xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_ICP_DATA_PULL, temp_db);
                    if ((MQTTSuccess != xMQTTStatus))
                    {
                        APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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

                    xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_OB1203_DATA_PULL, temp_db);
                    if ((MQTTSuccess != xMQTTStatus))
                    {
                        APP_ERR_PRINT("** ICM Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        AWS_ACTIVITY_INDICATION;
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

                        xMQTTStatus = sendMessage (pTopics_publish, (uint8_t)ID_BULK_SENS_DATA_PULL, temp_db);
                        if ((MQTTSuccess != xMQTTStatus))
                        {
                            APP_ERR_PRINT("** Bulk Sensor Message Publish failed **\r\n");
                        }
                        else
                        {
                            AWS_ACTIVITY_INDICATION;
                        }
                 }
                 break;
                case ID_TEMPERATURE_DATA_PUSH:
                {
                    APP_PRINT("Topic Received from Cloud %s \r\n",mq_data.led_data.led_act_topic_msg);
                    if (FSP_SUCCESS != parse_temperature_led_actuation_message (&mq_data))
                    {
                        APP_ERR_PRINT("** LED Topic Actuation returned  Error **\r\n");
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
                }
                break;
            }
        }
        else
        {
            /* Do nothing */
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

    /* mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if(LFS_ERR_OK != lfs_err)
    {
        /* reformat if we can't mount the file system
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

static void temperatureLedDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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

static void spo2LedDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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

static void IAQDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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

static void OAQDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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

static void HS3001DataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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
 * @brief      ICM data publish Callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void ICMDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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
 * @brief      ICP data publish Callback function
 * @param[in]  pContext     pointer to struct representing an MQTT connection
 * @param[out]  pPublishInfo pointer PUBLISH packet parameters
 * @retval
 * @retval
 ***********************************************************************************************************************/

static void ICPDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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

static void OB1203DataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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

static void BulkDataCallback( MQTTAgentCommandContext_t * pContext, MQTTPublishInfo_t * pPublishInfo )
{
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
 * @param[in] pTopicNames           pointer to Topic Names
 * @param[in] Topics_index          Topic Index
 * @param[in] message               Message Payload
 *
 * @return    MQTTSuccess           if PUBLISH was successfully sent.
 * @return    Any Other Error code  if Failed to generate MQTT PUBLISH payload for PUBLISH or PUBLISH was unsuccessfully sent.
 ******************************************************************************************************************************/
static MQTTStatus_t sendMessage(const char **pTopicNames, uint8_t Topics_index, double *message)
{
    MQTTStatus_t    xMQTTStatus;
    const char    * pcTopicFilter = NULL;
    int             payloadlength = 0;

    pcTopicFilter = pTopicNames[Topics_index];
    memset(pubPayload, 0x00, sizeof(pubPayload));
    /* Generate the payload for the PUBLISH. */
    switch (Topics_index)
        {
            case ID_IAQ_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_IAQ_JSON,
                                message[0], message[1], message[2]);
                break;
            case ID_OAQ_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_OAQ_JSON, message[0]);
                break;
            case ID_HS_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_HS3001_JSON, message[0],message[1]);
                break;
            case ID_ICM_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_ICM_JSON,
                                message[0], message[1], message[2], message[3], message[4], message[5], message[6], message[7],
                                message[8]);
                break;
            case ID_ICP_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_ICP_JSON, message[0], message[1]);
                break;
            case ID_OB1203_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_OB1203_JSON, message[0], message[1],
                                message[2], message[3]);
                break;
            case ID_BULK_SENS_DATA_PULL:
                payloadlength = snprintf (pubPayload, sizeof(pubPayload), PUBLISH_PAYLOAD_FORMAT_BULK_JSON, message[0], message[1],
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

    /* Publish messages. */
    xMQTTStatus = prvPublishToTopic(MQTTQoS1,(char *)pcTopicFilter,strlen( pcTopicFilter ),( uint8_t * )pubPayload,(size_t)payloadlength,MQTT_EXAMPLE_NUM_PUBLISH_RETRIES);

    return xMQTTStatus;
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
            IotLogInfo("\r\nCOLD LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nCOLD LED OFF \r\n");
        }

        /* GREEN LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_warm_led_on))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_OFF);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nWARM LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_temperature_led_off))
        {
            led_on_off (RGB_LED_RED, LED_ON);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);

            IotLogInfo("\r\nWARM LED OFF \r\n");
        }

        /* RED LED */
        if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_hot_led_on))
        {
            led_on_off (RGB_LED_RED, LED_OFF);
            led_on_off (RGB_LED_GREEN, LED_ON);
            led_on_off (RGB_LED_BLUE, LED_ON);
            IotLogInfo("\r\nHOT LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_temperature_led_off))
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
            IotLogInfo("\r\nSPO2 LED ON\r\n");
        }
        else if (RESET_VALUE == strcmp ((char *) lmq_data->led_data.led_act_topic_msg, msg_spo2_led_off))
        {
            led_on_off (LED_BLUE, LED_OFF);
            IotLogInfo("\r\nSPO2 LED OFF \r\n");
        }
     }
    return status;
}

/*******************************************************************************************************************************
 * @brief Subscribe to a MQTT topic filter.
 *
 * @param[in]       xQoS                    The quality of service (QoS) to use.
 * @param[in]       pcTopicFilter           Pointer to the shadow topic buffer.
 * @param[in]       usTopicFilterLength     Indicates the length of the shadow topic buffer.
 *
 * @return          MQTTSuccess             if SUBSCRIBE was successfully sent.
 * @return          Any Other Error code    if SUBSCRIBE was unsuccessfully sent.
 ******************************************************************************************************************************/

static MQTTStatus_t prvSubscribeToTopic( MQTTQoS_t xQoS,
                                         char * pcTopicFilter,
                                         size_t xTopicFilterLength )
{
    MQTTStatus_t xCommandStatus;
    MQTTAgentSubscribeArgs_t xSubscribeArgs = { 0 };
    MQTTSubscribeInfo_t xSubscribeInfo = { 0 };
    MQTTAgentCommandContext_t xCommandContext = { 0UL };
    MQTTAgentCommandInfo_t xCommandParams = { 0UL };
    uint32_t ulNotifiedValue = 0U;

    /* Complete the subscribe information.  The topic string must persist for
     * duration of subscription! */
    xSubscribeInfo.pTopicFilter = pcTopicFilter;
    xSubscribeInfo.topicFilterLength = ( uint16_t ) xTopicFilterLength;
    xSubscribeInfo.qos = xQoS;
    xSubscribeArgs.pSubscribeInfo = &xSubscribeInfo;
    xSubscribeArgs.numSubscriptions = 1;

    /* Complete an application defined context associated with this subscribe message.
     * This gets updated in the callback function so the variable must persist until
     * the callback executes. */
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.pArgs = ( void * ) &xSubscribeArgs;

    xCommandParams.blockTimeMs = MQTT_EXAMPLE_MAX_COMMAND_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvSubscribeCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xCommandContext;

    xTaskNotifyStateClear( NULL );

    /* Loop in case the queue used to communicate with the MQTT agent is full and
     * attempts to post to it time out.  The queue will not become full if the
     * priority of the MQTT agent task is higher than the priority of the task
     * calling this function. */
    do
    {
        xCommandStatus = MQTTAgent_Subscribe( &xGlobalMqttAgentContext,
                                              &xSubscribeArgs,
                                              &xCommandParams );

        if( xCommandStatus == MQTTSuccess )
        {
            /*
             * If command was enqueued successfully, then agent will either process the packet successfully, or if
             * there is a disconnect, then it either retries the subscribe message while reconnecting and resuming
             * persistent sessions or cancel the operation and invokes the callback for failed response.
             * Hence the caller task wait indefinitely for a success or failure response from agent.
             */
            ( void ) xTaskNotifyWait( 0UL,
                                      UINT32_MAX,
                                      &ulNotifiedValue,
                                      portMAX_DELAY );
            xCommandStatus = ( MQTTStatus_t ) ( ulNotifiedValue );
        }

        if( xCommandStatus != MQTTSuccess )
        {
            if( xGetMQTTAgentState() == MQTT_AGENT_STATE_DISCONNECTED )
            {
                ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
            }
        }
    } while( xCommandStatus != MQTTSuccess );

    return xCommandStatus;
}

/*******************************************************************************************************************************
 * @brief prvSubscribeCommandCallback function
 *
 * This function subscribes topic filter callback and sends a notification to the task called MQTTAgent_Subscribe() to let the
 * task know the SUBSCRIBE operation completed.
 *
 * @param[in] pxCommandContext      Context of the initial command.
 * @param[in] pxReturnInfo          The result of the command.
 * 
 ******************************************************************************************************************************/

static void prvSubscribeCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                         MQTTAgentReturnInfo_t * pxReturnInfo )
{
    BaseType_t xSubscriptionAdded = pdFALSE;
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs = ( MQTTAgentSubscribeArgs_t * ) pxCommandContext->pArgs;

    /* Check if the subscribe operation is a success. Only one topic is subscribed by this demo. */
    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        /* Add subscription so that incoming publishes are routed to the application callback. */
        xSubscriptionAdded = xAddMQTTTopicFilterCallback( pxSubscribeArgs->pSubscribeInfo->pTopicFilter,
                                                          pxSubscribeArgs->pSubscribeInfo->topicFilterLength,
                                                          prvIncomingPublishCallback,
                                                          NULL,
                                                          pdTRUE );
        configASSERT( xSubscriptionAdded == pdTRUE );
    }

    xTaskNotify( pxCommandContext->xTaskToNotify,
                 ( uint32_t ) ( pxReturnInfo->returnCode ),
                 eSetValueWithOverwrite );
}

/*******************************************************************************************************************************
 * @brief prvIncomingPublishCallback function
 *
 * @param[in] pvIncomingPublishCallbackContext  Context of the initial command.
 * @param[in] pxPublishInfo                     Deserialized publish.
 *
 * @return    None
 ******************************************************************************************************************************/

static void prvIncomingPublishCallback( void * pvIncomingPublishCallbackContext, MQTTPublishInfo_t * pxPublishInfo )
{
    uint8_t index;
    for (index = 0; index < MAX_NUM_SUBSCRIBE_TOPIC; index++)
    {
        if( ( pxPublishInfo->topicNameLength == strlen( pTopics_subscribe[index] ) ) &&
          ( 0 == strncmp( pTopics_subscribe[index], pxPublishInfo->pTopicName, pxPublishInfo->topicNameLength ) ) )
        {
            s_callback_subscribe[index].p_func(pvIncomingPublishCallbackContext,pxPublishInfo);
        }
    }
}
/*-----------------------------------------------------------*/

/*******************************************************************************************************************************
 * @brief Publish a message to a MQTT topic.
 *
 * @param[in]       xQoS                    The quality of service (QoS) to use.
 * @param[in]       pcTopic                 Points to the topic.
 * @param[in]       xTopicLength            The length of the topic.
 * @param[in]       pucPayload              Points to the payload.
 * @param[in]       xPayloadLength          The length of the payload.
 * @param[in]       lNumRetries             The number of retries when publishing fails.
 *
 * @return          MQTTSuccess             if PUBLISH was successfully sent.
 * @return          Any Other Error code    if PUBLISH was unsuccessfully sent.
 ******************************************************************************************************************************/

static MQTTStatus_t prvPublishToTopic( MQTTQoS_t xQoS,
                                       char * pcTopic,
                                       size_t xTopicLength,
                                       uint8_t * pucPayload,
                                       size_t xPayloadLength,
                                       int32_t lNumRetries )
{
    MQTTPublishInfo_t xPublishInfo = { 0UL };
    MQTTAgentCommandContext_t xCommandContext = { 0 };
    MQTTStatus_t xCommandStatus;
    MQTTAgentCommandInfo_t xCommandParams = { 0UL };
    uint32_t ulNotifiedValue = 0U;

    xTaskNotifyStateClear( NULL );

    /* Configure the publish operation. */
    xPublishInfo.qos = xQoS;
    xPublishInfo.pTopicName = pcTopic;
    xPublishInfo.topicNameLength = ( uint16_t ) xTopicLength;
    xPublishInfo.pPayload = pucPayload;
    xPublishInfo.payloadLength = xPayloadLength;

    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();

    xCommandParams.blockTimeMs = MQTT_EXAMPLE_MAX_COMMAND_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvPublishCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xCommandContext;

    do
    {
        xCommandStatus = MQTTAgent_Publish( &xGlobalMqttAgentContext,
                                            &xPublishInfo,
                                            &xCommandParams );

        if( xCommandStatus == MQTTSuccess )
        {
            /*
             * If command was enqueued successfully, then agent will either process the packet successfully, or if
             * there is a disconnect, then it either retries the publish after reconnecting and resuming session
             * (only for persistent sessions) or cancel the operation and invokes the callback for failed response.
             * Hence the caller task wait indefinitely for a success or failure response from agent.
             */
            ( void ) xTaskNotifyWait( 0UL,
                                      UINT32_MAX,
                                      &ulNotifiedValue,
                                      portMAX_DELAY );
            xCommandStatus = ( MQTTStatus_t ) ( ulNotifiedValue );
        }

        if( ( xCommandStatus != MQTTSuccess ) && ( lNumRetries > 0 ) )
        {
            if( xGetMQTTAgentState() == MQTT_AGENT_STATE_DISCONNECTED )
            {
                ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
            }

            if( xQoS == MQTTQoS1 )
            {
                xPublishInfo.dup = true;
            }
        }
    } while( ( xCommandStatus != MQTTSuccess ) && ( lNumRetries-- > 0 ) );

    return xCommandStatus;
}
/*-----------------------------------------------------------*/

/*******************************************************************************************************************************
 * @brief prvPublishCommandCallback function
 *
 * This function sends a notification to the task that called MQTTAgent_Publish() to let the task know the PUBLISH operation
 * completed.
 *
 * @param[in] pxCommandContext  Context of the initial command.
 * @param[in] pxReturnInfo      The result of the command.
 *
 * @return    None
 ******************************************************************************************************************************/

static void prvPublishCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                       MQTTAgentReturnInfo_t * pxReturnInfo )
{
    if( pxCommandContext->xTaskToNotify != NULL )
    {
        ( void ) xTaskNotify( pxCommandContext->xTaskToNotify,
                              pxReturnInfo->returnCode,
                              eSetValueWithOverwrite );
    }
}

/*******************************************************************************************************************************
 * @brief Imports a public key into the PKCS #11 module.
 *
 * @param   None
 *
 * @return  CKR_OK       upon successful key creation.
 * @return  Otherwise    a positive PKCS #11 error code.
 ******************************************************************************************************************************/

static CK_RV storePublicKey(void)
{
    CK_RV xResult = CKR_OK;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Initialize the PKCS Module */
    if( xResult == CKR_OK )
    {
        xResult = xInitializePkcs11Token();
    }

    if( xResult == CKR_OK )
    {
        xResult = xInitializePkcs11Session( &xSession );
    }
    /* Store code signing public key to public_key variable */
    public_key = g_code_sign_pkey;
    if( xResult == CKR_OK )
    {
        xResult = xProvisionPublicKey(xSession,
                                      (uint8_t *)public_key,
                                      (strlen(public_key) + 1),
                                      CKK_EC,
                                      ( uint8_t * ) pkcs11configLABEL_CODE_VERIFICATION_KEY,
                                      &xObject );
        if( ( xResult != CKR_OK ) || ( xObject == CK_INVALID_HANDLE ) )
        {
            APP_ERR_PRINT( "Failed to provision device public key. %d \r\n", (int)xResult );
        }
    }
    return xResult;
}
