/***********************************************************************************************************************
 * File Name    : usb_phid_example_app.c
 * Description  : Example application layer for the USB PHID EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - phid_main_thread_entry.c calls a single function: usb_phid_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "usb_phid_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PHID example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_phid_example_app_run(void)
{
    BaseType_t err_queue = pdFALSE;
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize USB module */
    err = usb_init();
    handle_error(err, "usb_init failed");

    while (true)
    {
        /* Check if the USB event is received */
        err_queue = xQueueReceive(g_event_queue, &gp_usb_phid_event, portMAX_DELAY);
        if (pdTRUE != err_queue)
        {
            /* Unexpected queue receive failure */
            continue;
        }

        /* Check for USB event */
        switch (gp_usb_phid_event->event)
        {
            /* Configured event */
            case USB_STATUS_CONFIGURED:
            {
                printf("\r\nUSB status: Configured successfully\r\n");
                break;
            }

            /* Receive class request */
            case USB_STATUS_REQUEST:
            {
                err = usb_enumeration();
                handle_error(err, "usb_enumeration failed");
                break;
            }

            /* Complete class request */
            case USB_STATUS_REQUEST_COMPLETE:
            {
                err = usb_status_update();
                handle_error(err, "usb_status_update failed");

                if (USB_SET_REPORT == (gp_usb_phid_event->setup.request_type & USB_BREQUEST))
                {
                    if (CAPS_LOCK_BIT == (g_numlock & CAPS_LOCK_BIT))
                    {
                        printf("\r\nCaps Lock is ON\r\n");
                    }
                    else
                    {
                        printf("\r\nCaps Lock is OFF\r\n");
                    }
                    printf(EP_GUIDE);
                }
                break;
            }

            /* Write complete event */
            case USB_STATUS_WRITE_COMPLETE:
            {
                vTaskDelay(DELAY / portTICK_PERIOD_MS);
                err = usb_write_operation();
                handle_error(err, "usb_write_operation failed");
                break;
            }

            /* Suspend event */
            case USB_STATUS_SUSPEND:
            {
                printf("\r\nUSB status: Suspend\r\n");
                break;
            }

            /* Detach event */
            case USB_STATUS_DETACH:
            {
                printf("\r\nUSB status: Detach\r\n");
                break;
            }

            default:
            {
                /* Do nothing */
                break;
            }
        }

        vTaskDelay(TASK_DELAY);
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
