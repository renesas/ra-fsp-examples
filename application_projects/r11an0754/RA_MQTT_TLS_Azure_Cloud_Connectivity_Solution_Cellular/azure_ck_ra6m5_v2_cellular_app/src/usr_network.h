/***********************************************************************************************************************
 * File Name    : usr_network.h
 * Description  : Contains declarations of data structures and functions used in usr_network.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include <application_thread.h>

#ifndef USR_NETWORK_H_
#define USR_NETWORK_H_

#define USR_IPV4_ADDRESS         (IP_ADDRESS(0,0,0,0))
#define USR_IPV4_MASK            (IP_ADDRESS(255,255,255,0))
#define USR_IPV4_GATEWAY_ADDRESS (IP_ADDRESS(0,0,0,0))
#define USR_IP_TASK_STACK_SIZE   (2048*8)
#define USR_IP_THREAD_PRIORITY   (2)
#define USR_ARP_CACHE_SIZE       (520 * 1)

#ifndef SAMPLE_SNTP_SERVER_NAME
#define SAMPLE_SNTP_SERVER_NAME           "0.pool.ntp.org"    /* SNTP Server.  */
#endif /* SAMPLE_SNTP_SERVER_NAME */

#ifndef SAMPLE_SNTP_SYNC_MAX
#define SAMPLE_SNTP_SYNC_MAX             (10)
#endif /* SAMPLE_SNTP_SYNC_MAX */

#ifndef SAMPLE_SNTP_UPDATE_MAX
#define SAMPLE_SNTP_UPDATE_MAX            (10)
#endif /* SAMPLE_SNTP_UPDATE_MAX */

#ifndef SAMPLE_SNTP_UPDATE_INTERVAL
#define SAMPLE_SNTP_UPDATE_INTERVAL       (NX_IP_PERIODIC_RATE / 2)
#endif /* SAMPLE_SNTP_UPDATE_INTERVAL */

/* Default time. GMT: Friday, Jan 1, 2023 12:00:00 AM. Epoch timestamp: 1684958653 */
#ifndef SAMPLE_SYSTEM_TIME 
#define SAMPLE_SYSTEM_TIME                (1684958653)
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

#endif /* USR_NETWORK_H_ */
