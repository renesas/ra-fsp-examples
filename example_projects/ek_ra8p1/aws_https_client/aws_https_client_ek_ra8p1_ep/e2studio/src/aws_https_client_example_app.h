/***********************************************************************************************************************
 * File Name    : aws_https_client_example_app.h
 * Description  : Public interface for the AWS HTTPS Client example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AWS_HTTPS_CLIENT_EXAMPLE_APP_H_
#define AWS_HTTPS_CLIENT_EXAMPLE_APP_H_

#include "aws_https_client_ep.h"

#define EP_VERSION      "1.2"

#define MODULE_NAME     "AWS Core HTTP"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                       *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and              "\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME"                    \r\n"\

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of AWS HTTPS client library with"\
                        "\r\nEthernet driver on Renesas RA MCUs based on Renesas FSP using FreeRTOS. AWS client"\
                        "\r\nis used to connect to HTTPS Adafruit server which is cloud platform. On successful"\
                        "\r\nconnection, menu is displayed enabling the user to send GET, PUT, POST requests to"\
                        "\r\nAdafruit server. On POST/PUT request, ADC output is read and uploaded to Adafruit server."\
                        "\r\nOn GET request, the last updated data is received from Adafruit. The status and"\
                        "\r\nresponses of the requests made to server are displayed on the serial terminal."

#define ETH_PREINIT     "\r\n\r\n--------------------------------------------------------------------------------"\
                        "\r\nEthernet adapter configuration for Renesas "KIT_NAME": Pre IP Init       "\
                        "\r\n--------------------------------------------------------------------------------"

#define ETH_POSTINIT    "\r\n\r\n--------------------------------------------------------------------------------"\
                        "\r\nEthernet adapter configuration for Renesas "KIT_NAME": Post IP Init       "\
                        "\r\n--------------------------------------------------------------------------------"

#define PRINT_MENU      "\r\n\r\nSelect from the below menu options"\
                        "\r\n 1. GET request"\
                        "\r\n 2. PUT request"\
                        "\r\n 3. POST request\r\n"

/* Single entry-point called in user_app_thread_entry.c */
void aws_https_client_example_app_run(void);

#endif /* AWS_HTTPS_CLIENT_EXAMPLE_APP_H_ */
