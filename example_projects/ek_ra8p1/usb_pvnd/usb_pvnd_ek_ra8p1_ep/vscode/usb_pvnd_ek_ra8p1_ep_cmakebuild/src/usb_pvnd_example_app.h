/***********************************************************************************************************************
 * File Name    : usb_pvnd_example_app.h
 * Description  : Public interface for the USB PVND example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PVND_EXAMPLE_APP_H_
#define USB_PVND_EXAMPLE_APP_H_

#include <stdio.h>
#include "usb_pvnd_ep.h"

/* Macro for the example project version */
#define EP_VERSION      "1.0"

/* Macro for the module name */
#define MODULE_NAME     "r_usb_pvnd"

/* Macro for the EP banner */
#define BANNER_INFO     "\r\n*******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                        "\r\n*   Example Project Version %s                                   *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                              *"\
                        "\r\n*******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* Macro for the EP information */
#define EP_INFO         "\r\nThe project demonstrates the basic functionalities of USB Peripheral Vendor class"\
                        "\r\ndriver on Renesas RA MCUs based on Renesas FSP. In this example, the application will"\
                        "\r\nconfigure the MCU as a Vendor Peripheral device. This Peripheral device will be"\
                        "\r\nconnected to the USB Host Vendor device which is another RA board. After the"\
                        "\r\nenumeration is completed, the Vendor Peripheral board will read 20 bytes of data from"\
                        "\r\nthe Vendor Host board and write back the same data to the Vendor Host board. The Vendor"\
                        "\r\nHost and Vendor Peripheral applications are designed to continuously transfer data"\
                        "\r\nbetween both the boards. The user will be able to see all the operation sequence and"\
                        "\r\nstatus on the serial terminal."

/* Single entry-point called in usb_peri_vendor_thread_entry.c */
void usb_pvnd_example_app_run(void);

#endif /* USB_PVND_EXAMPLE_APP_H_ */
