/***********************************************************************************************************************
 * File Name    : usr_app.c
 * Description  : Contains data structures and functions used for the Application init and its utilities
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "common_utils.h"
#include "usr_hal.h"
#include "usr_app.h"
#include "nx_azure_iot_hub_client.h"
#include "sample_config.h"

/* Define sample properties count.  */
#define MAX_TEMP_COUNT                        1
#define MAX_PB_COUNT                          1

extern SAMPLE_CLIENT  client;
#define iothub_client client.iothub_client
/* Define sample properties.  */

static const CHAR *temperature_properties[MAX_TEMP_COUNT][2] = {{"MCU Temperature", "73.12"}};
static const CHAR *button_properties[MAX_PB_COUNT][2] = {{"Push Button", "ON"}};

int sendMessage_ts(char *ltemp_str, UINT msg_id)
{
    UINT status = 0;
    CHAR buffer[30];
    UINT buffer_length;
    NX_PACKET *packet_ptr;

    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create (&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        IotLog("Telemetry message create failed!: error code = 0x%08x\r\n", status);
        return -1;
    }

    /* Add properties to telemetry message.  */
    for (int index = 0; index < MAX_TEMP_COUNT; index++)
    {
        if ((status = nx_azure_iot_hub_client_telemetry_property_add (
                packet_ptr, (UCHAR*) temperature_properties[index][0],
                (USHORT) strlen (temperature_properties[index][0]), (UCHAR*) ltemp_str, (USHORT) strlen (ltemp_str),
                NX_WAIT_FOREVER)))
        {
            IotLog("Telemetry property add failed!: error code = 0x%08x\r\n", status);
            return -1;
        }
    }

    if (status)
    {
        nx_azure_iot_hub_client_telemetry_message_delete (packet_ptr);
        return -1;
    }

    buffer_length = (UINT) snprintf (buffer, sizeof(buffer), "{\"Message ID\":%u}", msg_id);
    if (nx_azure_iot_hub_client_telemetry_send (&iothub_client, packet_ptr, (UCHAR*) buffer, buffer_length,
    NX_WAIT_FOREVER))
    {
        IotLog("Telemetry message send failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_hub_client_telemetry_message_delete (packet_ptr);
        return -1;
    }
    IotLog("Telemetry message send: %s.\r\n", buffer);
    return 0;
}

int sendMessage_pb(uint16_t switch_num, UINT msg_id)
{
    UINT status = 0;
    CHAR buffer[30];
    UINT buffer_length;
    NX_PACKET *packet_ptr;

    FSP_PARAMETER_NOT_USED(switch_num);
    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create (&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
    {
        IotLog("Telemetry message create failed!: error code = 0x%08x\r\n", status);
        return -1;
    }

    /* Add properties to telemetry message.  */
    for (int index = 0; index < MAX_PB_COUNT; index++)
    {
        if ((status = nx_azure_iot_hub_client_telemetry_property_add (packet_ptr, (UCHAR*) button_properties[index][0],
                                                                      (USHORT) strlen (button_properties[index][0]),
                                                                      (UCHAR*) button_properties[index][1],
                                                                      (USHORT) strlen (button_properties[index][1]),
                                                                      NX_WAIT_FOREVER)))
        {
            IotLog("Telemetry property add failed!: error code = 0x%08x\r\n", status);
            return -1;
        }
    }

    if (status)
    {
        nx_azure_iot_hub_client_telemetry_message_delete (packet_ptr);
        return -1;
    }

    buffer_length = (UINT) snprintf (buffer, sizeof(buffer), "{\"Message ID\":%u}", msg_id);
    if (nx_azure_iot_hub_client_telemetry_send (&iothub_client, packet_ptr, (UCHAR*) buffer, buffer_length,
                                                NX_WAIT_FOREVER))
    {
        IotLog("Telemetry message send failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_hub_client_telemetry_message_delete (packet_ptr);
        return -1;
    }
    IotLog("Telemetry message send: %s.\r\n", buffer);
    return 0;
}

UINT usr_packet_pool_init(void)
{

    UINT status = NX_SUCCESS;
    /* Create the packet pool. */
    status = nx_packet_pool_create (&g_packet_pool0, "g_packet_pool0 Packet Pool",
                                    G_PACKET_POOL0_PACKET_SIZE,
                                    &g_packet_pool0_pool_memory[0],
                                    G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    return (status);
}


 /*********************************************************************************************************************
  * convert float to string format and store it in buffer
  *********************************************************************************************************************/
void print_float(char *buffer, size_t buflen, double value)
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
        snprintf (buffer, buflen, "%c%d.%02d", sign, abs (integer), abs (fraction));
    }
    else
    {
        snprintf (buffer, buflen, "%d.%02d", abs (integer), abs (fraction));
    }
}
