/***********************************************************************************************************************
 * File Name    : dhcpv6_client.h
 * Description  : Contains macros, variables, data structures, and functions used in dhcpv6_client.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DHCPV6_CLIENT_H_
#define DHCPV6_CLIENT_H_

#include <stdio.h>
#include "terminal.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the NULL char value */
#define NULL_CHAR                       ('\0')

/* Macros to print info and error messages and to trap the error */
#define PRINT_INFO_STR(str)             (term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)              (term_print_data(TERM_OP_MSG_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)                 (term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(UINT), &(err)))

/* Macros for the DHCPv6 Client parameters */
#define DHCPV6_IANA_ID                  (0xC0DEDBAD)                /* Identity association unique identifier */
#define DHCPV6_T1                       (NX_DHCPV6_INFINITE_LEASE)  /* Time to start the IPv6 address renewal */
#define DHCPV6_T2                       (NX_DHCPV6_INFINITE_LEASE)  /* Time to start the IPv6 address rebinding */

/* Macro for the event flag set when the DHCPv6 Client is bound to an IPv6 address */
#define DHCPV6_CLIENT_BOUND_EVENT       (0x1)

/* Macro for 60 seconds wait time for the DHCPv6 Client to be bound to an IPv6 address */
#define DHCPV6_EVENT_WAIT_TIME          (60000U)

/* Macros for the ping to the DNS Server */
#define PING_DATA                       "ABCDE"
#define PING_WAIT_TIME                  (3U * TX_TIMER_TICKS_PER_SECOND)

/* Structure to store the IA address lease info */
typedef struct st_dhcpv6_lease_info
{
    ULONG renew_time;
    ULONG rebind_time;
    ULONG deprecated_time;
    ULONG expired_time;
} dhcpv6_lease_info_t;

/* Function declarations */
void nx_common_init(void);
UINT packet_pool_init(void);
UINT ip_init(void);
UINT ip_link_check(void);
UINT ip_address_set(void);
UINT dhcpv6_client_init(void);
UINT dhcpv6_client_duid_create(void);
UINT dhcpv6_client_iana_create(void);
UINT dhcpv6_client_options_request(void);
UINT dhcpv6_client_start(void);
UINT dhcpv6_client_solicit(void);
UINT dhcpv6_client_bound_wait(void);
UINT dhcpv6_client_valid_ip_address_check(void);
UINT dhcpv6_client_ip_address_get(NXD_ADDRESS * const p_address);
UINT dhcpv6_dns_server_address_get(NXD_ADDRESS * const p_address);
UINT dns_server_ping(NXD_ADDRESS * const p_address);
UINT dhcpv6_lease_info_get(dhcpv6_lease_info_t * const p_lease_info);
UINT dhcpv6_client_release(void);
UINT dhcpv6_client_stop(void);
UINT dhcpv6_client_delete(void);
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6);

#endif /* DHCPV6_CLIENT_H_ */
