/***********************************************************************************************************************
 * File Name    : ipc_squeue.c
 * Description  : This source file implements the functions declared in ipc_squeue.h
 *                It configures and operates the dual-core communication interface
 *                by combining IPC hardware mechanisms with FreeRTOS queue-based message handling.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ipc_squeue.h"
/* Declare resource for receiving queue*/
static QueueHandle_t g_queue_receive;
static StaticQueue_t g_queue_receive_memory;
static uint8_t g_queue_receive_queue_memory[4 * ITEM_NUM];

/* Send mutex resources declare*/
static SemaphoreHandle_t g_send_mutex = NULL;
static StaticSemaphore_t g_send_mutex_mem;

/* Receive mutex resources declare*/
static SemaphoreHandle_t g_receive_mutex = NULL;
static StaticSemaphore_t g_receive_mutex_mem;

static ipc_callback_args_t callback_mem;

static BackupData_t pend_data = {0};

volatile uint8_t g_flag_queue_full = RESET_FLAG;

#if _RA_CORE == CPU0

 volatile uint8_t g_flag_ipc_ready = RESET_FLAG;

 const bsp_ipc_semaphore_handle_t g_ipc_tx_ready =
 { .semaphore_num = 0 };
 const bsp_ipc_semaphore_handle_t g_ipc_rx_ready =
 { .semaphore_num = 1 };
#elif _RA_CORE == CPU1
 const bsp_ipc_semaphore_handle_t g_ipc_tx_ready =
 { .semaphore_num = 1 };
 const bsp_ipc_semaphore_handle_t g_ipc_rx_ready =
 { .semaphore_num = 0 };
#endif

const ipc_instance_t *ipc_receive_instance;

/* Static Function declarations */
static void wait_rqueue_processed(void);
static queue_status_t send_item(uint32_t data, TickType_t timeout);
static void ipc_callback(ipc_callback_args_t *p_args);
static void (*sfunc)(ipc_callback_args_t *p_args);

/*******************************************************************************************************************//**
 * @brief     IPC interrupt callback function.
 *
 * @param[in] p_args  Pointer to the IPC callback arguments structure.
 *
 * @return    None.
 **********************************************************************************************************************/
void ipc_callback(ipc_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch(p_args->event)
    {
        case IPC_EVENT_IRQ0:
            g_flag_queue_full ^= FULL_FLAG;
            break;
        case IPC_EVENT_IRQ1:
#if _RA_CORE == CPU0
            g_flag_ipc_ready = SET_FLAG;
            break;
#endif
        case IPC_EVENT_IRQ2:
        case IPC_EVENT_IRQ3:
        case IPC_EVENT_IRQ4:
        case IPC_EVENT_IRQ5:
        case IPC_EVENT_IRQ6:
        case IPC_EVENT_IRQ7:
        {
            /* Invoke the application's IPC callback for all remaining events */
            sfunc = p_args->p_context;
            sfunc(p_args);
            break;
        }
        case IPC_EVENT_MESSAGE_RECEIVED:
        {
            uint32_t eq_buf = p_args->message;
            BaseType_t result = xQueueSendFromISR(g_queue_receive, &eq_buf, &xHigherPriorityTaskWoken);
            if (errQUEUE_FULL == result)
            {
                if(RESET_FLAG == pend_data.is_data_pending)
                {
                    /* Notify to Send CPU that queue Full */
                    R_IPC_EventGenerate(ipc_receive_instance->p_ctrl, IPC_GENERATE_EVENT_IRQ0);
                    /* Push data to backup buffer
                     * Keep the semaphore until queue have space to receive */
                    pend_data.data = eq_buf;
                    pend_data.is_data_pending = SET_FLAG;
                }
            }
            /* Release the semaphore to process the next data */
            R_BSP_IpcSemaphoreGive(&g_ipc_rx_ready);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            break;
        }
        default:
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function to send each 4-byte to other CPU by use IPC-FIFO hardware.
 * @param[in] data : 4-byte data to be transmitted.
 * @param[in] timeout:  Maximum time to wait if the FIFO is full (in FreeRTOS ticks).
 * @return    queue_status_t  Status of the transmission
 **********************************************************************************************************************/
static queue_status_t send_item(uint32_t data, TickType_t timeout)
{
    /* Wait until the processing queue on the receiving CPU is ready */
    wait_rqueue_processed();

    if (g_flag_queue_full && TIMEOUT_NONE == timeout)
    {
        return QUEUE_FULL;
    }
    else if (g_flag_queue_full && TIMEOUT_FOREVER == timeout)
    {
        while(g_flag_queue_full)
        {
            __WFI();
        }
    }
    else
    {
        TickType_t start = xTaskGetTickCount();
        while (g_flag_queue_full)
        {
            if ((xTaskGetTickCount() - start) > timeout)
            {
                // Timeout!
                return QUEUE_FULL;
            }
        }
    }

    R_IPC_MessageSend(ipc_receive_instance->p_ctrl, data);
    return QUEUE_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     Sends a data buffer to the other CPU using IPC-FIFO hardware.
 * @param[in] pBuffer   Pointer to the data buffer to be transmitted.
 * @param[in] length    Length of the buffer in bytes (must be a multiple of 4).
 * @param[in] timeout   Maximum time to wait if the FIFO is full (in FreeRTOS ticks).
 * @retval int32_t      Number of bytes successfully transmitted.
 *                      Returns 0 if no data was sent.
 *                      Returns a negative error if error occurred.
 **********************************************************************************************************************/
int32_t send_to_queue(void * pBuffer, uint32_t length, TickType_t timeout)
{
    /* The buffer to send must be supplied */
    APP_ASSERT(NULL != pBuffer);
    APP_ASSERT(RESET_VALUE != length);

    uint32_t * buf = pBuffer;
    queue_status_t err = QUEUE_FULL;

    /* Calculate the number of item base on length of buffer and item size */
    uint32_t item_num = (length + ITEM_SIZE - 1)/ITEM_SIZE;

    /* Take the semaphore to ensure continuous and synchronized delivery of messages to the receiving core. */
    xSemaphoreTake(g_send_mutex, portMAX_DELAY);

    /* Send message length first */
    err = send_item(length,timeout);
    if (QUEUE_SUCCESS != err)
    {
        /* No data sent, returning 0 bytes */
        return 0;
    }

    /* loop entire buffer and send each 4byte data use IPC-FIFO 32bits */
    for (int32_t i = 0; i <  (int32_t) (item_num - 1U); i++)
    {
       err = send_item(buf[i],timeout);
       if (QUEUE_SUCCESS != err)
       {
           /* return sent bytes */
           return (i+1)*4;
       }
    }
    /* Send last item */
    if (item_num > 0)
    {
       uint32_t last_item = buf[item_num-1];
       int32_t remain_bytes = length % ITEM_SIZE;
       if (remain_bytes != 0 )
       {
           uint32_t mask = (1U << (remain_bytes * 8)) - 1;
           last_item &= mask;
       }

       err = send_item(last_item, timeout);
       if (QUEUE_SUCCESS != err)
       {
           /* return sent bytes */
           return ((int32_t) length - remain_bytes);
       }
    }
    xSemaphoreGive(g_send_mutex);

    /* return sent bytes */
    return (int32_t)length;
}

/*******************************************************************************************************************//**
 * @brief     Waits until all data in the IPC-FIFO queue has been processed by the receiving CPU.
 *
 * @param     None.
 * @return    None.
 **********************************************************************************************************************/
static void wait_rqueue_processed(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_BSP_IpcSemaphoreTake(&g_ipc_tx_ready);
    while (FSP_SUCCESS != err)
    {
        vTaskDelay(1);
        err = R_BSP_IpcSemaphoreTake(&g_ipc_tx_ready);
    }
}

/*******************************************************************************************************************//**
 * @brief     Receives data from the IPC-FIFO queue sent by the other CPU.
 *
 * @param[out] pBuffer   Pointer to the buffer where received data will be stored.
 * @param[in]  length    Maximum number of bytes to receive (must be a multiple of 4).
 * @param[in]  timeout   Maximum time to wait for data if the FIFO is empty (in FreeRTOS ticks).
 *
 * @retval int32_t       Number of bytes successfully received.
 *                       Returns 0 if no data was received.
 *                       Returns a negative error if error occurred.
 **********************************************************************************************************************/
int32_t receive_from_queue(void * pBuffer, uint32_t length, TickType_t  timeout)
{
    /* The buffer to send must be supplied */
    APP_ASSERT(NULL != pBuffer);
    APP_ASSERT(RESET_VALUE != length);

    /* Initialize variables */
    uint32_t mes_length = RESET_VALUE;
    uint32_t item_to_receive = RESET_VALUE;
    uint32_t * buf = pBuffer;
    BaseType_t result = pdFAIL;

    /* check the message length first */
    result  = xQueuePeek(g_queue_receive, &mes_length, timeout);
    if (pdPASS != result)
    {
        /* No data received, returning 0 bytes */
        return 0;
    }

    if (mes_length == 0 || mes_length > MAX_MESSAGE_LENGTH || length < mes_length)
    {
        /* No data received, returning 0 bytes */
        return 0;
    }

    /* Calculate the number of item base on message length of buffer and item size */
    item_to_receive = (mes_length + ITEM_SIZE - 1)/ITEM_SIZE;

    TickType_t start = xTaskGetTickCount();

    /* Wait until request length data enough in FreeRtos queue
     * or don't get any data if param timeout is TIMEOUT_NONE or specific timeout occurs*/
    while (uxQueueMessagesWaiting(g_queue_receive) - 1 < item_to_receive)
    {
        if (TIMEOUT_NONE == timeout)
        {
            /* No data received, returning 0 bytes */
            return 0;
        }
        else if (TIMEOUT_FOREVER == timeout)
        {
            /* Check if any data pending, push to queue */
            if (SET_FLAG == pend_data.is_data_pending && uxQueueSpacesAvailable(g_queue_receive))
            {
                /* Push pending data to queue */
                xQueueSend(g_queue_receive, &pend_data.data, timeout);
                pend_data.is_data_pending = RESET_FLAG;

                /* Notify to CPU send that ready to receive */
                R_IPC_EventGenerate(ipc_receive_instance->p_ctrl, IPC_GENERATE_EVENT_IRQ0);
            }
            vTaskDelay(1);
        }
        else
        {
            if ((xTaskGetTickCount() - start) > timeout)
            {
                /* Timeout!
                 * No data received, returning 0 bytes */
                return 0;
            }
        }
    }
    /* Get the message length out of queue */
    xQueueReceive(g_queue_receive, &mes_length, timeout);

    /* Take the semaphore to ensure that incoming messages
     * from the sending core are received without interruption and in correct order. */
    xSemaphoreTake(g_receive_mutex, portMAX_DELAY);

    /* Read entire message base on message length */
    for (uint32_t i = 0; i < item_to_receive; i++)
    {
        result = xQueueReceive(g_queue_receive, &buf[i], 0);
        if (pdPASS == result)
        {
            /* Check if any data pending, push to queue */
            if (SET_FLAG == pend_data.is_data_pending && uxQueueSpacesAvailable(g_queue_receive))
            {
                xQueueSend(g_queue_receive, &pend_data.data, 0);
                pend_data.is_data_pending = RESET_FLAG;
                R_IPC_EventGenerate(ipc_receive_instance->p_ctrl, IPC_GENERATE_EVENT_IRQ0);
            }
        }
    }
    xSemaphoreGive(g_receive_mutex);
    /* returning number bytes received */
    return (int32_t) mes_length;
}

/*******************************************************************************************************************//**
 * @brief     Initializes the IPC queue interface and FreeRtos resource for dual-core communication.
 *
 * @param[in]  ipc0  Pointer to the first IPC instance.
 * @param[in]  ipc1  Pointer to the second IPC instance.
 *
 * @return     fsp_err_t  Status of the initialization (e.g., success or error code).
 **********************************************************************************************************************/
fsp_err_t queue_ipc_init(const ipc_instance_t *ipc0, const ipc_instance_t *ipc1)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initialize IPC1 */
    err = R_IPC_Open(ipc0->p_ctrl, ipc0->p_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }
    /* Initialize IPC0 */
    err= R_IPC_Open(ipc1->p_ctrl, ipc1->p_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }

    /* Create receive queue  */
    g_queue_receive =xQueueCreateStatic (ITEM_NUM, 4, &g_queue_receive_queue_memory[0], &g_queue_receive_memory);
    if(NULL == g_queue_receive)
    {
        __BKPT(0);
    }

    /* Create Writer Mutex */
    g_send_mutex = xSemaphoreCreateMutexStatic (&g_send_mutex_mem);
    if (NULL == g_send_mutex)
    {
        __BKPT(0);
    }
    /* Create Receiver Mutex */
    g_receive_mutex = xSemaphoreCreateMutexStatic(&g_receive_mutex_mem);
#if _RA_CORE == CPU0
    /* Set the IPC callback to the middleware-level ipc_callback.
     * Events not handled by the middleware will be forwarded to the application's callback for processing. */
    err = g_ipc0.p_api->callbackSet(ipc0->p_ctrl, ipc_callback, ipc0->p_cfg->p_callback, &callback_mem);
    ipc_receive_instance = ipc1;
#elif _RA_CORE == CPU1
    err = g_ipc1.p_api->callbackSet(ipc1->p_ctrl, ipc_callback, ipc1->p_cfg->p_callback, &callback_mem);
    ipc_receive_instance = ipc0;
#endif
    if (FSP_SUCCESS != err)
    {
        __BKPT(0);
    }
#if _RA_CORE == CPU0
    /* Start second core */
    R_BSP_SecondaryCoreStart();

    /* Wait until CPU1 finishes initializing IPC */
    while(!g_flag_ipc_ready)
    {
        __WFI();
    }
#elif _RA_CORE == CPU1
    R_IPC_EventGenerate(ipc0->p_ctrl, IPC_GENERATE_EVENT_IRQ1);
#endif
    return FSP_SUCCESS;
}

