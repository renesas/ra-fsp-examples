/***********************************************************************************************************************
 * File Name    : net_link_ep.h
 * Description  : Contains macros, data structures and functions used in the net_link_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NET_LINK_EP_H_
#define NET_LINK_EP_H_

#include "nx_api.h"
#include "terminal.h"

/* Macro for the sleep time of the Net Link Thread, in ticks */
#define NET_LINK_THREAD_SLEEP_TICK      (1U)

/* Macro for the size of the buffer used to retrieve the DHCP options */
#define NET_DHCP_OPTION_SIZE            (32U)

/* Macro for the time to wait for the IP address to be resolved */
#define NET_IP_ADDRESS_RESOLVED_WAIT    (10U * TX_TIMER_TICKS_PER_SECOND)

/* Macro for the time to wait for the ICMP ping response */
#define NET_ICMP_PING_WAIT              (10U * TX_TIMER_TICKS_PER_SECOND)

/* Enumeration for network physical link events */
typedef enum e_net_link_event
{
    LINK_DOWN_EVENT = 1U << 0,
    LINK_UP_EVENT   = 1U << 1,
    LINK_ANY_EVENT  = 0x03U,
} net_link_event_t;

/* Structure for IPv4 address */
typedef union u_ipv4_addr
{
    ULONG full;
    struct st_block_ip
    {
        UCHAR d;
        UCHAR c;
        UCHAR b;
        UCHAR a;
    } block;
} ipv4_addr_t;

/* Structure for MAC address */
typedef union u_mac_addr
{
    ULONG64 full;
    struct st_group_mac_addr
    {
        ULONG msb;
        ULONG lsb;
    } group;
    struct st_block_mac_addr
    {
        UCHAR b;
        UCHAR a;
        UCHAR rev1;
        UCHAR rev2;
        UCHAR f;
        UCHAR e;
        UCHAR d;
        UCHAR c;
    } block;
} mac_addr_t;

/* Structure for the network system configuration */
typedef struct st_net_info
{
    mac_addr_t phy_addr;
    ipv4_addr_t ipv4_addr;
    ipv4_addr_t subnet_mask;
    ipv4_addr_t default_gateway;
    ipv4_addr_t dhcp_addr;
} net_info_t;

/* Extern variables - Defined in net_link_ep.c */
extern NX_IP g_ip0;
extern net_info_t g_network_info;

/* Function declarations */
UINT net_system_startup(void);
UINT net_link_event_get(ULONG * const p_events);
UINT net_system_link_up_process(void);
UINT net_system_ping(ULONG ip_addr);

#endif /* NET_LINK_EP_H_ */
