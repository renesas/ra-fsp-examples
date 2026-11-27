/***********************************************************************************************************************
 * File Name    : NetX_wifi_example_app.h
 * Description  : Public interface for the NetX Wi-Fi example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_WIFI_EXAMPLE_APP_H_
#define NETX_WIFI_EXAMPLE_APP_H_

#define EP_VERSION          "1.0"

#define MODULE_NAME         "NetX_Wifi"

#define BANNER_INFO         "\r\n********************************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                           *"\
                            "\r\n*   Example Project Version "EP_VERSION\
                            "                                                *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                            "\r\n********************************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe example project demonstrates the functionalities of NetX Duo Wi-Fi Driver"\
		                    "\r\nusing Azure RTOS on Renesas RA MCUs using Renesas Silex UART Pmod."\
		                    "\r\nThe project utilizes some functionalities of Wi-Fi Onchip Silex module which are"\
							"\r\nnot supported by NetX Duo. The user can either scan Wi-Fi networks or manually"\
							"\r\nenter network credentials. After connecting, the EP performs DNS lookup and ping"\
							"\r\noperations on the user entered URL or IP address. It also demonstrates"\
							"\r\nTCP client socket communication by connecting to the user entered TCP server"\
							"\r\nIP address and port number. Once connected, the user can send 'on' or 'off'"\
							"\r\ncommands to control the on-board LED. The LED state is displayed on the"\
							"\r\nterminal, and acknowledgment messages are sent back to the server. Invalid"\
							"\r\ncommands do not affect the current LED state.\r\n"

#define EP_MENU             "Menu:"\
                            "\r\nPress 1 to scan for Wi-Fi Access Points"\
                            "\r\nPress 2 to enter Wi-Fi Access Point's credentials"\
                            "\r\nUser input: "

#define SUB_MENU            "\r\nSelect security type:"\
                            "\r\n0. Open"\
                            "\r\n1. WPA"\
                            "\r\n2. WPA2"\
                            "\r\nUser input: "

/* Function declarations */
void azure_wifi_thread_entry_run(void);
void terminal_thread_entry_run(void);

#endif /* NETX_WIFI_EXAMPLE_APP_H_ */
