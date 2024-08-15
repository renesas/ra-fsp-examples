/***********************************************************************************************************************
 * File Name    : common_utils.c
 * Description  : Contains macros data structures and functions used in common_utils.c for RTT rtos aware framework
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

#if (BSP_CFG_RTOS == AZURE_RTOS)
/*
 * global variables
 */
TX_QUEUE rtt_op_data_queue;
TX_QUEUE rtt_ip_data_queue;
TX_EVENT_FLAGS_GROUP rtt_event;
TX_BYTE_POOL   byte_pool;

static CHAR *byte_pool_name = "rtt_processing_block";
static CHAR byte_memory[BYTE_POOL_SIZE] = {'\0'};
static uint8_t queue_memory_op_rtt_obj[20] = {'\0'};
static uint8_t queue_memory_ip_rtt_obj[20] = {'\0'};
#endif

UINT rtt_framework_init(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)

    UINT err= TX_SUCCESS;

    err = tx_byte_pool_create(&byte_pool,byte_pool_name,(VOID *)&byte_memory[0],BYTE_POOL_SIZE);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    err = tx_queue_create(&rtt_op_data_queue, (CHAR *)"rtt_op_queue", 1, &queue_memory_op_rtt_obj, sizeof(queue_memory_op_rtt_obj));
    if (TX_SUCCESS !=err)
    {
        tx_byte_pool_delete(&byte_pool);
        return err;
    }

    err = tx_queue_create(&rtt_ip_data_queue, (CHAR *)"ftp_client_thread_input_queue", 1, &queue_memory_ip_rtt_obj, sizeof(queue_memory_ip_rtt_obj));
    if (TX_SUCCESS !=err)
    {
        tx_byte_pool_delete(&byte_pool);
        tx_queue_delete(&rtt_op_data_queue);
    }

    return err;
#endif
}

/*
 * Can be called any where after rtt_framework_init() function call
 * for graceful clean up
 */
void rtt_thread_service_cleanup(void)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    tx_byte_pool_delete(&byte_pool);

    /* if any user specific queue has been added, it should be clean up as per specific EP */
    tx_queue_delete(&rtt_op_data_queue);
    tx_queue_delete(&rtt_ip_data_queue);

    /* halt */
    APP_ERR_TRAP(true);
#endif
}


