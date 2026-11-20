/***********************************************************************************************************************
 * File Name    : usb_hvnd_example_app.c
 * Description  : Example application layer for the USB HVND EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_host_vendor_thread_entry.c calls a single function: usb_hvnd_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "usb_hvnd_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void process_usb_events(void);
static void process_usb_request_complete(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB HVND example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hvnd_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize the UART console used by printf */
    if (FSP_SUCCESS != uart_console_init())
    {
        /* UART failed to open - cannot print anything */
        return;
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize USB module */
    err = usb_init();
    handle_error(err, "usb_init failed");
    printf("\r\n\r\nUSB opened successfully");

    while (true)
    {
        /* Handle error if sending the USB event to the queue failed */
        if (g_b_queue_err)
        {
            handle_error(FSP_ERR_ABORTED, "Failed to send USB event to the queue");
        }

        /* Check if the USB event is received */
        err = queue_receive();
        handle_error(err, "queue_receive failed");

        process_usb_events();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes the USB event received from the queue.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_usb_events(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for USB event */
    switch (gp_usb_hvnd_event->event)
    {
        /* Configured event */
        case USB_STATUS_CONFIGURED:
        {
            printf("\r\n\r\nUSB configured successfully");

            err = usb_pipes_get();
            handle_error(err, "usb_pipes_get failed");

            printf("\r\n\r\nBulk In pipe: %d, Bulk Out pipe: %d", g_bulk_in_pipe, g_bulk_out_pipe);

            /* Request the control transfer without data */
            err = usb_control_transfer(SET_VENDOR_NO_DATA, RESET_VALUE);
            handle_error(err, "usb_control_transfer failed");
            printf("\r\n\r\nUSB host control transfer for USB_SET_VENDOR_NO_DATA is initiated from Host Vendor class");
            break;
        }

        /* Read complete event */
        case USB_STATUS_READ_COMPLETE:
        {
            /* Check for bulk in pipe */
            if ((g_bulk_in_pipe == gp_usb_hvnd_event->pipe) && (FSP_ERR_USB_FAILED != gp_usb_hvnd_event->status))
            {
                /* Data comparison read from device */
                err = usb_data_check();
                handle_error(err, "usb_data_check failed\r\n");

                /* Write data back to device */
                err = usb_bulk_write(gp_usb_hvnd_event->data_size);
                handle_error(err, "usb_bulk_write failed");
                printf("\r\n\r\nUSB write is initiated from Host Vendor class");
            }
            break;
        }

        /* Write complete event */
        case USB_STATUS_WRITE_COMPLETE:
        {
            /* Check for bulk out pipe */
            if ((g_bulk_out_pipe == gp_usb_hvnd_event->pipe) && (FSP_ERR_USB_FAILED != gp_usb_hvnd_event->status))
            {
                if (USB_APL_MXPS == g_max_packet_size)
                {
                    /* Send zero-length packet */
                    err = usb_bulk_write(RESET_VALUE);
                    handle_error(err, "usb_bulk_write failed");
                    printf("\r\n\r\nZero-length packet is successfully written from Host Vendor");
                }
                else
                {
                    /* Read data back */
                    err = usb_bulk_read();
                    handle_error(err, "usb_bulk_read failed");
                    printf("\r\n\r\nUSB read is initiated from Host Vendor class");
                }
            }
            break;
        }

        /* Complete class request */
        case USB_STATUS_REQUEST_COMPLETE:
        {
            process_usb_request_complete();
            break;
        }

        /* Detach event */
        case USB_STATUS_DETACH:
        {
            printf("\r\n\r\nUSB status: Detach");
            break;
        }

        /* Suspend event */
        case USB_STATUS_SUSPEND:
        {
            printf("\r\n\r\nUSB status: Suspend");
            break;
        }

        /* Resume event */
        case USB_STATUS_RESUME:
        {
            printf("\r\n\r\nUSB status: Resume");
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
 * @brief     This function processes the completed vendor class request and starts the next step of the sequence.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_usb_request_complete(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (USB_SET_VENDOR_NO_DATA == (gp_usb_hvnd_event->setup.request_type & USB_BREQUEST))
    {
        /* Request the control transfer to send data */
        err = usb_control_transfer(SET_VENDOR, REQ_SIZE);
        handle_error(err, "usb_control_transfer failed");
        printf("\r\n\r\nUSB host control transfer for USB_SET_VENDOR is initiated from Host Vendor class");
    }
    else if (USB_SET_VENDOR == (gp_usb_hvnd_event->setup.request_type & USB_BREQUEST))
    {
        /* Request the control transfer to receive data */
        err = usb_control_transfer(GET_VENDOR, REQ_SIZE);
        handle_error(err, "usb_control_transfer failed");
        printf("\r\n\r\nUSB host control transfer for USB_GET_VENDOR is initiated from Host Vendor class");
    }
    else if (USB_GET_VENDOR == (gp_usb_hvnd_event->setup.request_type & USB_BREQUEST))
    {
        /* Start writing data to the bulk out pipe */
        buffer_init(g_buf, BUF_SIZE);
        err = usb_bulk_write((uint32_t)(BUF_SIZE - g_max_packet_size));
        handle_error(err, "usb_bulk_write failed");
        printf("\r\n\r\nUSB write operation is initiated from Host Vendor class");
    }
    else
    {
        /* Unsupported request */
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
        printf("\r\nReturned Error Code: 0x%x", (unsigned int)err);
        uart_console_flush();
        usb_deinit();
        __BKPT(0);
    }
}
