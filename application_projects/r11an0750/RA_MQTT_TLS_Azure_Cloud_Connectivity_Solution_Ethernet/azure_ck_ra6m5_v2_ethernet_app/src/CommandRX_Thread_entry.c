/***********************************************************************************************************************
 * File Name    : CommandRX_Thread_entry.c
 * Description  : Contains entry function of Command message .
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
