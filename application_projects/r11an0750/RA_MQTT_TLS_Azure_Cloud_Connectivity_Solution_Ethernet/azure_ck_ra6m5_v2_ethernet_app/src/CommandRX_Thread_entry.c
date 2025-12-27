/***********************************************************************************************************************
 * File Name    : CommandRX_Thread_entry.c
 * Description  : Contains entry function of Command message .
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "CommandRX_Thread.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_hub_client.h"
#include "sample_config.h"

//----
extern SAMPLE_CLIENT  client;
#define iothub_client client.iothub_client
#define SAMPLE_ALL_EVENTS                                               ((ULONG)0xFFFFFFFF)
#define SAMPLE_COMMAND_RECEIVE_EVENT                                    ((ULONG)0x00000010)
static void sample_command_action(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr);
ULONG app_events;
static TX_EVENT_FLAGS_GROUP sample_events;
//----

/* CommandRX_Thread entry function */
void CommandRX_Thread_entry(void)
{
    /* TODO: add your own code here */
    while (1)
    {
        /* Pickup sample event flags.  */
        tx_event_flags_get(&(sample_events), SAMPLE_ALL_EVENTS, TX_OR_CLEAR, &app_events, NX_WAIT_FOREVER);

        if (app_events & SAMPLE_COMMAND_RECEIVE_EVENT)
        {
            sample_command_action(&iothub_client);
        }
        tx_thread_sleep (1);
    }
}


static void sample_command_action(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr)
{
    UINT status = 0;
    const UCHAR *component_name_ptr;
    USHORT component_name_length;
    const UCHAR *command_name_ptr;
    USHORT command_name_length;
    USHORT context_length;
    VOID *context_ptr;
    NX_PACKET *packet_ptr;

    /* Loop to receive command message.  */
    while (1)
    {
        if ((status = nx_azure_iot_hub_client_command_message_receive(hub_client_ptr,
                                                                      &component_name_ptr, &component_name_length,
                                                                      &command_name_ptr, &command_name_length,
                                                                      &context_ptr, &context_length,
                                                                      &packet_ptr, NX_NO_WAIT)))
        {
            return;
        }

        printf("Received command: %.*s", (INT)command_name_length, (CHAR *)command_name_ptr);
        printf("\r\n");
    }
}
