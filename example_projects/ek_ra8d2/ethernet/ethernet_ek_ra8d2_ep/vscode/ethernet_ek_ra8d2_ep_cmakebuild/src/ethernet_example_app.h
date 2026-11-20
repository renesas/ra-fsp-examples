/***********************************************************************************************************************
 * File Name    : ethernet_example_app.h
 * Description  : Public interface for the Ethernet example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ETHERNET_EXAMPLE_APP_H_
#define ETHERNET_EXAMPLE_APP_H_

#include "ethernet_ep.h"

#define EP_VERSION        "1.0"

#define MODULE_NAME       "Ethernet using FreeRTOS+TCP"

#define BANNER_INFO       "\r\n********************************************************************************"\
                          "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module         *"\
                          "\r\n*   Example Project Version "EP_VERSION"                                                *"\
                          "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                          "\r\n********************************************************************************"\
                          "\r\nRefer to readme.txt file for more details on Example Project and"\
                          "\r\nFSP User's Manual for more information about "MODULE_NAME"\r\n"

#define ETH_PREINIT       "\r\n--------------------------------------------------------------------------------"\
                          "\r\nEthernet adapter configuration for Renesas "KIT_NAME": Pre IP Init       "\
                          "\r\n--------------------------------------------------------------------------------"

#define ETH_POSTINIT      "\r\n--------------------------------------------------------------------------------"\
                          "\r\nEthernet adapter configuration for Renesas "KIT_NAME": Post IP Init       "\
                          "\r\n--------------------------------------------------------------------------------"

void ethernet_example_app_run(void);

#endif /* ETHERNET_EXAMPLE_APP_H_ */
