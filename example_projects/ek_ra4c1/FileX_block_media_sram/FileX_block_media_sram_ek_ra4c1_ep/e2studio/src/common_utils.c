/***********************************************************************************************************************
* File Name    : common_utils.c
* Description  : Contains macros, data structures, and functions commonly used in the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

#if (BSP_CFG_RTOS == 1U)
/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern TX_THREAD terminal_thread;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static uint8_t g_terminal_byte_pool_memory [TERMINAL_BYTE_POOL_SIZE];
static uint8_t g_terminal_output_queue_memory [TERMINAL_OUTPUT_QUEUE_SIZE];
static uint8_t g_terminal_input_queue_memory [TERMINAL_INPUT_QUEUE_SIZE];

static TX_BYTE_POOL g_terminal_byte_pool;
static TX_QUEUE g_terminal_output_queue;
static TX_QUEUE g_terminal_input_queue;

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Waits for the terminal thread to initialize and sleep before sending data to it.
 * @retval     TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t terminal_thread_init_check(void)
{
    uint32_t status = TX_SUCCESS;
    uint32_t run_count = RESET_VALUE;

    /* Get terminal thread run count */
    while (true)
    {
        status = tx_thread_info_get(&terminal_thread, TX_NULL, TX_NULL, &run_count,
                                     TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
        if(TX_SUCCESS != status)
        {
            return status;
        }

        if(RESET_VALUE < run_count)
        {
            return TX_SUCCESS;
        }

        tx_thread_sleep(1U);
    }
}

/*******************************************************************************************************************//**
 * @brief      Initializes the terminal framework by creating necessary queues and memory pools.
 * @retval     TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t terminal_framework_init(void)
{
    uint32_t status = TX_SUCCESS;

    /* Create an output queue for the terminal */
    status = tx_queue_create (&g_terminal_output_queue, (CHAR*) "Term Output Queue", 1,
                              &g_terminal_output_queue_memory, TERMINAL_OUTPUT_QUEUE_SIZE);
    APP_ERR_RET(TX_SUCCESS != status, status);

    /* Create an input queue for the terminal */
    status = tx_queue_create (&g_terminal_input_queue, (CHAR*) "Term Input Queue", 1,
                              &g_terminal_input_queue_memory, TERMINAL_INPUT_QUEUE_SIZE);
    APP_ERR_RET(TX_SUCCESS != status, status);

    /* Create a memory byte pool for the terminal */
    status = tx_byte_pool_create(&g_terminal_byte_pool, (CHAR*) "Term Byte Pool",
                                 (VOID *)g_terminal_byte_pool_memory, TERMINAL_BYTE_POOL_SIZE);
    APP_ERR_RET(TX_SUCCESS != status, status);

    return status;
}

/*******************************************************************************************************************//**
 * @brief      Receives and converts data from the input queue into a character buffer.
 *             To be called by the User/Application Thread.
 * @param[out] p_msg   Pointer to the buffer for received message.
 * @param[out] p_size  Pointer to store the size of the received message.
 * @param[in]  wait    Timeout value for queue reception.
 * @retval     TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t terminal_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait)
{
    terminal_msg_t * p_terminal_msg = NULL;

    /* Wait for an input queue */
    uint32_t status = tx_queue_receive(&g_terminal_input_queue, (VOID *)&p_terminal_msg , wait);
    APP_ERR_RET(TX_SUCCESS != status, status);

    if (NULL != p_terminal_msg)
    {
        memcpy(p_msg, p_terminal_msg->msg, p_terminal_msg->size);
        *p_size = p_terminal_msg->size;
    }

    /* Release the memory allocated for this terminal frame */
    tx_byte_release(p_terminal_msg);

    return status;
}

/*******************************************************************************************************************//**
 * @brief      Receives and converts data from the output queue.
 *             To be called by the User/Application Thread.
 * @param[out] pp_msg_st  Pointer to store the address of the received message structure.
 * @param[in]  wait       Timeout value for queue reception.
 * @retval     TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t terminal_get_output_queue(void ** pp_msg_st, uint32_t wait)
{
    /* Wait for an output queue */
    uint32_t status = tx_queue_receive(&g_terminal_output_queue, (VOID *)pp_msg_st , wait);
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Sends data to the terminal input queue.
 *             Allocates memory for the message and queues it for transmission.
 * @param[in]  p_data   Pointer to the data buffer to be sent.
 * @param[in]  size     Size of the data to be sent.
 * @retval     TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t terminal_send_input_queue(uint32_t id, void * const p_data, uint32_t size)
{
    uint32_t status = TX_SUCCESS;
    terminal_msg_t * p_terminal_msg = NULL;

    /* Allocate memory for terminal message structure and output message. */
    status = tx_byte_allocate(&g_terminal_byte_pool, (VOID **)&p_terminal_msg,
                              sizeof(terminal_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    APP_ERR_RET(TX_SUCCESS != status, status);

    /* Copy the message. */
    memcpy(p_terminal_msg->msg, p_data, size);

    /* Null-terminate the message. */
    p_terminal_msg->msg[size] = NULL_CHAR;

    /* Set additional information for the message. */
    p_terminal_msg->id = id;
    p_terminal_msg->time = tx_time_get();
    p_terminal_msg->size = size + sizeof(char);

    /* Send the terminal message structure pointer to the input queue. */
    status = tx_queue_send(&g_terminal_input_queue, (VOID *)&p_terminal_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_terminal_msg);
        return status;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     Sends data to the terminal output queue.
 *            Allocates memory for the message and queues it for transmission.
 * @param[in] id        Message ID.
 * @param[in] p_data    Pointer to the data buffer to be sent.
 * @param[in] size      Size of the data to be sent.
 * @retval    TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
uint32_t terminal_send_output_queue(uint32_t id, void * const p_data, uint32_t size)
{
    uint32_t status = TX_SUCCESS;
    terminal_msg_t * p_terminal_msg = NULL;

    /* Allocate memory for terminal message structure and output message. */
    status = tx_byte_allocate(&g_terminal_byte_pool, (VOID **)&p_terminal_msg,
                              sizeof(terminal_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    APP_ERR_RET(TX_SUCCESS != status, status);

    /* Copy the message. */
    memcpy(p_terminal_msg->msg, p_data, size);

    /* Null-terminate the message. */
    p_terminal_msg->msg[size] = NULL_CHAR;

    /* Set additional information for the message. */
    p_terminal_msg->id = id;
    p_terminal_msg->time = tx_time_get();
    p_terminal_msg->size = size + sizeof(char);

    /* Send the terminal message pointer to the output queue. */
    status = tx_queue_send(&g_terminal_output_queue, (VOID *)&p_terminal_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_terminal_msg);
        return status;
    }

    return status;
}
#endif
