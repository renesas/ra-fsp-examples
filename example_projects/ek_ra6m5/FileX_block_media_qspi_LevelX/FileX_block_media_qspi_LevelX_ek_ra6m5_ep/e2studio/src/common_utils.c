/***********************************************************************************************************************
 * File Name    : common_utils.c
 * Description  : Contains macros data structures and functions used in common_utils.c for RTT rtos aware framework
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

#include "common_utils.h"

#if (BSP_CFG_RTOS == AZURE_RTOS)
/*
 * global variables
 */
TX_QUEUE g_rtt_op_data_queue;
TX_QUEUE g_rtt_ip_data_queue;
TX_EVENT_FLAGS_GROUP g_rtt_event;
TX_BYTE_POOL   g_byte_pool;

static CHAR *byte_pool_name = "rtt_processing_block";
static CHAR byte_memory[BYTE_POOL_SIZE] = {'\0'};
static uint8_t queue_memory_op_rtt_obj[20] = {'\0'};
static uint8_t queue_memory_ip_rtt_obj[20] = {'\0'};
#endif

UINT rtt_framework_init(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)

    UINT err= TX_SUCCESS;

    err = tx_byte_pool_create(&g_byte_pool,byte_pool_name,(VOID *)&byte_memory[0],BYTE_POOL_SIZE);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    err = tx_queue_create(&g_rtt_op_data_queue, (CHAR *)"rtt_op_queue", 1, &queue_memory_op_rtt_obj, sizeof(queue_memory_op_rtt_obj));
    if (TX_SUCCESS !=err)
    {
        tx_byte_pool_delete(&g_byte_pool);
        return err;
    }

    err = tx_queue_create(&g_rtt_ip_data_queue, (CHAR *)"rtt_input_queue", 1, &queue_memory_ip_rtt_obj, sizeof(queue_memory_ip_rtt_obj));
    if (TX_SUCCESS !=err)
    {
        tx_byte_pool_delete(&g_byte_pool);
        tx_queue_delete(&g_rtt_op_data_queue);
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
    tx_byte_pool_delete(&g_byte_pool);

    /* if any user specific queue has been added, it should be clean up as per specific EP */
    tx_queue_delete(&g_rtt_op_data_queue);
    tx_queue_delete(&g_rtt_ip_data_queue);

    /* halt */
    APP_ERR_TRAP(true);
#endif
}


