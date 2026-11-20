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

#if (BSP_CFG_RTOS == AZURE_RTOS)
/* Private variables */
static TX_QUEUE s_term_op_data_queue;
static TX_QUEUE s_term_ip_data_queue;
static TX_BYTE_POOL s_byte_pool;
static const CHAR * byte_pool_name = "term_processing_block";
static CHAR byte_memory[BYTE_POOL_SIZE] = {'\0'};
static uint8_t queue_memory_op_term_obj[TERM_QUEUE_STORAGE_SIZE] = {'\0'};
static uint8_t queue_memory_ip_term_obj[TERM_QUEUE_STORAGE_SIZE] = {'\0'};
#endif /* BSP_CFG_RTOS == AZURE_RTOS */

/* Private functions */
static UINT term_framework_init(VOID);
static VOID process_term_op_msg(VOID);
static UINT memory_allocate_term(TX_BYTE_POOL * pool, term_msg_t ** p_buf, uint32_t size);
static VOID user_input(CHAR * buf, INT len);
static VOID handle_error(uint32_t err, const CHAR * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
VOID terminal_thread_entry_run(VOID)
{
    UINT err = TX_SUCCESS;
    CHAR term_buffer[IP_MSG_MAX_SIZE] = {'\0'};

    /* Allocate memory block for address-based message exchange */
    term_msg_t *p_data = NULL;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Initialize terminal framework */
    err = term_framework_init();
    handle_error(err, "term_framework_init failed\r\n");

    while (1)
    {
        /* Check for user input */
        if (uart_console_has_char())
        {
            user_input(term_buffer, (int)sizeof(term_buffer));
            UINT read_bytes = strlen(term_buffer) + 1;

            err = memory_allocate_term(&s_byte_pool, &p_data, sizeof(term_msg_t) + read_bytes);
            if (TX_SUCCESS != err)
            {
                printf("Error in processing, please check again\r\n");
            }
            else
            {
                /* Send data to the user thread */
                p_data->msg_id = TERM_INPUT_MESSAGE_TYPE_STR_QUEUED;
                p_data->msg_data_size = read_bytes;
                memcpy(p_data->p_msg, term_buffer, p_data->msg_data_size);
#if (BSP_CFG_RTOS == AZURE_RTOS)
                tx_queue_send(&s_term_ip_data_queue, (term_msg_t *)&p_data, TX_WAIT_FOREVER);
#endif /* BSP_CFG_RTOS == AZURE_RTOS */

                memset(term_buffer, '\0', sizeof(term_buffer));
            }
        }

        /* Print messages from the user thread to terminal output using event-based checking */
        process_term_op_msg();

#if (BSP_CFG_RTOS == AZURE_RTOS)
        /* Sleep briefly to yield CPU to higher priority threads */
        tx_thread_sleep(1);
#endif /* BSP_CFG_RTOS == AZURE_RTOS */
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is called by user/application threads to print messages to the terminal.
 * @param[in] id                        Message ID indicating the type of message.
 * @param[in] size                      Size of the message data in bytes.
 * @param[in] p_data                    Pointer to the message data to be sent.
 * @retval    None.
 **********************************************************************************************************************/
VOID app_term_print_data(event_id_t id, uint32_t size, const VOID * p_data)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;
    term_msg_t *p_display_data = NULL;

    /* Allocate memory for the terminal message (structure + payload) */
    err = memory_allocate_term(&s_byte_pool, &p_display_data, sizeof(term_msg_t) + size);
    handle_error(err, "memory_allocate_term failed\r\n");

    p_display_data->msg_id = id;
    p_display_data->msg_time = tx_time_get();
    p_display_data->msg_data_size = size;
    memcpy(p_display_data->p_msg, p_data, size);

    /* Send the allocated message to the terminal output queue */
    err = tx_queue_send(&s_term_op_data_queue, (term_msg_t *)&p_display_data, TX_WAIT_FOREVER);
    handle_error(err, "tx_queue_send failed\r\n");
#endif /* BSP_CFG_RTOS == AZURE_RTOS */
}

/*******************************************************************************************************************//**
 * @brief      This function reads user input from the terminal input queue.
 *             After processing the received message, the allocated message block is released.
 * @param[out] p_get_data               Pointer to the variable that receives the user input value.
 * @retval     TX_SUCCESS               Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
UINT check_for_term_user_ip(uint8_t * p_get_data)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;
    term_msg_t *p_input_msg = NULL;

    err = tx_queue_receive(&s_term_ip_data_queue, (VOID *)&p_input_msg, 0xFF);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    if ((NULL != p_input_msg) && (TERM_INPUT_MESSAGE_TYPE_STR_QUEUED == p_input_msg->msg_id))
    {
        /* User input received */
        *p_get_data = (uint8_t)(atoi((char *)p_input_msg->p_msg));
    }

    /* Release the memory block allocated for the message */
    err = tx_byte_release(p_input_msg);

    return err;
#endif /* BSP_CFG_RTOS == AZURE_RTOS */
}

/*******************************************************************************************************************//**
 * @brief     This function waits until the Terminal thread has run at least once.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
VOID term_thread_init_check(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;

    thread_info_t terminal_thread_info =
    {
        .thread_ptr = &terminal_thread,
    };

    do
    {
        err = tx_thread_info_get(terminal_thread_info.thread_ptr,
                                 TX_NULL,
                                 &terminal_thread_info.state,
                                 &terminal_thread_info.run_count,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL,
                                 TX_NULL);

        if (terminal_thread_info.run_count >= 1)
        {
            break;
        }
        else
        {
            /* Sleep briefly to yield CPU to higher priority threads */
            tx_thread_sleep(1);
        }
    } while (err == TX_SUCCESS);
#endif /* BSP_CFG_RTOS == AZURE_RTOS */
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the terminal framework resources.
 * @param[in] None.
 * @retval    TX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static UINT term_framework_init(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)

    UINT err = TX_SUCCESS;

    err = tx_byte_pool_create(&s_byte_pool, (CHAR *)byte_pool_name, (VOID *)&byte_memory[0], BYTE_POOL_SIZE);
    if (TX_SUCCESS != err)
    {
        return err;
    }

    err = tx_queue_create(&s_term_op_data_queue, (CHAR *)"term_op_queue", 1, &queue_memory_op_term_obj,\
                          sizeof(queue_memory_op_term_obj));
    if (TX_SUCCESS != err)
    {
        tx_byte_pool_delete(&s_byte_pool);
        return err;
    }

    err = tx_queue_create(&s_term_ip_data_queue, (CHAR *)"term_ip_queue", 1, &queue_memory_ip_term_obj,\
                          sizeof(queue_memory_ip_term_obj));
    if (TX_SUCCESS != err)
    {
        tx_byte_pool_delete(&s_byte_pool);
        tx_queue_delete(&s_term_op_data_queue);
        return err;
    }

    return err;

#endif /* BSP_CFG_RTOS == AZURE_RTOS */
}

/*******************************************************************************************************************//**
 * @brief     This function receives messages from the user thread via tx_queue_receive, displays them on the
 *            terminal, and releases the memory blocks allocated for the messages after processing.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static VOID process_term_op_msg(VOID)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;
    term_msg_t *term_op_data = NULL;
    bool trap_required = false;

    /* Process and print all received messages */
    do
    {
        err = tx_queue_receive(&s_term_op_data_queue, (VOID *)&term_op_data, 1);
        if (TX_SUCCESS == err)
        {
            /* Handle the received message */
            switch (term_op_data->msg_id)
            {
                case TERM_OUTPUT_MESSAGE_BANNER:
                {
                    /* Get FLEX pack version info and print EP information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);
                    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
                           version.version_id_b.patch );
                    uart_console_flush();
                    printf(EP_INFO);
                    uart_console_flush();
                    break;
                }

                case TERM_OUTPUT_MESSAGE_APP_INFO_STR:
                {
                    printf("\r\n\r\nInfo: %s", term_op_data->p_msg);
                    uart_console_flush();
                    break;
                }

                case TERM_OUTPUT_MESSAGE_APP_ERR_STR:
                {
                    printf("\r\n[ERR] %s", term_op_data->p_msg);
                    break;
                }

                case TERM_OUTPUT_MESSAGE_APP_ERR_TRAP:
                {
                    /* Extract error code and halt the application */
                    UINT err_code = *(UINT *)term_op_data->p_msg;
                    printf("Returned Error Code: 0x%X\r\n", (unsigned int)err_code);
                    uart_console_flush();
                    trap_required = true;
                    break;
                }

                case TERM_OUTPUT_MESSAGE_APP_PRINT_MENU:
                {
                    /* Print the EP guide */
                    printf(EP_GUIDE);
                    uart_console_flush();
                    break;
                }

                default:
                {
                    printf("\r\nThe terminal received an unknown message ID\r\n");
                    break;
                }
            }

            /* Release the memory block allocated for the message */
            err = tx_byte_release((VOID *) term_op_data);
            handle_error(err, "tx_byte_release failed\r\n");

            if (trap_required)
            {
                trap_required = false;
                __BKPT(0);
            }
        }
    } while (err != TX_QUEUE_EMPTY);
#endif /* BSP_CFG_RTOS == AZURE_RTOS */
}

/*******************************************************************************************************************//**
 * @brief     This function allocates memory for message transfer via queue.
 * @param[in] pool                      Pointer to byte memory pool.
 * @param[in] p_buf                     Pointer to the pointer that will receive the allocated buffer address.
 * @param[in] size                      Total size of memory to allocate in bytes.
 * @retval    TX_SUCCESS                Allocated successfully
 * @retval    Any other error codes     Failed to allocate.
 **********************************************************************************************************************/
static UINT memory_allocate_term(TX_BYTE_POOL * pool, term_msg_t ** p_buf, uint32_t size)
{
#if (BSP_CFG_RTOS == AZURE_RTOS)
    UINT err = TX_SUCCESS;
    term_msg_t *buf = NULL;

    /* Allocate memory block from the byte pool */
    err = tx_byte_allocate(pool, (VOID **)&buf, size, TX_WAIT_FOREVER);
    if (TX_SUCCESS == err)
    {
        /* Assign buffer address */
        *p_buf = buf;
    }

    return err;
#endif /* BSP_CFG_RTOS == AZURE_RTOS */
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static VOID user_input(CHAR * buf, INT len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static VOID handle_error(uint32_t err, const CHAR * err_str)
{
    if (0U != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%X\r\n", (UINT)err);
        uart_console_flush();
        __BKPT(0);
    }
}
