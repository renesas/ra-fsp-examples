/***********************************************************************************************************************
 * File Name    : usbx_host_hub_example_app.h
 * Description  : Public interface for the USBX Host Hub example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_HOST_HUB_EXAMPLE_APP_H_
#define USBX_HOST_HUB_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USBX_Host_Hub"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                       *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates basic functionalities of the USBX Host Hub driver on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP using Azure RTOS. The USBX HHID driver enumerates"\
                        "\r\nHuman Interface Devices limited to keyboards and mice using USBX middleware. The user"\
                        "\r\nwill be able to use an RA board as a host device. Upon connecting a keyboard or mouse"\
                        "\r\nto the hub, the user can press a key on the keyboard or move and click the mouse."\
                        "\r\nThe keyboard character, mouse position (x, y), mouse button clicked (left or right),"\
                        "\r\nerror and information messages will be displayed on the terminal.\r\n"

/* Single entry-point called in usbx_host_hid_key_board_hub_thread_entry.c */
void usbx_host_hid_key_board_hub_example_app_run(void);

/* Single entry-point called in usbx_host_hid_mouse_hub_thread_entry.c */
void usbx_host_hid_mouse_hub_example_app_run(void);

/* Function declaration */
void usbx_host_hub_hid_class_uninit(void);

#endif /* USBX_HOST_HUB_EXAMPLE_APP_H_ */
