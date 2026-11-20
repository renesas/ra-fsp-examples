/***********************************************************************************************************************
 * File Name    : usb_multiport_example_app.h
 * Description  : Public interface for the USB Multiport example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_MULTIPORT_EXAMPLE_APP_H_
#define USB_MULTIPORT_EXAMPLE_APP_H_

#include "usb_multiport_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USB Multiport"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                       *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of USB Multiport on Renesas"\
                        "\r\nRA MCUs based on Renesas FSP using FreeRTOS. USB PCDC is configured for Full-Speed"\
                        "\r\nwith a lower priority than USB HMSC, which is configured for High-Speed. The terminal"\
                        "\r\nis used for the input and status display. Tera Term is used to display the output and"\
                        "\r\nalso to capture the data from it based on user input on the terminal. The project checks"\
                        "\r\nfor the predefined file and reads the content and sends it to USB PCDC for display on"\
                        "\r\nTera Term. Data captured from Tera Term (max 512 bytes) will be written to the file."\
                        "\r\nThe EP information and status will be displayed on the terminal.\r\n"

#define EP_MENU         "\r\n\r\nMain menu:"\
                        "\r\n1. Read the content from file"\
                        "\r\n2. Write the content to file"\
                        "\r\nUser input: "

/* Single entry-point called by usb_hmsc_thread_entry.c */
void usb_hmsc_thread_entry_run(void);

/* Single entry-point called by usb_pcdc_thread_entry.c */
void usb_pcdc_thread_entry_run(void);

#endif /* USB_MULTIPORT_EXAMPLE_APP_H_ */
