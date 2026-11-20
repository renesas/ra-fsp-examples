/***********************************************************************************************************************
 * File Name    : dhcpv6_server.h
 * Description  : Contains macros, variables, data structures, and functions used in dhcpv6_server.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DHCPV6_SERVER_H_
#define DHCPV6_SERVER_H_

#include <stdio.h>
#include "terminal.h"

/* Macro for 10 seconds wait time for the Ethernet link to be enabled */
#define LINK_ENABLE_WAIT_TIME           (10000U)

/* Macros for menu options */
#define DHCPV6_SERVER_START             (1U)
#define PRINT_DHCPV6_LEASED_IP_INFO     (2U)

/* Macro to fill an IPv6 address from eight 16-bit fields */
#define FILL_NXD_IPV6_ADDRESS(ipv6,f0,f1,f2,f3,f4,f5,f6,f7) (\
        {do {\
            (ipv6).nxd_ip_address.v6[0] = (((uint32_t)(f0) << 16) & 0xFFFF0000) | ((uint32_t)(f1) & 0x0000FFFF);\
            (ipv6).nxd_ip_address.v6[1] = (((uint32_t)(f2) << 16) & 0xFFFF0000) | ((uint32_t)(f3) & 0x0000FFFF);\
            (ipv6).nxd_ip_address.v6[2] = (((uint32_t)(f4) << 16) & 0xFFFF0000) | ((uint32_t)(f5) & 0x0000FFFF);\
            (ipv6).nxd_ip_address.v6[3] = (((uint32_t)(f6) << 16) & 0xFFFF0000) | ((uint32_t)(f7) & 0x0000FFFF);\
            (ipv6).nxd_ip_version       = NX_IP_VERSION_V6;\
            } while (0);})

/* Structure to store leased IPs info */
typedef struct st_ipv6_info
{
    NXD_ADDRESS ipv6_address;
    USHORT mac_high;
    ULONG mac_low;
} ipv6_info_t;

/* Extern variables - Defined in dhcpv6_server.c */
extern NX_DHCPV6_SERVER g_dhcpv6_server;
extern NXD_ADDRESS g_dns_ipv6_address;
extern NXD_ADDRESS g_start_ipv6_address;
extern NXD_ADDRESS g_end_ipv6_address;

/* Function declarations */
void nx_common_init(void);
UINT packet_pool_init(void);
UINT ip_init(void);
UINT ip_link_check(void);
UINT ip_address_set(void);
UINT dhcpv6_server_init(void);
UINT dhcpv6_dns_address_set(void);
UINT dhcpv6_server_duid_set(void);
UINT dhcpv6_ip_address_range_set(void);
UINT dhcpv6_server_start(void);
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6);
void str_mac(UCHAR * str, USHORT mac_msw, ULONG mac_lsw);

#endif /* DHCPV6_SERVER_H_ */
