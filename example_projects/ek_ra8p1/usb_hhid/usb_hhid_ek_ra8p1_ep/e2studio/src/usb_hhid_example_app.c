/***********************************************************************************************************************
 * File Name    : usb_hhid_example_app.c
 * Description  : Example application layer for the USB HHID EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_hhid_thread_entry.c calls a single function: usb_hhid_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "usb_hhid_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static uint8_t s_buf[KEYBOARD_REPORT_SIZE] = {RESET_VALUE};  /* USB receive data */
static uint32_t s_setup_data = RESET_VALUE;
static uint16_t s_mxps = RESET_VALUE;

/* Private functions */
static fsp_err_t set_protocol(usb_instance_ctrl_t * p_ctrl, uint8_t protocol, uint8_t device_address);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB HHID example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hhid_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    usb_event_info_t * p_usb_event = NULL;
    BaseType_t err_queue = pdFALSE;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open basic USB driver */
    err = usb_init();
    handle_error(err, "usb_init failed\r\n");

    printf("\r\nPlease connect a wired USB keyboard...\r\n");

    while (true)
    {
        if (true == g_err_flag)
        {
            err = FSP_ERR_USB_FAILED;
            handle_error(err, "Failed to send USB event through the queue\r\n");
        }

        /* Receive message from queue */
        err_queue = xQueueReceive(g_event_queue, &p_usb_event, (portMAX_DELAY));
        if (pdTRUE != err_queue)
        {
            /* Unexpected queue receive failure */
            continue;
        }

        switch (p_usb_event->event)
        {
            case USB_STATUS_CONFIGURED:
            {
                printf("\r\nUSB configured successfully\r\n");

                /* Get max packet size for the connected HID device. The max packet size is set to the area
                 * Set the direction (USB_HID_IN/USB_HID_OUT) */
                err = usb_hhid_max_packet_size_get(&g_usb_basic_ctrl, &s_mxps, USB_HID_IN, p_usb_event->device_address);
                handle_error(err, "usb_hhid_max_packet_size_get failed\r\n");

                /* Send the HID request (set protocol) to HID device */
                err = set_protocol(&g_usb_basic_ctrl, BOOT_PROTOCOL, p_usb_event->device_address);
                handle_error(err, "set_protocol failed\r\n");
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                /* Send data received to queue */
                err_queue = xQueueSend(g_data_queue, &s_buf[RESET_VALUE], portMAX_DELAY);
                if (pdTRUE != err_queue)
                {
                    /* Unexpected queue send failure */
                    continue;
                }

                /* Send data receive request to HHID device */
                err = usb_read(&g_usb_basic_ctrl, s_buf, sizeof(s_buf), p_usb_event->device_address);

                /* FSP_ERR_USB_FAILED can occur when the keyboard is removed
                 * before the next read request is submitted. Allow the detach
                 * event to be processed instead of trapping the application. */
                if (FSP_ERR_USB_FAILED != err)
                {
                    handle_error(err, "usb_read failed\r\n");
                }
                break;
            }

            case USB_STATUS_REQUEST_COMPLETE:
            {
                printf("\r\nUSB status request completed successfully\r\n");

                /* Check for USB class request type is USB_HID_SET_PROTOCOL */
                if (USB_HID_SET_PROTOCOL == (p_usb_event->setup.request_type & USB_BREQUEST))
                {
                    /* Send data receive request to HHID device */
                    err = usb_read(&g_usb_basic_ctrl, s_buf, sizeof(s_buf), p_usb_event->device_address);
                    handle_error(err, "usb_read failed\r\n");
                }
                printf("\r\nEnter any key as specified from a to z, A to Z, 0 to 9, and special characters"
                       "\r\n(e.g., !, @, #, $, %%, ^, &, *, (, )) on keyboard\r\n");
                break;
            }

            case USB_STATUS_DETACH:
            {
                printf("\r\nUSB keyboard removed");
                printf("\r\nPlease connect a wired USB keyboard...\r\n");
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sends SetProtocol request to HID device.
 * @param[in] p_ctrl                    Pointer to usb_instance_ctrl_t structure.
 * @param[in] protocol                  Protocol type.
 * @param[in] device_address            Device address that sends this request.
 * @retval    FSP_SUCCESS               Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
static fsp_err_t set_protocol(usb_instance_ctrl_t * p_ctrl, uint8_t protocol, uint8_t device_address)
{
    fsp_err_t err = FSP_SUCCESS;

    usb_setup_t setup;
    setup.request_type = SET_PROTOCOL;      /* bRequestCode: SET_PROTOCOL, bmRequestType */
    setup.request_value = protocol;         /* wValue: Protocol type */
    setup.request_index = RESET_VALUE;      /* wIndex: Interface */
    setup.request_length = RESET_VALUE;     /* wLength: Zero */

    /* Request control transfer */
    err = usb_host_control_transfer(p_ctrl, &setup, (uint8_t *)&s_setup_data, device_address);

    return err;
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
