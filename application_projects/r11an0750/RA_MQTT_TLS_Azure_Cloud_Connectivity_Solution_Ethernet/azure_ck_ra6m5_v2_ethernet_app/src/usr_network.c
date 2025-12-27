/***********************************************************************************************************************
 * File Name    : usr_network.c
 * Description  : Contains data structures and functions used for the Network init and its utilities
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "nx_secure_tls.h"
#include "nx_secure_tls_api.h"
#include "nxd_dhcp_client.h"
#include "nx_api.h"
#include "common_utils.h"
#include "usr_network.h"
#include "usr_app.h"
#include <application_thread.h>

static UINT configure_dhcp(void);
static void dhcp_client_init0(void);
static UINT dns_create();
static UINT sntp_time_sync();

/* IP instance */
 NX_IP g_ip0;
/* DNS instance */
 NX_DNS g_dns0;
/* DHCP instance. */
 NX_DHCP g_dhcp_client0;
/* SNTP Instance */
 NX_SNTP_CLIENT   g_sntp_client0;
/* System clock time for UTC.  */
 ULONG    unix_time_base;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[USR_IP_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[USR_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

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

    nx_system_initialize();
	
    /* Create an IP instance.  */
    status = nx_ip_create(&g_ip0, "NetX IP Instance 0",
							USR_IPV4_ADDRESS, USR_IPV4_MASK,
							&g_packet_pool0, g_netxduo_ether_0,
							&g_ip0_stack_memory[0], USR_IP_TASK_STACK_SIZE,
							USR_IP_THREAD_PRIORITY);

    /* Check for IP create errors.  */
    if (NX_SUCCESS != status)
    {
        IotLog("nx_ip_create fail: %u\r\n", status);
        return status;
    }


    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory, G_IP0_ARP_CACHE_SIZE);

    /* Check for ARP enable errors.  */
    if (status)
    {
        IotLog("nx_arp_enable fail: %u\r\n", status);
        return status;
    }

    /* Enable ICMP traffic.  */
    status = nx_icmp_enable(&g_ip0);

    /* Check for ICMP enable errors.  */
    if (status)
    {
        IotLog("nx_icmp_enable fail: %u\r\n", status);
        return status;
    }

    /* Enable TCP traffic.  */
    status = nx_tcp_enable(&g_ip0);

    /* Check for TCP enable errors.  */
    if (status)
    {
        IotLog("nx_tcp_enable fail: %u\r\n", status);
        return status;
    }

    /* Enable UDP traffic.  */
    status = nx_udp_enable(&g_ip0);

    /* Check for UDP enable errors.  */
    if (status)
    {
        IotLog("nx_udp_enable fail: %u\r\n", status);
        return status;
    }

    /* Add code for DHCP Client */
    dhcp_client_init0();

    /* Now we're ready to start the DHCP Client.  */
    status =  configure_dhcp();
	
	if (status)
	{
	    IotLog("Configuring the DHCP Client failed: %u\r\n", status);
		return status;
	}

    status = dns_create();
	if (status)
	{
	    IotLog("DNS Create Failed: %u\r\n", status);
		return status;
	}

	/* Sync up time by SNTP at start up.  */
	for (UINT i = 0; i < SAMPLE_SNTP_SYNC_MAX; i++)
	{
	
		/* Start SNTP to sync the local time.	*/
		status = sntp_time_sync();

		/* Check status.  */
		if(status == NX_SUCCESS)
			break;
	}
	
	/* Check status.  */
	if (status)
	{
		IotLog("SNTP Time Sync failed.\r\n");
		IotLog("Set Time to default value: SAMPLE_SYSTEM_TIME.");
		unix_time_base = SAMPLE_SYSTEM_TIME;
	}
	else
	{
		IotLog("SNTP Time Sync successfully.\r\n");
	}
	 
	/* Use time to init the seed.	*/
	unix_time_get(&unix_time);
	srand(unix_time);

	/* Initialize TLS.	*/
	nx_secure_tls_initialize();

    return status;
}


/*******************************************************************************************************************//**
 * @brief     Create the DHCP instance and Set the DHCP Client packet pool.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void dhcp_client_init0(void)
{
    /* Create the DHCP instance. */
    UINT status = nx_dhcp_create(&g_dhcp_client0,
                                 &g_ip0,
                                 "g_dhcp_client0");
    if(NX_SUCCESS != status)
    {
       IotLog("nx_dhcp_create failed.");
        APP_ERR_TRAP(status);
    }

    /* Set the DHCP Client packet pool. */
    status = nx_dhcp_packet_pool_set(&g_dhcp_client0, &g_packet_pool0);
    if(NX_SUCCESS != status)
    {
        IotLog("nx_dhcp_packet_pool_set failed");
        APP_ERR_TRAP(status);
    }
}



/*********************************************************************************************************************
 * @brief  configure_dhcp function
 *
 * This function configure the IP mode to DHCP
 ********************************************************************************************************************/
UINT configure_dhcp(void)
{
    CHAR str[96] = {0};
    UINT status = NX_SUCCESS;
    ULONG address = 0;
    ULONG mask = 0;
    ULONG actual_status;

    status = nx_dhcp_start(&g_dhcp_client0);
    if(NX_DHCP_ALREADY_STARTED != status)
    {
        if (NX_SUCCESS != status)
        {
            snprintf(str,sizeof(str),"Could not start DHCP client, ret = 0x%x\r\n",status);
            return status;
        }
    }
    else
    {
       	IotLog("Already Running!!!\r\n");
    }

    IotLog("\r\nWaiting for IP address.");

    /* Wait util address is solved. */
    nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &actual_status, NX_WAIT_FOREVER);

    status = nx_ip_address_get(&g_ip0, &address, &mask);
    if(NX_SUCCESS != status)
	address = mask = 0;

    IotLog("\r\n");
    IotLog("IP Configuration\r\n");
    IotLog("\r\n  IP Address  : ");
    print_ipv4_addr(address, str, sizeof(str));
    IotLog(str);
    IotLog("\r\n");

    /* If no IP address assigned, return with error */
    if(address == 0)
		return 1;

    IotLog("  Netmask     : ");
    print_ipv4_addr(mask, str, sizeof(str));
    IotLog(str);
    IotLog("\r\n");

    status = nx_dhcp_server_address_get(&g_dhcp_client0, &address);
    if (NX_SUCCESS != status)
		address = 0;

    /*If no DHCP Server address assigned, return with error*/
    if(address == 0)
		return status;

    IotLog("  DHCP Server : ");
    print_ipv4_addr(address, str, sizeof(str));
    IotLog(str);
    IotLog("\r\n");

    IotLog("**************************************\r\n");
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
 
	UINT	 status;
	ULONG	 dns_server_address[3];
	UINT	 dns_server_address_size = 12;

	/* Create a DNS instance for the Client.  Note this function will create
	the DNS Client packet pool for creating DNS message packets intended
	for querying its DNS server. */
	status = nx_dns_create(&g_dns0, &g_ip0, (UCHAR *)"DNS Client");
	if (status)
	{
		return(status);
	}

	/* Is the DNS client configured for the host application to create the pecket pool? */
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL
 
	/* Yes, use the packet pool created above which has appropriate payload size
	for DNS messages. */
	status = nx_dns_packet_pool_set(&g_dns0, g_ip0.nx_ip_default_packet_pool);
	if (status)
	{
		nx_dns_delete(&g_dns0);
		return(status);
	}
#endif /* NX_DNS_CLIENT_USER_CREATE_PACKET_POOL */
 

	/* Retrieve DNS server address.  */
	nx_dhcp_interface_user_option_retrieve(&g_dhcp_client0, 0, NX_DHCP_OPTION_DNS_SVR, (UCHAR *)(dns_server_address),
										&dns_server_address_size);

	/* Add an IPv4 server address to the Client list. */
	status = nx_dns_server_add(&g_dns0, dns_server_address[0]);
	if (status)
	{
		nx_dns_delete(&g_dns0);
		return(status);
	}

	/* Output DNS Server address.	*/
	IotLog("DNS Server address: %lu.%lu.%lu.%lu\r\n",
		(dns_server_address[0] >> 24),
		(dns_server_address[0] >> 16 & 0xFF),
		(dns_server_address[0] >> 8 & 0xFF),
		(dns_server_address[0] & 0xFF));

	return(NX_SUCCESS);
 }


/*******************************************************************************************************************//**
 * @brief     	Sync up the local time
 * @param		None
 * @retval     	NX_SUCCESS		Sync up successfully
 * @retval     	Other			Sync up unsuccessfully	
 **********************************************************************************************************************/
static UINT sntp_time_sync()
{
	UINT	 status;
	UINT	 server_status;
	ULONG	 sntp_server_address;
	UINT	 i;

	IotLog("SNTP Time Sync...\r\n");

#ifndef SAMPLE_SNTP_SERVER_ADDRESS
	/* Look up SNTP Server address. */
	status = nx_dns_host_by_name_get(&g_dns0, (UCHAR *)SAMPLE_SNTP_SERVER_NAME, &sntp_server_address, 5 * NX_IP_PERIODIC_RATE);

	/* Check status.  */
	if (status)
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
	if (status)
	{
		return(status);
	}

	/* Use the IPv4 service to initialize the Client and set the IPv4 SNTP server. */
	status = nx_sntp_client_initialize_unicast(&g_sntp_client0, sntp_server_address);

	/* Check status.  */
	if (status)
	{
		nx_sntp_client_delete(&g_sntp_client0);
		return(status);
	}
 
	/* Set local time to 0 */
	status = nx_sntp_client_set_local_time(&g_sntp_client0, 0, 0);

	/* Check status.  */
	if (status)
	{
		nx_sntp_client_delete(&g_sntp_client0);
		return(status);
	}

	/* Run Unicast client */
	status = nx_sntp_client_run_unicast(&g_sntp_client0);

	/* Check status.  */
	if (status)
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

			/* Get the system time in second.	*/
			system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

			/* Convert to Unix epoch and minus the current system time.  */
			unix_time_base = (sntp_seconds - (system_time_in_second + SAMPLE_UNIX_TO_NTP_EPOCH_SECOND));

			/* Time sync successfully.  */

			/* Stop and delete SNTP.  */
			nx_sntp_client_stop(&g_sntp_client0);
			nx_sntp_client_delete(&g_sntp_client0);

			return(NX_SUCCESS);
		}
 
		/* Sleep.	*/
		tx_thread_sleep(SAMPLE_SNTP_UPDATE_INTERVAL);
	}
 
	/* Time sync failed.  */

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
 
	/* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).	*/
	*unix_time =  unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

	return(NX_SUCCESS);
}

/*******************************************************************************************************************//**
 * @brief     	print_ipv4_addr function
 * @param[in]	address			Address to print
 * @param		str				Pointer to the address that stored the IPv4 address
 * @param[in]	len				Length of the IPv4 address
 * @retval     	None
 **********************************************************************************************************************/
void print_ipv4_addr(ULONG address, char *str, size_t len)
{
	if(NULL == str)
	{
		IotLog("\r\n NULL pointer in function print_ipv4_addr\r\n");
		return;
	}

	snprintf(str, len, "%lu.%lu.%lu.%lu",
			(address >> 24) & 0xff,
			(address >> 16) & 0xff,
			(address >> 8) & 0xff,
			(address >> 0) & 0xff);
}
