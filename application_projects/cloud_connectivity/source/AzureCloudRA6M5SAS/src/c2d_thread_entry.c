/***********************************************************************************************************************
 * File Name    : c2d_thread_entry.c
 * Description  : Contains entry function of Azure IOT SDK Application cloud to device thread.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#include "c2d_thread.h"
#include "nx_azure_iot.h"
#include "nx_azure_iot_hub_client.h"
#include "sample_config.h"
#include "usr_app.h"

#define iothub_client client.iothub_client
#define MAX_LED_COUNT 1

extern SAMPLE_CLIENT client;
/* Define LED properties.  */
static const CHAR *led_properties[MAX_LED_COUNT][2] = {{ "LED", "ON" }};

/* Cloud2Device Thread entry function */
void c2d_thread_entry(void)
{

    NX_PACKET *packet_ptr;
    UINT status = RESET_VALUE;
    USHORT property_buf_size = RESET_VALUE;
    const UCHAR *property_buf;
    mqtt_rx_payload_t led_action_data =  { '\0' };

    /* TODO: add your own code here */
    while (true)
    {
        if ((status = nx_azure_iot_hub_client_cloud_message_receive (&iothub_client, &packet_ptr, NX_WAIT_FOREVER)))
        {
            IotLog("C2D receive failed!: error code = 0x%08x\r\n", status);
            break;
        }

        if ((status = nx_azure_iot_hub_client_cloud_message_property_get (&iothub_client, packet_ptr,
                                                                          (UCHAR*) led_properties[0][0],
                                                                          (USHORT) strlen (led_properties[0][0]),
                                                                          &property_buf, &property_buf_size)) == NX_AZURE_IOT_SUCCESS)
        {
            IotLog("Receive message from Cloud: %s = %s\r\n", led_properties[0][0],property_buf);
        }

        if (!(strcmp ((char*) property_buf, "ON")))
        {
            strncpy ((char*) &led_action_data.value.led_data.led_act_topic_msg[0], "ON", LED_ON_SIZE + 1);
        }
        else if (!(strcmp ((char*) property_buf, "OFF")))
        {
            strncpy ((char*) &led_action_data.value.led_data.led_act_topic_msg[0], "OFF", LED_OFF_SIZE + 1);
        }

        led_action_data.id = ID_LED_ACT;

        tx_queue_send (&g_topic_queue, &led_action_data, USR_MSQ_WAIT);
        nx_packet_release (packet_ptr);
    }
}
