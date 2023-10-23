/***********************************************************************************************************************
 * File Name    : azuure_wifi_thread_entry.c
 * Description  : Contains NetX functions call for Wifi demonstration
 **********************************************************************************************************************/
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

#include "azure_wifi_thread.h"
#include "common_utils.h"
#include "NetX_wifi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup azure_wifi_thread_entry
 * @{
 **********************************************************************************************************************/

extern WiFiParameters_t g_wifi;
extern SocketPrameters_t g_socket;

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;

#if defined(ETHER_BUFFER_PLACE_IN_SECTION)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#elif defined(WIFI_BUFFER_PLACE_IN_SECTION)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) WIFI_BUFFER_PLACE_IN_SECTION;
#endif

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* Private function declarations.*/
static UINT ip0_init(void);
static UINT packet_pool0_init(void);
static UINT network_init(void);
static void convert_to_lowercase(uint8_t * p_str);

/*tcp socket variable*/
NX_TCP_SOCKET  tcp_client_socket;

/* Azure Wifi Thread entry function */
void azure_wifi_thread_entry(void)
{

    /*Local variable declarations*/
    char input_buff[BUFF_LEN] = {RESET_VALUE};             // Buffer for storing user input
    char tcp_serv_ip[BUFF_LEN] = {RESET_VALUE};            // Buffer to store entered tcp server IP
    uint8_t ip_addr[IP_BUFF_LEN] = {RESET_VALUE};          // IP address for ping operation
    int8_t dns_retry_count = WIFI_MAX_TRY;                 // Max retry count for DNS lookup
    int8_t ping_retry_count = WIFI_MAX_TRY;                // Max retry count for ping operation
    uint32_t bytes_copied=RESET_VALUE;                     // Number of bytes received from tcp server connection.
    fsp_err_t err = FSP_SUCCESS;                           // Return status variable for Error handling.
    uint8_t connect_menu_option = RESET_VALUE;             // Index to store menu option selected


    /* Initialize the RTT Thread.*/
        rtt_thread_init_check();

    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Opening WiFi module */
    err = rm_wifi_onchip_silex_open(&g_wifi_onchip_silex_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("** rm_wifi_onchip_silex_open API for rm_wifi_onchip_silex failed **");
        ERROR_TRAP(err);
    }
    PRINT_INFO_STR("wifi_onchip_silex_module initialized successfully");

    /* Reseting WiFi status flag */
    g_wifi.status = INT32_MAX;

    /* Connecting to WiFi AP */
    do
    {
        /* Menu for User Selection */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
        /*Accepting user Input for Menu option */
        get_user_input(input_buff);
        connect_menu_option = (uint8_t) atoi(input_buff);
        switch(connect_menu_option)
        {
            case SCAN_AP:
            {
                /* Scan for APs and connect to user chosen one */
                err = scan_and_select();
                if(FSP_SUCCESS != err)
                {
                    PRINT_ERR_STR("WiFi Access Point scanning failed.If multiple failures, please enter WiFi AP credentials.");
                    PRINT_ERR_STR("Please Use option 2 for manual connect to desired Wifi AP available.");
                }
                else
                {
                    /* Connecting to user selected WiFi AP */
                    PRINT_STR("Connecting to :");
                    PRINT_STR(g_wifi.ssid);
                    err = rm_wifi_onchip_silex_connect(g_wifi.ssid, g_wifi.security, g_wifi.pwd);
                    /* Handle error */
                    if(FSP_SUCCESS != err)
                    {
                        PRINT_ERR_STR("** rm_wifi_onchip_silex_connect API for rm_wifi_onchip_silex failed **");
                        PRINT_ERR_STR("Incorrect Access Point credentials entered. Try again.");
                    }
                }
            }
            break;
            case ENTER_SSID:
            {
                /* User manually enter WiFi AP credentials to connect */
                err = manual_connect();
                if(FSP_SUCCESS != err)
                {
                    PRINT_ERR_STR("Incorrect Access Point credentials entered. Try again.");
                }
            }
            break;
            default:
                PRINT_INFO_STR("Invalid Input.");
                break;
        }

        /* Status check of WiFi connectivity */
        err = rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
        if(FSP_SUCCESS != g_wifi.status)
        {
            PRINT_INFO_STR("WiFi not connected.");
        }
        else
        {
            PRINT_INFO_STR("WiFi connected.");
        }

    }while(FSP_SUCCESS != g_wifi.status);

    /* Get and print assigned IP address */
    err = rm_wifi_onchip_silex_ip_addr_get(g_wifi.ip_addr_device);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("** rm_wifi_onchip_silex_ip_addr_get API failed **");
        wifi_deinit();
        ERROR_TRAP(err);
    }
    /*Print MCU IP address*/
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_MCU_IP_ADDRESS, BUFF_LEN, g_wifi.ip_addr_device);

    /* Ping demonstration */
    do
    {
        /* User input for IP address or URL to ping */
        PRINT_STR("Enter the IP Address or URL to ping");
        get_user_input(input_buff);
        PRINT_STR("Pinging. Please wait");
        /* DNS lookup for URL or IP */
        err = dns_query(input_buff, ip_addr);
        if(FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("DNS lookup failed. Check IP/URL entered.");
            if(RESET_VALUE >= (dns_retry_count--))
            {
                PRINT_ERR_STR("Failed maximum retry count for DNS lookup. Reset MCU");
                wifi_deinit();
                ERROR_TRAP(err);
            }
        }
        /* Only if DNS lookup is successful, ping would be performed. */
        else
        {
            /* Ping URL or IP address */
            err = ping(ip_addr);
            if(FSP_SUCCESS != err)
            {
                PRINT_ERR_STR("Ping Unsuccessful");
                /* If ping fails maximum times, then EP fails */
                if(RESET_VALUE >= (ping_retry_count--))
                {
                    PRINT_ERR_STR("Failed maximum retry count for ping operation. Reset MCU");
                    wifi_deinit();
                    ERROR_TRAP(err);
                }
                PRINT_INFO_STR("Network device may be off-line or invalid URL/IP address. Try again or enter different URL/IP.");
                /* Reseting DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    }while(FSP_SUCCESS != err);
    PRINT_INFO_STR("Ping Successful");

    /* IP layer initialization */
    err = network_init();
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("** Network Initialization failed **");
        ERROR_TRAP(err);
    }

    /* Starting TCP client-server communication */
    PRINT_STR("Start TCP server in PC and start listening.");
    UINT status;
    /* Create the socket.  */
    status = nx_tcp_socket_create(&g_ip0, &tcp_client_socket, "TCP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY,
                                  NX_IP_TIME_TO_LIVE, NETXDUO_TESTS_TCP_WINDOW_SIZE, NX_NULL, NX_NULL);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_tcp_socket_create API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("TCP Socket created in client mode on MCU.");

    /* Resetting DNS and ping retry_count */
    dns_retry_count = WIFI_MAX_TRY;
    ping_retry_count = WIFI_MAX_TRY;
    /* User input for TCP server IP address and its validation */
    do
    {
        /* User input for TCP server IP address */
        PRINT_STR("Enter TCP server IP address to connect");
        /*Clear input buffer before input value*/
        memset(tcp_serv_ip,NULL_CHAR,sizeof(tcp_serv_ip));
        get_user_input(tcp_serv_ip);
        PRINT_STR("Validating TCP server IP");
        /* DNS lookup for TCP server IP address */
        err = dns_query(tcp_serv_ip, (uint8_t *) &g_socket.ip_addr_server);
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("DNS lookup failed. Check TCP server IP address entered.");
            if(RESET_VALUE >= (dns_retry_count--))
            {
                PRINT_ERR_STR("Failed maximum retry count for DNS lookup. Reset MCU");
                wifi_deinit();
                ERROR_TRAP(err);
            }
        }
        /* Only if DNS lookup is successful, ping would be performed. */
        else
        {
            PRINT_STR("Pinging TCP server IP");
            /* Pinging IP address to validate */
            err = ping((uint8_t *) &g_socket.ip_addr_server);
            /* Handle error */
            if(FSP_SUCCESS != err)
            {
                PRINT_ERR_STR("Ping Unsuccessful");
                /* If ping fails maximum times, then EP fails */
                if(RESET_VALUE >= (ping_retry_count--))
                {
                    PRINT_ERR_STR("Failed maximum retry count for ping");
                    wifi_deinit();
                    ERROR_TRAP(err);
                }
                PRINT_INFO_STR("TCP server may be off-line or invalid server IP address");
                /* Reseting DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }

        }
    }while(FSP_SUCCESS != err);

    /* User input for TCP server port number */
    PRINT_STR("Enter TCP server port");
    memset(input_buff,NULL_CHAR,sizeof(input_buff));
    get_user_input(input_buff);
    g_socket.port = (uint32_t) atoi(input_buff);

    /* Bind the socket to a port. */
    status = nx_tcp_client_socket_bind(&tcp_client_socket, 3050, NETXDUO_TESTS_SOCKET_TIMEOUT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_tcp_client_socket_bind API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("TCP socket bind successful");

    /*Connect to TCP server running*/
    status = nx_tcp_client_socket_connect(&tcp_client_socket,SOCKETS_htonl(g_socket.ip_addr_server),g_socket.port, NETXDUO_TESTS_SOCKET_TIMEOUT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_tcp_client_socket_connect API failed");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("TCP socket connected");

    NX_PACKET * p_nx_packet = NX_NULL;

    /* Allocate a packet. */
    status = nx_packet_allocate(&g_packet_pool0, &p_nx_packet, NX_TCP_PACKET, NETXDUO_TESTS_SOCKET_TIMEOUT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_allocate API failed");
        ERROR_TRAP(status);
    }

    /* Sending message to server */
    strcpy((char *) g_socket.send_buff, "Hello");

    /* Append data. */
    status = nx_packet_data_append(p_nx_packet, g_socket.send_buff, sizeof(g_socket.send_buff), &g_packet_pool0,
                                   NETXDUO_TESTS_SOCKET_TIMEOUT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_data_append API failed");
        ERROR_TRAP(status);
    }

    /* Send the packet. */
    status = nx_tcp_socket_send(&tcp_client_socket, p_nx_packet, NETXDUO_TESTS_SOCKET_TIMEOUT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_tcp_socket_send API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Sent Hello to the TCP Server");

    while (true)
    {
        /* Receiving message from server */
        memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));
        /* Receive the packet. */
        status = nx_tcp_socket_receive(&tcp_client_socket, &p_nx_packet,NETXDUO_TESTS_SOCKET_TIMEOUT);
        if(NX_SUCCESS != status)
        {
            if(NX_NO_PACKET == status)
            {
                PRINT_STR("Waiting for server message");
                continue;
            }
            else
            {
                PRINT_ERR_STR("nx_tcp_socket_receive API failed");
                ERROR_TRAP(status);
            }
        }
        else
        {
            /*Retrieve the data string from packet received*/
            status = nx_packet_data_retrieve(p_nx_packet, g_socket.recv_buff, &bytes_copied);
            if(NX_SUCCESS != status)
            {
                PRINT_ERR_STR("nx_packet_data_retrieve API failed");
                ERROR_TRAP(status);
            }
            convert_to_lowercase(g_socket.recv_buff);
            if(RESET_VALUE == strcmp((char *) g_socket.recv_buff, "on"))
            {
                #if defined (BOARD_RA6T3_MCK)
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_LOW);
                #else
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_HIGH);
                #endif
                app_rtt_print_data(RTT_OUTPUT_TCP_RECV_LED_STATUS, BUFF_LEN, g_socket.recv_buff);

                /* Sending acknowledgment to server */
                memcpy((char *) g_socket.send_buff, " Receive Success",BUFF_LEN);

                /* Append data to the send buffer */
                status = nx_packet_data_append(p_nx_packet, g_socket.send_buff, sizeof(g_socket.send_buff), &g_packet_pool0,
                                               NETXDUO_TESTS_SOCKET_TIMEOUT);
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_packet_data_append API failed");
                    ERROR_TRAP(status);
                }

                /* Send the packet. */
                status = nx_tcp_socket_send(&tcp_client_socket, p_nx_packet, NETXDUO_TESTS_SOCKET_TIMEOUT);
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_tcp_socket_send API failed");
                    ERROR_TRAP(status);
                }
            }
            else if(RESET_VALUE == strcmp((char *) g_socket.recv_buff, "off"))
            {
                #if defined (BOARD_RA6T3_MCK)
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_HIGH);
                #else
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_LOW);
                #endif
                app_rtt_print_data(RTT_OUTPUT_TCP_RECV_LED_STATUS, BUFF_LEN, g_socket.recv_buff);

                /* Sending acknowledgment to server */
                memcpy((char *) g_socket.send_buff, " Receive Success",BUFF_LEN);

                /* Append data. */
                status = nx_packet_data_append(p_nx_packet, g_socket.send_buff, sizeof(g_socket.send_buff), &g_packet_pool0,
                                               NETXDUO_TESTS_SOCKET_TIMEOUT);
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_packet_data_append API failed");
                    ERROR_TRAP(status);
                }

                /* Send the packet. */
                status = nx_tcp_socket_send(&tcp_client_socket, p_nx_packet, NETXDUO_TESTS_SOCKET_TIMEOUT);
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_tcp_socket_send API failed");
                    ERROR_TRAP(status);
                }

            }
            else
            {
                app_rtt_print_data(RTT_OUTPUT_TCP_RECV_INVALID, BUFF_LEN, g_socket.recv_buff);

                /* Sending acknowledgment to server */
                memcpy((char *) g_socket.send_buff, " Receive Invalid",BUFF_LEN);

                /* Append data. */
                status = nx_packet_data_append(p_nx_packet, g_socket.send_buff, sizeof(g_socket.send_buff), &g_packet_pool0,
                                               NETXDUO_TESTS_SOCKET_TIMEOUT);
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_packet_data_append API failed");
                    ERROR_TRAP(status);
                }

                /* Send the packet. */
                status = nx_tcp_socket_send(&tcp_client_socket, p_nx_packet, NETXDUO_TESTS_SOCKET_TIMEOUT);
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_tcp_socket_send API failed");
                    ERROR_TRAP(status);
                }
            }
        }
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions converts the input string pointed by the pointer to lower case.
 * @param[in]   p_str                       Pointer to string of uint8_t data-type.
 * @retval      None.
 **********************************************************************************************************************/
static void convert_to_lowercase(uint8_t * p_str)
{

    uint8_t index = RESET_VALUE;
    if(p_str != NULL)
    {
        for (index=RESET_VALUE;p_str[index]!=NULL_CHAR;index++)
        {
            p_str[index] = (uint8_t) tolower(p_str[index]);
        }
    }
}

/*********************************************************************************************************************
 * @brief      This function initializes the NetX system and Packet pool and IP initializations.
 * @param[IN]  None
 * @retval     NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful network initialization
 **********************************************************************************************************************/
static UINT network_init()
{
    UINT status = NX_SUCCESS;
    nx_system_initialize();

    status = packet_pool0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool0_init function Failed.");
        return status;
    }
    status = ip0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip0_init function Failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the packet pool.
 * @param[IN] None
 * @retval    NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful network initialization
 **********************************************************************************************************************/
static UINT packet_pool0_init(void)
{
    UINT status = NX_SUCCESS;

    /* Create the packet pool. */
         status = nx_packet_pool_create(&g_packet_pool0,
                    "g_packet_pool0 Packet Pool",
                    G_PACKET_POOL0_PACKET_SIZE,
                    &g_packet_pool0_pool_memory[0],
                    G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create API Failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the ip instance and enables the ARP, TCP, ICMP.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT ip0_init(void)
{
    UINT status = NX_SUCCESS;

    /* Create the ip instance. */
    status = nx_ip_create(&g_ip0,
                    "g_ip0 IP Instance",
                    IP_ADDRESS(g_wifi.ip_addr_device[INDEX_ZERO], g_wifi.ip_addr_device[INDEX_ONE],
                                g_wifi.ip_addr_device[INDEX_TWO], g_wifi.ip_addr_device[INDEX_THREE]),
                    G_IP0_SUBNET_MASK,
                    &g_packet_pool0,
                    G_IP0_NETWORK_DRIVER,
                    &g_ip0_stack_memory[0],
                    G_IP0_TASK_STACK_SIZE,
                    G_IP0_TASK_PRIORITY);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create API Failed.");
        return status;
    }

    /* Enable the ARP */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable API Failed.");
        return status;
    }

    /* Enable the TCP */
    status = nx_tcp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable API Failed.");
        return status;
    }

    /* Enable the ping using Internet Control Message Protocol */
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable API Failed.");
        return status;
    }

    /* This function enables the IGMP management component for the specified IP instance */
    status = nx_igmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_igmp_enable API Failed.");
        return status;
    }

    /* Check the connection status. */
    ULONG current_state;
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, 1000U);
    if(NX_IP_LINK_ENABLED != current_state)
    {
        PRINT_ERR_STR(" IP status check could not be completed within the timeout specified");
    }
    else if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_status_check API Failed.");
    }
    else
    {
        PRINT_INFO_STR("IP Status check successful");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup azure_wifi_thread_entry)
 **********************************************************************************************************************/

