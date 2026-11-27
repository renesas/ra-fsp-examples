/***********************************************************************************************************************
 * File Name    : wifi_example_app.h
 * Description  : Public interface for the Wi-Fi example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef WIFI_EXAMPLE_APP_H_
#define WIFI_EXAMPLE_APP_H_

#define EP_VERSION      "1.1"

#define MODULE_NAME     "rm_wifi_onchip_silex"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module  *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the functionality of Wi-Fi OnChip Silex on"\
                        "\r\nRenesas RA MCUs using Renesas Silex UART Pmod. The user can either scan Wi-Fi"\
                        "\r\nnetworks or manually enter network credentials. After connecting, the EP performs"\
                        "\r\nDNS lookup and ping operations on the user entered URL or IP address. It also"\
                        "\r\ndemonstrates TCP client socket communication by connecting to the user entered"\
                        "\r\nTCP server IP address and port number. Once connected, the server can send 'on'"\
                        "\r\nor 'off' commands to control the on-board LED. The LED state is displayed on"\
                        "\r\nthe terminal, and acknowledgment messages are sent back to the server. Invalid"\
                        "\r\ncommands do not affect the current LED state.\r\n"

#define EP_MENU         "\r\nMenu:"\
                        "\r\nPress 1 to scan for Wi-Fi Access Points"\
                        "\r\nPress 2 to enter Wi-Fi Access Point's credentials"\
                        "\r\nUser input: "
#if defined (BOARD_RA2E2_EK)
#define EP_NOTE         "\r\nWi-Fi Access Point scanning is not available for this board."\
                        "\r\nPlease press 2 for entering Wi-Fi Access Point's credentials manually.\r\n"
#endif /*BOARD_RA2E2_EK */

#define SUB_MENU2       "\r\nSelect security type:"\
                        "\r\n0. Open"\
                        "\r\n1. WPA"\
                        "\r\n2. WPA2"\
                        "\r\nUser input: "

#define EP_GUIDE        "\r\nSend 'on' or 'off' message from server.\r\n"

#define EP_ERROR        "Connection failure.\r\n"\
		                "Please check TCP server settings and network connectivity\r\n"

/* Single entry-point called in wifi_thread_entry.c */
void wifi_example_app_run(void);

#endif /* WIFI_EXAMPLE_APP_H_ */
