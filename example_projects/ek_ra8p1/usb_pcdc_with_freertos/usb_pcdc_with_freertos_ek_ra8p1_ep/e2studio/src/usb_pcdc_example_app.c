/***********************************************************************************************************************
 * File Name    : usb_pcdc_example_app.c
 * Description  : Example application layer for the USB PCDC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_pcdc_thread_entry.c calls a single function: usb_pcdc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "usb_pcdc_example_app.h"
#include "console/uart_console.h"

/* Buffer used for USB transmission */
static uint8_t s_usb_tx_buffer[512] __attribute__((aligned(4)));

/* Private functions */
static void set_pcdc_line_coding(volatile usb_pcdc_linecoding_t * p_line_coding, const uart_cfg_t * p_uart_test_cfg);
static void handle_app_error(app_error_t error);
static void handle_usb_event(const app_usb_event_info_t * p_usb_event);
static void handle_uart_data(void);
static void handle_usb_data(uint32_t data_size);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PCDC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_pcdc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    queue_evt_t q_instance;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open USB instance */
    err = usb_init();
    handle_error(err, "usb_init failed\r\n");
    printf("\r\nUSB opened successfully\r\n");

    /* Open the UART with initial configuration */
    memcpy(&g_uart_test_cfg, &g_uart_cfg, sizeof(g_uart_test_cfg));
    memcpy(&g_sci_extend_cfg, g_uart_cfg.p_extend, sizeof(g_sci_extend_cfg));
    g_uart_test_cfg.p_extend = &g_sci_extend_cfg;
    g_uart_test_cfg.p_context = &g_uart_ctrl;

    err = uart_init(&g_uart_ctrl, &g_uart_test_cfg);
    handle_error(err, "uart_init failed\r\n");

    /* First time synchronization of the line coding between UART & USB */
    set_pcdc_line_coding(&g_line_coding, &g_uart_test_cfg);

    /* Initialize the semaphores to allow one synchronization event to occur */
    BaseType_t err_semaphore = xSemaphoreGive(g_uart_tx_mutex);
    if (pdTRUE != err_semaphore)
    {
        handle_error(FSP_ERR_INTERNAL, "xSemaphoreGive on g_uart_tx_mutex failed\r\n");
    }

    err_semaphore = xSemaphoreGive(g_usb_tx_semaphore);
    if (pdTRUE != err_semaphore)
    {
        handle_error(FSP_ERR_INTERNAL, "xSemaphoreGive on g_usb_tx_semaphore failed\r\n");
    }

    while (1)
    {
        /* Wait for an application event */
        BaseType_t err_queue = xQueueReceive(g_event_queue, (void * const)&q_instance, portMAX_DELAY);
        if (pdTRUE != err_queue)
        {
            /* Unexpected queue receive failure */
            continue;
        }

        /* Process the received application event */
        switch (q_instance.peripheral)
        {
            case PERIPHERAL_ERROR:
            {
                handle_app_error(q_instance.u.error);
                break;
            }

            case PERIPHERAL_USB_EVENT:
            {
                handle_usb_event(&q_instance.u.usb);
                break;
            }

            case PERIPHERAL_UART:
            {
                handle_uart_data();
                break;
            }

            case PERIPHERAL_USB:
            {
                handle_usb_data(q_instance.u.data_size);
                break;
            }

            case PERIPHERAL_NONE:
            default:
            {
                /* Do nothing */
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief      This function initializes USB PCDC line coding based on the UART configuration.
 * @param[out] p_line_coding            Pointer to the line coding structure to update.
 * @param[in]  p_uart_test_cfg          Pointer to the UART configuration.
 * @retval     None.
 **********************************************************************************************************************/
static void set_pcdc_line_coding(volatile usb_pcdc_linecoding_t * p_line_coding, const uart_cfg_t * p_uart_test_cfg)
{
    /* Configure the line coding based on initial settings */
    if (UART_STOP_BITS_1 == p_uart_test_cfg->stop_bits)
    {
        p_line_coding->b_char_format = 0;
    }
    else if (UART_STOP_BITS_2 == p_uart_test_cfg->stop_bits)
    {
        p_line_coding->b_char_format = 2;
    }
    else
    {
        /* Do nothing */
    }

    if (UART_PARITY_OFF == p_uart_test_cfg->parity)
    {
        p_line_coding->b_parity_type = 0;
    }
    else if (UART_PARITY_ODD == p_uart_test_cfg->parity)
    {
        p_line_coding->b_parity_type = 1;
    }
    else if (UART_PARITY_EVEN == p_uart_test_cfg->parity)
    {
        p_line_coding->b_parity_type = 2;
    }
    else
    {
        /* Do nothing */
    }

    if (UART_DATA_BITS_8 == p_uart_test_cfg->data_bits)
    {
        p_line_coding->b_data_bits = 8;
    }
    else if (UART_DATA_BITS_7 == p_uart_test_cfg->data_bits)
    {
        p_line_coding->b_data_bits = 7;
    }
    else if (UART_DATA_BITS_9 == p_uart_test_cfg->data_bits)
    {
        p_line_coding->b_data_bits = 9;
    }
    else
    {
        /* Do nothing */
    }

    /* Ideally put the baud rate into p_line_coding;
     * but FSP does not have an API to calculate the baud-rate
     * based on the UART configuration values */
}

/*******************************************************************************************************************//**
 * @brief     This function handles application error notifications.
 * @param[in] error                     Application error to handle.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_app_error(app_error_t error)
{
    switch (error)
    {
        case APP_ERROR_USB_TX_SEMAPHORE:
        {
            handle_error(FSP_ERR_INTERNAL, "USB TX semaphore operation failed\r\n");
            break;
        }

        case APP_ERROR_USB_READ:
        {
            handle_error(FSP_ERR_USB_FAILED, "usb_read failed\r\n");
            break;
        }

        case APP_ERROR_USB_CONTROL_DATA_GET:
        {
            handle_error(FSP_ERR_USB_FAILED, "usb_peri_control_data_get failed\r\n");
            break;
        }

        case APP_ERROR_USB_CONTROL_DATA_SET:
        {
            handle_error(FSP_ERR_USB_FAILED, "usb_peri_control_data_set failed\r\n");
            break;
        }

        case APP_ERROR_USB_CONTROL_STATUS_SET:
        {
            handle_error(FSP_ERR_USB_FAILED, "usb_peri_control_status_set failed\r\n");
            break;
        }

        case APP_ERROR_UART_BAUD_CALCULATE:
        {
            handle_error(FSP_ERR_INTERNAL, "uart_baud_calculate failed\r\n");
            break;
        }

        case APP_ERROR_UART_INIT:
        {
            handle_error(FSP_ERR_INTERNAL, "uart_init failed\r\n");
            break;
        }

        case APP_ERROR_UART_RX_QUEUE:
        {
            handle_error(FSP_ERR_INTERNAL, "UART receive queue operation failed\r\n");
            break;
        }

        case APP_ERROR_UART_TX_SEMAPHORE:
        {
            handle_error(FSP_ERR_INTERNAL, "UART TX semaphore operation failed\r\n");
            break;
        }

        case APP_ERROR_UART_PARITY:
        {
            handle_error(FSP_ERR_PARITY, "UART parity error\r\n");
            break;
        }

        case APP_ERROR_UART_FRAMING:
        {
            handle_error(FSP_ERR_FRAMING, "UART framing error\r\n");
            break;
        }

        case APP_ERROR_UART_OVERFLOW:
        {
            handle_error(FSP_ERR_RXBUF_OVERFLOW, "UART receive overflow\r\n");
            break;
        }

        case APP_ERROR_UART_BREAK_DETECT:
        {
            handle_error(FSP_ERR_BREAK_DETECT, "UART break detected\r\n");
            break;
        }

        case APP_ERROR_NONE:
        default:
        {
            /* Do nothing */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles USB status event notifications.
 * @param[in] p_usb_event               Pointer to the USB event information.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_usb_event(const app_usb_event_info_t * p_usb_event)
{
    switch (p_usb_event->event)
    {
        case APP_USB_EVENT_READ_COMPLETE:
        {
            printf("\r\nUSB status: USB_STATUS_READ_COMPLETE\r\n");
            break;
        }

        case APP_USB_EVENT_WRITE_COMPLETE:
        {
            printf("\r\nUSB status: USB_STATUS_WRITE_COMPLETE\r\n");
            break;
        }

        case APP_USB_EVENT_CONFIGURED:
        {
            printf("\r\nUSB status configured successfully\r\n");
            break;
        }

        case APP_USB_EVENT_GET_LINE_CODING:
        {
            printf("\r\nUSB status: USB_STATUS_REQUEST"
                   "\r\nRequest type: USB_PCDC_GET_LINE_CODING\r\n");

            printf("\r\n*********GET***********"
                   "\r\nbitrate = %lu"
                   "\r\nchar_format = %u"
                   "\r\nparity_type = %u"
                   "\r\ndata_bit = %u"
                   "\r\n***********************\r\n",
                   (unsigned long)p_usb_event->line_coding.bitrate,
                   (unsigned int)p_usb_event->line_coding.char_format,
                   (unsigned int)p_usb_event->line_coding.parity_type,
                   (unsigned int)p_usb_event->line_coding.data_bits);
            uart_console_flush();
            break;
        }

        case APP_USB_EVENT_SET_LINE_CODING_REQUEST:
        {
            printf("\r\nUSB status: USB_STATUS_REQUEST"
                   "\r\nRequest type: USB_PCDC_SET_LINE_CODING\r\n");
            break;
        }

        case APP_USB_EVENT_SET_LINE_CODING_COMPLETE:
        {
            printf("\r\nUSB status: USB_STATUS_REQUEST_COMPLETE"
                   "\r\nRequest type: USB_PCDC_SET_LINE_CODING\r\n");

            printf("\r\n*********SET***********"
                   "\r\nbitrate = %lu"
                   "\r\nchar_format = %u"
                   "\r\nparity_type = %u"
                   "\r\ndata_bit = %u"
                   "\r\n***********************\r\n",
                   (unsigned long)p_usb_event->line_coding.bitrate,
                   (unsigned int)p_usb_event->line_coding.char_format,
                   (unsigned int)p_usb_event->line_coding.parity_type,
                   (unsigned int)p_usb_event->line_coding.data_bits);
            uart_console_flush();
            break;
        }

        case APP_USB_EVENT_SET_CONTROL_LINE_STATE:
        {
            printf("\r\nUSB status: USB_STATUS_REQUEST"
                   "\r\nRequest type: USB_PCDC_SET_CONTROL_LINE_STATE\r\n");

            printf("\r\n******LINE STATE*******"
                   "\r\nbdtr = %u"
                   "\r\nbrts = %u"
                   "\r\n***********************\r\n",
                   (unsigned int)p_usb_event->bdtr,
                   (unsigned int)p_usb_event->brts);
            break;
        }

        case APP_USB_EVENT_DETACH:
        {
            printf("\r\nUSB status: USB_STATUS_DETACH\r\n");
            break;
        }

        case APP_USB_EVENT_SUSPEND:
        {
            printf("\r\nUSB status: USB_STATUS_SUSPEND\r\n");
            break;
        }

        case APP_USB_EVENT_RESUME:
        {
            printf("\r\nUSB status: USB_STATUS_RESUME\r\n");
            break;
        }

        case APP_USB_EVENT_NONE:
        default:
        {
            /* Do nothing */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function transfers received UART data to USB.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_uart_data(void)
{
    /* Select the queue based on the UART data size */
    QueueHandle_t * p_queue = (2 == g_uart_ctrl.data_bytes) ? &g_usb_tx_x2_queue : &g_usb_tx_queue;

    UBaseType_t msg_waiting_count = uxQueueMessagesWaiting(*p_queue);
    uint32_t const rx_data_size = (2 == g_uart_ctrl.data_bytes) ? 2U : 1U;

    if (0U < msg_waiting_count)
    {
        uint32_t const max_items = (uint32_t)(sizeof(s_usb_tx_buffer) / rx_data_size);

        /* Pull out as many items from the queue as possible */
        uint32_t const unload_count = (msg_waiting_count < max_items) ? (uint32_t)msg_waiting_count : max_items;

        /* Wait for the previous USB transfer to complete */
        BaseType_t err_semaphore = xSemaphoreTake(g_usb_tx_semaphore, portMAX_DELAY);
        if (pdTRUE != err_semaphore)
        {
            handle_error(FSP_ERR_INTERNAL, "xSemaphoreTake on g_usb_tx_semaphore failed\r\n");
        }

        for (uint32_t itr = 0U, idx = 0U; itr < unload_count; itr++, idx += rx_data_size)
        {
            if (pdTRUE != xQueueReceive(*p_queue, &s_usb_tx_buffer[idx], portMAX_DELAY))
            {
                handle_error(FSP_ERR_INTERNAL, "Did not receive expected count of characters\r\n");
            }
        }

        /* Write data to the host machine */
        fsp_err_t err = usb_write(&g_basic_ctrl, &s_usb_tx_buffer[0], unload_count * rx_data_size, USB_CLASS_PCDC);
        handle_error(err, "usb_write failed\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function transfers received USB data to UART.
 * @param[in] data_size                 Number of bytes received from USB.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_usb_data(uint32_t data_size)
{
    if (true == g_usb_attach)
    {
        if (0U < data_size)
        {
            /* Wait until previously queued data is completely transmitted */
            BaseType_t err_semaphore = xSemaphoreTake(g_uart_tx_mutex, portMAX_DELAY);
            if (pdTRUE != err_semaphore)
            {
                handle_error(FSP_ERR_INTERNAL, "xSemaphoreTake on g_uart_tx_mutex failed\r\n");
            }

            /* Write data to the UART TX pin */
            fsp_err_t err = uart_write(&g_uart_ctrl, g_user_data_buf, data_size);
            handle_error(err, "uart_write failed\r\n");
        }
        else
        {
            /* Buffer is physically transmitted since UART_EVENT_TX_COMPLETE was generated. */
            /* Continue to read data from USB. */
            /* The amount of data received will be known when USB_STATUS_READ_COMPLETE event occurs. */
            fsp_err_t err = usb_read(&g_basic_ctrl, g_user_data_buf, DATA_LEN, USB_CLASS_PCDC);
            handle_error(err, "usb_read failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        usb_deinit();
        uart_deinit();
        __BKPT(0);
    }
}
