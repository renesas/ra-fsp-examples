/***********************************************************************************************************************
 * File Name    : usb_pvnd_example_app.c
 * Description  : Example application layer for the USB PVND EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_peri_vendor_thread_entry.c calls a single function: usb_pvnd_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdint.h>
#include "usb_pvnd_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void process_usb_events(void);
static void process_usb_request(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PVND example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_pvnd_example_app_run(void)
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
    switch (gp_usb_pvnd_event->event)
    {
        /* Configured event */
        case USB_STATUS_CONFIGURED:
        {
            printf("\r\n\r\nUSB configured successfully");

            err = usb_pipes_get();
            handle_error(err, "usb_pipes_get failed");

            printf("\r\n\r\nBulk In pipe: %d, Bulk Out pipe: %d", g_bulk_in_pipe, g_bulk_out_pipe);
            break;
        }

        /* Read complete event */
        case USB_STATUS_READ_COMPLETE:
        {
            /* Check for bulk out pipe */
            if ((g_bulk_out_pipe == gp_usb_pvnd_event->pipe) && (FSP_ERR_USB_FAILED != gp_usb_pvnd_event->status))
            {
                /* Data comparison read from host */
                err = usb_data_check();
                handle_error(err, "usb_data_check failed");

                /* Write data back to host */
                err = usb_bulk_write(gp_usb_pvnd_event->data_size);
                handle_error(err, "usb_bulk_write failed");
                printf("\r\n\r\nUSB write is initiated from Peripheral Vendor class");
            }
            break;
        }

        /* Write complete event */
        case USB_STATUS_WRITE_COMPLETE:
        {
            /* Check for bulk in pipe */
            if ((g_bulk_in_pipe == gp_usb_pvnd_event->pipe) && (FSP_ERR_USB_FAILED != gp_usb_pvnd_event->status))
            {
                if (USB_APL_MXPS == g_max_packet_size)
                {
                    /* Send zero-length packet */
                    err = usb_bulk_write(RESET_VALUE);
                    handle_error(err, "usb_bulk_write failed");
                    printf("\r\n\r\nZero-length packet is successfully written from Peripheral Vendor");
                }
                else
                {
                    /* Read data back */
                    err = usb_bulk_read();
                    handle_error(err, "usb_bulk_read failed");
                    printf("\r\n\r\nUSB read is initiated from Peripheral Vendor class");
                }
            }
            break;
        }

        /* Receive class request */
        case USB_STATUS_REQUEST:
        {
            process_usb_request();
            break;
        }

        /* Complete class request */
        case USB_STATUS_REQUEST_COMPLETE:
        {
            if (USB_GET_VENDOR == (gp_usb_pvnd_event->setup.request_type & USB_BREQUEST))
            {
                /* Start reading data */
                err = usb_bulk_read();
                handle_error(err, "usb_bulk_read failed");
                printf("\r\n\r\nUSB read operation is initiated from Peripheral Vendor class");
            }
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
 * @brief     This function processes the vendor class request received from the host.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_usb_request(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (USB_SET_VENDOR_NO_DATA == (gp_usb_pvnd_event->setup.request_type & USB_BREQUEST))
    {
        /* Set ACK to host */
        err = usb_status_set();
        handle_error(err, "usb_status_set failed\r\n");
        printf("\r\n\r\nUSB ACK is successfully sent to host");
    }
    else if (USB_SET_VENDOR == (gp_usb_pvnd_event->setup.request_type & USB_BREQUEST))
    {
        /* Get data from host */
        err = usb_data_get();
        handle_error(err, "usb_data_get failed\r\n");
        printf("\r\n\r\nUSB data is received from host");
    }
    else if (USB_GET_VENDOR == (gp_usb_pvnd_event->setup.request_type & USB_BREQUEST))
    {
        /* Set data to host */
        err = usb_data_set();
        handle_error(err, "usb_data_set failed");
        printf("\r\n\r\nUSB data is successfully set in Peripheral");
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
