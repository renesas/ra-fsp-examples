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
#include "filex_sdram_example_app.h"

/* Private variables */
#if (BSP_CFG_RTOS != 0U)
#if (BSP_CFG_RTOS == 1U)
#include "tx_api.h"
static TX_BYTE_POOL s_term_byte_pool;
static TX_QUEUE s_term_input_queue;
static TX_QUEUE s_term_output_queue;
static uint8_t s_term_byte_pool_memory[TERM_BYTE_POOL_SIZE];
#elif (BSP_CFG_RTOS == 2U)
static QueueHandle_t s_term_input_queue;
static QueueHandle_t s_term_output_queue;
#endif /* BSP_CFG_RTOS */

/* Private function */
static uint32_t term_framework_init(void);
static uint32_t terminal_input_handle(void);
static uint32_t terminal_output_handle(void);
static uint32_t term_get_output_queue(void ** pp_msg_st, uint32_t wait);
static uint32_t term_send_input_queue(terminal_event_t id, void * const p_data, uint32_t size);
static void conv_ul64_to_str(ULONG64 value, CHAR * p_buf, UINT len);
static void user_input(char * buf, int len);
static void handle_error(uint32_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    uint32_t status = TX_SUCCESS;

    uint32_t err = uart_console_init();
    if (FSP_SUCCESS != err)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    status = term_framework_init();
    handle_error(status, "term_framework_init failed\r\n");

    while (true)
    {
        /* Handle user input from terminal */
        status = terminal_input_handle();
        handle_error(status, "terminal_input_handle failed\r\n");

        /* Handle other threads that need output to the terminal application via terminal interface */
        status = terminal_output_handle();
        handle_error(status, "terminal_output_handle failed\r\n");

        tx_thread_sleep(TERMINAL_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the terminal thread to initialize and sleeps before sending data to it.
 * @param[in] None.
 * @retval    TX_SUCCESS                Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
UINT terminal_thread_init_check(void)
{
    UINT status = TX_SUCCESS;
    ULONG run_count = RESET_VALUE;

    /* Get terminal thread run count */
    while (true)
    {
        status = tx_thread_info_get(&terminal_thread,
                                    TX_NULL,
                                    TX_NULL,
                                    &run_count,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL);
        if (TX_SUCCESS != status)
        {
            return status;
        }

        if (RESET_VALUE < run_count)
        {
            return TX_SUCCESS;
        }

        tx_thread_sleep(ONE_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief      This function receives data from the input queue and copies it to the output buffer.
 *             To be called by the User/Application Thread.
 * @param[out] p_msg                    Pointer to the buffer for received message.
 * @param[out] p_size                   Pointer to store the size of the received message.
 * @param[in]  wait                     Timeout value for queue reception.
 * @retval     FSP_SUCCESS              Operated successfully.
 * @retval     Any other error codes    Failed to operate.
 **********************************************************************************************************************/
uint32_t term_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait)
{
    terminal_msg_t * p_term_msg = NULL;

    /* Wait for an input queue */
#if (BSP_CFG_RTOS == 1U)
    UINT status = tx_queue_receive(&s_term_input_queue, (VOID *)&p_term_msg, wait);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }
#elif (BSP_CFG_RTOS == 2U)
    BaseType_t status = xQueueReceive(s_term_input_queue, &p_term_msg, wait);
    if (pdTRUE != status)
    {
        return FSP_ERR_ASSERTION;
    }
#endif /* BSP_CFG_RTOS */

    if (NULL == p_term_msg)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    memcpy(p_msg, p_term_msg->msg, p_term_msg->size);
    *p_size = p_term_msg->size;

    /* Release the memory allocated for this terminal frame */
#if (BSP_CFG_RTOS == 1U)
    tx_byte_release((VOID *)p_term_msg);
#elif (BSP_CFG_RTOS == 2U)
    vPortFree(p_term_msg);
#endif /* BSP_CFG_RTOS */

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function sends data to the terminal output queue, allocates memory for the message, and
 *            queues it for transmission.
 * @param[in] id                        Message ID.
 * @param[in] p_data                    Pointer to the data buffer to be sent.
 * @param[in] size                      Size of the data to be sent.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
uint32_t term_send_output_queue(terminal_event_t id, void * const p_data, uint32_t size)
{
    if ((NULL == p_data) && (0U != size))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    terminal_msg_t * p_term_msg = NULL;

    /* Allocate memory for terminal message structure and message buffer */
#if (BSP_CFG_RTOS == 1U)
    UINT status = tx_byte_allocate(&s_term_byte_pool, (VOID **)&p_term_msg,
                                   sizeof(terminal_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }
#elif (BSP_CFG_RTOS == 2U)
    p_term_msg = (terminal_msg_t *)pvPortMalloc(sizeof(terminal_msg_t) + size + sizeof(char));
    if (NULL == p_term_msg)
    {
        return FSP_ERR_OUT_OF_MEMORY;
    }
#endif /* BSP_CFG_RTOS */

    /* Copy the message into the terminal message structure */
    if (0U != size)
    {
        memcpy(p_term_msg->msg, p_data, size);
    }

    /* Add a null character to terminate the terminal message buffer */
    p_term_msg->msg[size] = NULL_CHAR;

    /* Set additional information for the message */
    p_term_msg->id = id;

#if (BSP_CFG_RTOS == 1U)
    p_term_msg->time = tx_time_get();
#elif (BSP_CFG_RTOS == 2U)
    p_term_msg->time = xTaskGetTickCount();
#endif /* BSP_CFG_RTOS */

    p_term_msg->size = size + sizeof(char);

    /* Send the terminal message pointer to the output queue */
#if (BSP_CFG_RTOS == 1U)
    status = tx_queue_send(&s_term_output_queue, (VOID *)&p_term_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_term_msg);
        return (uint32_t)status;
    }
#elif (BSP_CFG_RTOS == 2U)
    BaseType_t status = xQueueSend(s_term_output_queue, (void *)&p_term_msg, portMAX_DELAY);
    if (pdTRUE != status)
    {
        vPortFree(p_term_msg);
        return FSP_ERR_ASSERTION;
    }
#endif /* BSP_CFG_RTOS */

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the terminal framework by creating necessary queues and memory pools.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static uint32_t term_framework_init(void)
{
#if (BSP_CFG_RTOS == 1U)
    CHAR * p_mem = TX_NULL;

    /* Create a memory byte pool for the terminal */
    UINT status = tx_byte_pool_create(&s_term_byte_pool, (CHAR *)"Term Byte Pool",
                                      (VOID *)s_term_byte_pool_memory, TERM_BYTE_POOL_SIZE);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }

    /* Create an output queue for the terminal */
    status = tx_byte_allocate(&s_term_byte_pool, (VOID **)&p_mem, TERM_OUTPUT_QUEUE_SIZE, TX_NO_WAIT);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }

    status = tx_queue_create(&s_term_output_queue, (CHAR *)"Term Output Queue", 1, p_mem, TERM_OUTPUT_QUEUE_SIZE);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }

    /* Create an input queue for the terminal */
    status = tx_byte_allocate(&s_term_byte_pool, (VOID **)&p_mem, TERM_INPUT_QUEUE_SIZE, TX_NO_WAIT);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }

    status = tx_queue_create(&s_term_input_queue, (CHAR *)"Term Input Queue", 1, p_mem, TERM_INPUT_QUEUE_SIZE);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }

#elif (BSP_CFG_RTOS == 2U)
    /* Create an output queue for the terminal */
    s_term_output_queue = xQueueCreate(TERM_OUTPUT_QUEUE_SIZE/sizeof(void *), sizeof(void *));
    if (NULL == s_term_output_queue)
    {
        return FSP_ERR_OUT_OF_MEMORY;
    }

    /* Create an input queue for the terminal */
    s_term_input_queue = xQueueCreate(TERM_INPUT_QUEUE_SIZE/sizeof(void *), sizeof(void *));
    if (NULL == s_term_input_queue)
    {
        return FSP_ERR_OUT_OF_MEMORY;
    }
#endif /* BSP_CFG_RTOS */

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function gets input data from the terminal and puts it into the terminal input queue.
 * @param[in] None.
 * @retval    TX_SUCCESS                Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static uint32_t terminal_input_handle(void)
{
    uint32_t status = TX_SUCCESS;
    uint32_t r_len = RESET_VALUE;
    char input_buff[TERM_BUFFER_SIZE] = {NULL_CHAR};

    if (uart_console_has_char())
    {
        user_input(input_buff, (int)sizeof(input_buff));

        r_len = (uint32_t)strlen(input_buff);
        if (0U == r_len)
        {
            return TX_SUCCESS;
        }

        /* Send input data to the input queue */
        status = term_send_input_queue(TERMINAL_INPUT_MESSAGE, input_buff, r_len);
        if (FSP_SUCCESS != status)
        {
            return status;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function transfers a message to the terminal based on the terminal frame taken from the terminal
 *            output queue.
 * @param[in] None.
 * @retval    TX_SUCCESS                Transferred successfully.
 * @retval    Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static uint32_t terminal_output_handle(void)
{
    uint32_t status = TX_SUCCESS;
    uint32_t err_code = RESET_VALUE;
    terminal_msg_t * p_terminal_msg = NULL;
    CHAR str_size [STR_UL64_MAX_LEN] = {RESET_VALUE};
    CHAR str_size_align [STR_UL64_MAX_LEN] = {RESET_VALUE};
    fsp_pack_version_t version = {RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                         };
    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};
    media_property_t media = {.format_type = RESET_VALUE, .free_size = RESET_VALUE,
                              .phy_size = RESET_VALUE, .total_size = RESET_VALUE};
    bool trap_required = false;

    while (true)
    {
        /* Retrieve a message from output queue */
        status = term_get_output_queue((void **)&p_terminal_msg, TX_NO_WAIT);
        if (FSP_SUCCESS == status)
        {
            switch (p_terminal_msg->id)
            {
                case TERMINAL_OUTPUT_EP_BANNER:
                {
                    /* Get FSP version information */
                    R_FSP_VersionGet(&version);

                    /* Print the EP banner to terminal application on PC */
                    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
                           version.version_id_b.patch);
                    uart_console_flush();
                    break;
                }

                case TERMINAL_OUTPUT_EP_INFO:
                {
                    /* Print the EP information to terminal application on PC */
                    printf(EP_INFO);
                    uart_console_flush();
                    break;
                }

                case TERMINAL_OUTPUT_APP_INFO_STR:
                {
                    printf("%s", p_terminal_msg->msg);
                    break;
                }

                case TERMINAL_OUTPUT_APP_ERR_STR:
                {
                    printf("\r\n[ERR] %s\r\n", p_terminal_msg->msg);
                    break;
                }

                case TERMINAL_OUTPUT_APP_ERR_TRAP:
                {
                    err_code = *(uint32_t *)p_terminal_msg->msg;
                    printf("Returned Error Code: 0x%X\r\n", (unsigned int)err_code);
                    uart_console_flush();
                    filex_deinit();
                    trap_required = true;
                    break;
                }

                case TERMINAL_OUTPUT_APP_MENU:
                {
                    printf(EP_MENU);
                    uart_console_flush();
                    break;
                }

                case TERMINAL_OUTPUT_APP_ENTRY_INFO:
                {
                    entry = *(entry_info_t *)p_terminal_msg->msg;
                    if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                    {
                        /* Print directory information */
                        printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                               entry.time.month, entry.time.date, entry.time.year,
                               entry.time.hour, entry.time.min, entry.time.sec,
                               "<DIR>          ", entry.name);
                    }
                    else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                    {
                        /* Print file information */
                        conv_ul64_to_str(entry.size, str_size, STR_UL64_MAX_LEN);
                        snprintf(str_size_align, STR_UL64_MAX_LEN, "%15s", str_size);
                        printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                               entry.time.month, entry.time.date, entry.time.year,
                               entry.time.hour, entry.time.min, entry.time.sec,
                               str_size_align, entry.name);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;
                }

                case TERMINAL_OUTPUT_APP_DIR_PROPERTY:
                {
                    dir = *(dir_property_t *)p_terminal_msg->msg;

                    /* Print directory property */
                    conv_ul64_to_str(dir.size, str_size, STR_UL64_MAX_LEN);
                    snprintf(str_size_align, STR_UL64_MAX_LEN, "%15s", str_size);
                    printf("         %6u File(s) %s bytes\r\n", dir.file, str_size_align);
                    printf("         %6u Dir(s)\r\n\r\n", dir.subdir);
                    break;
                }

                case TERMINAL_OUTPUT_APP_MEDIA_PROPERTY:
                {
                    media = *(media_property_t *)p_terminal_msg->msg;

                    conv_ul64_to_str(media.total_size, str_size, STR_UL64_MAX_LEN);
                    printf("Media total size is:   %15s bytes\r\n", str_size);

                    conv_ul64_to_str (media.free_size, str_size, STR_UL64_MAX_LEN);
                    printf("Media free size is:    %15s bytes\r\n", str_size);

                    printf("Format type is:        %11s\r\n", media.format_type);
                    break;
                }

                default:
                {
                    /* Do nothing */
                    break;
                }
            }

            /* Release terminal message allocated block */
#if (BSP_CFG_RTOS == 1U)
            status = (uint32_t)tx_byte_release((VOID *)p_terminal_msg);
            if (TX_SUCCESS != status)
            {
                return status;
            }
#elif (BSP_CFG_RTOS == 2U)
            vPortFree(p_terminal_msg);
#endif /* BSP_CFG_RTOS */

            if (trap_required)
            {
                trap_required = false;
                __BKPT(0);
            }
        }

        /* If the queue is empty */
        else if (TX_QUEUE_EMPTY == status)
        {
            return TX_SUCCESS;
        }

        /* If the API returns an error */
        else
        {
            /* Print an error message to the terminal */
            printf("\r\n[ERR] term_get_output_queue failed\r\n");
            return status;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief      This function receives a terminal message pointer from the output queue.
 *             To be called by the Terminal Thread.
 * @param[out] pp_msg_st                Pointer to store the address of the received message structure.
 * @param[in]  wait                     Timeout value for queue reception.
 * @retval     FSP_SUCCESS              Received successfully.
 * @retval     Any other error codes    Failed to receive.
 **********************************************************************************************************************/
static uint32_t term_get_output_queue(void ** pp_msg_st, uint32_t wait)
{
    if (NULL == pp_msg_st)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Wait for an output queue */
#if (BSP_CFG_RTOS == 1U)
    UINT status = tx_queue_receive(&s_term_output_queue, (VOID *)pp_msg_st, wait);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }
#elif (BSP_CFG_RTOS == 2U)
    BaseType_t status = xQueueReceive(s_term_output_queue, (void *)pp_msg_st, wait);
    if (pdTRUE != status)
    {
        return FSP_ERR_ASSERTION;
    }
#endif /* BSP_CFG_RTOS */

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This function sends data to the terminal input queue, allocates memory for the message, and
 *             queues it for transmission.
 * @param[in]  id                       Message ID.
 * @param[in]  p_data                   Pointer to the data buffer to be sent.
 * @param[in]  size                     Size of the data to be sent.
 * @retval     FSP_SUCCESS              Operated successfully.
 * @retval     Any other error codes    Failed to operate.
 **********************************************************************************************************************/
static uint32_t term_send_input_queue(terminal_event_t id, void * const p_data, uint32_t size)
{
    if ((NULL == p_data) && (0U != size))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    terminal_msg_t * p_term_msg = NULL;

    /* Allocate memory for terminal message structure and message buffer */
#if (BSP_CFG_RTOS == 1U)
    UINT status = tx_byte_allocate(&s_term_byte_pool, (VOID **)&p_term_msg,
                                   sizeof(terminal_msg_t) + size + sizeof(char), TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return (uint32_t)status;
    }
#elif (BSP_CFG_RTOS == 2U)
    p_term_msg = (terminal_msg_t *)pvPortMalloc(sizeof(terminal_msg_t) + size + sizeof(char));
    if (NULL == p_term_msg)
    {
        return FSP_ERR_OUT_OF_MEMORY;
    }
#endif /* BSP_CFG_RTOS */

    if (0U != size)
    {
        /* Copy the message */
        memcpy(p_term_msg->msg, p_data, size);
    }

    /* Null-terminate the message */
    p_term_msg->msg[size] = NULL_CHAR;

    /* Set additional information for the message */
    p_term_msg->id = id;

#if (BSP_CFG_RTOS == 1U)
    p_term_msg->time = tx_time_get();
#elif (BSP_CFG_RTOS == 2U)
    p_term_msg->time = xTaskGetTickCount();
#endif /* BSP_CFG_RTOS */

    p_term_msg->size = size + sizeof(char);

    /* Send the terminal message structure pointer to the input queue */
#if (BSP_CFG_RTOS == 1U)
    status = tx_queue_send(&s_term_input_queue, (VOID *)&p_term_msg, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        tx_byte_release((VOID *)p_term_msg);
        return (uint32_t)status;
    }
#elif (BSP_CFG_RTOS == 2U)
    BaseType_t status = xQueueSend(s_term_input_queue, (void *)&p_term_msg, portMAX_DELAY);
    if (pdTRUE != status)
    {
        vPortFree(p_term_msg);
        return FSP_ERR_ASSERTION;
    }
#endif /* BSP_CFG_RTOS */

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function converts a ULONG 64 value to a string.
 * @param[in] value                     Value is of type ULONG 64 bits.
 * @param[in] p_buf                     Pointer to string buffer.
 * @param[in] len                       Length of input buffer.
 * @retval    None.
 **********************************************************************************************************************/
static void conv_ul64_to_str(ULONG64 value, CHAR * p_buf, UINT len)
{
    UINT units = RESET_VALUE;
    UINT thousands = RESET_VALUE;
    UINT millions = RESET_VALUE;
    UINT billions = RESET_VALUE;

    /* Clean input buffer */
    memset(p_buf, NULL_CHAR, len);

    /* Check the value range */
    if (BILLION <= value)
    {
        billions = (UINT)(value / BILLION);
        millions = (UINT)((value / MILLION) % THOUSAND);
        thousands = (UINT)((value / THOUSAND) % THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u,%.3u,%.3u", billions, millions, thousands, units);
    }
    else if (MILLION <= value)
    {
        millions = (UINT)(value / MILLION);
        thousands = (UINT)((value / THOUSAND) % THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u,%.3u", millions, thousands, units);
    }
    else if(THOUSAND <= value)
    {
        thousands = (UINT)(value / THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u", thousands, units);
    }
    else
    {
        units = (UINT)value;
        snprintf(p_buf, len, "%u", units);
    }
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
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
static void handle_error(uint32_t err, const char * err_str)
{
    if (0U != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%X\r\n", (unsigned int)err);
        uart_console_flush();
        __BKPT(0);
    }
}

#endif  /* BSP_CFG_RTOS != 0U */
