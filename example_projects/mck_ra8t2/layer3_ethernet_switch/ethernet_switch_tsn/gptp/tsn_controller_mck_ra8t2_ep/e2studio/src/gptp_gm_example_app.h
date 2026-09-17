/***********************************************************************************************************************
 * File Name    : gptp_gm_example_app.h
 * Description  : Public interface for the gPTP Grand Master example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPTP_GM_EXAMPLE_APP_H_
#define GPTP_GM_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"
#define MODULE_NAME     "gPTP Multi-Node"
#define BANNER_INFO \
    "\r\n**********************************************************************************" \
    "\r\n*   Renesas FSP Example Project for " MODULE_NAME " Module                       *" \
    "\r\n*   Example Project Version %s                                                  *" \
    "\r\n*   Flex Software Pack Version  %u.%u.%u                                            *" \
    "\r\n**********************************************************************************" \
    "\r\n"
#define EP_INFO \
    "\r\nThis example demonstrates IEEE 802.1AS gPTP time synchronization across a scalable" \
    "\r\nmulti-node Ethernet network on Renesas RA MCUs, using the gPTP ESWM (Ethernet Switch" \
    "\r\nwith gPTP) middleware. Every node in the chain ends up sharing a common hardware" \
    "\r\nclock, and synchronization accuracy is verified by comparing the 1 PPS output of" \
    "\r\neach board on an oscilloscope.\r\n"
#define GPTP_STATUS_INFO \
    "=====================================\n" \
    "GRAND MASTER STATUS [inst %u]\n" \
    "=====================================\n" \
    "Timestamp       : %u,%03u s\n" \
    "Role            : %s\n" \
    "Port %u          : %s\n" \
    "Port %u          : %s\n" \
    "=====================================\n\n"

/* Public functions declarations */
void gptp_example_app_run(void);

#endif /* GPTP_GM_EXAMPLE_APP_H_ */
