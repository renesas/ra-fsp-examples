/***********************************************************************************************************************
* File Name    : common_utils.c
* Description  : Contains macros, data structures, and functions commonly used in the EP.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

#if (BSP_CFG_RTOS != 0U)
/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
#if (BSP_CFG_RTOS == 1U)
#include "tx_api.h"
static TX_BYTE_POOL g_term_byte_pool;
static TX_QUEUE g_term_input_queue;
static TX_QUEUE g_term_output_queue;
static uint8_t g_term_byte_pool_memory[TERM_BYTE_POOL_SIZE];
#elif (BSP_CFG_RTOS == 2U)
static QueueHandle_t g_term_input_queue;
static QueueHandle_t g_term_output_queue;
#endif

static volatile bool g_term_init_flag = false;

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Waits until the terminal framework is initialized before sending data to it.
 * @retval     None.
 **********************************************************************************************************************/
void term_framework_init_check(void)
{
    /* Check the initialization flag of the terminal framework */
    while (true != g_term_init_flag)
    {
#if (BSP_CFG_RTOS == 1U)
        tx_thread_sleep(1U);
#elif (BSP_CFG_RTOS == 2U)
        vTaskDelay(1);
#endif
    }
}

/*******************************************************************************************************************//**
 * @brief      Initializes the terminal framework by creating necessary queues and memory pools.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t term_framework_init(void)
{
#if (BSP_CFG_RTOS == 1U)
    CHAR * p_mem = TX_NULL;

    /* Create a memory byte pool for the terminal */
    uint32_t status = tx_byte_pool_create(&g_term_byte_pool, (CHAR*) "Term Byte Pool",
                                          (VOID *)g_term_byte_pool_memory, TERM_BYTE_POOL_SIZE);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Create an output queue for the terminal */
    status = tx_byte_allocate(&g_term_byte_pool, (VOID **) &p_mem, TERM_OUTPUT_QUEUE_SIZE, TX_NO_WAIT);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    status = tx_queue_create(&g_term_output_queue, (CHAR*) "Term Output Queue", 1,
                             p_mem, TERM_OUTPUT_QUEUE_SIZE);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Create an input queue for the terminal */
    status = tx_byte_allocate(&g_term_byte_pool, (VOID **) &p_mem, TERM_INPUT_QUEUE_SIZE, TX_NO_WAIT);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    status = tx_queue_create(&g_term_input_queue, (CHAR*) "Term Input Queue", 1,
                             p_mem, TERM_INPUT_QUEUE_SIZE);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    /* Create an output queue for the terminal */
    g_term_output_queue = xQueueCreate(TERM_OUTPUT_QUEUE_SIZE/sizeof(void *), sizeof(void *));
    if (NULL == g_term_output_queue)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Create an input queue for the terminal */
    g_term_input_queue = xQueueCreate(TERM_INPUT_QUEUE_SIZE/sizeof(void *), sizeof(void *));
    if (NULL == g_term_input_queue)
    {
        return FSP_ERR_INVALID_POINTER;
    }
#endif

    /* Set the initialization flag to notify that the terminal framework has been initialized */
    g_term_init_flag = true;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      Receives and converts data from the input queue into a character buffer.
 *             To be called by the User/Application Thread.
 * @param[out] p_msg   Pointer to the buffer for received message.
 * @param[out] p_size  Pointer to store the size of the received message.
 * @param[in]  wait    Timeout value for queue reception.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t term_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait)
{
    term_msg_t * p_term_msg = NULL;

    /* Wait for an input queue */
#if (BSP_CFG_RTOS == 1U)
    uint32_t status = tx_queue_receive(&g_term_input_queue, (VOID *)&p_term_msg, wait);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueReceive(g_term_input_queue, &p_term_msg, wait);
    if ((uint32_t)pdTRUE != status)
    {
        return FSP_ERR_ASSERTION;
    }
#endif

    if (NULL != p_term_msg)
    {
        memcpy(p_msg, p_term_msg->msg, p_term_msg->size);
        *p_size = p_term_msg->size;
    }

    /* Release the memory allocated for this terminal frame */
#if (BSP_CFG_RTOS == 1U)
    tx_byte_release(p_term_msg);
#elif (BSP_CFG_RTOS == 2U)
    vPortFree(p_term_msg);
#endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      Receives and converts data from the output queue.
 *             To be called by the User/Application Thread.
 * @param[out] pp_msg_st  Pointer to store the address of the received message structure.
 * @param[in]  wait       Timeout value for queue reception.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t term_get_output_queue(void ** pp_msg_st, uint32_t wait)
{
    /* Wait for an output queue */
#if (BSP_CFG_RTOS == 1U)
    uint32_t status = tx_queue_receive(&g_term_output_queue, (VOID *)pp_msg_st, wait);
    if (TX_SUCCESS != status)
    {
        return status;
    }
#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueReceive(g_term_output_queue, (void *)pp_msg_st, wait);
    if ((uint32_t)pdTRUE != status)
    {
        return FSP_ERR_ASSERTION;
    }
#endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      Sends data to the terminal input queue.
 *             Allocates memory for the message and queues it for transmission.
 * @param[in]  id       Message ID.
 * @param[in]  p_data   Pointer to the data buffer to be sent.
 * @param[in]  size     Size of the data to be sent.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t term_send_input_queue(uint32_t id, void * const p_data, uint32_t size)
{
    term_msg_t * p_term_msg = NULL;

    /* Allocate memory for terminal message structure and output message */
#if (BSP_CFG_RTOS == 1U)
    uint32_t status = tx_byte_allocate(&g_term_byte_pool, (VOID **)&p_term_msg,
                                       sizeof(term_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    p_term_msg = (term_msg_t*) pvPortMalloc(sizeof(term_msg_t) + size + sizeof(char));
    if (NULL == p_term_msg)
    {
        return FSP_ERR_INVALID_POINTER;
    }
#endif

    /* Copy the message */
    memcpy(p_term_msg->msg, p_data, size);

    /* Null-terminate the message */
    p_term_msg->msg[size] = NULL_CHAR;

    /* Set additional information for the message */
    p_term_msg->id = id;
#if (BSP_CFG_RTOS == 1U)
    p_term_msg->time = tx_time_get();
#elif (BSP_CFG_RTOS == 2U)
    p_term_msg->time = xTaskGetTickCount();
#endif
    p_term_msg->size = size + sizeof(char);

    /* Send the terminal message structure pointer to the input queue */
#if (BSP_CFG_RTOS == 1U)
    status = tx_queue_send(&g_term_input_queue, (VOID *)&p_term_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_term_msg);
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueSend(g_term_input_queue, (void *)&p_term_msg, portMAX_DELAY);
    if (pdTRUE != status)
    {
        vPortFree(p_term_msg);
        return FSP_ERR_ASSERTION;
    }
#endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     Sends data to the terminal output queue.
 *            Allocates memory for the message and queues it for transmission.
 * @param[in] id        Message ID.
 * @param[in] p_data    Pointer to the data buffer to be sent.
 * @param[in] size      Size of the data to be sent.
 * @retval    FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t term_send_output_queue(uint32_t id, void * const p_data, uint32_t size)
{
    term_msg_t * p_term_msg = NULL;

    /* Allocate memory for terminal message structure and output message */
#if (BSP_CFG_RTOS == 1U)
    uint32_t status = tx_byte_allocate(&g_term_byte_pool, (VOID **)&p_term_msg,
                                       sizeof(term_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    p_term_msg = (term_msg_t*) pvPortMalloc(sizeof(term_msg_t) + size + sizeof(char));
    if (NULL == p_term_msg)
    {
        return FSP_ERR_INVALID_POINTER;
    }
#endif

    /* Copy the message into the terminal message structure */
    memcpy(p_term_msg->msg, p_data, size);

    /* Add a null character to terminate the terminal message buffer */
    p_term_msg->msg[size] = NULL_CHAR;

    /* Set additional information for the message */
    p_term_msg->id = id;
#if (BSP_CFG_RTOS == 1U)
    p_term_msg->time = tx_time_get();
#elif (BSP_CFG_RTOS == 2U)
    p_term_msg->time = xTaskGetTickCount();
#endif
    p_term_msg->size = size + sizeof(char);

    /* Send the terminal message pointer to the output queue */
#if (BSP_CFG_RTOS == 1U)
    status = tx_queue_send(&g_term_output_queue, (VOID *)&p_term_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_term_msg);
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueSend(g_term_output_queue, (void *)&p_term_msg, portMAX_DELAY);
    if (pdTRUE != status)
    {
        vPortFree(p_term_msg);
        return FSP_ERR_ASSERTION;
    }
#endif

    return FSP_SUCCESS;
}

#endif  /* BSP_CFG_RTOS != 0U */
