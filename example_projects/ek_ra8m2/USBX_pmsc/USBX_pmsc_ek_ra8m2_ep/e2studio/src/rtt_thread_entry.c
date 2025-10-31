/***********************************************************************************************************************
 * File Name    : rtt_thread_entry.c
 * Description  : Contains entry function of RTT Thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "rtt_thread.h"
#include "usbx_pmsc_ep.h"
#include "common_utils.h"

/* Local functions */
static void process_rtt_op_msg(VOID);
static UINT memory_allocate_rtt(TX_BYTE_POOL *pool, rtt_msg_t **p_buf, uint32_t size);

extern TX_THREAD rtt_thread;

/* RTT_Thread entry function */
void rtt_thread_entry(void)
{
    UINT err = TX_SUCCESS;
    CHAR rtt_buffer[IP_MSG_MAX_SIZE]         = {'\0'};
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {'\0'};

    /* Allocate memory block for address-based message exchange */
    rtt_msg_t *p_data = NULL;

    /* Print the current time */
    snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n", tx_time_get());

    APP_DBG_PRINT(timestamp_info);

    /* Initialize RTT framework */
    err = rtt_framework_init();
    if (TX_SUCCESS != err)
    {
        APP_PRINT("\r\nERROR: rtt_framework_init failed\r\n");
        APP_ERR_TRAP(err);
    }

    while (1)
    {
        /* Check for user input */
        if (APP_CHECK_DATA)
        {
            UINT read_bytes = APP_READ(rtt_buffer);

            err = memory_allocate_rtt(&g_byte_pool, &p_data, sizeof(rtt_msg_t) + read_bytes);
            if (TX_SUCCESS != err)
            {
                APP_PRINT("Error in processing, please check again\r\n");
            }
            else
            {
                /* Send data to the user thread */
                p_data->msg_id = RTT_INPUT_MESSAGE_TYPE_STR_QUEUED;
                p_data->msg_data_size = strlen(rtt_buffer)+1;
                memcpy(p_data->p_msg, rtt_buffer, p_data->msg_data_size);
#if (BSP_CFG_RTOS == AZURE_RTOS)
                tx_queue_send(&g_rtt_ip_data_queue, (rtt_msg_t *)&p_data, TX_WAIT_FOREVER);
#endif

                memset(rtt_buffer, '\0', sizeof(rtt_buffer));
            }
        }

        /* Print messages from the user thread to RTT output using event-based checking */
        process_rtt_op_msg();
#if (BSP_CFG_RTOS == AZURE_RTOS)
        /* Sleep briefly to yield CPU to higher priority threads */
        tx_thread_sleep(1);
#endif
    }
}

/*******************************************************************************************************************//**
 * @brief     Receives messages from the user thread via tx_queue_receive and displays them on the RTT Viewer.
 *            Also releases the memory blocks allocated for the messages after processing.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void process_rtt_op_msg(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    rtt_msg_t *rtt_op_data = NULL;
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {'\0'};

    /* Process and print all received messages */
    do
    {
        err = tx_queue_receive(&g_rtt_op_data_queue, (VOID *)&rtt_op_data, 1);

        if (err == TX_SUCCESS)
        {
            /* Print the timestamp from the message */
            snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n",\
                     (ULONG) rtt_op_data->msg_time);

            APP_DBG_PRINT(timestamp_info);

            /* Handle the received message */
            switch (rtt_op_data->msg_id)
            {
                case RTT_OUTPUT_MESSAGE_BANNER:
                {
                    /* Get FLEX pack version info and print banner */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);
                    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
                              version.version_id_b.patch );
                    APP_PRINT(EP_INFO);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_INFO_STR:
                {
                    APP_PRINT("\r\nINFO: %s", rtt_op_data->p_msg);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_ERR_STR:
                {
                    APP_PRINT("\r\nERROR: %s", rtt_op_data->p_msg);
                }
                break;

                case RTT_OUTPUT_MESSAGE_APP_ERR_TRAP:
                {
                    /* Extract error code and halt the application */
                    UINT err_val = *(UINT*)rtt_op_data->p_msg;
                    APP_ERR_TRAP(err_val);
                }
                break;

                default:
                {
                    APP_PRINT("\r\nRTT received an unknown message ID");
                }
                break;
            }

            /* Send a newline and return carriage requesting flushing the buffer */
            APP_PRINT("\r\n");

            /* Release the memory block allocated for the message */
            err= tx_byte_release((void *) rtt_op_data);
        }
    }while (err != TX_QUEUE_EMPTY);
#endif
    return;
}

/*******************************************************************************************************************//**
 * @brief     Called by user/application threads to print messages to the RTT Viewer.
 * @param[IN] id        Message ID indicating the type of message.
 * @param[IN] size      Size of the message data in bytes.
 * @param[IN] p_data    Pointer to the message data to be sent.
 * @retval    Any other error code apart from TX_SUCCESS indicates an unsuccessful operation.
 **********************************************************************************************************************/
UINT app_rtt_print_data(event_id_t id, uint32_t size, void * const p_data)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    rtt_msg_t *p_display_data = NULL;
    UINT err = TX_SUCCESS;

    /* Allocate memory for the RTT message (structure + payload) */
    err = memory_allocate_rtt(&g_byte_pool, &p_display_data, sizeof(rtt_msg_t) + size);
    if (TX_SUCCESS != err)
    {
        APP_PRINT("\r\nError: memory_allocate_rtt failed\r\n");
        rtt_thread_service_cleanup();
    }

    p_display_data->msg_id = id;
    p_display_data->msg_time = tx_time_get();
    p_display_data->msg_data_size = size;
    memcpy(p_display_data->p_msg, p_data, size);

    /* Send the allocated message to the RTT output queue */
    err = tx_queue_send(&g_rtt_op_data_queue, (rtt_msg_t *)&p_display_data, TX_WAIT_FOREVER);
    return err;
#endif
}

/*******************************************************************************************************************//**
 * @brief     Allocates memory for message transfer via queue.
 * @param[IN] pool        Pointer to byte memory pool.
 * @param[IN] p_buf       Pointer to the pointer that will receive the allocated buffer address.
 * @param[IN] size        Total size of memory to allocate in bytes.
 * @retval    Any other error code apart from TX_SUCCESS indicates an unsuccessful operation.
 **********************************************************************************************************************/
static UINT memory_allocate_rtt(TX_BYTE_POOL *pool, rtt_msg_t **p_buf, uint32_t size)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    rtt_msg_t *buf = NULL;

    /* Allocate memory block from the byte pool */
    err = tx_byte_allocate(pool, (VOID **)&buf, size, TX_WAIT_FOREVER);

    if (TX_SUCCESS == err)
    {
        /* Assign buffer address */
        *p_buf = buf;
    }
    return err;
#endif
}

/*******************************************************************************************************************//**
 * @brief      Reads user input from the RTT input queue. Upon receiving a message (from either RTT sender or
 *             receiver queue), the allocated message block is released.
 *             Note:
 *               - The sender queue for the RTT thread is user input from the RTT Viewer.
 *               - The receiver queue is used when the user wants to print/display any message to the RTT Viewer.
 *
 * @param[OUT] p_get_data        Pointer to the buffer where received data will be copied.
 * @retval     Any other error code apart from TX_SUCCESS indicates an unsuccessful operation.
 **********************************************************************************************************************/
UINT check_for_rtt_user_ip(uint8_t * p_get_data)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    rtt_msg_t *r_data1 = NULL;

    err = tx_queue_receive(&g_rtt_ip_data_queue, (VOID *)&r_data1, 0xFF);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    if ((NULL != r_data1) && (RTT_INPUT_MESSAGE_TYPE_STR_QUEUED == r_data1->msg_id))
    {
        /* Conversion from input string to integer value */
        *p_get_data = (uint8_t)(atoi((char *)r_data1->p_msg));
    }

    /* Release the memory block allocated for the message */
    err= tx_byte_release(r_data1);

    return err;
#endif
}

/*******************************************************************************************************************//**
 * @brief      Waits for the RTT thread to initialize and enter sleep state before sending data to it.
 *             This function must be called from the user thread.
 * @param[IN]  None
 * @retval     None
 **********************************************************************************************************************/
VOID rtt_thread_init_check(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    thread_info_t rtt_thread_info =
    {
     .thread_ptr = &rtt_thread,
    };

    do
    {
        err = tx_thread_info_get(rtt_thread_info.thread_ptr,
                                 TX_NULL,
                                 &rtt_thread_info.state,
                                 &rtt_thread_info.run_count,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL);

        if (rtt_thread_info.run_count >= 1)
        {
            break;
        }
        else
        {
            /* Sleep briefly to yield CPU to higher priority threads */
            tx_thread_sleep(1);
        }
    } while (err == TX_SUCCESS);
#endif
}
