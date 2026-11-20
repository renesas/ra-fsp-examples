/***********************************************************************************************************************
 * File Name    : usb_pmsc_example_app.c
 * Description  : Example application layer for the USB PMSC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - pmsc_thread_entry.c calls a single function: usb_pmsc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "usb_pmsc_example_app.h"
#include "console/uart_console.h"

/* Global variable */
const usb_descriptor_t g_usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 gp_apl_string_table,            /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PMSC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_pmsc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    BaseType_t err_queue = pdFALSE;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Print EP information */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize USB module in PMSC (Peripheral Mass Storage Class) mode */
    err = usb_init();
    handle_error(err, "\r\nusb_init failed");

    while (true)
    {
        err_queue = usb_pmsc_event_get();
        if (pdTRUE != err_queue)
        {
            printf("\r\nNo USB event received. Please check the USB connection");
        }
        switch (gp_usb_event->event)
        {
            case USB_STATUS_CONFIGURED:
            {
                printf("\r\nUSB configured successfully");
                break;
            }

            case USB_STATUS_DETACH:
            case USB_STATUS_SUSPEND:
            {
                printf("\r\nUSB removed successfully");
                break;
            }

            default:
            {
                /* Do nothing */
                break;
            }
        }
        vTaskDelay(1);
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
        printf("\r\n\r\n[ERR] %s", err_str);
        printf("\r\nReturned Error Code: 0x%x", (unsigned int)err);
        uart_console_flush();
        usb_deinit();
        __BKPT(0);
    }
}
