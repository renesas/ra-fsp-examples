/***********************************************************************************************************************
 * File Name    : dhcpv4_server_ep.h
 * Description  : Contains macros, data structures, and functions used in dhcpv4_server_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DHCPV4_SERVER_EP_H_
#define DHCPV4_SERVER_EP_H_

#include <stdbool.h>
#include "dhcpv4_server_thread.h"
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null character */
#define NULL_CHAR                       ('\0')

/* Macros for the names of the NetX Duo resources */
#define PACKET_POOL_NAME                "g_packet_pool0 Packet Pool"
#define IP_NAME                         "g_ip0 IP Instance"
#define DHCP_SERVER_NAME                "g_dhcp_server0"

/* Macro for the index corresponding to the network interface */
#define INTERFACE_INDEX                 (0U)

/* Macros for the IP address range assigned by the DHCP server */
#define START_IP_ADDRESS_LIST           (IP_ADDRESS(192, 168, 0, 10))
#define END_IP_ADDRESS_LIST             (IP_ADDRESS(192, 168, 0, 20))

/* Macros for the network configuration */
#define NX_DHCP_SERVER_IP_ADDRESS       (IP_ADDRESS(192, 168, 0, 2))
#define NX_DHCP_ROUTER_IP_ADDRESS       (IP_ADDRESS(192, 168, 0, 2))
#define NX_DHCP_DNS_IP_ADDRESS          (IP_ADDRESS(192, 168, 0, 2))

/* Macro for the wait time for the link to be enabled. This timeout is defined for 10 seconds */
#define LINK_ENABLE_WAIT_TIME           (10U * TX_TIMER_TICKS_PER_SECOND)

/* Macros for the menu options to start the DHCPv4 server, stop it, and print the DHCPv4 client information */
#define DHCPV4_SERVER_START             (1U)
#define DHCPV4_SERVER_STOP              (2U)
#define DHCPV4_CLIENT_INFO              (3U)

/* Union for splitting an IPv4 address into its four bytes */
typedef union
{
    ULONG ip_address;
    struct
    {
        UCHAR ip_d;
        UCHAR ip_c;
        UCHAR ip_b;
        UCHAR ip_a;
    } IP_t;
} ip_address_t;

/* External variable - Defined in dhcpv4_server_ep.c */
extern NX_DHCP_SERVER g_dhcp_server0;

/* Function declarations */
void nx_common_init(void);
UINT packet_pool_init(void);
UINT ip_init(void);
UINT ip_link_check(void);
UINT dhcpv4_server_init(void);
UINT dhcpv4_ip_address_list_create(void);
UINT dhcpv4_network_parameters_set(void);
UINT dhcpv4_server_start(void);
UINT dhcpv4_server_stop(void);
void dhcpv4_server_deinit(void);

#endif /* DHCPV4_SERVER_EP_H_ */
