/***********************************************************************************************************************
 * File Name    : usr_network.h
 * Description  : Contains declarations of data structures and functions used in usr_network.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include <application_thread.h>

#ifndef USR_NETWORK_H_
#define USR_NETWORK_H_

#define USR_IPV4_ADDRESS         (IP_ADDRESS(0,0,0,0))
#define USR_IPV4_MASK            (IP_ADDRESS(255,255,255,0))
#define USR_IPV4_GATEWAY_ADDRESS (IP_ADDRESS(0,0,0,0))
#define USR_IP_TASK_STACK_SIZE   (2048)
#define USR_IP_THREAD_PRIORITY   (2)
#define USR_ARP_CACHE_SIZE       (520 * 1)

#ifndef SAMPLE_SNTP_SERVER_NAME
#define SAMPLE_SNTP_SERVER_NAME           "0.pool.ntp.org"    /* SNTP Server.  */
#endif /* SAMPLE_SNTP_SERVER_NAME */

#ifndef SAMPLE_SNTP_SYNC_MAX
#define SAMPLE_SNTP_SYNC_MAX             (3)
#endif /* SAMPLE_SNTP_SYNC_MAX */

#ifndef SAMPLE_SNTP_UPDATE_MAX
#define SAMPLE_SNTP_UPDATE_MAX            (10)
#endif /* SAMPLE_SNTP_UPDATE_MAX */

#ifndef SAMPLE_SNTP_UPDATE_INTERVAL
#define SAMPLE_SNTP_UPDATE_INTERVAL       (NX_IP_PERIODIC_RATE / 2)
#endif /* SAMPLE_SNTP_UPDATE_INTERVAL */

/* Default time. GMT: Friday, Jan 1, 2021 12:00:00 AM. Epoch timestamp: 1609459200.  */
#ifndef SAMPLE_SYSTEM_TIME 
#define SAMPLE_SYSTEM_TIME                (1609459200)
#endif /* SAMPLE_SYSTEM_TIME  */

/* Seconds between Unix Epoch (1/1/1970) and NTP Epoch (1/1/1999) */
#define SAMPLE_UNIX_TO_NTP_EPOCH_SECOND   (0x83AA7E80)

/* IP instance */
extern NX_IP g_ip0;
/* DNS instance */
extern NX_DNS g_dns0;
/* Stack memory for g_ip0. */
extern uint8_t g_ip0_stack_memory[USR_IP_TASK_STACK_SIZE];
/* ARP cache memory for g_ip0. */
extern uint8_t g_ip0_arp_cache_memory[USR_ARP_CACHE_SIZE];
/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
extern NX_PACKET_POOL g_packet_pool0;
extern uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))];

void print_ipv4_addr(ULONG address, char *str, size_t len);

#endif /* USR_NETWORK_H_ */
