/***********************************************************************************************************************
* File Name    : net_link.h
* Description  : Contains macros, data structures, and common functions used in the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef NET_LINK_H_
#define NET_LINK_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_app.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define NET_LINK_THREAD_SLEEP_TICK              (1U)
#define NET_DHCP_OPTION_SIZE                    (32U)
#define NET_IP_ADDRESS_RESOLVED_WAIT            (50 * NX_IP_PERIODIC_RATE)
#define NET_IP_LINK_WAIT                        (50 * NX_IP_PERIODIC_RATE)
#define NET_ICMP_PING_WAIT                      (50 * NX_IP_PERIODIC_RATE)

#define MEMPOOL_SIZE                            (65536U)
#define BYTE_SIZE                               (4U)

#define DEVICE_FRAME_LENGTH_FULL_SPEED          (93U)
#define DEVICE_FRAME_LENGTH_HI_SPEED            (93U)
#define STRING_FRAMEWORK_LENGTH                 (61U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH           (2U)

#define USB_CONFIG_NUMB                         (1U)
#define USB_INTERFACE_NUMB0                     (0U)

/* IP address range assigned by the DHCP server */
#define START_IP_ADDRESS_LIST                   (IP_ADDRESS(192, 168, 2, 10))
#define END_IP_ADDRESS_LIST                     (IP_ADDRESS(192, 168, 2, 255))

#define USB_RNDIS_DHCP_SERVER_IP_ADDRESS        (IP_ADDRESS(192, 168, 2, 1))
#define USB_RNDIS_DHCP_SERVER_DEFAULT_GATEWAY   (USB_RNDIS_DHCP_SERVER_IP_ADDRESS)

#define ETHERNET_PORT_INTERFACE_INDEX           (0)
#define USB_RNDIS_PORT_INTERFACE_INDEX          (1)

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
UINT net_system_ping(ULONG ip_addr);

#endif /* NET_LINK_H_ */
