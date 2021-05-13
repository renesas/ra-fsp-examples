/***********************************************************************************************************************
 * File Name    : dhcp_client_thread_entry.c
 * Description  : Contains entry function of DHCPV6 Client.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "dhcp_client_thread.h"
#include "common_utils.h"
#include "dhcpv6_client_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_dhcpv6_client_ep
 * @{
 **********************************************************************************************************************/

/* Function declaration. */
static UINT run_dhcpv6_client_session(NX_IP *ip_ptr, NX_DHCPV6 *client_ptr);
static void nx_common_init0(void);
static void packet_pool_init0(void);
static void ip_init0(void);
static void dhcpv6_client_init0(void);
/* State change notification handler */
static VOID dhcpv6_state_change_notify(struct NX_DHCPV6_STRUCT *dhcpv6_ptr, UINT old_state, UINT new_state);
/* Server error handler */
static VOID dhcpv6_server_error_handler(struct NX_DHCPV6_STRUCT *dhcpv6_ptr, UINT op_code, UINT status_code, UINT message_type);

/* Global variables*/
/* event flag group variable to synchronize dhcpv6 client bound mechanism. */
TX_EVENT_FLAGS_GROUP dhcpv6_client_event_flags_group;
/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP instance. */
NX_DHCPV6 g_dhcpv6_client0;
uint8_t g_dhcpv6_client0_stack_memory[G_DHCPV6_CLIENT0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_dhcpv6_client0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* DHCP Client Thread entry function */
void dhcp_client_thread_entry(void)
{
    UINT               status  = NX_SUCCESS;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();
    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the NetX system.*/
    nx_common_init0();

    /* Initialize the packet pool.*/
    packet_pool_init0();

    /* create the ip instance.*/
    ip_init0();

    /* Initialize the dhcpv6 client.*/
    dhcpv6_client_init0();

    PRINT_INFO_STR("Network Initialization completed successfully.");

    /* Start and run a brief DHCPv6 Client session. */
    status = run_dhcpv6_client_session(&g_ip0, &g_dhcpv6_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("run_dhcpv6_client_session failed.");
        /* Internal DHCP error or NetX internal error. We cannot continue this test. */
        nx_dhcpv6_client_delete(&g_dhcpv6_client0);
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("DHCPV6 Client EP completed.");

    while (true)
    {
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Initialization of NetX system.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void nx_common_init0(void)
{
    /* Initialize the NetX system. */
    nx_system_initialize ();
}

/*******************************************************************************************************************//**
 * @brief     Create the packet pool.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void packet_pool_init0(void)
{
    /* Create the packet pool. */
    UINT status = nx_packet_pool_create(&g_packet_pool0,
                                        "g_packet_pool0 Packet Pool",
                                        G_PACKET_POOL0_PACKET_SIZE,
                                        &g_packet_pool0_pool_memory[0],
                                        G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     Create the ip instance and enables ARP,udp,icmp,nx_ipv6,nxd_icmp.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void ip_init0(void)
{
    UINT status        = NX_SUCCESS;
    UINT address_index = RESET_VALUE;

    /* Create the ip instance. */
    status = nx_ip_create(&g_ip0,
                          "g_ip0 IP Instance",
                          G_IP0_ADDRESS,
                          G_IP0_SUBNET_MASK,
                          &g_packet_pool0,
                          g_netxduo_ether_0,
                          &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE,
                          G_IP0_TASK_PRIORITY);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create failed.");
        ERROR_TRAP(status);
    }

    /* Enables Address Resolution Protocol (ARP).*/
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable udp.*/
    status = nx_udp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable icmp.*/
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable IPv6. */
    status = nxd_ipv6_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_ipv6_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable ICMPv6 (Required for neighbor discovery). */
    status = nxd_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_icmp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Wait for the link to be enabled. */
    ULONG actual_state;
    /* Wait for link to be initialized so MAC address is valid. */
    /* Wait for init to finish. */
    status = nx_ip_interface_status_check (&g_ip0, 0, NX_IP_INITIALIZE_DONE, &actual_state, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_interface_status_check failed.");
        ERROR_TRAP(status);
    }

    /* Set the link local IPv6 address (Prefix set to 10). */
    status = nxd_ipv6_address_set(&g_ip0, address_index, NULL, 10, NULL);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_ipv6_address_set failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     Create the DHCPV6 client instance.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void dhcpv6_client_init0(void)
{
    /* Create the DHCP instance. */
    UINT status = nx_dhcpv6_client_create(&g_dhcpv6_client0,
                                          &g_ip0,
                                          "g_dhcpv6_client0",
                                          &g_packet_pool0,
                                          g_dhcpv6_client0_stack_memory,
                                          G_DHCPV6_CLIENT0_TASK_STACK_SIZE,
                                          dhcpv6_state_change_notify,
                                          dhcpv6_server_error_handler
    );
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_client_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function runs a DHCP Client session.
 * @param[IN] *client_ptr    pointer to an NX_DHCPV6 instance, an already created DHCPV6 Client instance.
 * @param[IN] *ip_ptr        pointer to an NX_IP instance, an already created IP instance.
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT run_dhcpv6_client_session(NX_IP *ip_ptr, NX_DHCPV6 *client_ptr)
{
    UINT        status            = NX_SUCCESS;
    ULONG       actual_status     = RESET_VALUE;
    ULONG       actual_events     = RESET_VALUE;
    UINT        address_count     = RESET_VALUE;
    ULONG       T1                = RESET_VALUE;
    ULONG       T2                = RESET_VALUE;
    ULONG       preferred_lifetime= RESET_VALUE;
    ULONG       valid_lifetime    = RESET_VALUE;
    NX_PACKET   *packet_ptr       = NULL;
    NXD_ADDRESS dns_server_address= {RESET_VALUE};
    NXD_ADDRESS client_address    = {RESET_VALUE};

    PRINT_INFO_STR("Checking Ethernet Link...");

    /* Wait for the link to come up.  */
    do
    {
        status = nx_ip_driver_direct_command(&g_ip0, NX_LINK_GET_STATUS, &actual_status);

    } while (NX_TRUE != actual_status);

    PRINT_INFO_STR("Ethernet link is up.");

    /* Create an event flags group to synchronize dhcpv6 client bound mechanism. */
    status = tx_event_flags_create(&dhcpv6_client_event_flags_group, "my_event_group_name");
    if(TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_create failed.");
        return status;
    }

    /* Create a Link Layer Plus Time DUID for the DHCPv6 Client. Set time ID field
      to NULL; the DHCPv6 Client API will supply one. */
    status = nx_dhcpv6_create_client_duid(client_ptr, NX_DHCPV6_DUID_TYPE_LINK_TIME,
                                          NX_DHCPV6_HW_TYPE_IEEE_802, 0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_create_client_duid failed.");
        return status;
    }

    /* Create the IANA (also required) for the DHCPv6 Client. Request lease renewal times T1 and T2. */
    status = nx_dhcpv6_create_client_iana(client_ptr, DHCPV6_IANA_ID, DHCPV6_T1,  DHCPV6_T2);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_create_client_iana failed.");
        return status;
    }

    /* Set the time zone as request to server and add requests for the IPv6 addresses of the DNS and NTP
     * servers on the network.
     */
    status = nx_dhcpv6_request_option_timezone(client_ptr, NX_TRUE);
    status += nx_dhcpv6_request_option_DNS_server(client_ptr, NX_TRUE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_request_option_DNS_server failed.");
        return status;
    }

    /* Start up the NetX DHCPv6 Client thread task. */
    status =  nx_dhcpv6_start(client_ptr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_start failed.");
        return status;
    }
    PRINT_INFO_STR("DHCPV6 Client started.");

    /* Start the DHCPv6 by broadcasting a Solicit message. */
    status = nx_dhcpv6_request_solicit(client_ptr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_request_solicit failed.");
        return status;
    }
    PRINT_INFO_STR("Sent the Solicit message. Waiting for bound state...");

    /* Check if the Client is bound. This is set in the state change callback. */
    status = tx_event_flags_get(&dhcpv6_client_event_flags_group, DHCP_EVENT, TX_AND_CLEAR, &actual_events, DHCPV6_EVENT_WAIT_TIME);
    if((TX_SUCCESS == status) && (true == actual_events))
    {
        /* Now check if the IPv6 address is registered with the IP instance. */
        nx_dhcpv6_get_valid_ip_address_count(client_ptr, &address_count);

        /* Does the IP instance have at least one valid IP address? */
        if (RESET_VALUE >= address_count)
        {
            actual_events = NX_FALSE;
            PRINT_ERR_STR("Client is bound but the IPv6 address not registered with the IP instance.");
            return NX_IP_ADDRESS_ERROR;
        }

        /* Is the DHCPv6 Client bound to an IPv6 address registered with the IP instance? */
        /* Yes it does.  Query the DHCPv6 Client for our IPv6 address, only so that we can display it. */
        status = nx_dhcpv6_get_IP_address(client_ptr, &client_address);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dhcpv6_get_IP_address failed.");
            return status;
        }
        /* Print the client bound IPV6 address. */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(NXD_ADDRESS ), &client_address);

        /* Yes, let's also get the DNS Server address. */
        status = nx_dhcpv6_get_DNS_server_address(client_ptr, 0, &dns_server_address);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dhcpv6_get_DNS_server_address failed.");
            return status;
        }
        /* Print the DNS Server address. */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DNS_SERVER_IP, sizeof(NXD_ADDRESS ), &dns_server_address);

        /* Ping the DNS Server to test our IPv6 address.*/
        status = nxd_icmp_ping(ip_ptr, &dns_server_address, "ABCDE", sizeof("ABCDE"), &packet_ptr, 3*NX_IP_PERIODIC_RATE);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nxd_icmp_ping failed.");
            return status;
        }
        PRINT_INFO_STR("Pinged the DNS Server successfully.");

        /* Retrieve the client’s assigned IA lease data.  */
        status = nx_dhcpv6_get_lease_time_data(client_ptr, &T1, &T2, &preferred_lifetime,
                                               &valid_lifetime);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dhcpv6_get_lease_time_data failed.");
            return status;
        }
        /* Print client’s assigned IA lease data. */
        ULONG lease_data[4] = {RESET_VALUE};
        lease_data[0] = T1;
        lease_data[1] = T2;
        lease_data[2] = preferred_lifetime;
        lease_data[3] = valid_lifetime;
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_VAL, sizeof(lease_data), &lease_data);

        /* Release the IPv6 address. We are no longer going to use it.  */
        status = nx_dhcpv6_request_release(client_ptr);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dhcpv6_request_release failed.");
            return status;
        }
        PRINT_INFO_STR("Released IP address back to Server.");
    }
    PRINT_INFO_STR("Now Stopping and deleting the DHCPv6 Client.");

    /* Stop the DHCPv6 Client thread task. */
    status = nx_dhcpv6_stop(client_ptr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_stop failed.");
        return status;
    }

    /* Delete the DHCPv6 Client. */
    status = nx_dhcpv6_client_delete(client_ptr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_client_delete failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       This function is called when the DHCPv6 Client changes DHCPv6 state. Since this
 *              is called from the DHCPv6 stack, the code should be brief with no blocking calls.
 *              and we set a event flag for the application to check.
 * @param[IN]   dhcpv6_ptr    pointer to the DHCPv6 Client instance.
 * @param[IN]   old_state     previous state the DHCPv6 Client was in.
 * @param[IN]   new_state     current state the DHCPv6 Client just changed to.
 * @retval      None
 **********************************************************************************************************************/
static VOID dhcpv6_state_change_notify(struct NX_DHCPV6_STRUCT *dhcpv6_ptr, UINT old_state, UINT new_state)
{
    NX_PARAMETER_NOT_USED(dhcpv6_ptr);
    NX_PARAMETER_NOT_USED(old_state);

    if (new_state == NX_DHCPV6_STATE_BOUND_TO_ADDRESS)
    {
        /* Set event flag. */
        tx_event_flags_set(&dhcpv6_client_event_flags_group, DHCP_EVENT, TX_OR);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is called when the DHCPv6 Client receives an error code from
 *              the server. Since this is called from the DHCPv6 stack, the code should be brief
 *              with no blocking calls.
 * @param[IN]   dhcpv6_ptr    pointer to the DHCPv6 Client instance.
 * @param[IN]   op_code       section of DHCPv6 request causing the error e.g. IA_ADDRESS, IA-NA, Option Field
 * @param[IN]   status_code   specifies the error status code received from the Server
 * @param[IN]   message_type  specifies which type of response ADVERTISE, REPLY etc) associated with the error status
 * @retval      None
 **********************************************************************************************************************/
static VOID dhcpv6_server_error_handler(struct NX_DHCPV6_STRUCT *dhcpv6_ptr, UINT op_code, UINT status_code, UINT message_type)
{
    NX_PARAMETER_NOT_USED(op_code);
    NX_PARAMETER_NOT_USED(dhcpv6_ptr);
    NX_PARAMETER_NOT_USED(status_code);
    NX_PARAMETER_NOT_USED(message_type);
}

/*******************************************************************************************************************//**
 * @brief       This function processes ipv6 address into string to print.
 * @param[IN]   ipv6     IPV6 address.
 * @param[OUT]  str      converted string.
 * @retval      None.
 **********************************************************************************************************************/
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6)
{
    sprintf((char *)str,"%lX:%lX:%lX:%lX:%lX:%lX:%lX:%lX",
    (((uint32_t)ipv6.nxd_ip_address.v6[0] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[0] & 0x0000FFFF ) >> 0),
    (((uint32_t)ipv6.nxd_ip_address.v6[1] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[1] & 0x0000FFFF ) >> 0),
    (((uint32_t)ipv6.nxd_ip_address.v6[2] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[2] & 0x0000FFFF ) >> 0),
    (((uint32_t)ipv6.nxd_ip_address.v6[3] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[3] & 0x0000FFFF ) >> 0)
    );
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_dhcpv6_client_ep)
 **********************************************************************************************************************/
