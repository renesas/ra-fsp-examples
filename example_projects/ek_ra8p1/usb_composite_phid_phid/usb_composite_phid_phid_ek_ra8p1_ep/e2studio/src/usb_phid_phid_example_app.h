/***********************************************************************************************************************
 * File Name    : usb_phid_phid_example_app.h
 * Description  : Public interface for the USB PHID + PHID Composite example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PHID_PHID_EXAMPLE_APP_H_
#define USB_PHID_PHID_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_usb_composite"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module       *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis project demonstrates the basic functionality of USB PHID + PHID Composite\r\n" \
                        "Device on Renesas RA MCUs based on Renesas FSP. The RA board is configured to\r\n"\
                        "function as either a Keyboard or a Mouse. On entering \"k\" from the terminal,\r\n"\
                        "the MCU will work as a Keyboard. The MCU sends characters a to z and numbers\r\n"\
                        "0 to 9 to the host PC. On entering \"m\" from the terminal, the MCU will work\r\n"\
                        "as a Mouse. The MCU sends Mouse coordinates to the host PC, making the pointer\r\n"\
                        "move in a square pattern on the host PC screen.\r\n"

#define EP_GUIDE        "\r\nEnter \"k\" character to run Keyboard emulator or \"m\" character to run "\
		                "Mouse emulator.\r\n"

/* Function declarations */
void phid_handler_thread_entry_run(void);
void terminal_thread_entry_run(void);

#endif /* USB_PHID_PHID_EXAMPLE_APP_H_ */
