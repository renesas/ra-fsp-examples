/***********************************************************************************************************************
 * File Name    : usb_hmsc_example_app.h
 * Description  : Public interface for the USB HMSC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HMSC_EXAMPLE_APP_H_
#define USB_HMSC_EXAMPLE_APP_H_

#include "usb_hmsc_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USB Host Mass Storage Class (HMSC)"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module  *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates basic functionalities of USB Host Mass Storage"\
                        "\r\nClass (HMSC) driver with the FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas"\
                        "\r\nFSP. FreeRTOS+FAT uses the underlying block media driver which utilizes USB HMSC driver"\
                        "\r\nto perform file operations on the USB storage devices. Information and error messages"\
                        "\r\nwill be printed on the terminal.\r\n"

#define EP_MENU         "\r\n\r\nMain menu:"\
                        "\r\n1. Write 10 KB of data to ra_usb.txt file"\
                        "\r\n2. Format USB drive"\
                        "\r\n3. Safely eject the USB drive"\
                        "\r\n4. Initialize FreeRTOS+FAT (Valid only after Safely eject option is executed)"\
                        "\r\nUser input: "

/* Single entry-point called by usb_hmsc_thread_entry.c */
void usb_hmsc_thread_entry_run(void);

#endif /* USB_HMSC_EXAMPLE_APP_H_ */
