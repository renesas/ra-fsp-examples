/***********************************************************************************************************************
 * File Name    : terminal.c
 * Description  : Contains data structures and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "filex_sram_example_app.h"
#include "terminal.h"

#if (BSP_CFG_RTOS == 1)
/* Private variables */
static TX_QUEUE s_term_op_queue;
static TX_QUEUE s_term_ip_queue;
static uint8_t s_queue_memory_op_term_obj[20] = {NULL_CHAR};
static uint8_t s_queue_memory_ip_term_obj[20] = {NULL_CHAR};
static TX_BYTE_POOL s_term_byte_pool;
static CHAR s_term_byte_pool_memory[TERM_BYTE_POOL_SIZE] = {NULL_CHAR};
static CHAR s_term_ip_buf[TERM_IP_MAX_SIZE] = {NULL_CHAR};

/* Private functions */
static UINT term_framework_init(void);
static UINT term_memory_allocate(TX_BYTE_POOL * pool, term_msg_t ** p_buf, uint32_t size, ULONG wait);
static UINT term_process_op_msg(void);
static UINT term_process_ip_msg(void);
static void term_ip_read(char * buf, int len);
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
    handle_error(err, "term_framework_init failed\r\n");

    while (true)
    {
        /* Forward the user input to the application thread */
        err = term_process_ip_msg();
        handle_error(err, "term_process_ip_msg failed\r\n");

        /* Process messages from the application threads */
        err = term_process_op_msg();
        handle_error(err, "term_process_op_msg failed\r\n");

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
        if (RESET_VALUE < term_thread_info.run_count)
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
    p_display_data->msg_data_size = size;
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
 * @brief      This function gets a line of user input from the terminal input queue.
 * @param[out] p_buf                    Pointer to the buffer to store the null-terminated user input.
 * @param[in]  size                     Size of the buffer in bytes.
 * @retval     TX_SUCCESS               Got the user input successfully.
 * @retval     Any other error codes    Failed to get the user input within the wait time.
 **********************************************************************************************************************/
UINT term_ip_get(CHAR * const p_buf, uint32_t size)
{
    term_msg_t * p_ip_data = NULL;
    uint32_t length = RESET_VALUE;
    UINT err = TX_SUCCESS;

    if ((NULL == p_buf) || (RESET_VALUE == size))
    {
        return TX_PTR_ERROR;
    }

    err = tx_queue_receive(&s_term_ip_queue, (void *)&p_ip_data, 0xFF);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    if ((NULL != p_ip_data) && (TERM_IP_MSG_STR == p_ip_data->msg_id))
    {
        /* Copy the user input */
        length = (p_ip_data->msg_data_size < size) ? p_ip_data->msg_data_size : size;
        memcpy(p_buf, p_ip_data->msg_data, length);
        p_buf[length - 1U] = NULL_CHAR;
    }
    else
    {
        /* Release the invalid message allocated block and report the error */
        tx_byte_release((void *)p_ip_data);
        return TX_QUEUE_ERROR;
    }

    /* Release the user input message allocated block */
    err = tx_byte_release((void *)p_ip_data);

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

    err = tx_queue_create(&s_term_ip_queue, (CHAR *)"Term Input Queue", 1, &s_queue_memory_ip_term_obj,
                          sizeof(s_queue_memory_ip_term_obj));
    if (TX_SUCCESS != err)
    {
        tx_queue_delete(&s_term_op_queue);
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
                    printf(EP_INFO "\r\n");
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
                    filex_deinit();

                    trap_required = true;
                    break;
                }

                case TERM_OP_MSG_MENU:
                {
                    printf(EP_MENU);
                    break;
                }

                case TERM_OP_MSG_ENTRY_INFO:
                {
                    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                                         };
                    entry = *(entry_info_t *)p_term_op_data->msg_data;
                    if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                    {
                        /* Print directory information */
                        printf("\r\n\r\n%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s",
                               entry.time.month, entry.time.date, entry.time.year,
                               entry.time.hour, entry.time.min, entry.time.sec,
                               "<DIR>          ", entry.name);
                    }
                    else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                    {
                        /* Print file information */
                        printf("\r\n\r\n%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %15lu %s",
                               entry.time.month, entry.time.date, entry.time.year,
                               entry.time.hour, entry.time.min, entry.time.sec,
                               entry.size, entry.name);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;
                }

                case TERM_OP_MSG_DIR_PROPERTY:
                {
                    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};
                    dir = *(dir_property_t *)p_term_op_data->msg_data;

                    /* Print directory property */
                    printf("\r\n         %6u File(s) %16lu bytes", dir.file, dir.size);
                    printf("\r\n         %6u Dir(s)", dir.subdir);
                    break;
                }

                default:
                {
                    /* Do nothing */
                    break;
                }
            }

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
 * @brief     This function reads a line of user input from the terminal and sends it to the terminal input queue.
 * @param[in] None.
 * @retval    TX_SUCCESS                Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static UINT term_process_ip_msg(void)
{
    UINT err = TX_SUCCESS;
    term_msg_t * p_ip_data = NULL;

    if (!uart_console_has_char())
    {
        return TX_SUCCESS;
    }

    term_ip_read(s_term_ip_buf, sizeof(s_term_ip_buf));

    /* Allocate memory for the user input message. The terminal thread must not wait here for the memory, because
     * it is the thread that releases the memory of the output messages */
    err = term_memory_allocate(&s_term_byte_pool, &p_ip_data, sizeof(term_msg_t) + strlen(s_term_ip_buf) + 1U,
                               TX_NO_WAIT);
    if (TX_SUCCESS == err)
    {
        /* Send the data to the application thread */
        p_ip_data->msg_id = TERM_IP_MSG_STR;
        p_ip_data->msg_data_size = strlen(s_term_ip_buf) + 1U;
        memcpy(p_ip_data->msg_data, s_term_ip_buf, p_ip_data->msg_data_size);

        err = tx_queue_send(&s_term_ip_queue, (term_msg_t *)&p_ip_data, TX_NO_WAIT);
        if (TX_SUCCESS != err)
        {
            tx_byte_release(p_ip_data);
        }
    }

    /* The pool or the queue is temporarily full: drop this line, it is not a fatal error */
    if ((TX_NO_MEMORY == err) || (TX_QUEUE_FULL == err))
    {
        err = TX_SUCCESS;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief          This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in, out] buf                  Pointer to buffer where the input string is stored.
 * @param[in]      len                  Maximum number of characters to read (buffer size).
 * @retval         None.
 **********************************************************************************************************************/
static void term_ip_read(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = NULL_CHAR;
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
