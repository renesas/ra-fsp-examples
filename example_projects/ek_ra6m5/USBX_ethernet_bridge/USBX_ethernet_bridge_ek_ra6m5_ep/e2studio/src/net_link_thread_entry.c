/***********************************************************************************************************************
* File Name    : net_link_thread_entry.c
* Description  : Contains data structures and functions used in the network physical link thread
**********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "net_link_thread.h"
#include "net_link.h"
#include "ux_device_class_rndis.h"
#include "ux_network_driver.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern TX_THREAD net_app_thread;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static UINT net_system_startup(void);
static UINT usbx_rndis_init(void);
static UINT net_system_get_info(net_info_t * p_info);
static UINT dhcpv4_server_init(void);
static void net_dhcp_client_info(net_info_t * p_info);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT) = {RESET_VALUE};
static uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4) = {RESET_VALUE};
static net_info_t g_client_network_info = {RESET_VALUE};
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE] = {RESET_VALUE};
static NX_DHCP_SERVER g_dhcp_server0 = {RESET_VALUE};
static uint8_t g_dhcp_server0_stack_memory[G_DHCP_SERVER0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_dhcp_server0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT) = {RESET_VALUE};

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
NX_IP g_ip0 = {RESET_VALUE};
NX_PACKET_POOL g_packet_pool0 = {RESET_VALUE};

net_info_t g_usb_network_info = {RESET_VALUE};
uint8_t g_packet_pool0_pool_memory[(G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION = {RESET_VALUE};

/***********************************************************************************************************************
 * Global function prototypes
 **********************************************************************************************************************/
UINT usbx_status_callback (ULONG status);

/***********************************************************************************************************************
 * External Variables
 **********************************************************************************************************************/
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_language_id_framework[];
extern uint8_t g_string_framework[];

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
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
    TERM_ERROR_TRAP(TX_SUCCESS != status, status, "net_system_startup failed\r\n");

    while (true)
    {
        /* Wait for USB attach event flag.  */
        tx_event_flags_get (&g_usb_event_flags, USB_CONNECTION_ANY_EVENT, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);
        TERM_ERROR_TRAP(TX_SUCCESS != status, status, "tx_event_flags_get for USB event failed\r\n");

        if(events & USB_ATTACH_FLAG)
        {
            PRINT_INFO_STR("\r\nThe host PC's IPv4 address is used to create TCP/UDP servers.");

            net_dhcp_client_info(&g_client_network_info);

            /* Send an output queue to print the DHCP client network configuration. */
            PRINT_INFO_STR("\r\nThe Host PC network information:");
            PRINT_ID_DATA(TERMINAL_OUTPUT_NET_INFO, g_client_network_info);

            /* Get the network configuration details. */
            status = net_system_get_info(&g_usb_network_info);
            TERM_ERROR_TRAP(NX_SUCCESS != status, status, "net_system_get_info failed\r\n");

            /* Send an output queue to print the DHCP server network configuration. */
            PRINT_INFO_STR("\r\nThe RA board network information:");
            PRINT_ID_DATA(TERMINAL_OUTPUT_NET_INFO, g_usb_network_info);

            /* Send an output queue to request print the menu options. */
            PRINT_EP_MENU;

            /* Resume the network application thread. */
            tx_thread_resume(&net_app_thread);
        }
        if(events & USB_REMOVE_FLAG)
        {
            /* Suspend the network application thread. */
            tx_thread_suspend(&net_app_thread);
        }

        tx_thread_sleep (NET_LINK_THREAD_SLEEP_TICK);
    }
}

/***********************************************************************************************************************
 * @brief Initializes the USBX RNDIS.
 *
 * This function performs the following tasks:
 * - Initializes the USBX systems.
 * - Initializes the RNDIS parameters.
 * - Initializes USBX device stack.
 * - Registers the device rndis class to USBX device stack.
 * - Initializes the USBX network layers.
 * - Initializes the USB basic driver.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT usbx_rndis_init(void)
{
    UINT status = NX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the USBX system. */
    status = ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    TERM_ERROR_RET(UX_SUCCESS != status, status, "ux_system_initialize failed");

    /* Initialize the RNDIS parameters */
    UX_SLAVE_CLASS_RNDIS_PARAMETER parameter;

    /* Set NULL to the parameters for callback when insertion/extraction of an RNDIS device.*/
    parameter.ux_slave_class_rndis_instance_activate = UX_NULL;
    parameter.ux_slave_class_rndis_instance_deactivate = UX_NULL;

    /* Define the MAC address used by the MCU for the RNDIS interface. */
    parameter.ux_slave_class_rndis_parameter_local_node_id[0] = 0x00;
    parameter.ux_slave_class_rndis_parameter_local_node_id[1] = 0x1e;
    parameter.ux_slave_class_rndis_parameter_local_node_id[2] = 0x58;
    parameter.ux_slave_class_rndis_parameter_local_node_id[3] = 0x41;
    parameter.ux_slave_class_rndis_parameter_local_node_id[4] = 0xb8;
    parameter.ux_slave_class_rndis_parameter_local_node_id[5] = 0x78;

    /* Define the MAC address for the RNDIS interface presented to the host PC. */
    parameter.ux_slave_class_rndis_parameter_remote_node_id[0] = 0x00;
    parameter.ux_slave_class_rndis_parameter_remote_node_id[1] = 0x1e;
    parameter.ux_slave_class_rndis_parameter_remote_node_id[2] = 0x58;
    parameter.ux_slave_class_rndis_parameter_remote_node_id[3] = 0x41;
    parameter.ux_slave_class_rndis_parameter_remote_node_id[4] = 0xb8;
    parameter.ux_slave_class_rndis_parameter_remote_node_id[5] = 0x79;

    /* Set extra parameters used by the RNDIS query command with certain OIDs. */
    parameter.ux_slave_class_rndis_parameter_vendor_id = 0x045b ;
    parameter.ux_slave_class_rndis_parameter_driver_version = 0x1127;
    ux_utility_memory_copy(parameter.ux_slave_class_rndis_parameter_vendor_description, "ELOGIC RNDIS", 12);

    /* Initialize USBX device stack. */
    status = ux_device_stack_initialize(g_device_framework_hi_speed,
                                        DEVICE_FRAME_LENGTH_HI_SPEED,
                                        g_device_framework_full_speed,
                                        DEVICE_FRAME_LENGTH_FULL_SPEED,
                                        g_string_framework,
                                        STRING_FRAMEWORK_LENGTH,
                                        g_language_id_framework,
                                        LANGUAGE_ID_FRAME_WORK_LENGTH,
                                        &usbx_status_callback);
    TERM_ERROR_RET(UX_SUCCESS != status, status, "ux_device_stack_initialize failed");

    /* Register the device RNDIS class to USBX device stack. */
    status = ux_device_stack_class_register (_ux_system_slave_class_rndis_name, _ux_device_class_rndis_entry,
                                             USB_CONFIG_NUMB, USB_INTERFACE_NUMB0, &parameter);
    TERM_ERROR_RET(UX_SUCCESS != status, status, "ux_device_stack_class_register failed");

    /* Initialize the USBX network layer.*/
    status = ux_network_driver_init();
    TERM_ERROR_RET(UX_SUCCESS != status, status, "ux_network_driver_init failed");

    /* Initialize the USB basic module */
    err = (UINT) R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    TERM_ERROR_RET(FSP_SUCCESS != err, (UINT) err, "R_USB_Open API failed");

    PRINT_INFO_STR("Initialize USBX RNDIS stack successfully.");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     Create the DHCPV4 server instance.
 * @param[IN] None
 * @retval    NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT dhcpv4_server_init(void)
{
    UINT status             = NX_SUCCESS;
    UINT addresses_added    = RESET_VALUE;

    /* Create the DHCP server instance. */
    status = nx_dhcp_server_create (&g_dhcp_server0, &g_ip0, g_dhcp_server0_stack_memory, G_DHCP_SERVER0_TASK_STACK_SIZE,
                                    "g_dhcp_server0", &g_packet_pool0);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_dhcp_server_create failed");

    /* Create a pool of IP addresses from the given range. */
    status = nx_dhcp_create_server_ip_address_list(&g_dhcp_server0, USB_RNDIS_PORT_INTERFACE_INDEX,
                                                   START_IP_ADDRESS_LIST, END_IP_ADDRESS_LIST, &addresses_added);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_dhcp_create_server_ip_address_list failed");


    /* Set network parameters for RNDIS interface. */
    status = nx_dhcp_set_interface_network_parameters(&g_dhcp_server0, USB_RNDIS_PORT_INTERFACE_INDEX, NX_DHCP_SUBNET_MASK,
                                                      USB_RNDIS_DHCP_SERVER_DEFAULT_GATEWAY, USB_RNDIS_DHCP_SERVER_IP_ADDRESS);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_dhcp_set_interface_network_parameters failed");

    /* Start the DHCP Server. */
    status = nx_dhcp_server_start(&g_dhcp_server0);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_dhcp_server_start failed");

    return status;
}

/***********************************************************************************************************************
 * @brief Initializes the NetX system and sets up the network configuration.
 *
 * This function performs the following tasks:
 * - Initializes the NetX and FileX systems.
 * - Creates a packet pool for network packets.
 * - Creates an IP instance for the Ethernet interface.
 * - Enables ARP, ICMP, UDP, and TCP protocols.
 * - Waits for the network link to be established.
 * - Creates and starts a DHCP server for assign IP to Host PC.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT net_system_startup(void)
{
    UINT status = NX_SUCCESS;
    ULONG events = RESET_VALUE;
    ULONG links = NX_NULL;

    /* Initialize the NetX system. */
    nx_system_initialize();

    /* Initialize the FileX file system. */
    fx_system_initialize();

    /* Initialize the USBX RNDIS class. */
    status = usbx_rndis_init();
    TERM_ERROR_RET(TX_SUCCESS != status, status, "usbx_rndis_init failed\r\n");

    /* Create a packet pool for the network packet buffer. */
    status = nx_packet_pool_create(&g_packet_pool0, "NX Packet Pool",
                                    G_PACKET_POOL0_PACKET_SIZE, &g_packet_pool0_pool_memory[0],
                                    G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_packet_pool_create failed");

    /* Create an IP instance for the Ethernet interface. */
    status = nx_ip_create(&g_ip0, "g_ip0 IP Instance", G_IP0_ADDRESS, G_IP0_SUBNET_MASK,
                          &g_packet_pool0, G_IP0_NETWORK_DRIVER, &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE, G_IP0_TASK_PRIORITY);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_ip_create failed");

    /* Attach the secondary interface to the IP instance */
    status =  nx_ip_interface_attach(&g_ip0, "RNDIS_PORT", USB_RNDIS_DHCP_SERVER_IP_ADDRESS, G_IP0_SUBNET_MASK, _ux_network_driver_entry);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_ip_interface_attach failed");

    status = nx_ip_interface_status_check(&g_ip0, USB_RNDIS_PORT_INTERFACE_INDEX, NX_IP_INITIALIZE_DONE, &links, NET_IP_ADDRESS_RESOLVED_WAIT);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_ip_interface_status_check failed");

    PRINT_INFO_STR("Initialize the IP instance for the USB RNDIS interface successfully.");

    /* Wait for USB attach event flags. */
    tx_event_flags_get (&g_usb_event_flags, USB_ATTACH_FLAG, TX_OR, &events, TX_WAIT_FOREVER);
    TERM_ERROR_RET(TX_SUCCESS != status, status, "tx_event_flags_get for USB event failed\r\n");

    /* Enable the Address Resolution Protocol (ARP). */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_arp_enable failed");

    /* Enable the Internet Control Message Protocol (ICMP). */
    status = nx_icmp_enable(&g_ip0);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_icmp_enable failed");

    /* Enable the User Datagram Protocol (UDP). */
    status = nx_udp_enable(&g_ip0);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_udp_enable failed");

    /* Enable the Transmission Control Protocol (TCP). */
    status = nx_tcp_enable(&g_ip0);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_tcp_enable failed");

    status = nx_ip_interface_status_check (&g_ip0, USB_RNDIS_PORT_INTERFACE_INDEX, NX_IP_LINK_ENABLED, &links,
                                           NET_IP_LINK_WAIT);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_ip_interface_status_check failed");

    status = dhcpv4_server_init();
    TERM_ERROR_RET(NX_SUCCESS != status, status, "dhcpv4_server_init failed");

    return status;
}

/***********************************************************************************************************************
 * @brief Retrieves the DHCP client network configuration information.
 *
 * This function gets the MAC address, IPv4 address, subnet mask,
 * default gateway, and DHCP server address for the specified network.
 *
 * @param[out] p_info   Pointer to a net_info_t structure where the retrieved
 *                      network information will be stored.
 *
 * @retval              None.
 **********************************************************************************************************************/
static void net_dhcp_client_info(net_info_t * p_info)
{
    /* Wait until the DHCP server successfully assigns an IP address to the client. */
    while (RESET_VALUE == g_dhcp_server0.nx_dhcp_number_clients)
    {
        tx_thread_sleep (NET_LINK_THREAD_SLEEP_TICK);
    }

    /* Get the MAC address of the Host PC. */
    ULONG client_mac_addr_msb   = RESET_VALUE;
    ULONG client_mac_addr_lsb   = RESET_VALUE;

    client_mac_addr_msb = g_dhcp_server0.client_records[g_dhcp_server0.nx_dhcp_number_clients-1].nx_dhcp_client_mac_msw;
    client_mac_addr_lsb = g_dhcp_server0.client_records[g_dhcp_server0.nx_dhcp_number_clients-1].nx_dhcp_client_mac_lsw;

    /* Get the IPv4 address of the Host PC. */
    ULONG client_ipv4           = RESET_VALUE;

    client_ipv4 = g_dhcp_server0.client_records[g_dhcp_server0.nx_dhcp_number_clients-1].nx_dhcp_assigned_ip_address;

    /* Store the network configuration. */
    p_info->phy_addr.group.msb = client_mac_addr_msb;
    p_info->phy_addr.group.lsb = client_mac_addr_lsb;
    p_info->ipv4_addr.full = client_ipv4;
    p_info->subnet_mask.full = NX_DHCP_SUBNET_MASK;
}

/***********************************************************************************************************************
 * @brief Retrieves the DHCP server network configuration information.
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

    status = nx_ip_interface_physical_address_get(&g_ip0, USB_RNDIS_PORT_INTERFACE_INDEX, &mac_addr_msb, &mac_addr_lsb);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_ip_interface_physical_address_get failed");

    /* Get the IPv4 address of the RA board. */
    ULONG dev_ip_addr = NX_NULL;
    ULONG dev_subnet_mask = NX_NULL;

    status = nx_ip_interface_address_get(&g_ip0, USB_RNDIS_PORT_INTERFACE_INDEX, &dev_ip_addr, &dev_subnet_mask);
    TERM_ERROR_RET(NX_SUCCESS != status, status, "nx_ip_interface_address_get failed");

    /* Store the network configuration. */
    p_info->phy_addr.group.msb = mac_addr_msb;
    p_info->phy_addr.group.lsb = mac_addr_lsb;
    p_info->ipv4_addr.full = dev_ip_addr;
    p_info->subnet_mask.full = dev_subnet_mask;

    return status;
}

/***********************************************************************************************************************
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

/***********************************************************************************************************************
 * @brief     In this function, usb callback events will be captured into usb event flag.
 * @param[IN] usb_status    Whenever any event occurred, status gets update
 * @retval    UX_SUCCESS
 **********************************************************************************************************************/
UINT usbx_status_callback (ULONG usb_status)
{
    switch (usb_status)
    {
        case UX_DEVICE_ATTACHED:
        {
            PRINT_INFO_STR("The USB is attached.\r\n");
            tx_event_flags_set(&g_usb_event_flags, USB_ATTACH_FLAG, TX_OR);
        }
        break;

        case UX_DEVICE_REMOVED:
        {
            PRINT_INFO_STR("The USB is removed.\r\n");
            tx_event_flags_set(&g_usb_event_flags, USB_REMOVE_FLAG, TX_OR);
        }
        break;

        default:
        {
            /* do nothing */
        }
        break;
    }

    return UX_SUCCESS;
}

