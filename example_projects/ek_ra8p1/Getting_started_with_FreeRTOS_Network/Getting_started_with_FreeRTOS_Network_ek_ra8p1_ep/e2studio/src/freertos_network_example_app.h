/***********************************************************************************************************************
 * File Name    : freertos_network_example_app.h
 * Description  : Public interface for the Getting started with FreeRTOS Network example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FREERTOS_NETWORK_EXAMPLE_APP_H_
#define FREERTOS_NETWORK_EXAMPLE_APP_H_

#include "ethernet_ep.h"

#define EP_VERSION       "1.0"

#define MODULE_NAME       "Ethernet using FreeRTOS+TCP"

#define BANNER_INFO      "\r\n********************************************************************************"\
                         "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module         *"\
                         "\r\n*   Example Project Version "EP_VERSION"                                                *"\
                         "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                         "\r\n********************************************************************************"\
                         "\r\nRefer to readme.txt file for more details on Example Project and              "\
                         "\r\nFSP User's Manual for more information about "MODULE_NAME"                    "\
                         "\r\n********************************************************************************\r\n"

#define ETH_PREINIT      "\r\n \r\n--------------------------------------------------------------------------------"\
                         "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Pre IP Init       "\
                         "\r\n--------------------------------------------------------------------------------\r\n"

#define ETH_POSTINIT     "\r\n \r\n--------------------------------------------------------------------------------"\
                         "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Post IP Init       "\
                         "\r\n--------------------------------------------------------------------------------\r\n"

#define MAIN_MENU        "\r\n\r\nMain menu:"\
                         "\r\n1. Ping Request:   Specify the IP address to ping (e.g., 1 192.168.123.4)"\
                         "\r\n2. TCP Client:     <Server_IP>:<Server_Port> (e.g., 2 192.168.123.4:1234)"\
                         "\r\n3. UDP Client:     <Server_IP>:<Server_Port> (e.g., 3 192.168.123.4:1234)"\
                         "\r\n4. DNS Resolution: <Domain_Name> (e.g., 4 www.example.com)"\
                         "\r\n5. Start HTTP Server and launch a web browser to access the user interface"\
                         "\r\nUser input: "

void freertos_network_example_app_run(void);
#endif /* FREERTOS_NETWORK_EXAMPLE_APP_H_ */
