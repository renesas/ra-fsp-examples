/***********************************************************************************************************************
 * File Name    : terminal.c
 * Description  : Contains data structures and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "terminal.h"

#if (BSP_CFG_RTOS == AZURE_RTOS)
/* Global variables */
TX_QUEUE g_terminal_op_data_queue;
TX_QUEUE g_terminal_ip_data_queue;
TX_EVENT_FLAGS_GROUP g_terminal_event;
TX_BYTE_POOL g_byte_pool;
#endif /* BSP_CFG_RTOS */

/*******************************************************************************************************************//**
 * @brief     This function initializes the terminal framework resources.
 * @param[in] None.
 * @retval    TX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT terminal_framework_init(VOID)
{
    UINT err = TX_SUCCESS;
#if (BSP_CFG_RTOS == AZURE_RTOS)
    /* Must be static: tx_byte_pool_create()/tx_queue_create() register these addresses as the permanent
     * backing storage for g_byte_pool and the two queues, which are used for the life of the program - if
     * these were ordinary locals, that storage would be reclaimed the moment this function returns, and
     * every later allocate/release/send/receive call would be reading and writing stale stack memory. */
    static CHAR byte_memory[BYTE_POOL_SIZE] = {NULL_CHAR};
    static uint8_t queue_memory_op_terminal_obj[20] = {NULL_CHAR};
    static uint8_t queue_memory_ip_terminal_obj[20] = {NULL_CHAR};

    err = tx_byte_pool_create(&g_byte_pool, (CHAR *)BYTE_POOL_NAME, (VOID *)&byte_memory[0], BYTE_POOL_SIZE);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    err = tx_queue_create(&g_terminal_op_data_queue, (CHAR *)TERMINAL_QUEUE, 1U,
                          &queue_memory_op_terminal_obj, sizeof(queue_memory_op_terminal_obj));
    if (TX_SUCCESS != err)
    {
        tx_byte_pool_delete(&g_byte_pool);
        return err;
    }

    err = tx_queue_create(&g_terminal_ip_data_queue, (CHAR *)NETX_WIFI_QUEUE, 1U,
                          &queue_memory_ip_terminal_obj, sizeof(queue_memory_ip_terminal_obj));
    if (TX_SUCCESS != err)
    {
        tx_byte_pool_delete(&g_byte_pool);
        tx_queue_delete(&g_terminal_op_data_queue);
    }
#endif /* BSP_CFG_RTOS */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function prints message to terminal.
 * @param[in] id                        Message id.
 * @param[in] size                      Message size.
 * @param[in] p_data                    Message.
 * @retval    TX_SUCCESS                Printed successfully.
 * @retval    Any other error codes     Failed to print.
 **********************************************************************************************************************/
UINT app_terminal_print_data(event_id_t id, uint32_t size, void * const p_data)
{
    UINT err = TX_SUCCESS;
#if (BSP_CFG_RTOS == AZURE_RTOS)
    terminal_msg_t *p_display_data = NULL;

    /* Allocate memory for terminal display message data structure */
    err = memory_allocate_terminal(&g_byte_pool, &p_display_data, sizeof(terminal_msg_t) + size);
    if (TX_SUCCESS != err)
    {
        terminal_thread_service_cleanup();
        return err;
    }

    p_display_data->msg_id = id;
    p_display_data->msg_time = tx_time_get();
    p_display_data->msg_data_size = size;
    memcpy(p_display_data->p_msg, p_data, size);

    /* Send allocated address */
    err = tx_queue_send(&g_terminal_op_data_queue, (terminal_msg_t *)&p_display_data, TX_WAIT_FOREVER);
#endif /* BSP_CFG_RTOS */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function allocates memory for message transfer through queue.
 * @param[in] pool                      Pointer to byte memory pool.
 * @param[in] p_buf                     Pointer to the user display data.
 * @param[in] size                      Total size.
 * @retval    TX_SUCCESS                Allocated successfully.
 * @retval    Any other error codes     Failed to allocated.
 **********************************************************************************************************************/
UINT memory_allocate_terminal(TX_BYTE_POOL * pool, terminal_msg_t ** p_buf, uint32_t size)
{
    UINT err = TX_SUCCESS;
#if (BSP_CFG_RTOS == AZURE_RTOS)

    terminal_msg_t * p_buffer = NULL;

    /* Block allocate for user input */
    err = tx_byte_allocate(pool, (VOID **)&p_buffer, size, TX_WAIT_FOREVER);

    if (TX_SUCCESS == err)
    {
        /* Assign buffer address */
        *p_buf = p_buffer;
    }
#endif /* BSP_CFG_RTOS */

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads and releases data to terminal.
 * @param[out] p_get_data               Pointer to get data buffer
 * @retval     TX_SUCCESS               Read and released successfully.
 * @retval     Any other error codes    Failed to read and release.
 **********************************************************************************************************************/
UINT check_for_terminal_user_IP(char * const p_get_data)
{
    UINT err = TX_SUCCESS;

#if (BSP_CFG_RTOS == AZURE_RTOS)

    terminal_msg_t * p_r_data1 = NULL;

    err = tx_queue_receive(&g_terminal_ip_data_queue, (VOID *)&p_r_data1 , 0xFF);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    if ((NULL != p_r_data1) && (TERMINAL_INPUT_MESSAGE_TYPE_STR_QUEUED == p_r_data1->msg_id))
    {
         /* Copy data over here */
        memcpy(p_get_data, (CHAR *)p_r_data1->p_msg, p_r_data1->msg_data_size);
    }

     /* Release print message allocated block */
    err = tx_byte_release(p_r_data1);

#endif /* BSP_CFG_RTOS */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the Terminal thread to initialize and sleep before sending data to it.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
VOID terminal_thread_init_check(VOID)
{
    UINT err = TX_SUCCESS;
#if (BSP_CFG_RTOS == AZURE_RTOS)

    thread_info_t terminal_thread_info =
    {
     .thread_ptr = &terminal_thread,
    };

    do
    {
        err = tx_thread_info_get(terminal_thread_info.thread_ptr, TX_NULL, &terminal_thread_info.state,
                                 &terminal_thread_info.run_count, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

        if (1U <= terminal_thread_info.run_count)
        {
            break;
        }
        else
        {
            /* 1-tick sleep is needed to signal to higher priority threads */
            tx_thread_sleep(1);
        }
    } while (TX_SUCCESS == err);
#endif /* BSP_CFG_RTOS */
}

/*******************************************************************************************************************//**
 * @brief     This function cleans up terminal service resources.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
VOID terminal_thread_service_cleanup(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    tx_byte_pool_delete(&g_byte_pool);

    /* If any user specific queue has been added, it should be clean up as per specific EP */
    tx_queue_delete(&g_terminal_op_data_queue);
    tx_queue_delete(&g_terminal_ip_data_queue);

    /* Halt */
    __BKPT(0);
#endif /* BSP_CFG_RTOS */
}
