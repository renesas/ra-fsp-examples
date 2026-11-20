/***********************************************************************************************************************
 * File Name    : usbx_phid_example_app.h
 * Description  : Public interface for the USBX PHID example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_PHID_EXAMPLE_APP_H_
#define USBX_PHID_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USBX_PHID"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                           *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of the USBX Peripheral"\
                        "\r\nHuman Interface Device (HID) module running on Renesas RA MCUs based on Renesas"\
                        "\r\nFSP using Azure RTOS. The RA board acts as an HID device and is configured as"\
                        "\r\na keyboard. When any key is pressed in the terminal, the RA board sends characters"\
                        "\r\n'a' to 'z' followed by '1' to '9' and '0' to the host PC. USB status and error"\
                        "\r\nmessages are displayed on the terminal.\r\n"

#define EP_GUIDE        "\r\n\r\nPress any key in the terminal to send data"\
                        "\r\nUser input: "

#define EP_NOTE         "\r\n\r\nNote: Open Notepad within 3 seconds to observe the received keyboard input"

/* Single entry-point called in usbx_phid_thread_entry.c */
void usbx_phid_example_app_run(void);

/* Function declaration */
void usbx_phid_uninit(void);

#endif /* USBX_PHID_EXAMPLE_APP_H_ */
