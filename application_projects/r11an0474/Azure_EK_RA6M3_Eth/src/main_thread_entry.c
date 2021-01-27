/***********************************************************************************************************************
 * File Name    : main_thread_entry.c
 * Description  : Main thread source file
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

#include "iot_init.h"
#include "iot_crypto.h"
#include "iot_logging_task.h"
#include "azure_mqtt.h"
#include "user_cfg.h"
#include "azure.h"
#include "aws_dev_mode_key_provisioning.h"
#include <azure/iot/az_iot_hub_client.h>
#include <main_thread.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#define LOGGING_TASK_STACK_SIZE         (1 * 1024)
#define LOGGING_TASK_STACK_PRIORITY     (6)
#define LOGGING_TASK_QUEUE_SIZE         (1 * 1024)
/*
 * @brief Total length of the hash in bytes.
 */
#define _MD5_HASH_LENGTH_BYTES    ( 16 )
#define IOT_DEVICE_IDENTIFIER_LENGTH ( _MD5_HASH_LENGTH_BYTES * 2 )
/**
 * @brief Device metrics name format
 */
#define IOT_METRICS_NAME "?SDK=" IOT_SDK_NAME "&Version=4.0.0&Platform=" IOT_PLATFORM_NAME "&AFRDevID=%.*s"
 /**
  * @brief Length of #AWS_IOT_METRICS_NAME.
  */
#define IOT_METRICS_NAME_LENGTH    ( ( uint16_t ) ( sizeof( IOT_METRICS_NAME ) + IOT_DEVICE_IDENTIFIER_LENGTH ) )
#define IP(a,b,c,d)             (const uint8_t[]) {a,b,c,d}
#define MAC(a,b,c,d,e,f)        (const uint8_t[]) {a,b,c,d,e,f}

/* Global Variables declaration */
char hostName[WIDTH_256] = {0};
char pPublishPayload[ WIDTH_128 ] = { 0 };
char p_name[WIDTH_64] = {PROP_TEMP};
static char deviceMetrics[IOT_METRICS_NAME_LENGTH + 1] = { 0 };
/* Handle of the MQTT connection used in this demo. */
IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/*Function Prototypes */
static int platform_init(void);
static void init_network_interface(void);
int send_telemetry(IotMqttConnection_t MqttConnection, const char *msg);
int send_method_response(char* topic);
fsp_err_t sensor_init(void);

extern bsp_leds_t g_bsp_leds;
extern TaskHandle_t main_thread;

/* Function Prototypes */
const char *pcApplicationHostnameHook( void );
int getAzureHubInfo(char *hostName);


const char *pcApplicationHostnameHook( void )
{
    return HOSTNAME;
}

const char *getDeviceMetrics( void )
{
    return deviceMetrics;
}

/*
 * @brief Get the device metrics length.
 */
uint16_t getDeviceMetricsLength( void )
{
    return ( uint16_t )( strlen( deviceMetrics ) );
}


/* Main Thread entry function */
/* pvParameters contains TaskHandle_t */
void main_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    int status = EXIT_SUCCESS;

    /* Initialize platform components */
    status = platform_init();
    if(EXIT_SUCCESS != status)
        __BKPT(0);

    //Initialize MQTT Client
    status = azure_mqtt_init();
    if(EXIT_SUCCESS == status)
    {
        /* Connect to Azure DPS to retrieve Azure IoT Hub connection info */
        status = getAzureHubInfo((char*)hostName);
        if(pdPASS != status)
        {
            printf("\r\nconnect_dps function failed!!!\r\n");
            __BKPT(0);
        }
    }

    /* Connect to Azure IoT Hub */
    mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    printf("\r\nConnecting to Azure IoT Hub..\r\n\r\n");
    status = azure_hub_connect(hostName, &mqttConnection);
    if(pdPASS != status)
    {
        printf("\r\nazure_mqtt_connect failed!!!\r\n");
        __BKPT(0);
    }

    printf("\r\nSuccessfully connected to Azure IoT Hub\r\n");

    while(true)
    {
        float sens_data;
        uint32_t len = 0;

        xQueueReceive(g_sensor_queue, &sens_data, portMAX_DELAY );

        /* Create JSON */
        len += (unsigned int)snprintf((char*)(pPublishPayload + len), WIDTH_128, "{\r\n");
        len += (unsigned int)snprintf((char*)(pPublishPayload + len), WIDTH_128, "\"%s\": \"%.2f\",\r\n",p_name, sens_data);

        /* Delete the comma after the last value and terminate the JSON with a curly brace */
        len -= 3;
        len += (unsigned int)snprintf((char*)(pPublishPayload + len), WIDTH_128, "\r\n}\r\n");

        /* publish payload */
        send_telemetry(mqttConnection, pPublishPayload);

        printf("%s\r\n", pPublishPayload);
    }
}

/*Function to initialize platform specific components */
static int platform_init(void)
{
    BaseType_t fr_status = 0;
    int status = EXIT_SUCCESS;
    fsp_err_t fsp_err = FSP_SUCCESS;
    ProvisioningParams_t params = {0};
    CK_RV xResult = 0;

    /* Initialize logging task */
    fr_status = xLoggingTaskInitialize(LOGGING_TASK_STACK_SIZE, LOGGING_TASK_STACK_PRIORITY, LOGGING_TASK_QUEUE_SIZE);
    if (pdPASS != fr_status)
    {
        __BKPT(0);
    }

    fsp_err = sensor_init();
    if(FSP_SUCCESS != fsp_err)
    {
        __BKPT(0);
    }

    /* Initialize platform specific crypto initialization */
    if(mbedtls_platform_setup(NULL) != 0)
    {
        printf("\r\nmbedtls_platform_setup Failed!!!\r\n");
        status = EXIT_FAILURE;
    }

    if(status == EXIT_SUCCESS)
    {
        CRYPTO_Init();

        status = psa_crypto_init();
        if(status != EXIT_SUCCESS)
        {
            printf("\r\npsa_crypto_init failed!!!, ret: %d\r\n",status);
            status = EXIT_FAILURE;
        }
    }

    /* Set up network interface */
    init_network_interface();

    /* Initialize LittleFS */
    fsp_err = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if(FSP_SUCCESS != fsp_err)
        __BKPT(0);

    fsp_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if(FSP_SUCCESS != fsp_err)
        __BKPT(0);

    fsp_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if(FSP_SUCCESS != fsp_err)
        __BKPT(0);

    /* Write the keys into a secure region in data flash. */
    params.pucClientPrivateKey        = (uint8_t*)DEVICE_PRIVATE_KEY_FILE;
    params.pucClientCertificate       = (uint8_t*)DEVICE_X509_TRUST_PEM_FILE;
    params.ulClientPrivateKeyLength  = 1 + strlen((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen((const char *) params.pucClientCertificate);
    xResult = vAlternateKeyProvisioning(&params);
    if(CKR_OK != xResult)
        __BKPT(0);

    /* Initialize common libraries required by network manager and demo */
    if(IotSdk_Init() == false)
    {
        printf("\r\nFailed to initialize the common library!!\r\n");
        IotSdk_Cleanup();
        __BKPT(0);
    }

    if(SOCKETS_Init() != pdPASS)
        __BKPT(0);

    return status;
}

void vApplicationIPNetworkEventHook (eIPCallbackEvent_t eNetworkEvent)
{
    if(eNetworkUp == eNetworkEvent)
    {
        uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
        char cBuffer[ 16 ];

        /* Signal application the network is UP */
        xTaskNotifyFromISR(main_thread, eNetworkUp, eSetBits, NULL);

        /* The network is up and configured.  Print out the configuration
        obtained from the DHCP server. */
        FreeRTOS_GetAddressConfiguration( &ulIPAddress,
                                          &ulNetMask,
                                          &ulGatewayAddress,
                                          &ulDNSServerAddress );

        /* Convert the IP address to a string then print it out. */
        FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
        printf( "IP Address: %s\r\n", cBuffer );

        /* Convert the net mask to a string then print it out. */
        FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
        printf( "Subnet Mask:  %s\r\n", cBuffer );

        /* Convert the IP address of the gateway to a string then print it out. */
        FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
        printf( "Gateway IP:     %s\r\n", cBuffer );

        /* Convert the IP address of the DNS server to a string then print it out. */
        FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
        printf( "DNS server IP: %s\r\n", cBuffer );
    }
}

/* Function to initialize network interface */
static void init_network_interface(void)
{
    BaseType_t xReturn;

    xReturn = FreeRTOS_IPInit(IP(0,0,0,0), IP(255,255,255,0), IP(0,0,0,0),
                              IP(0,0,0,0), MAC(0x00,0x11,0x22,0x33,0x44,0x56));
    if(xReturn == pdTRUE)
    {
        uint32_t ip_status = 0;
        if(xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY) != pdTRUE)
        {
            __BKPT(0);
        }
    }
}

/* Process the incoming payload */
int send_method_response(char* topic)
{
    bsp_io_level_t level;
    bsp_leds_t leds = g_bsp_leds;

    azure_mqtt_publish(mqttConnection, topic, "");

    for(int idx = 0; idx < leds.led_count; idx++)
    {
        R_IOPORT_PinRead(&g_ioport_ctrl, leds.p_leds[idx], &level);
        R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[idx], !level);
    }

    return 0;
}
