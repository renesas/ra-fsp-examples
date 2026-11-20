/***********************************************************************************************************************
 * File Name    : usb_hcdc_example_app.c
 * Description  : Example application layer for the USB HCDC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_hcdc_thread_entry.c calls a single function: usb_hcdc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "usb_hcdc_example_app.h"
#include "r_usb_cstd_rtos.h"
#include "console/uart_console.h"

/* Private variables */
static usb_hcdc_linecoding_t s_serial_state;
static usb_hcdc_linecoding_t s_com_parm;

/* Send buffer */
static uint8_t s_snd_buf[8] BSP_ALIGN_VARIABLE(4) = {RESET_VALUE};

/* Receive buffer */
static uint8_t s_rcv_buf[512] BSP_ALIGN_VARIABLE(4) = {RESET_VALUE};

/* Dummy variable to send */
static uint8_t s_usb_dummy = RESET_VALUE;

/* Private functions */
static void set_line_coding(usb_instance_ctrl_t * p_ctrl, uint8_t device_address);
static void usb_data_process(usb_event_info_t * event_info);
static void set_control_line_state(usb_instance_ctrl_t * p_ctrl, uint8_t device_address);
static void get_line_coding(usb_instance_ctrl_t * p_ctrl, uint8_t device_address);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB HCDC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hcdc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    usb_event_info_t * event_info = NULL;
    BaseType_t err_queue = pdFALSE;
    s_snd_buf[0] = KIT_INFO;
    fsp_pack_version_t version = {RESET_VALUE};

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

    err = usb_init();
    handle_error(err, "usb_init failed\r\n");

    while (true)
    {
        if (g_err_flag)
        {
            err = FSP_ERR_USB_FAILED;
            handle_error(err, "Failed to send USB event through the queue\r\n");
        }

        /* Receive and process a message from the queue */
        err_queue = xQueueReceive(g_usb_queue, &event_info, portMAX_DELAY);
        if (pdTRUE != err_queue)
        {
            err = FSP_ERR_USB_FAILED;
            handle_error(err, "Failed to receive USB event through the queue\r\n");
        }

        switch (event_info->event)
        {
            case USB_STATUS_CONFIGURED:
            {
                /* CDC class request "SetLineCoding" */
                set_line_coding(&g_basic_ctrl, event_info->device_address);
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                /* CDC class communication data process */
                usb_data_process(event_info);
                break;
            }

            case USB_STATUS_WRITE_COMPLETE:
            {
                /* Start receiving report data */
                err = usb_read(&g_basic_ctrl, s_rcv_buf, CDC_READ_DATA_LEN, event_info->device_address);
                handle_error(err, "usb_read failed\r\n");
                break;
            }

            case USB_STATUS_REQUEST_COMPLETE:
            {
                /* Check complete request "SetLineCoding" */
                if (USB_CDC_SET_LINE_CODING == (event_info->setup.request_type & USB_BREQUEST))
                {
                    /* Send CDC class request "SetControlLineState" */
                    set_control_line_state(&g_basic_ctrl, event_info->device_address);
                }
                /* Check complete request "SetControlLineState" */
                else if (USB_CDC_SET_CONTROL_LINE_STATE == (event_info->setup.request_type & USB_BREQUEST))
                {
                    /* CDC class request "GetLineCoding" */
                    get_line_coding(&g_basic_ctrl, event_info->device_address);
                }
                else if (USB_CDC_GET_LINE_CODING == (event_info->setup.request_type & USB_BREQUEST))
                {
                    err = usb_write(&g_basic_ctrl, s_snd_buf, CDC_WRITE_DATA_LEN, event_info->device_address);
                    handle_error(err, "usb_write failed\r\n");
                }
                else
                {
                    /* Unsupported request */
                }
                break;
            }

            default:
            {
                /* Do nothing */
                break;
            }
        }

        /* Allow other tasks to run */
        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sets line coding information for host control transfer.
 * @param[in] p_ctrl                    Pointer to the control structure.
 * @param[in] device_address            USB device address.
 * @retval    None.
 **********************************************************************************************************************/
static void set_line_coding(usb_instance_ctrl_t * p_ctrl, uint8_t device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    usb_setup_t setup;

    /* bRequestCode: SET_LINE_CODING, bmRequestType */
    setup.request_type = SET_LINE_CODING;

    /* wValue: Zero */
    setup.request_value = 0U;

    /* wIndex: Interface */
    setup.request_index = 0U;

    /* Data: Line Coding Structure */
    setup.request_length = LINE_CODING_LENGTH;

    s_com_parm.dwdte_rate = (uint32_t)USB_HCDC_SPEED_9600;
    s_com_parm.bdata_bits = USB_HCDC_DATA_BIT_8;
    s_com_parm.bchar_format = USB_HCDC_STOP_BIT_1;
    s_com_parm.bparity_type = USB_HCDC_PARITY_BIT_NONE;

    /* Request control transfer */
    err = usb_host_control_transfer(p_ctrl, &setup, (uint8_t *)&s_com_parm, device_address);
    handle_error(err, "usb_host_control_transfer failed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs data processing with a peripheral device.
 * @param[in] event_info                Data process in HCDC event type.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_data_process(usb_event_info_t * event_info)
{
    fsp_err_t err = FSP_SUCCESS;

    if (USB_CLASS_HCDC == event_info->type)
    {
        if (RESET_VALUE < event_info->data_size)
        {
            /* Send the response command to the USB device */
            switch (s_snd_buf[0])
            {
                case KIT_INFO:
                {
                    s_snd_buf[0] = NEXT_STEPS;
                    break;
                }

                case NEXT_STEPS:
                {
                    s_snd_buf[0] = CARRIAGE_RETURN;
                    break;
                }

                case CARRIAGE_RETURN:
                {
                    s_snd_buf[0] = KIT_INFO;
                    break;
                }

                default:
                {
                    /* Do nothing */
                    break;
                }
            }

            err = usb_write(&g_basic_ctrl, s_snd_buf, CDC_WRITE_DATA_LEN, event_info->device_address);
            handle_error(err, "usb_write failed\r\n");
            printf("\r\nReceived data: %s", s_rcv_buf);
        }
        else
        {
            /* Send the data reception request when the zero-length packet is received */
            err = usb_read(&g_basic_ctrl, s_rcv_buf, event_info->data_size, event_info->device_address);
            handle_error(err, "usb_read failed\r\n");
        }
    }
    else
    {
        /* Start receiving the "SerialState" class notification */
        err = usb_read(&g_basic_ctrl, (uint8_t *) &s_serial_state, USB_HCDC_SERIAL_STATE_MSG_LEN,
                       event_info->device_address);
        handle_error(err, "usb_read failed\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sets control line state information for host control transfer.
 * @param[in] p_ctrl                    Pointer to control structure.
 * @param[in] device_address            USB device address.
 * @retval    None.
 **********************************************************************************************************************/
static void set_control_line_state(usb_instance_ctrl_t * p_ctrl, uint8_t device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    usb_setup_t setup;

    /* bRequestCode: SET_CONTROL_LINE_STATE, bmRequestType */
    setup.request_type = SET_CONTROL_LINE_STATE;

    /* wValue: Zero */
    setup.request_value = 0U;

    /* wIndex: Interface */
    setup.request_index = 0U;

    /* wLength: Zero */
    setup.request_length = 0U;

    err = usb_host_control_transfer(p_ctrl, &setup, &s_usb_dummy, device_address);
    handle_error(err, "usb_host_control_transfer failed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function gets line coding information for host control transfer.
 * @param[in] p_ctrl                    Pointer to control structure.
 * @param[in] device_address            USB device address.
 * @retval    None.
 **********************************************************************************************************************/
static void get_line_coding(usb_instance_ctrl_t * p_ctrl, uint8_t device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    usb_setup_t setup;

    /* bRequestCode: GET_LINE_CODING, bmRequestType */
    setup.request_type = GET_LINE_CODING;

    /* wValue: Zero */
    setup.request_value = 0U;

    /* wIndex: Interface */
    setup.request_index = 0U;

    /* Data: Line Coding Structure */
    setup.request_length = LINE_CODING_LENGTH;

    /* Request control transfer */
    err = usb_host_control_transfer(p_ctrl, &setup, (uint8_t *)&s_com_parm, device_address);
    handle_error(err, "usb_host_control_transfer failed\r\n");
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
        __BKPT(0);
    }
}
