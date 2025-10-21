/***********************************************************************************************************************
 * File Name    : usbx_pprn_read_thread_entry.c
 * Description  : Contains macros and functions used for usbx_pprn_read functionality.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usbx_pprn_read_thread.h"
#include "r_usb_basic.h"
#include "common_utils.h"
#include "usbx_pprn_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_pprn_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Exported global variables and functions (to be accessed by other files)
 **********************************************************************************************************************/
extern UX_DEVICE_CLASS_PRINTER *device_printer;

/*******************************************************************************************************************//**
 * Private global variables and functions
 **********************************************************************************************************************/
static char print_buff[BUFFER_SIZE_MIN] = {RESET_VALUE};
static uint8_t device_printer_buffer[PRINT_REQUEST_SIZE] = {RESET_VALUE};

/* USBX Printer Read Thread entry function */
void usbx_pprn_read_thread_entry(void)
{
    UINT status             = UX_SUCCESS;
    uint32_t actual_length  = RESET_VALUE;
    UINT index              = RESET_VALUE;

    while (true)
    {
        /* Check status of device printer read and actual length of data */
        while ((status != UX_SUCCESS) || (actual_length == RESET_VALUE))
        {
            if (device_printer == UX_NULL)
            {
                /* Wait for a while before the next check */
                tx_thread_sleep(SLEEP_TIME_10);
            }
            else
            {
                /* Read data from the printer class */
                status = ux_device_class_printer_read(device_printer, device_printer_buffer,\
                                                      PRINT_REQUEST_SIZE, (ULONG *) &actual_length);
                /* Error handle */
                if (UX_SUCCESS != status)
                {
                    PRINT_ERR_STR("ux_device_class_printer_read API failed");
                    ERROR_TRAP(status);
                }
            }
        }

        /* Print the received data */
        sprintf(print_buff, "Printing %ld bytes:", (ULONG) actual_length);
        PRINT_INFO_STR(print_buff);
        memset(print_buff, RESET_VALUE, sizeof(print_buff));

        /* Print the data page wise */
        for (index = RESET_VALUE; index < actual_length; index++)
        {
#ifdef USE_PRINTER
            if ((index & MAX_PRINT_LINE) == RESET_VALUE)
            {
                PRINT_NEXT_LINE("");
            }
#endif
            PRINT_DATA(device_printer_buffer[index]);
        }

        /* Reset the device printer buffer and actual length to print next data */
        memset(device_printer_buffer, RESET_VALUE, PRINT_REQUEST_SIZE);
        actual_length = RESET_VALUE;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_pprn_ep)
 **********************************************************************************************************************/
