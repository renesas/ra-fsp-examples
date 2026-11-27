/***********************************************************************************************************************
 * File Name    : http_client_example.h
 * Description  : Public interface for the Wi-Fi On Chip HTTP Client example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef HTTP_CLIENT_EXAMPLE_H_
#define HTTP_CLIENT_EXAMPLE_H_

#define EP_VERSION      "1.1"

#define MODULE_NAME     "rm_http_onchip_da16xxx"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module              *"\
						"\r\n*   Example Project Version "EP_VERSION"                                                *"\
						"\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
						"\r\n********************************************************************************"\
						"\r\nRefer to readme.txt file for more details on Example Project and"\
						"\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the functionalities of the HTTP Client"\
                        "\r\nwithin the DA16200 module employing the FSP Integrated Onchip DA16XXX"\
                        "\r\nMiddleware driver, and leveraging FreeRTOS on Renesas RA MCUs. EP uses the"\
                        "\r\nDA16200MOD module to connect to an Access Point and communicate with a web"\
                        "\r\nserver using HTTP methods (GET, PUT, POST). After sending each request,"\
                        "\r\nexample project waits for the HTTP response and prints it to the terminal."\
                        "\r\nStatus and failure messages are also displayed on the terminal.\r\n"

#define EP_MENU         "\r\nMenu:"\
                        "\r\nPress 1 to scan for Wi-Fi Access Points"\
                        "\r\nPress 2 to enter Wi-Fi Access Point's credentials"\
                        "\r\nUser input: "

#define SUB_MENU1       "\r\nSelect HTTP method:"\
                        "\r\n1. GET Request"\
                        "\r\n2. PUT Request"\
                        "\r\n3. POST Request"\
                        "\r\nUser input: "

#define SUB_MENU2       "\r\nSelect security type:"\
                        "\r\n0. Open"\
                        "\r\n1. WPA"\
						"\r\n2. WPA2"\
                        "\r\nUser input: "

/* Single entry point — call from http_client_thread_entry() */
void http_client_example_run(void);

#endif /* HTTP_CLIENT_EXAMPLE_H_ */
