/***********************************************************************************************************************
 * File Name    : usb_phid_phid_example_app.c
 * Description  : Example application layer for the USB PHID + PHID Composite EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - phid_handler_thread_entry calls a single function: phid_handler_thread_entry_run().
 *                - terminal_thread_entry calls a single function: terminal_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "usb_phid_phid_example_app.h"
#include "usb_phid_phid_ep.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PHID + PHID Composite handler thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void phid_handler_thread_entry_run(void)
{
    BaseType_t err_queue = pdFALSE;
    fsp_err_t err = FSP_SUCCESS;
    usb_event_info_t * p_usb_basic_event = NULL;

    /* Yield so the lower-priority terminal thread gets to open the terminal and print banner first */
    vTaskDelay(500 / portTICK_PERIOD_MS);

    /* Open USB instance */
    err = usb_init();
    handle_error(err, "usb_init failed\r\n");

    while (true)
    {
        /* Check if USB event is received */
        err_queue = xQueueReceive(g_event_queue, &p_usb_basic_event, (portMAX_DELAY));
        if (pdTRUE != err_queue)
        {
            handle_error(FSP_ERR_INTERNAL, "No USB Event received. Please check USB connection\r\n");
        }

        /* Check for USB event */
        switch (p_usb_basic_event->event)
        {
            /* Configured event */
            case USB_STATUS_CONFIGURED:
            {
                printf("\r\nUSB status configured successfully\r\n");
                break;
            }

            /* Receive Class Request */
            case USB_STATUS_REQUEST:
            {
                err = usb_enumeration(p_usb_basic_event);
                handle_error(err, "usb_enumeration failed\r\n");
                break;
            }

            /* Complete Class Request */
            case USB_STATUS_REQUEST_COMPLETE:
            {
                err = usb_status_update(p_usb_basic_event);
                handle_error(err, "usb_status_update failed\r\n");
                break;
            }

            /* Write complete event */
            case USB_STATUS_WRITE_COMPLETE:
            {
                err = usb_write_operation(p_usb_basic_event);
                handle_error(err, "usb_write_operation failed\r\n");
                break;
            }

            /* Suspend event */
            case USB_STATUS_SUSPEND:
            {
                printf("\r\nUSB STATUS: SUSPEND\r\n");
                break;
            }

            /* Detach event */
            case USB_STATUS_DETACH:
            {
                printf("\r\nUSB STATUS: DETACH\r\n");
                break;
            }
            default:
            {
                break;
            }
        }
        vTaskDelay(TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open - cannot print anything */
    }

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Prints EP info on the terminal */
    printf(EP_INFO);

    /* Delay for open USB */
    vTaskDelay(DELAY_2_SEC / portTICK_PERIOD_MS);

    printf(EP_GUIDE);

    while (true)
    {
        /* Check for user input */
        if (uart_console_has_line())
        {
            int command = getchar();

            /* Check for run Keyboard emulator */
            if (('K' == command) || ('k' == command))
            {
                printf("\r\nNOTE: Please open Notepad. The Keyboard emulator will start after 3 seconds\r\n");

                /* Delay for the user to open Notepad */
                vTaskDelay(DELAY_3_SEC / portTICK_PERIOD_MS);

                /* Set key flag */
                g_b_key_flag = true;

                printf("NOTE: Run Keyboard emulator\r\n");

                /* Delay for the user check output */
                vTaskDelay(DELAY_1_SEC / portTICK_PERIOD_MS);
            }
            /* Check for run Mouse emulator */
            else if (('M' == command) || ('m' == command))
            {
                printf("\r\nNOTE: See the movement of the Mouse pointer on the screen\r\n");

                /* Delay for the user to stand Mouse pointer */
                vTaskDelay(DELAY_3_SEC / portTICK_PERIOD_MS);

                /* Set Mouse flag */
                g_b_mouse_flag = true;

                printf("NOTE: Run Mouse emulator\r\n");

                /* Delay for the user check output */
                vTaskDelay(DELAY_1_SEC / portTICK_PERIOD_MS);
            }
            else
            {
                /* Do nothing */
            }
            printf(EP_GUIDE);
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
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        usb_deinit();
        __BKPT(0);
    }
}
