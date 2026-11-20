/***********************************************************************************************************************
 * File Name    : usbx_hhid_example_app.h
 * Description  : Public interface for the USBX HHID example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_HHID_EXAMPLE_APP_H_
#define USBX_HHID_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USBX_HHID"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                           *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                                *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO        "\r\nThe example project demonstrates basic functionalities of USBX Host Human"\
                       "\r\nInterface Device module on Renesas RA MCUs based on Renesas FSP using Azure"\
                       "\r\nRTOS. A wired keyboard or mouse will be enumerated by the RA MCU, which is"\
                       "\r\nconfigured as USBX HHID. Upon connecting the keyboard or the mouse, the user"\
                       "\r\ncan press a key on the keyboard, move and press the mouse. The keyboard"\
                       "\r\ncharacter, mouse coordinates (x and y), mouse button pressed (left, right),"\
                       "\r\nerror and information messages will be displayed on the terminal."

/* Entry-points called by hhid_thread_entry.c */
void usbx_hhid_example_app_run(void);

#endif /* USBX_HHID_EXAMPLE_APP_H_ */
