/***********************************************************************************************************************
* File Name    : net_link_thread_entry.c
* Description  : Contains data structures and functions used in the network physical link thread
**********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <net_link_thread.h>
#include "net_link.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern TX_THREAD net_app_thread;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static UINT net_system_startup(void);
static UINT net_system_link_up_process(void);
static UINT net_system_get_info(net_info_t * p_info);
static VOID net_link_status_change_callback (NX_IP * p_ip, UINT index, UINT status);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
static uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);
static TX_EVENT_FLAGS_GROUP g_net_link_event_flags;

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
NX_IP g_ip0;
NX_PACKET_POOL g_packet_pool0;
NX_DHCP g_dhcp_client0;
net_info_t g_network_info;
uint8_t g_packet_pool0_pool_memory[(G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;;

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Callback function for link status changes.
 *
 * This function is called when the link status changes. It sets the event flag
 * to notify the system of the link status change.
 *
 * @param[in] p_ip Pointer to the NX_IP instance.
 * @param[in] index Index of the IP instance interface.
 * @param[in] status New link status.
 **********************************************************************************************************************/
static VOID net_link_status_change_callback (NX_IP * p_ip, UINT index, UINT status)
{
    NX_PARAMETER_NOT_USED(p_ip);
    NX_PARAMETER_NOT_USED(index);

    /* Set the link status change event flag. */
    tx_event_flags_set(&g_net_link_event_flags, NX_TRUE << status, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief Entry function for the network link thread.
 *
 * This function manages the network link status and handles events related to
 * link changes. It also starts the network application thread and manages
 * link-up and link-down events.
 **********************************************************************************************************************/
void net_link_thread_entry(void)
{
    UINT status = NX_SUCCESS;
    ULONG events = RESET_VALUE;

    /* Send an output queue to print the EP banner. */
    PRINT_EP_BANNER;
    tx_thread_sleep (NET_LINK_THREAD_SLEEP_TICK);

    /* Send an output queue to print the EP info. */
    PRINT_EP_INFO;
    tx_thread_sleep (NET_LINK_THREAD_SLEEP_TICK);

    /* Startup the network system. */
    status = net_system_startup();
    TERM_ERR_TRAP(TX_SUCCESS != status, status, "net_system_startup failed\r\n");

    /* Send an output queue to print the current network configuration. */
    PRINT_ID_DATA(TERMINAL_OUTPUT_NET_INFO, g_network_info);

    /* Start the network application thread, which does not start automatically during initialization. */
    tx_thread_resume(&net_app_thread);

    while (true)
    {
        /* Wait for any NetX link status change event flags.  */
        status = tx_event_flags_get(&g_net_link_event_flags, LINK_ANY_EVENT, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);
        TERM_ERR_TRAP(TX_SUCCESS != status, status, "tx_event_flags_get for NetX link event failed\r\n");

        /* Handling the physical link-up event. */
        if (events & LINK_UP_EVENT)
        {
            /* Perform necessary procedures when physical link-up. */
            status = net_system_link_up_process();
            TERM_ERR_TRAP(TX_SUCCESS != status, status, "net_system_link_up_process failed\r\n");

            /* Send an output queue to print the current network configuration. */
            PRINT_ID_DATA(TERMINAL_OUTPUT_NET_INFO, g_network_info);

            /* Resume the network application thread. */
            tx_thread_resume(&net_app_thread);
        }

        /* Handling the physical link-down event */
        if (events & LINK_DOWN_EVENT)
        {
            /* Suspend the network application thread. */
            tx_thread_suspend(&net_app_thread);
        }

        tx_thread_sleep (NET_LINK_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief Initializes the NetX system and sets up the network configuration.
 *
 * This function performs the following tasks:
 * - Initializes the NetX and FileX systems.
 * - Creates event flags for network link events.
 * - Creates a packet pool for network packets.
 * - Creates an IP instance for the Ethernet interface.
 * - Sets up a callback for link status changes.
 * - Enables ARP, ICMP, UDP, and TCP protocols.
 * - Waits for the network link to be established.
 * - Creates and starts a DHCP client for IP address resolution.
 * - Retrieves network configuration details.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT net_system_startup(void)
{
    UINT status = NX_SUCCESS;

    /* Initialize the NetX system. */
    nx_system_initialize();

    /* Initialize the FileX file system. */
    fx_system_initialize();

    /* Create the network physical link event flags. */
    status = tx_event_flags_create(&g_net_link_event_flags, "Network Link Events");
    APP_ERR_RET(NX_SUCCESS != status, status, "tx_event_flags_create for NetX link event failed");

    /* Create a packet pool for the network packet buffer. */
    status = nx_packet_pool_create(&g_packet_pool0, "NX Packet Pool",
                                    G_PACKET_POOL0_PACKET_SIZE, &g_packet_pool0_pool_memory[0],
                                    G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    APP_ERR_RET(NX_SUCCESS != status, status, "nx_packet_pool_create failed");

    /* Create an IP instance for the Ethernet interface. */
    status = nx_ip_create(&g_ip0, "g_ip0 IP Instance", NX_NULL, NX_NULL,
                          &g_packet_pool0, G_IP0_NETWORK_DRIVER, &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE, G_IP0_TASK_PRIORITY);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_create failed");

    /* Set a callback function to notify the physical interface of changes. */
    status = nx_ip_link_status_change_notify_set(&g_ip0, net_link_status_change_callback);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_link_status_change_notify_set failed");

    /* Enable the Address Resolution Protocol (ARP). */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_arp_enable failed");

    /* Enable the Internet Control Message Protocol (ICMP). */
    status = nx_icmp_enable(&g_ip0);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_icmp_enable failed");

    /* Enable the User Datagram Protocol (UDP). */
    status = nx_udp_enable(&g_ip0);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_udp_enable failed");

    /* Enable the Transmission Control Protocol (TCP). */
    status = nx_tcp_enable(&g_ip0);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_tcp_enable failed");

    /* Wait for the network physical link to be established. */
    ULONG events = NX_NULL;
    status = tx_event_flags_get(&g_net_link_event_flags, LINK_UP_EVENT, TX_AND_CLEAR, &events, TX_WAIT_FOREVER);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "tx_event_flags_get for NetX link up event failed");

    /* Create a DHCP instance for the DHCP client service. */
    status = nx_dhcp_create(&g_dhcp_client0, &g_ip0, "NX DHCP Client");
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_create failed");

    /* Set the packet pool for the DHCP client instance. */
    status = nx_dhcp_packet_pool_set(&g_dhcp_client0, &g_packet_pool0);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_packet_pool_set failed");

    /* Start the DHCP client service process. */
    status = nx_dhcp_start(&g_dhcp_client0);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_start failed");

    /* Wait for the DHCP client to resolve the IP address. */
    ULONG link = NX_NULL;
    status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &link, NX_WAIT_FOREVER);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_status_check for resolving the IP address failed");

    /* Get the network configuration details. */
    status = net_system_get_info(&g_network_info);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "net_system_get_info failed");

    return status;
}

/*******************************************************************************************************************//**
 * @brief Processes the network link-up event.
 *
 * This function checks if the DHCP client has resolved the IP address.
 * If not, it enables the network link, stops the DHCP client service,
 * reinitializes the DHCP client, and starts the service again.
 * Finally, it retrieves the current network configuration details.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT net_system_link_up_process(void)
{
    UINT status = NX_SUCCESS;
    ULONG link = NX_NULL;

    /* Wait for the DHCP client to resolve the IP address. */
    status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &link, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        /* Send a direct command to enable the link. */
        status = nx_ip_driver_direct_command(&g_ip0, NX_LINK_ENABLE, &link);
        TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_driver_direct_command for enabling the link failed");

        /* Stop the DHCP client service. */
        status = nx_dhcp_stop(&g_dhcp_client0);
        TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_stop failed");

        /* Reinitialize the DHCP client to clean up parameters. */
        status = nx_dhcp_reinitialize(&g_dhcp_client0);
        TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_reinitialize failed");

        /* Start the DHCP Client service. */
        status = nx_dhcp_start(&g_dhcp_client0);
        TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_start failed");

        /* Wait for the DHCP client to resolve the IP address. */
        status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &link, NX_WAIT_FOREVER);
        TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_status_check for resolving the IP address failed");
    }

    /* Get the network configuration details. */
    status = net_system_get_info(&g_network_info);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "net_system_get_info failed");

    return status;
}

/*******************************************************************************************************************//**
 * @brief Retrieves the network configuration information.
 *
 * This function gets the MAC address, IPv4 address, subnet mask,
 * default gateway, and DHCP server address for the specified network.
 *
 * @param[out] p_info   Pointer to a net_info_t structure where the retrieved
 *                      network information will be stored.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT net_system_get_info(net_info_t * p_info)
{
    UINT status = NX_SUCCESS;

    /* Get the MAC address of the RA board. */
    ULONG mac_addr_msb = NX_NULL;
    ULONG mac_addr_lsb = NX_NULL;
    status = nx_ip_interface_physical_address_get(&g_ip0, 0, &mac_addr_msb, &mac_addr_lsb);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_interface_physical_address_get failed");

    /* Get the IPv4 address of the RA board. */
    ULONG dev_ip_addr = NX_NULL;
    ULONG dev_subnet_mask = NX_NULL;
    status = nx_ip_address_get(&g_ip0, &dev_ip_addr, &dev_subnet_mask);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_ip_address_get failed");

    /* Retrieve the IP address of the default gateway. */
    UCHAR dhcp_option[NET_DHCP_OPTION_SIZE];
    UINT dhcp_option_size = NET_DHCP_OPTION_SIZE;
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_GATEWAYS, dhcp_option, &dhcp_option_size);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_user_option_retrieve for retrieving the default gateway's IP address failed");
    ULONG default_gateway_addr = IP_ADDRESS(dhcp_option[3], dhcp_option[2], dhcp_option[1], dhcp_option[0]);

    /* Retrieve the IP address of the DHCP server. */
    dhcp_option_size = NET_DHCP_OPTION_SIZE;
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DHCP_SERVER, dhcp_option, &dhcp_option_size);
    TERM_ERR_TRAP(NX_SUCCESS != status, status, "nx_dhcp_user_option_retrieve for retrieving the DHCP server's IP address failed");
    ULONG dhcp_server_addr = IP_ADDRESS(dhcp_option[3], dhcp_option[2], dhcp_option[1], dhcp_option[0]);

    /* Store the network configuration. */
    p_info->phy_addr.group.msb = mac_addr_msb;
    p_info->phy_addr.group.lsb = mac_addr_lsb;
    p_info->ipv4_addr.full = dev_ip_addr;
    p_info->subnet_mask.full = dev_subnet_mask;
    p_info->default_gateway.full = default_gateway_addr;
    p_info->dhcp_addr.full = dhcp_server_addr;

    return status;
}

/*******************************************************************************************************************//**
 * @brief Sends an ICMP ping request to the specified IP address.
 *
 * This function allocates a packet for the ping command, sends a ping request,
 * and releases the allocated packet afterward.
 *
 * @param[in] ip_addr The IP address to which the ping request will be sent.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
UINT net_system_ping(ULONG ip_addr)
{
    UINT status = NX_SUCCESS;
    NX_PACKET * p_packet = NULL;

    /* Allocate an ICMP packet for the ping command. */
    status = nx_packet_allocate(&g_packet_pool0, &p_packet, NX_ICMP_PACKET, NX_IP_PERIODIC_RATE);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Send a ping request to the specified address. */
    status = nx_icmp_ping(&g_ip0, ip_addr, "Hello", sizeof("Hello"), &p_packet, NET_ICMP_PING_WAIT);

    /* Release the packet. */
    nx_packet_release(p_packet);

    return status;
}
