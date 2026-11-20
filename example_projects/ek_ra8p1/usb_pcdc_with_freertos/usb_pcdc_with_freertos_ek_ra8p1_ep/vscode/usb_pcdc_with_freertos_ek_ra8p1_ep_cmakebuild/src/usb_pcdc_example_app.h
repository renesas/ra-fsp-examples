/***********************************************************************************************************************
 * File Name    : usb_pcdc_example_app.h
 * Description  : Public interface for the USB PCDC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PCDC_EXAMPLE_APP_H_
#define USB_PCDC_EXAMPLE_APP_H_

#include "usb_pcdc_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_usb_pcdc_freertos"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module   *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of the USB PCDC driver on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP using FreeRTOS. In this project, the user will be"\
                        "\r\nable to use the RA board as a CDC to UART converter by connecting through a COM port"\
                        "\r\nfrom the host PC. The converter performs the data reception and data transmission"\
                        "\r\nasynchronously. The user can connect any UART device to receive the data typed on the"\
                        "\r\nserial terminal (e.g., Tera Term) and send any data to the converter which will show up"\
                        "\r\non the serial terminal. The user can also change the baud rate, stop bit and parity bit"\
                        "\r\nconfiguration at runtime through the serial terminal. The EP status will be displayed"\
                        "\r\non the terminal.\r\n"

/* Single entry-point called in usb_pcdc_thread_entry.c */
void usb_pcdc_example_app_run(void);

#endif /* USB_PCDC_EXAMPLE_APP_H_ */
