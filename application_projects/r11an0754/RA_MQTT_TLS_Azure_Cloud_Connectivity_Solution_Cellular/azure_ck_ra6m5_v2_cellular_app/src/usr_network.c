/***********************************************************************************************************************
 * File Name    : usr_network.c
 * Description  : Contains data structures and functions used for the Network init and its utilities
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
#include "nx_secure_tls.h"
#include "nx_secure_tls_api.h"
#include "nxd_dhcp_client.h"
#include "nx_api.h"
#include "common_utils.h"
#include "usr_network.h"
#include "usr_app.h"
#include <application_thread.h>
#include "ewf_interface.h"
#include "ewf.h"
#include "ewf_adapter.h"
#include <ctype.h>

static UINT dns_create();
UINT sntp_time_sync();

#define NETXDUO_EXAMPLE_IP_STACK_SIZE       (2048U)

ULONG    unix_time_base;
extern ewf_adapter* adapter_ptr;

extern uint32_t modem_ip_addr;
extern ULONG g_gateway_address;
extern ULONG g_dns_address;

uint8_t* response_ptr;
uint32_t response_length;

/* IP instance */
 NX_IP g_ip0;
/* DNS instance */
 NX_DNS g_dns0;


/* SNTP Instance */
 NX_SNTP_CLIENT   g_sntp_client0;
/* System clock time for UTC.  */

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[USR_IP_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[USR_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4);


/*******************************************************************************************************************//**
 * @brief      Netx initialize
 * @param[in]  void
 * @retval     NX_SUCCESS		Initialize successfully
 * @retval     Other			Initialize unsuccessfully
 **********************************************************************************************************************/
UINT usr_netx_init(void)
{
    UINT    status    = NX_SUCCESS;
    ULONG   unix_time = 0;
    ULONG   ip_address = 0;
    ULONG   network_mask = 0;
    ULONG   gateway_address = 0;
    ULONG   host_ip_address = 0;

    nx_system_initialize();

    /* Create the packet pool. */
    status = nx_packet_pool_create (&g_packet_pool0, "g_packet_pool0 Packet Pool",
                                    G_PACKET_POOL0_PACKET_SIZE,
                                    &g_packet_pool0_pool_memory[0],
                                    G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    if (NX_SUCCESS != status)
    {
        IotLog("Packet Pool Creation failed: %u\r\n", status);
        return (status);
    }

    /* Create an IP instance.  */
    status = nx_ip_create(&g_ip0, "NetX IP Instance 0",
                          modem_ip_addr , USR_IPV4_MASK,
                          &g_packet_pool0, nx_driver_ewf_adapter,
                          &g_ip0_stack_memory[0], USR_IP_TASK_STACK_SIZE,
                          USR_IP_THREAD_PRIORITY);

    if (NX_SUCCESS != status)
    {
        IotLog("nx_ip_create failed: %u\r\n", status);
        return (status);
    }

    /* Save the adapter pointer in the IP instance */
    g_ip0.nx_ip_reserved_ptr = adapter_ptr;
    g_ip0.nx_ip_interface->nx_interface_additional_link_info = adapter_ptr;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory, G_IP0_ARP_CACHE_SIZE);
    /* Check for ARP enable errors.  */
    if (NX_SUCCESS != status)
    {
        IotLog("nx_arp_enable fail: %u\r\n", status);
        return (status);
    }

    /* Enable ICMP traffic.  */
    status = nx_icmp_enable(&g_ip0);
    /* Check for ICMP enable errors.  */
    if (NX_SUCCESS != status)
    {
        IotLog("nx_icmp_enable fail: %u\r\n", status);
        return (status);
    }

     /* Enable TCP traffic.  */
    status = nx_tcp_enable(&g_ip0);
    /* Check for TCP enable errors.  */
    if (NX_SUCCESS != status)
    {
        IotLog("nx_tcp_enable fail: %u\r\n", status);
        return (status);
    }

    /* Enable UDP traffic.  */
    status = nx_udp_enable(&g_ip0);
    /* Check for UDP enable errors.  */
    if (NX_SUCCESS != status)
    {
        IotLog("nx_udp_enable fail: %u\r\n", status);
        return (status);
    }

    nx_ip_gateway_address_set(&g_ip0, g_gateway_address);
    /* Get IP address and gateway address. */
    nx_ip_address_get(&g_ip0, &ip_address, &network_mask);
    nx_ip_gateway_address_get(&g_ip0, &gateway_address);

    /* Output IP address and gateway address. */
    IotLog("Modem IP address: %lu.%lu.%lu.%lu\r\n",
           (ip_address >> 24),
           (ip_address >> 16 & 0xFF),
           (ip_address >> 8 & 0xFF),
           (ip_address & 0xFF));
    IotLog("Mask: %lu.%lu.%lu.%lu\r\n",
           (network_mask >> 24),
           (network_mask >> 16 & 0xFF),
           (network_mask >> 8 & 0xFF),
           (network_mask & 0xFF));
    IotLog("Gateway: %lu.%lu.%lu.%lu\r\n",
           (gateway_address >> 24),
           (gateway_address >> 16 & 0xFF),
           (gateway_address >> 8 & 0xFF),
           (gateway_address & 0xFF));



    status = dns_create();
    if (NX_SUCCESS != status)
    {
        IotLog("DNS Create Failed: %u\r\n", status);
        return status;
    }

    g_dns0.nx_dns_server_ip_array[0].nxd_ip_address.v4 = g_dns_address;
    /* Look up an IPv4 address over IPv4. */
    status = nx_dns_host_by_name_get(&g_dns0, (UCHAR *)"RACLOUDHUB.azure-devices.net", &host_ip_address, 0xFFFFFFFF);


    /* Sync up time by SNTP at start up.  */
    for (UINT i = 0; i < SAMPLE_SNTP_SYNC_MAX; i++)
    {
        /* Start SNTP to sync the local time.  */
        status = sntp_time_sync();
        /* Check status.  */
        if(status == NX_SUCCESS)
            break;
    }

    /* Check status.  */
    if (status)
    {
        IotLog("SNTP Time Sync failed. with Retry\r\n");
        IotLog("Set Time to default value: SAMPLE_SYSTEM_TIME and Continue...\r\n");
        unix_time_base = SAMPLE_SYSTEM_TIME;
        status = NX_SUCCESS;
    }
    else
    {
        IotLog("SNTP Time Sync successfully.\r\n");
    }


    /* Use time to init the seed.  */
    unix_time_get(&unix_time);
    srand(unix_time);

    /* Initialize TLS.  */
    nx_secure_tls_initialize();

    return status;
}

/*******************************************************************************************************************//**
 * @brief     	dns_create function
 * @param		None
 * @retval     	NX_SUCCESS		Create successfully
 * @retval     	Other			Create unsuccessfully	
 **********************************************************************************************************************/
static UINT dns_create()
{
    UINT status;
 
    /* Create a DNS instance for the Client.  Note this function will create
    the DNS Client packet pool for creating DNS message packets intended
    for querying its DNS server. */
    status = nx_dns_create(&g_dns0, &g_ip0, (UCHAR *)"DNS Client");
    if (NX_SUCCESS != status)
    {
        return(status);
    }

    /* Is the DNS client configured for the host application to create the pecket pool? */
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL
 
    /* Yes, use the packet pool created above which has appropriate payload size
    for DNS messages. */
    status = nx_dns_packet_pool_set(&g_dns0, g_ip0.nx_ip_default_packet_pool);
    if (NX_SUCCESS != status)
    {
        nx_dns_delete(&g_dns0);
        return(status);
    }
#endif /* NX_DNS_CLIENT_USER_CREATE_PACKET_POOL */

 
    /* Add an IPv4 server address to the Client list. */
    status = nx_dns_server_add(&g_dns0, g_dns_address);
    if (NX_SUCCESS != status)
    {
        nx_dns_delete(&g_dns0);
        return(status);
    }
 
    /* Output DNS Server address.  */
    IotLog("DNS Server address: %lu.%lu.%lu.%lu\r\n",
        (g_dns_address >> 24),
        (g_dns_address >> 16 & 0xFF),
        (g_dns_address >> 8 & 0xFF),
        (g_dns_address & 0xFF));

    return(NX_SUCCESS);
 }


/*******************************************************************************************************************//**
 * @brief     	Sync up the local time
 * @param		None
 * @retval     	NX_SUCCESS		Sync up successfully
 * @retval     	Other			Sync up unsuccessfully	
 **********************************************************************************************************************/
UINT sntp_time_sync()
{
 
    UINT    status;
    UINT    server_status;
    ULONG   sntp_server_address;
    UINT    i;
 
 
    IotLog("SNTP Time Sync...\r\n");
 
#ifndef SAMPLE_SNTP_SERVER_ADDRESS
    /* Look up SNTP Server address. */
    status = nx_dns_host_by_name_get(&g_dns0, (UCHAR *)SAMPLE_SNTP_SERVER_NAME, &sntp_server_address, 5 * NX_IP_PERIODIC_RATE);

    /* Check status.  */
    if (NX_SUCCESS != status)
    {
        return(status);
    }

#else /* !SAMPLE_SNTP_SERVER_ADDRESS */
    sntp_server_address = SAMPLE_SNTP_SERVER_ADDRESS;
#endif /* SAMPLE_SNTP_SERVER_ADDRESS */
 
    /* Create the SNTP Client to run in broadcast mode.. */
    status =  nx_sntp_client_create(&g_sntp_client0, &g_ip0, 0, &g_packet_pool0,
                                    NX_NULL,
                                    NX_NULL,
                                    NX_NULL /* no random_number_generator callback */);

    /* Check status.  */
    if (NX_SUCCESS != status)
    {
        return(status);
    }
 
    /* Use the IPv4 service to initialize the Client and set the IPv4 SNTP server. */
    status = nx_sntp_client_initialize_unicast(&g_sntp_client0, sntp_server_address);

    /* Check status.  */
    if (NX_SUCCESS != status)
    {
        nx_sntp_client_delete(&g_sntp_client0);
        return(status);
    }
 
    /* Set local time to 0 */
    status = nx_sntp_client_set_local_time(&g_sntp_client0, 0, 0);

    /* Check status.  */
    if (NX_SUCCESS != status)
    {
        nx_sntp_client_delete(&g_sntp_client0);
        return(status);
    }
 
    /* Run Unicast client */
    status = nx_sntp_client_run_unicast(&g_sntp_client0);

    /* Check status.  */
    if (NX_SUCCESS != status)
    {
        nx_sntp_client_stop(&g_sntp_client0);
        nx_sntp_client_delete(&g_sntp_client0);
        return(status);
    }

    /* Wait till updates are received */
    for (i = 0; i < SAMPLE_SNTP_UPDATE_MAX; i++)
    {
        /* First verify we have a valid SNTP service running. */
        status = nx_sntp_client_receiving_updates(&g_sntp_client0, &server_status);

        /* Check status.  */
        if ((status == NX_SUCCESS) && (server_status == NX_TRUE))
        {

            /* Server status is good. Now get the Client local time. */
            ULONG sntp_seconds;
            ULONG sntp_fraction;
            ULONG system_time_in_second;

            /* Get the local time.  */
            status = nx_sntp_client_get_local_time(&g_sntp_client0, &sntp_seconds, &sntp_fraction, NX_NULL);

            /* Check status.  */
            if (status != NX_SUCCESS)
            {
                continue;
            }

            /* Get the system time in second.  */
            system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

            /* Convert to Unix epoch and minus the current system time.  */
            unix_time_base = (sntp_seconds - (system_time_in_second + SAMPLE_UNIX_TO_NTP_EPOCH_SECOND));

            /* Time sync successfully.  */

            /* Stop and delete SNTP.  */
            nx_sntp_client_stop(&g_sntp_client0);
            nx_sntp_client_delete(&g_sntp_client0);

            return(NX_SUCCESS);
        }
        /* Sleep.  */
        tx_thread_sleep(SAMPLE_SNTP_UPDATE_INTERVAL);
    }
    /* Stop and delete SNTP.  */
    nx_sntp_client_stop(&g_sntp_client0);
    nx_sntp_client_delete(&g_sntp_client0);

    /* Return success.  */
    return(NX_NOT_SUCCESSFUL);
}

/*******************************************************************************************************************//**
 * @brief     	unix_time_get function
 * @param		unix_time
 * @retval     	NX_SUCCESS
 **********************************************************************************************************************/
UINT unix_time_get(ULONG *unix_time)
{

    /* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).  */
    *unix_time =  unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

    return(NX_SUCCESS);
}
