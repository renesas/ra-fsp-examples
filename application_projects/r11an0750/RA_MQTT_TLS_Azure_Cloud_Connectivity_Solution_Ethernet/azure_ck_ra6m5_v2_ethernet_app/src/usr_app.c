/***********************************************************************************************************************
 * File Name    : usr_app.c
 * Description  : Contains data structures and functions used for the Application init and its utilities
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "usr_app.h"
#include "nx_azure_iot_hub_client.h"
#include "sample_config.h"
#include "nx_api.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"
#include "nx_azure_iot_provisioning_client.h"
#include "user_choice.h"


#define DOUBLE_DECIMAL_PLACE_DIGITS                                    (2)
#define THREE_DECIMAL_PLACE_DIGITS                                     (3)

#define SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME                       "Environmental"


/* Define sample properties count.  */
#define MAX_TEMP_COUNT                        (1)
#define MAX_PB_COUNT                          (1)

/*******************************************************************************************************************//**
 * @brief       Packet pool initialize
 * @param[in]   None
 * @retval      NX_SUCCESS      Initialize successfully
 * @retval      Other           Initialize unsuccessfully
 ***********************************************************************************************************************/
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

/*******************************************************************************************************************//**
 * @brief       Send data to queue
 * @param[in]   queue_ptr       Pointer to the address of the queue
 * @param[in]   source_ptr      Pointer to the address of the data
 * @retval      NX_SUCCESS      Initialize successfully
 * @retval      Other           Initialize unsuccessfully
 ***********************************************************************************************************************/
UINT send_data_to_queue(TX_QUEUE *queue_ptr, VOID *source_ptr)
{
    UINT status = TX_SUCCESS;
    status = tx_queue_front_send(queue_ptr, source_ptr, TX_NO_WAIT);
    if(TX_QUEUE_FULL == status)
    {
        status = tx_queue_flush(queue_ptr);
        if (TX_SUCCESS == status)
        {
            status = tx_queue_front_send(queue_ptr, source_ptr, TX_NO_WAIT);
        }
        else
        {
            /* Do nothing */
        }
    }
    else if (TX_SUCCESS == status)
    {
        IotLogDebug("Send data to the queue successfully \r\n");
    }
    else
    {
        IotLogDebug("Send data to the queue unsuccessfully. Error code: %d \r\n", status);
    }
    return status;
}


