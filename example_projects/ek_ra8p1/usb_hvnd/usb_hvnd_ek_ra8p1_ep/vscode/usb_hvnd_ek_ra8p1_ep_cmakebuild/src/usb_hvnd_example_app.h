/***********************************************************************************************************************
 * File Name    : usb_hvnd_example_app.h
 * Description  : Public interface for the USB HVND example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HVND_EXAMPLE_APP_H_
#define USB_HVND_EXAMPLE_APP_H_

#include "usb_hvnd_ep.h"

/* Macro for the example project version */
#define EP_VERSION      "1.0"

/* Macro for the module name */
#define MODULE_NAME     "r_usb_hvnd"

/* Macro for the EP banner */
#define BANNER_INFO     "\r\n*******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                        "\r\n*   Example Project Version %s                                   *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                              *"\
                        "\r\n*******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* Macro for the EP information */
#define EP_INFO         "\r\nThe project demonstrates the basic functionalities of USB Host Vendor class driver"\
                        "\r\non Renesas RA MCUs based on Renesas FSP. In this example, the application will configure"\
                        "\r\nthe MCU as a Vendor Host device. This host device will be connected to the USB Peripheral"\
                        "\r\nVendor device which is another RA board. After the enumeration is completed, the Vendor"\
                        "\r\nHost board will write 20 bytes of data to the Vendor Peripheral board and read back the"\
                        "\r\nsame data from the Vendor Peripheral board. The Vendor Host and Vendor Peripheral"\
                        "\r\napplications are designed to continuously transfer data between both the boards. The"\
                        "\r\nuser will be able to see all the operation sequence and status on the serial terminal."

/* Single entry-point called in usb_host_vendor_thread_entry.c */
void usb_hvnd_example_app_run(void);

#endif /* USB_HVND_EXAMPLE_APP_H_ */
