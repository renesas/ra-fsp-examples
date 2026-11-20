/***********************************************************************************************************************
 * File Name    : NetX_https_client_example_app.h
 * Description  : Public interface for the NetX HTTPS Client example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_HTTPS_CLIENT_EXAMPLE_APP_H_
#define NETX_HTTPS_CLIENT_EXAMPLE_APP_H_

#include "NetX_https_client_ep.h"

/* Macro for the EP version */
#define EP_VERSION                  "1.0"

/* Macro for the module name */
#define MODULE_NAME                 "NetX_https_client"

/* Macro for the banner information */
#define BANNER_INFO                 "\r\n******************************************************************"\
                                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module     *"\
                                    "\r\n*   Example Project Version "EP_VERSION\
                                    "                                  *"\
                                    "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                    "\r\n******************************************************************"\
                                    "\r\nRefer to readme.txt file for more details on Example Project and"\
                                    "\r\nFSP User's Manual for more information about "MODULE_NAME" driver"

/* EP banner information */
#define EP_INFO                     "\r\n\r\nThe example project demonstrates the basic functionality of NetX Duo"\
                                    "\r\nHTTPS Client with Ethernet driver on Renesas RA MCUs based on Renesas FSP"\
                                    "\r\nusing AzureRTOS. The EP uses the DHCP Client to get the IP address from the"\
                                    "\r\nDHCP Server and HTTPS Client to connect to HTTPS Adafruit IO Server which"\
                                    "\r\nis a cloud platform. On successful connection, the menu is displayed enabling"\
                                    "\r\nthe user to send POST, PUT, GET requests to Adafruit IO Server. On POST/PUT"\
                                    "\r\nrequest, MCU die temperature is read using ADC and uploaded to the server."\
                                    "\r\nOn GET request, the last MCU die temperature is read from the Adafruit IO"\
                                    "\r\nServer. Error and info messages will be printed on the terminal."

/* User menu options */
#define EP_MENU                     "\r\n\r\nMain menu:"\
                                    "\r\n1. POST request"\
                                    "\r\n2. PUT request"\
                                    "\r\n3. GET request"\
                                    "\r\nUser input:\r\n"

/* Function declarations */
void https_client_thread_entry_run(void);

#endif /* NETX_HTTPS_CLIENT_EXAMPLE_APP_H_ */
