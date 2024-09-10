/***********************************************************************************************************************
 * File Name    : application_thread_entry.c
 * Description  : Contains entry function of Azure IOT SDK Application Init.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#include <application_thread.h>
#include "common_utils.h"
#include <usr_app.h>
#include <usr_hal.h>
#include <usr_network.h>

extern UINT unix_time_get(ULONG *unix_time);
static double mcu_temp __attribute__ ((used)) = RESET_VALUE;

extern TX_THREAD c2d_thread;

/* application_thread_entry function */
void application_thread_entry(void)
{
    UINT status = EXIT_SUCCESS;
    uint16_t switch_num = RESET_VALUE;
    char temp_str[MAX_PROPERTY_LEN] =  { RESET_VALUE };
    mqtt_rx_payload_t mq_data =  { RESET_VALUE };
    static UINT message_id = RESET_VALUE;
    fsp_pack_version_t version = {RESET_VALUE};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, AP_VERSION, version.major, version.minor, version.patch);
    APP_PRINT(AP_INFO);

    IotLog("HAL Initialization  \r\n");
    status = usr_hal_init ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErr("HAL Initialization  failed: %u\r\n", status);
        IotLogErrTrap(status);
    }

    status = _nx_crypto_initialize ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErr("_nx_crypto_initialize  failed: %u\r\n", status);
        IotLogErrTrap(status);
    }

    status = usr_packet_pool_init ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErr("Packet Pool Creation failed: %u\r\n", status);
        IotLogErrTrap(status);
    }

    status = usr_netx_init ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErr("NetX Initialization failed: %u\r\n", status);
        IotLogErrTrap(status);
    }

    sample_entry (&g_ip0, &g_packet_pool0, &g_dns0, unix_time_get);

    /* Resume the thread represented by "c2d_thread". */
    status = tx_thread_resume (&c2d_thread);

    do
    {
        status = tx_queue_receive (&g_topic_queue, (VOID*) &mq_data, TX_WAIT_FOREVER);

        if (TX_SUCCESS == status)
        {
            switch (mq_data.id)
            {
                case ID_PB:
                {
                    IotLog("Push Button S%d Pressed\t",mq_data.value.pb_data.pb_num);
                    switch_num = mq_data.value.pb_data.pb_num;
                    if (EXIT_SUCCESS != sendMessage_pb (switch_num, message_id++))
                    {
                        IotLogErr("** Push Button Message Publish failed **\r\n");
                    }
                    else
                    {
                        ACTIVITY_INDICATION
                    }
                }
                break;

                case ID_TEMPERATURE:
                {
                    mcu_temp = ((((float) mq_data.value.adc_data.adc_value) * 0.353793f) - 467.39f);
                    print_float (temp_str, sizeof(temp_str), mcu_temp);
                    IotLog("MCU Temperature %s \t",temp_str);
                    if (EXIT_SUCCESS != sendMessage_ts (temp_str, message_id++))
                    {
                        IotLogErr("** Temperature Sensor Message Publish failed **\r\n");
                    }
                    else
                    {
                        ACTIVITY_INDICATION
                        ;
                    }
                }
                break;

                case ID_LED_ACT:
                {
                    IotLog("Topic Received from Cloud %s \r\n",mq_data.value.led_data.led_act_topic_msg);
                    if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "ON"))
                    {
                        led_on_off (LED_GREEN, LED_ON);
                        IotLog("\r\nGreen LED ON\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "OFF"))
                    {
                        led_on_off (LED_GREEN, LED_OFF);
                        IotLog("\r\nGreen LED OFF\r\n");
                    }
                    else
                    {
                        IotLog("\r\nMsg Topic didn't match !!!\r\n");
                    }
                    ACTIVITY_INDICATION
                }
                break;

                default:
                {
                    IotLogErr("** Error in MQTT Messages.. MQTT Client Cleanup started  **\r\n");
                }
                break;
            }
            memset (&mq_data.value.led_data.led_act_topic_msg, '\0', sizeof(mqtt_rx_payload_t));
        }
    }
    while (1);
}
