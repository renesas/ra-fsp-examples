/***********************************************************************************************************************
 * File Name    : usb_phid_example_app.h
 * Description  : Public interface for the USB PHID example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PHID_EXAMPLE_APP_H_
#define USB_PHID_EXAMPLE_APP_H_

#include "usb_phid_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_usb_phid"

#define BANNER_INFO     "\r\n*******************************************************************"\
                        "\r\n*   Renesas FSP TrustZone Example Project for "MODULE_NAME" Module   *"\
                        "\r\n*   Example Project Version %s                                   *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                              *"\
                        "\r\n*******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of USB PHID driver on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP. The project configures RA board as a keyboard."\
                        "\r\nOn pressing any key in the terminal, the MCU sends characters 'a' to 'z' followed"\
                        "\r\nby '1' to '9' and '0' to the host PC.\r\n"

#define EP_GUIDE        "\r\nPress any key in the terminal to send data"\
                        "\r\nUser input: "

#define EP_NOTE         "\r\nNote: Open Notepad to observe the received keyboard input.\r\n"

/* Single entry-point called in phid_main_thread_entry.c */
void usb_phid_example_app_run(void);

#endif /* USB_PHID_EXAMPLE_APP_H_ */
