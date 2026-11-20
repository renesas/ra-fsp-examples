/***********************************************************************************************************************
 * File Name    : dhcpv4_client_ep.h
 * Description  : Contains macros, data structures, and functions used in dhcpv4_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DHCPV4_CLIENT_EP_H_
#define DHCPV4_CLIENT_EP_H_

#include <stdbool.h>
#include "dhcp_client_thread.h"
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null character */
#define NULL_CHAR                       ('\0')

/* Macros for the names of the NetX Duo resources */
#define PACKET_POOL_NAME                "g_packet_pool0 Packet Pool"
#define IP_NAME                         "g_ip0 IP Instance"
#define DHCP_CLIENT_NAME                "g_dhcp_client0"
#define EVENT_FLAGS_NAME                "g_dhcp_event_flags"

/* Macro for the wait time for the link to be enabled. This timeout is defined for 10 seconds */
#define LINK_ENABLE_WAIT_TIME           (10U * TX_TIMER_TICKS_PER_SECOND)

/* Macro for the DHCP bound event */
#define DHCP_EVENT                      ((ULONG)0x01)

/* Macro for the wait time for the DHCP bound event. This timeout is defined for 20 seconds */
#define DHCP_BOUND_WAIT_TIME            (20U * TX_TIMER_TICKS_PER_SECOND)

/* Macro for the wait time for getting assigned an IP address. This timeout is defined for 20 seconds */
#define WAIT_TO_BE_BOUND                (20U * TX_TIMER_TICKS_PER_SECOND)

/* Macro for the wait time for the ping response in ticks. This timeout is defined for 3 seconds */
#define PING_TIMEOUT                    (3U * TX_TIMER_TICKS_PER_SECOND)

/* Macros for the DHCP address lists */
#define MAX_DNS_SERVERS                 (5U)
#define MAX_GATEWAYS                    (5U)
#define IP_V4_SIZE                      (4U)

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

/* Function declarations */
void nx_common_init(void);
UINT packet_pool_init(void);
UINT ip_init(void);
UINT ip_link_check(void);
UINT ip_address_get(ULONG * const p_ip_address);
UINT ip_address_resolved_wait(void);
UINT ip_address_ping(ULONG ip_address, char * p_data, ULONG size, ULONG wait);
UINT dhcp_client_init(void);
UINT dhcp_client_start(void);
UINT dhcp_client_stop(void);
UINT dhcp_client_reinitialize(void);
UINT dhcp_client_release(void);
UINT dhcp_client_delete(void);
void dhcp_client_deinit(void);
UINT dhcp_event_flags_create(void);
UINT dhcp_state_change_notify(void);
UINT dhcp_bound_wait(void);
UINT dhcp_server_address_get(ULONG * const p_server_address);
UINT dhcp_lease_time_get(ULONG * const p_lease_time);
UINT dhcp_address_list_get(UINT option, ULONG * const p_list, UINT max_count, UINT * const p_count);
UINT ethernet_link_wait(void);

#endif /* DHCPV4_CLIENT_EP_H_ */
