/***********************************************************************************************************************
 * File Name    : terminal.c
 * Description  : Contains data structures and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "terminal.h"

#if (BSP_CFG_RTOS == 1)
/* Private variables */
static TX_QUEUE s_term_op_queue;
static uint8_t s_queue_memory_op_term_obj[20] = {NULL_CHAR};
static TX_BYTE_POOL s_term_byte_pool;
static CHAR s_term_byte_pool_memory[TERM_BYTE_POOL_SIZE] = {NULL_CHAR};

/* Private functions */
static UINT term_framework_init(void);
static UINT term_memory_allocate(TX_BYTE_POOL * pool, term_msg_t ** p_buf, uint32_t size, ULONG wait);
static UINT term_process_op_msg(void);
static void handle_error(UINT err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void term_thread_entry_run(void)
{
    UINT err = TX_SUCCESS;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    /* Initialize the terminal framework */
    err = term_framework_init();
    handle_error(err, "term_framework_init failed");

    while (true)
    {
        /* Process messages from the application threads */
        err = term_process_op_msg();
        handle_error(err, "term_process_op_msg failed");

        /* Sleep briefly to yield CPU to higher priority threads */
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the terminal thread to initialize before sending data to it.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void term_thread_init_check(void)
{
    UINT err = TX_SUCCESS;

    thread_info_t term_thread_info =
    {
        .thread_ptr = &terminal_thread,
    };

    do
    {
        err = tx_thread_info_get(term_thread_info.thread_ptr,
                                 TX_NULL,
                                 &term_thread_info.state,
                                 &term_thread_info.run_count,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL);

        if (1U <= term_thread_info.run_count)
        {
            break;
        }
        else
        {
            /* Sleep briefly to yield CPU to higher priority threads */
            tx_thread_sleep(1);
        }
    } while (TX_SUCCESS == err);
}

/*******************************************************************************************************************//**
 * @brief     This function sends message data to the terminal output queue.
 * @param[in] id                        Message ID indicating the type of message.
 * @param[in] size                      Size of the message data in bytes.
 * @param[in] p_data                    Pointer to the message data to be sent.
 * @retval    TX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
UINT term_print_data(event_id_t id, uint32_t size, void * const p_data)
{
    term_msg_t * p_display_data = NULL;
    UINT err = TX_SUCCESS;

    if ((RESET_VALUE < size) && (NULL == p_data))
    {
        return TX_PTR_ERROR;
    }

    /* Allocate memory for the terminal message (structure + payload) */
    err = term_memory_allocate(&s_term_byte_pool, &p_display_data, sizeof(term_msg_t) + size, TX_WAIT_FOREVER);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    p_display_data->msg_id = id;
    if (RESET_VALUE < size)
    {
        memcpy(p_display_data->msg_data, p_data, size);
    }

    /* Send the allocated message to the terminal output queue */
    err = tx_queue_send(&s_term_op_queue, (term_msg_t *)&p_display_data, TX_WAIT_FOREVER);
    if (TX_SUCCESS != err)
    {
        tx_byte_release(p_display_data);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the terminal framework by creating the byte pool and the queues.
 * @param[in] None.
 * @retval    TX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static UINT term_framework_init(void)
{
    UINT err = TX_SUCCESS;

    err = tx_byte_pool_create(&s_term_byte_pool, (CHAR *)"Term Byte Pool", (void *)&s_term_byte_pool_memory[0],
                              TERM_BYTE_POOL_SIZE);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    err = tx_queue_create(&s_term_op_queue, (CHAR *)"Term Output Queue", 1, &s_queue_memory_op_term_obj,
                          sizeof(s_queue_memory_op_term_obj));
    if (TX_SUCCESS != err)
    {
        tx_byte_pool_delete(&s_term_byte_pool);
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function allocates memory for message transfer through the queue.
 * @param[in]  pool                     Pointer to the byte memory pool.
 * @param[out] p_buf                    Pointer to the allocated terminal message buffer.
 * @param[in]  size                     Total size to allocate in bytes.
 * @param[in]  wait                     Wait option for the allocation, in ticks.
 * @retval     TX_SUCCESS               Allocated successfully.
 * @retval     Any other error codes    Failed to allocate.
 **********************************************************************************************************************/
static UINT term_memory_allocate(TX_BYTE_POOL * pool, term_msg_t ** p_buf, uint32_t size, ULONG wait)
{
    UINT err = TX_SUCCESS;
    term_msg_t * p_buffer = NULL;

    /* Allocate memory block from the byte pool */
    err = tx_byte_allocate(pool, (void **)&p_buffer, size, wait);
    if (TX_SUCCESS == err)
    {
        /* Assign buffer address */
        *p_buf = p_buffer;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function receives messages from application threads and displays them on the terminal.
 * @param[in] None.
 * @retval    TX_SUCCESS                Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static UINT term_process_op_msg(void)
{
    UINT err = TX_SUCCESS;
    bool trap_required = false;
    term_msg_t * p_term_op_data = NULL;

    /* Process all queued terminal messages */
    while (true)
    {
        err = tx_queue_receive(&s_term_op_queue, (void *)&p_term_op_data, TX_NO_WAIT);
        if (TX_SUCCESS == err)
        {
            switch (p_term_op_data->msg_id)
            {
                case TERM_OP_MSG_BANNER:
                {
                    /* Get FSP version information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);

                    printf(BANNER_INFO, version.version_id_b.major,
                            version.version_id_b.minor, version.version_id_b.patch);
                    printf(EP_INFO);
                    uart_console_flush();
                    break;
                }

                case TERM_OP_MSG_INFO_STR:
                {
                    printf("\r\n%s", p_term_op_data->msg_data);
                    break;
                }

                case TERM_OP_MSG_ERR_STR:
                {
                    printf("\r\n\r\n[ERR]: %s", p_term_op_data->msg_data);
                    break;
                }

                case TERM_OP_MSG_ERR_TRAP:
                {
                    UINT err_code = *(UINT *)p_term_op_data->msg_data;
                    printf("\r\nReturned Error Code: 0x%X", (unsigned int)err_code);
                    uart_console_flush();

                    /* Clean up active resources before entering the trap state */
                    usbx_hhid_deinit();

                    trap_required = true;
                    break;
                }

                case TERM_OP_MSG_KEY_PRESS:
                {
                    uint8_t key_pressed[BUFF_SIZE] = {'\0'};
                    memcpy(key_pressed, (uint8_t *)p_term_op_data->msg_data, BUFF_SIZE);
                    printf("\r\nKeyboard key pressed is : %s", key_pressed);
                    break;
                }

                case TERM_OP_MSG_LEFT_BUTTON_PRESS:
                {
                    printf("\r\nMouse Left button is pressed");
                    break;
                }

                case TERM_OP_MSG_RIGHT_BUTTON_PRESS:
                {
                    printf("\r\nMouse Right button is pressed");
                    break;
                }

                case TERM_OP_MSG_MOUSE_POS:
                {
                    mouse_pos_t mouse_info;
                    memcpy(&mouse_info, (mouse_pos_t *)(p_term_op_data->msg_data), sizeof (mouse_pos_t));
                    printf("\r\nx axis = %ld, y axis = %ld", mouse_info.mouse_x_pos_old,
                              mouse_info.mouse_y_pos_old);
                    break;
                }

                default:
                {
                    printf("\r\nUnknown message ID");
                    break;
                }
            }

            /* Send a newline and return carriage requesting flushing the buffer */
            printf("\r\n");

            /* Release the terminal message allocated block */
            err = tx_byte_release((void *)p_term_op_data);
            if (TX_SUCCESS != err)
            {
                return err;
            }

            if (trap_required)
            {
                __BKPT(0);
            }
        }
        else if (TX_QUEUE_EMPTY == err)
        {
            return TX_SUCCESS;
        }
        else
        {
            return err;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(UINT err, const char * err_str)
{
    if (0U != err)
    {
        printf("\r\n\r\n[ERR]: %s", err_str);
        printf("\r\nReturned Error Code: 0x%X", (unsigned int)err);
        uart_console_flush();
        __BKPT(0);
    }
}

#endif /* BSP_CFG_RTOS == 1 */
