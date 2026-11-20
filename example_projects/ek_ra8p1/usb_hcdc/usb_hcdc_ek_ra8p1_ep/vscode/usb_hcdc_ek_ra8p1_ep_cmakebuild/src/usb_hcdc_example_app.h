/***********************************************************************************************************************
 * File Name    : usb_hcdc_example_app.h
 * Description  : Public interface for the USB HCDC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HCDC_EXAMPLE_APP_H_
#define USB_HCDC_EXAMPLE_APP_H_

#include "usb_hcdc_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_usb_hcdc"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                          *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of the USB Host"\
                        "\r\nCommunication Device Class (HCDC) driver on Renesas RA MCUs using two RA boards."\
                        "\r\nBoard 1, running the USB HCDC example project, communicates with Board 2, running"\
                        "\r\nthe USB PCDC example project. Board 1 initiates the communication by sending commands"\
                        "\r\nto Board 2, which responds with the requested data. Board 1 then prints the received"\
                        "\r\ndata on the terminal.\r\n"

/* Single entry-point called in usb_hcdc_thread_entry.c */
void usb_hcdc_example_app_run(void);

#endif /* USB_HCDC_EXAMPLE_APP_H_ */
