/***********************************************************************************************************************
 * File Name    : usb_pcdc_ep.c
 * Description  : Contains data structures and functions used in usb_pcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_pcdc_ep.h"

/* Global variables */
uint8_t g_user_data_buf[DATA_LEN] = {RESET_VALUE};
volatile usb_pcdc_linecoding_t g_line_coding;
uart_cfg_t g_uart_test_cfg;
volatile bool g_usb_attach = false;
app_uart_extended_cfg_t g_sci_extend_cfg;

/* Private variables */
static usb_pcdc_ctrllinestate_t s_control_line_state =
{
    .bdtr = 0,
    .brts = 0,
};
static volatile bool s_usb_opened = false;
static app_baud_setting_t s_baud_setting;

/* Private functions */
static void set_uart_line_coding_cfg(uart_cfg_t * p_uart_test_cfg,
                                     const volatile usb_pcdc_linecoding_t * p_line_coding);
static void uart_error_notify_from_isr(app_error_t error);
static void usb_error_notify(app_error_t error);
static void usb_event_notify(app_usb_event_t event);

/*******************************************************************************************************************//**
 * @brief     This function initializes the USB module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS == err)
    {
        s_usb_opened = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from the USB module.
 * @param[in]  p_api_ctrl               Pointer to the USB control block.
 * @param[out] p_buf                    Pointer to the buffer used to store the received data.
 * @param[in]  size                     Number of bytes to read.
 * @param[in]  destination              USB endpoint address to read data from.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Read(p_api_ctrl, p_buf, size, destination);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to the USB module.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @param[in] p_buf                     Pointer to the buffer containing data to write.
 * @param[in] size                      Number of bytes to write.
 * @param[in] destination               USB endpoint address to write data to.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t usb_write(usb_ctrl_t * const p_api_ctrl, uint8_t const * const p_buf, uint32_t size, uint8_t destination)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_Write(p_api_ctrl, p_buf, size, destination);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets USB peripheral control data.
 * @param[in]  p_api_ctrl               Pointer to the USB control block.
 * @param[out] p_buf                    Pointer to the buffer used to store the received control data.
 * @param[in]  size                     Number of bytes to get.
 * @retval     FSP_SUCCESS              Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
fsp_err_t usb_peri_control_data_get(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_PeriControlDataGet(p_api_ctrl, p_buf, size);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets USB peripheral control data.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @param[in] p_buf                     Pointer to the buffer containing control data to send.
 * @param[in] size                      Number of bytes to set.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t usb_peri_control_data_set(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_PeriControlDataSet(p_api_ctrl, p_buf, size);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the USB peripheral control transfer status.
 * @param[in] p_api_ctrl                Pointer to the USB control block.
 * @param[in] status                    Control transfer status to set.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t usb_peri_control_status_set(usb_ctrl_t * const p_api_ctrl, usb_setup_status_t status)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_USB_PeriControlStatusSet(p_api_ctrl, status);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened USB module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_deinit(void)
{
    /* De-initialize the opened USB module */
    if (s_usb_opened)
    {
        R_USB_Close(&g_basic_ctrl);
        s_usb_opened = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the UART module.
 * @param[in] p_api_ctrl                Pointer to the UART control block.
 * @param[in] p_cfg                     Pointer to the UART configuration.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t uart_init(uart_ctrl_t * const p_api_ctrl, uart_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = SCI_UART_OPEN(p_api_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to the UART module.
 * @param[in] p_api_ctrl                Pointer to the UART control block.
 * @param[in] p_src                     Pointer to the buffer containing data to write.
 * @param[in] bytes                     Number of bytes to write.
 * @retval    FSP_SUCCESS               Data written successfully.
 * @retval    Any other error codes     Failed to write data.
 **********************************************************************************************************************/
fsp_err_t uart_write(uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
    fsp_err_t err = FSP_SUCCESS;

    err = SCI_UART_WRITE(p_api_ctrl, p_src, bytes);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function calculates the UART baud rate settings.
 * @param[in]  baudrate                 Desired baud rate.
 * @param[in]  bitrate_modulation       Enable or disable bitrate modulation.
 * @param[in]  baud_rate_error_x_1000   Maximum allowed baud rate error multiplied by 1000.
 * @param[out] p_baud_setting           Pointer to the calculated baud rate settings.
 * @retval     FSP_SUCCESS              Baud rate settings calculated successfully.
 * @retval     Any other error codes    Failed to calculate the baud rate settings.
 **********************************************************************************************************************/
fsp_err_t uart_baud_calculate(uint32_t baudrate, bool bitrate_modulation, uint32_t baud_rate_error_x_1000,
                              app_baud_setting_t * const p_baud_setting)
{
    fsp_err_t err = FSP_SUCCESS;

    err = SCI_UART_BAUD_CAL(baudrate, bitrate_modulation, baud_rate_error_x_1000, p_baud_setting);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened UART module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void uart_deinit(void)
{
    /* Close the opened UART module */
    if (g_uart_ctrl.open)
    {
        SCI_UART_CLOSE(&g_uart_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief      This function updates the UART configuration based on USB PCDC line coding.
 * @param[out] p_uart_test_cfg          Pointer to the UART configuration to update.
 * @param[in]  p_line_coding            Pointer to the USB PCDC line coding.
 * @retval     None.
 **********************************************************************************************************************/
static void set_uart_line_coding_cfg(uart_cfg_t * p_uart_test_cfg, const volatile usb_pcdc_linecoding_t * p_line_coding)
{
    /* Set number of parity bits */
    switch (p_line_coding->b_parity_type)
    {
        case 1:
        {
            p_uart_test_cfg->parity = UART_PARITY_ODD;
            break;
        }

        case 2:
        {
            p_uart_test_cfg->parity = UART_PARITY_EVEN;
            break;
        }

        default:
        {
            p_uart_test_cfg->parity = UART_PARITY_OFF;
            break;
        }
    }

    /* Set number of data bits */
    switch (p_line_coding->b_data_bits)
    {
        case 7:
        {
            p_uart_test_cfg->data_bits = UART_DATA_BITS_7;
            break;
        }

        case 9:
        {
            p_uart_test_cfg->data_bits = UART_DATA_BITS_9;
            break;
        }

        case 8:
        default:
        {
            p_uart_test_cfg->data_bits = UART_DATA_BITS_8;
            break;
        }
    }

    /* Set number of stop bits */
    switch (p_line_coding->b_char_format)
    {
        case 2:
        {
            p_uart_test_cfg->stop_bits = UART_STOP_BITS_2;
            break;
        }

        default:
        {
            p_uart_test_cfg->stop_bits = UART_STOP_BITS_1;
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sends a UART error notification from the UART callback to the application event queue.
 * @param[in] error                     Application error to notify.
 * @retval    None.
 **********************************************************************************************************************/
static void uart_error_notify_from_isr(app_error_t error)
{
    queue_evt_t instance;

    instance.peripheral = PERIPHERAL_ERROR;
    instance.u.error = error;

    (void) xQueueSendFromISR(g_event_queue, &instance, NULL);
}

/*******************************************************************************************************************//**
 * @brief     This function sends a USB error notification to the application event queue.
 * @param[in] error                     Application error to notify.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_error_notify(app_error_t error)
{
    queue_evt_t instance;

    instance.peripheral = PERIPHERAL_ERROR;
    instance.u.error = error;

    (void) xQueueSend(g_event_queue, &instance, 0U);
}

/*******************************************************************************************************************//**
 * @brief     This function sends a USB status event notification to the application event queue.
 * @param[in] event                     USB status event to notify.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_event_notify(app_usb_event_t event)
{
    queue_evt_t instance = {0};

    instance.peripheral = PERIPHERAL_USB_EVENT;
    instance.u.usb.event = event;

    (void) xQueueSend(g_event_queue, &instance, 0U);
}

/*******************************************************************************************************************//**
 * @brief     This function handles USB PCDC events, class requests, data transfer events, and status notifications.
 * @param[in] p_pcdc_event              Pointer to USB event information.
 * @param[in] task                      Handle to the calling task (unused).
 * @param[in] state                     USB on/off state (unused).
 * @retval    None.
 **********************************************************************************************************************/
void usb_pcdc_callback(usb_event_info_t * p_pcdc_event, usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(task);
    FSP_PARAMETER_NOT_USED(state);

    queue_evt_t instance;

    switch (p_pcdc_event->event)
    {
        /* USB read complete event */
        case USB_STATUS_READ_COMPLETE:
        {
            instance.peripheral = PERIPHERAL_USB;
            instance.u.data_size = p_pcdc_event->data_size;

            /* Send the event to the application */
            (void) xQueueSend(g_event_queue, &instance, 0U);

            usb_event_notify(APP_USB_EVENT_READ_COMPLETE);
            break;
        }

        /* USB write complete event */
        case USB_STATUS_WRITE_COMPLETE:
        {
            BaseType_t err_semaphore = xSemaphoreGive(g_usb_tx_semaphore);
            if (pdTRUE != err_semaphore)
            {
                usb_error_notify(APP_ERROR_USB_TX_SEMAPHORE);
            }
            usb_event_notify(APP_USB_EVENT_WRITE_COMPLETE);
            break;
        }

        /* USB configured event */
        case USB_STATUS_CONFIGURED:
        {
            g_usb_attach = true;

            /* Read data from the terminal */
            fsp_err_t err = usb_read(&g_basic_ctrl, g_user_data_buf, DATA_LEN, USB_CLASS_PCDC);
            if (FSP_SUCCESS != err)
            {
                usb_error_notify(APP_ERROR_USB_READ);
            }
            usb_event_notify(APP_USB_EVENT_CONFIGURED);
            break;
        }

        /* USB class request */
        case USB_STATUS_REQUEST:
        {
            if (USB_PCDC_SET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                /* Get USB peripheral control request data */
                fsp_err_t err = usb_peri_control_data_get(&g_basic_ctrl, (uint8_t *)&g_line_coding,
                                                          LINE_CODING_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    usb_error_notify(APP_ERROR_USB_CONTROL_DATA_GET);
                    break;
                }
                usb_event_notify(APP_USB_EVENT_SET_LINE_CODING_REQUEST);
            }

            else if (USB_PCDC_GET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                /* Set the class request data */
                fsp_err_t err = usb_peri_control_data_set(&g_basic_ctrl, (uint8_t *)&g_line_coding, LINE_CODING_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    usb_error_notify(APP_ERROR_USB_CONTROL_DATA_SET);
                    break;
                }

                queue_evt_t event_instance = {0};

                event_instance.peripheral = PERIPHERAL_USB_EVENT;
                event_instance.u.usb.event = APP_USB_EVENT_GET_LINE_CODING;
                event_instance.u.usb.line_coding.bitrate = g_line_coding.dw_dte_rate;
                event_instance.u.usb.line_coding.char_format = g_line_coding.b_char_format;
                event_instance.u.usb.line_coding.parity_type = g_line_coding.b_parity_type;
                event_instance.u.usb.line_coding.data_bits = g_line_coding.b_data_bits;

                (void) xQueueSend(g_event_queue, &event_instance, 0U);
            }

            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                /* Get the status of the control signals */
                fsp_err_t err = usb_peri_control_data_get(&g_basic_ctrl, (uint8_t *)&s_control_line_state,
                                                          sizeof(usb_pcdc_ctrllinestate_t));
                if (FSP_SUCCESS != err)
                {
                    usb_error_notify(APP_ERROR_USB_CONTROL_DATA_GET);
                    break;
                }

                s_control_line_state.bdtr = (unsigned char)((p_pcdc_event->setup.request_value >> 0) & 0x01);
                s_control_line_state.brts = (unsigned char)((p_pcdc_event->setup.request_value >> 1) & 0x01);

                /* Toggle the line state if the flow control pin is set to a value
                 * (other than SCI_UART_INVALID_16BIT_PARAM) */
                if (SCI_UART_INVALID_16BIT_PARAM != g_uart_ctrl.flow_pin)
                {
                    R_BSP_PinAccessEnable();
                    R_BSP_PinWrite(g_uart_ctrl.flow_pin,
                                   (s_control_line_state.brts == 0) ? BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
                    R_BSP_PinAccessDisable();
                }

                /* Set the USB status as ACK response */
                err = usb_peri_control_status_set(&g_basic_ctrl, USB_SETUP_STATUS_ACK);
                if (FSP_SUCCESS != err)
                {
                    usb_error_notify(APP_ERROR_USB_CONTROL_STATUS_SET);
                    break;
                }

                queue_evt_t event_instance = {0};

                event_instance.peripheral = PERIPHERAL_USB_EVENT;
                event_instance.u.usb.event = APP_USB_EVENT_SET_CONTROL_LINE_STATE;
                event_instance.u.usb.bdtr = s_control_line_state.bdtr;
                event_instance.u.usb.brts = s_control_line_state.brts;

                (void) xQueueSend(g_event_queue, &event_instance, 0U);
            }

            else
            {
                /* Do nothing */
            }
            break;
        }

        /* USB class request complete */
        case USB_STATUS_REQUEST_COMPLETE:
        {
            if (USB_PCDC_SET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                g_sci_extend_cfg.p_baud_setting = &s_baud_setting;
                g_uart_test_cfg.p_extend = &g_sci_extend_cfg;

                uint32_t const baud_rate = g_line_coding.dw_dte_rate;

                if (INVALID_SIZE < baud_rate)
                {
                    bool const enable_bitrate_modulation = true;
                    uint32_t const error_rate_x_1000 = BAUD_ERROR_RATE;

                    fsp_err_t err = uart_baud_calculate(baud_rate, enable_bitrate_modulation,
                                                        error_rate_x_1000, &s_baud_setting);
                    if (FSP_SUCCESS != err)
                    {
                        usb_error_notify(APP_ERROR_UART_BAUD_CALCULATE);
                        break;
                    }
                }

                /* Update UART configuration */
                set_uart_line_coding_cfg(&g_uart_test_cfg, &g_line_coding);

                /* De-initialize UART */
                uart_deinit();

                /* Reinitialize UART with the updated configuration */
                fsp_err_t err = uart_init(&g_uart_ctrl, &g_uart_test_cfg);
                if (FSP_SUCCESS != err)
                {
                    usb_error_notify(APP_ERROR_UART_INIT);
                    break;
                }

                /* Send the updated line coding information to the application */
                queue_evt_t event_instance = {0};

                event_instance.peripheral = PERIPHERAL_USB_EVENT;
                event_instance.u.usb.event = APP_USB_EVENT_SET_LINE_CODING_COMPLETE;
                event_instance.u.usb.line_coding.bitrate = g_line_coding.dw_dte_rate;
                event_instance.u.usb.line_coding.char_format = g_line_coding.b_char_format;
                event_instance.u.usb.line_coding.parity_type = g_line_coding.b_parity_type;
                event_instance.u.usb.line_coding.data_bits = g_line_coding.b_data_bits;

                (void) xQueueSend(g_event_queue, &event_instance, 0U);
            }
            break;
        }

        /* Detach state */
        case USB_STATUS_DETACH:
        {
            /* Reset the USB attached flag indicating USB is removed */
            g_usb_attach = false;
            memset(g_user_data_buf, RESET_VALUE, sizeof(g_user_data_buf));
            usb_event_notify(APP_USB_EVENT_DETACH);
            break;
        }

        /* Suspend state */
        case USB_STATUS_SUSPEND:
        {
            /* Reset the USB attached flag while USB is suspended */
            g_usb_attach = false;
            memset(g_user_data_buf, RESET_VALUE, sizeof(g_user_data_buf));
            usb_event_notify(APP_USB_EVENT_SUSPEND);
            break;
        }

        /* Resume state */
        case USB_STATUS_RESUME:
        {
            /* Set the USB attached flag */
            g_usb_attach = true;
            usb_event_notify(APP_USB_EVENT_RESUME);
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback function for UART module. It handles UART events such as receive, transmit
 *            complete, and errors.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void user_uart_callback(uart_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            queue_evt_t linstance;

            linstance.peripheral = PERIPHERAL_UART;
            linstance.u.data_size = (2 == g_uart_ctrl.data_bytes) ? 2 : 1;

            QueueHandle_t * p_queue = (2 == g_uart_ctrl.data_bytes) ? &g_usb_tx_x2_queue : &g_usb_tx_queue;

            /* Send received UART data to the corresponding queue */
            if (pdTRUE != (xQueueSendFromISR(*p_queue, &p_args->data, NULL)))
            {
                uart_error_notify_from_isr(APP_ERROR_UART_RX_QUEUE);
                break;
            }

            /* Send the event to the application */
            (void) xQueueSendFromISR(g_event_queue, &linstance, NULL);
            break;
        }

        case UART_EVENT_TX_COMPLETE:
        {
            queue_evt_t linstance;

            BaseType_t err_semaphore = xSemaphoreGiveFromISR(g_uart_tx_mutex, NULL);
            if (pdTRUE != err_semaphore)
            {
                uart_error_notify_from_isr(APP_ERROR_UART_TX_SEMAPHORE);
                break;
            }

            /* Indicate that all bytes were transferred and USB reading can continue */
            linstance.peripheral = PERIPHERAL_USB;
            linstance.u.data_size = 0;

            /* Send the event to the application */
            (void) xQueueSendFromISR(g_event_queue, &linstance, NULL);
            break;
        }

        case UART_EVENT_ERR_PARITY:
        {
            uart_error_notify_from_isr(APP_ERROR_UART_PARITY);
            break;
        }

        case UART_EVENT_ERR_FRAMING:
        {
            uart_error_notify_from_isr(APP_ERROR_UART_FRAMING);
            break;
        }

        case UART_EVENT_ERR_OVERFLOW:
        {
            uart_error_notify_from_isr(APP_ERROR_UART_OVERFLOW);
            break;
        }

        case UART_EVENT_BREAK_DETECT:
        {
            uart_error_notify_from_isr(APP_ERROR_UART_BREAK_DETECT);
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}
