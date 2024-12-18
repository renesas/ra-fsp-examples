/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#ifndef SAMPLE_CONFIG_H
#define SAMPLE_CONFIG_H

#ifdef __cplusplus
extern   "C" {
#endif

#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_provisioning_client.h"

/* This sample uses Symmetric key (SAS) to connect to IoT Hub by default,
   simply defining USE_DEVICE_CERTIFICATE and setting your device certificate in sample_device_identity.c
   to connect to IoT Hub with x509 certificate. Set up X.509 security in your Azure IoT Hub,
   refer to https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-security-x509-get-started  */
#define USE_DEVICE_CERTIFICATE                      1


/*
TODO`s: Configure core settings of application for your IoTHub.
*/
#define SAMPLE_PNP_MODEL_ID "dtmi:renesas:ra:ckra6m5:AZCKRA6M5ETH;2"
/* Defined, DPS is enabled.  */
//#define ENABLE_DPS_SAMPLE
/* Defined, telemetry is disabled.  */
#define DISABLE_TELEMETRY_SAMPLE
/* Defined, C2D is disabled.  */
#define DISABLE_C2D_SAMPLE
/* Defined, Direct method is disabled.  */
#define DISABLE_DIRECT_METHOD_SAMPLE
/* Defined, Device twin is disabled.  */
#define DISABLE_DEVICE_TWIN_SAMPLE

#ifndef ENABLE_DPS_SAMPLE

/* Required when DPS is not used.  */

#else /* !ENABLE_DPS_SAMPLE */
/* Required when DPS is used.  */
#ifndef ENDPOINT
#define ENDPOINT                                    "global.azure-devices-provisioning.net"
#endif /* ENDPOINT */

#ifndef ID_SCOPE
#define ID_SCOPE                                    "0ne000FFA42"
#endif /* ID_SCOPE */

#ifndef REGISTRATION_ID
#define REGISTRATION_ID                             "bdc98bd8-1dbc-4a46-980d-186539e4498a"
#endif /* REGISTRATION_ID */

#endif /* ENABLE_DPS_SAMPLE */

/* Optional SYMMETRIC KEY.  */
#ifndef DEVICE_SYMMETRIC_KEY
#ifndef ENABLE_DPS_SAMPLE
/* IOT HUB */ #define DEVICE_SYMMETRIC_KEY                "acV1Ypr2nB/yxdw7ugNO0V9iRWetZLvIHa88jNGxocA="

#else /* !ENABLE_DPS_SAMPLE */
/* DPS  */    #define DEVICE_SYMMETRIC_KEY                        "WuEbq/iOT/a7lDd+5xf/ugjZ2ZhvjAcFE13bFtciBUo="
#endif /* ENABLE_DPS_SAMPLE */
#endif /* DEVICE_SYMMETRIC_KEY */

/* Optional module ID.  */
#ifndef MODULE_ID
#define MODULE_ID                                   ""
#endif /* MODULE_ID */

#if (USE_DEVICE_CERTIFICATE == 1)

/* Using X509 certificate authenticate to connect to IoT Hub,
   set the device certificate as your device.  */

/* Device Key type. */
#ifndef DEVICE_KEY_TYPE
#define DEVICE_KEY_TYPE                             NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER
#endif /* DEVICE_KEY_TYPE */

#endif /* USE_DEVICE_CERTIFICATE */

/*
END TODO section
*/

/* Define the Azure RTOS IOT thread stack and priority.  */
#ifndef NX_AZURE_IOT_STACK_SIZE
#define NX_AZURE_IOT_STACK_SIZE                     (4096)
#endif /* NX_AZURE_IOT_STACK_SIZE */

#ifndef NX_AZURE_IOT_THREAD_PRIORITY
#define NX_AZURE_IOT_THREAD_PRIORITY                (4)
#endif /* NX_AZURE_IOT_THREAD_PRIORITY */

#ifndef SAMPLE_MAX_BUFFER
#define SAMPLE_MAX_BUFFER                           (256)
#endif /* SAMPLE_MAX_BUFFER */

/* Define the sample thread stack and priority.  */
#ifndef SAMPLE_STACK_SIZE
#define SAMPLE_STACK_SIZE                           (2048)
#endif /* SAMPLE_STACK_SIZE */

#ifndef SAMPLE_THREAD_PRIORITY
#define SAMPLE_THREAD_PRIORITY                      (16)
#endif /* SAMPLE_THREAD_PRIORITY */

/* Define sample properties count.  */
#define MAX_PROPERTY_COUNT                          (2)

/* Generally, IoTHub Client and DPS Client do not run at the same time, user can use union as below to
   share the memory between IoTHub Client and DPS Client. */
/* NOTE: If user can not make sure sharing memory is safe, IoTHub Client and DPS Client must be defined seperately.  */
typedef union SAMPLE_CLIENT_UNION
{
 NX_AZURE_IOT_HUB_CLIENT                         iothub_client;

#ifdef ENABLE_DPS_SAMPLE
 NX_AZURE_IOT_PROVISIONING_CLIENT                prov_client;
#endif /* ENABLE_DPS_SAMPLE */

} SAMPLE_CLIENT;

#ifdef __cplusplus
}
#endif
#endif /* SAMPLE_CONFIG_H */
