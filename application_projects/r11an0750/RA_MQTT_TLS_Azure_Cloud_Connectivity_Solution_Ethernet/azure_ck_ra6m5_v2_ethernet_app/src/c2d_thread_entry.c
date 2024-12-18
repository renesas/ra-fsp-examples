/***********************************************************************************************************************
 * File Name    : c2d_thread_entry.c
 * Description  : Contains entry function of Azure IOT SDK Application cloud to device thread.
 ***********************************************************************************************************************/
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
 * Copyright (C) 2020-2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "c2d_thread.h"
#include "nx_azure_iot.h"
#include "nx_azure_iot_hub_client.h"
#include "sample_config.h"
#include "usr_app.h"


#define iothub_client client.iothub_client
#define MAX_LED_COUNT (3)

extern SAMPLE_CLIENT client;
/* Define LED properties.  */
static const CHAR *led_properties[MAX_LED_COUNT][2] = {{ "LED", "ON" }};

extern TX_THREAD Sensor_Thread;
extern TX_THREAD CommandRX_Thread;

/* Cloud2Device Thread entry function */
void c2d_thread_entry(void)
{

    NX_PACKET *packet_ptr;
    UINT status = RESET_VALUE;
    USHORT property_buf_size = RESET_VALUE;
    const UCHAR *property_buf;
    mqtt_rx_payload_t led_action_data =  { '\0' };

    /* TODO: add your own code here */
    status = tx_thread_resume (&Sensor_Thread);
    status = tx_thread_resume (&CommandRX_Thread);
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

        memset((char*) &led_action_data.value.led_data.led_act_topic_msg[0],0x00,LED_MSG_SIZE);
        strncpy ((char*) &led_action_data.value.led_data.led_act_topic_msg[0],(char*) property_buf, property_buf_size);

        led_action_data.id = ID_LED_ACT;

        tx_queue_send (&g_topic_queue, &led_action_data, USR_MSQ_WAIT);
        nx_packet_release (packet_ptr);


    }
}
