/***********************************************************************************************************************
 * File Name    : usb_hhid_example_app.h
 * Description  : Public interface for the USB HHID example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HHID_EXAMPLE_APP_H_
#define USB_HHID_EXAMPLE_APP_H_

#include "usb_hhid_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USB HHID Class"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                      *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of USB Host Human Interface"\
                        "\r\nDevice Class (HHID) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP."\
                        "\r\nThe USB HHID driver configures keyboard as an HHID device. On pressing a key from"\
                        "\r\nthe keyboard, the received keycode is decoded and displayed on the terminal."\
                        "\r\nAdditionally, error and info messages will also be printed on the terminal.\r\n"

/* Single entry-point called in usb_hhid_thread_entry.c */
void usb_hhid_example_app_run(void);

#endif /* USB_HHID_EXAMPLE_APP_H_ */
