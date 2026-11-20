/***********************************************************************************************************************
 * File Name    : Getting_started_with_Azure_NetX_example_app.h
 * Description  : Public interface for the Getting_started_with_Azure_NetX example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GETTING_STARTED_WITH_AZURE_NETX_EXAMPLE_APP_H_
#define GETTING_STARTED_WITH_AZURE_NETX_EXAMPLE_APP_H_

#include "net_link_ep.h"
#include "net_app_ep.h"
#include "led_control_ep.h"

#define EP_VERSION                  "1.0"

#define MODULE_NAME                 "NetX"

#define BANNER_INFO                 "\r\n******************************************************************"\
                                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                  *"\
                                    "\r\n*   Example Project Version "EP_VERSION\
                                    "                                  *"\
                                    "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                    "\r\n******************************************************************"\
                                    "\r\nRefer to readme.txt file for more details on Example Project and"\
                                    "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                     "\r\nThe example project demonstrates Azure NetX Duo networking operations"\
                                    "\r\non the Ethernet physical layer of the RA MCU. The project will perform"\
                                    "\r\nvarious network operations using the NetX Duo network stack. The RA board"\
                                    "\r\nwill obtain its IP address using a DHCP client service. After obtaining"\
                                    "\r\nthe IP address, the project will allow the user to select options from a"\
                                    "\r\nmenu provided by the terminal application on the host PC.\r\n"

#define EP_MENU                     "\r\n\r\nMain menu:"\
                                    "\r\n1. Type '1 <x.x.x.x>:<port>' to perform TCP client operation."\
                                    "\r\n2. Type '2 <x.x.x.x>:<port>' to perform UDP client operation."\
                                    "\r\n3. Type '3 <x.x.x.x>' to get the PTR record for the specified address."\
                                    "\r\n        '3 <domain>'  to get the A record for the specified domain."\
                                    "\r\n4. Type '4' to start the web HTTP server."\
                                    "\r\nUser input:\r\n"

/* Extern variables - Defined in the generated thread files */
extern TX_THREAD net_link_thread;
extern TX_THREAD net_app_thread;

/* Function declarations */
void net_link_thread_entry_run(void);
void net_app_thread_entry_run(void);
void led_control_thread_entry_run(void);

#endif /* GETTING_STARTED_WITH_AZURE_NETX_EXAMPLE_APP_H_ */
