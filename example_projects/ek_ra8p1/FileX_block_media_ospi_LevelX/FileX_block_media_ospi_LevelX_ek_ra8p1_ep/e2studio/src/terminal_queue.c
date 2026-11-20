/***********************************************************************************************************************
* File Name    : terminal_queue.c
* Description  : Contains the byte-pool and queue transport functions used to exchange messages between the
*                application, console, and terminal threads.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdbool.h>
#include <string.h>
#include "hal_data.h"
#include "terminal_queue.h"

#if (BSP_CFG_RTOS != 0U)

/* Private variables */
#if (BSP_CFG_RTOS == 1U)
#include "tx_api.h"
  static TX_BYTE_POOL s_term_byte_pool;
  static TX_QUEUE s_term_input_queue;
  static TX_QUEUE s_term_output_queue;
  static uint8_t s_term_byte_pool_memory[TERM_BYTE_POOL_SIZE];
#elif (BSP_CFG_RTOS == 2U)
  static QueueHandle_t s_term_input_queue;
  static QueueHandle_t s_term_output_queue;
#endif

static volatile bool s_term_init_flag = false;

/***********************************************************************************************************************
 * Function Name: term_framework_init_check
 * Description  : This function waits until the terminal framework is initialized before sending data to it.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void term_framework_init_check(void)
{
    /* Check the initialization flag of the terminal framework */
    while (true != s_term_init_flag)
    {
#if (BSP_CFG_RTOS == 1U)
        thread_sleep(1U);
#elif (BSP_CFG_RTOS == 2U)
        vTaskDelay(1);
#endif
    }
}
/***********************************************************************************************************************
* End of function term_framework_init_check
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: term_framework_init
 * Description  : This function initializes the terminal framework by creating the necessary queues and byte pool.
 * Arguments    : None.
 * Return Value : TX_SUCCESS                Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT term_framework_init(void)
{
    UINT status = TX_SUCCESS;

#if (BSP_CFG_RTOS == 1U)
    CHAR * p_mem = TX_NULL;

    /* Create a memory byte pool for the terminal */
    status = tx_byte_pool_create(&s_term_byte_pool, (CHAR*) "Term Byte Pool", (VOID *)s_term_byte_pool_memory,
                                          TERM_BYTE_POOL_SIZE);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Create an output queue for the terminal */
    status = tx_byte_allocate(&s_term_byte_pool, (VOID **) &p_mem, TERM_OUTPUT_QUEUE_SIZE, TX_NO_WAIT);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    status = tx_queue_create(&s_term_output_queue, (CHAR*) "Term Output Queue", 1, p_mem, TERM_OUTPUT_QUEUE_SIZE);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Create an input queue for the terminal */
    status = tx_byte_allocate(&s_term_byte_pool, (VOID **) &p_mem, TERM_INPUT_QUEUE_SIZE, TX_NO_WAIT);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    status = tx_queue_create(&s_term_input_queue, (CHAR*) "Term Input Queue", 1, p_mem, TERM_INPUT_QUEUE_SIZE);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    /* Create an output queue for the terminal */
    s_term_output_queue = xQueueCreate(TERM_OUTPUT_QUEUE_SIZE/sizeof(void *), sizeof(void *));
    if (NULL == s_term_output_queue)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Create an input queue for the terminal */
    s_term_input_queue = xQueueCreate(TERM_INPUT_QUEUE_SIZE/sizeof(void *), sizeof(void *));
    if (NULL == s_term_input_queue)
    {
        return FSP_ERR_INVALID_POINTER;
    }
#endif

    /* Set the initialization flag to notify that the terminal framework has been initialized */
    s_term_init_flag = true;

    return status;
}
/***********************************************************************************************************************
* End of function term_framework_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: term_get_input_queue
 * Description  : This function receives and converts data from the input queue into a character buffer.
 *                To be called by the User/Application Thread.
 * Arguments    : p_msg                 Pointer to the buffer for received message.
 *                p_size                Pointer to store the size of the received message.
 *                wait                  Timeout value for queue reception.
 * Return Value : TX_SUCCESS            Received successfully.
 *                Any other error codes Failed to receive.
 **********************************************************************************************************************/
UINT term_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait)
{
    UINT status = TX_SUCCESS;
    terminal_msg_t * p_term_msg = NULL;

    /* Wait for an input queue */
#if (BSP_CFG_RTOS == 1U)
    status = tx_queue_receive(&s_term_input_queue, (VOID *)&p_term_msg, wait);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueReceive(s_term_input_queue, &p_term_msg, wait);
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

    return status;
}
/***********************************************************************************************************************
* End of function term_get_input_queue
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: term_get_output_queue
 * Description  : This function receives and converts data from the output queue.
 *                To be called by the User/Application Thread.
 * Arguments    : pp_msg_st             Pointer to store the address of the received message structure.
 *                wait                  Timeout value for queue reception.
 * Return Value : TX_SUCCESS            Received successfully.
 *                Any other error codes Failed to receive.
 **********************************************************************************************************************/
UINT term_get_output_queue(void ** pp_msg_st, uint32_t wait)
{
    UINT status = TX_SUCCESS;

    /* Wait for an output queue */
#if (BSP_CFG_RTOS == 1U)
    status = tx_queue_receive(&s_term_output_queue, (VOID *)pp_msg_st, wait);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueReceive(s_term_output_queue, (void *)pp_msg_st, wait);
    if ((uint32_t)pdTRUE != status)
    {
        return FSP_ERR_ASSERTION;
    }
#endif

    return status;
}
/***********************************************************************************************************************
* End of function term_get_output_queue
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: term_send_input_queue
 * Description  : This function sends data to the terminal input queue. Allocates memory for the message and queues
 *                it for transmission.
 * Arguments    : id                    Message ID.
 *                p_data                Pointer to the data buffer to be sent.
 *                size                  Size of the data to be sent.
 * Return Value : TX_SUCCESS            Sent successfully.
 *                Any other error codes Failed to send.
 **********************************************************************************************************************/
UINT term_send_input_queue(uint32_t id, void * const p_data, uint32_t size)
{
    UINT status = TX_SUCCESS;
    terminal_msg_t * p_term_msg = NULL;

    /* Allocate memory for terminal message structure and output message */
#if (BSP_CFG_RTOS == 1U)
    status = tx_byte_allocate(&s_term_byte_pool, (VOID **)&p_term_msg,
                                       sizeof(terminal_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    p_term_msg = (terminal_msg_t*) pvPortMalloc(sizeof(terminal_msg_t) + size + sizeof(char));
    if (NULL == p_term_msg)
    {
        return FSP_ERR_INVALID_POINTER;
    }
#endif

    /* Copy the message */
    memcpy(p_term_msg->msg, p_data, size);

    /* Null-terminate the message */
    p_term_msg->msg[size] = '\0';

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
    status = tx_queue_send(&s_term_input_queue, (VOID *)&p_term_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_term_msg);
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueSend(s_term_input_queue, (void *)&p_term_msg, portMAX_DELAY);
    if (pdTRUE != status)
    {
        vPortFree(p_term_msg);
        return FSP_ERR_ASSERTION;
    }
#endif

    return status;
}
/***********************************************************************************************************************
* End of function term_send_input_queue
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: term_send_output_queue
 * Description  : This function sends data to the terminal output queue. Allocates memory for the message and
 *                queues it for transmission.
 * Arguments    : id                    Message ID.
 *                p_data                Pointer to the data buffer to be sent.
 *                size                  Size of the data to be sent.
 * Return Value : TX_SUCCESS            Sent successfully.
 *                Any other error codes Failed to send.
 **********************************************************************************************************************/
UINT term_send_output_queue(uint32_t id, void * const p_data, uint32_t size)
{
    UINT status = TX_SUCCESS;
    terminal_msg_t * p_term_msg = NULL;

    /* Allocate memory for terminal message structure and output message */
#if (BSP_CFG_RTOS == 1U)
    status = tx_byte_allocate(&s_term_byte_pool, (VOID **)&p_term_msg,
                                       sizeof(terminal_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    p_term_msg = (terminal_msg_t*) pvPortMalloc(sizeof(terminal_msg_t) + size + sizeof(char));
    if (NULL == p_term_msg)
    {
        return FSP_ERR_INVALID_POINTER;
    }
#endif

    /* Copy the message into the terminal message structure */
    memcpy(p_term_msg->msg, p_data, size);

    /* Add a null character to terminate the terminal message buffer */
    p_term_msg->msg[size] = '\0';

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
    status = tx_queue_send(&s_term_output_queue, (VOID *)&p_term_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_term_msg);
        return status;
    }

#elif (BSP_CFG_RTOS == 2U)
    uint32_t status = (uint32_t)xQueueSend(s_term_output_queue, (void *)&p_term_msg, portMAX_DELAY);
    if (pdTRUE  != status)
    {
        vPortFree(p_term_msg);
        return FSP_ERR_ASSERTION;
    }
#endif

    return status;
}
/***********************************************************************************************************************
* End of function term_send_output_queue
***********************************************************************************************************************/

#endif  /* BSP_CFG_RTOS != 0U */

/***********************************************************************************************************************
 * Function Name: get_tx_flags
 * Description  : This function waits for the specified event flags and returns the actual flags received.
 * Arguments    : requested_flags           Requested event flags.
 *                actual_flags_ptr          Pointer to store the received event flags.
 * Return Value : TX_SUCCESS                Received successfully.
 *                Any other error codes     Failed to receive.
 **********************************************************************************************************************/
UINT get_tx_flags(ULONG requested_flags, ULONG * actual_flags_ptr)
{
    UINT status = TX_SUCCESS;

    status = tx_event_flags_get(&g_request_event,
                                requested_flags,
                                TX_OR_CLEAR,
                                actual_flags_ptr,
                                TX_WAIT_FOREVER);

    return status;
}
/***********************************************************************************************************************
* End of function get_tx_flags
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: set_tx_flags
 * Description  : This function sets the specified event flags.
 * Arguments    : requested_flags           Event flags to set.
 * Return Value : TX_SUCCESS                Set successfully.
 *                Any other error codes     Failed to set.
 **********************************************************************************************************************/
UINT set_tx_flags(ULONG requested_flags)
{
    UINT status = TX_SUCCESS;

    status = tx_event_flags_set(&g_request_event,
                                requested_flags,
                                TX_OR);

    return status;
}
/***********************************************************************************************************************
* End of function set_tx_flags
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: thread_sleep
 * Description  : This function suspends the calling thread for the specified number of timer ticks.
 * Arguments    : timer_ticks               Number of timer ticks to sleep.
 * Return Value : None.
 **********************************************************************************************************************/
void thread_sleep(ULONG timer_ticks)
{
    tx_thread_sleep(timer_ticks);
}
/***********************************************************************************************************************
* End of function thread_sleep
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: thread_suspend
 * Description  : This function identifies and suspends the calling thread.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void thread_suspend(void)
{
    tx_thread_suspend(tx_thread_identify());
}
/***********************************************************************************************************************
* End of function thread_suspend
***********************************************************************************************************************/
