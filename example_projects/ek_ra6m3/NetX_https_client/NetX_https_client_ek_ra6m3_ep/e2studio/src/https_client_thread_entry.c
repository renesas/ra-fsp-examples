/***********************************************************************************************************************
 * File Name    : https_client_thread_entry.c
 * Description  : Contains entry function of HTTPS Client.
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

/* Include Header Files. */
#include "https_client_thread.h"
#include "common_utils.h"
#include "https_client_app.h"
#include "adc_app.h"
#include "user_app.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_https_client_ep
 * @{
 **********************************************************************************************************************/

/* Auto generated global variables */
/* Packet pool instance. */
NX_PACKET_POOL g_packet_pool0;
/* Web HTTP Client instance. */
NX_WEB_HTTP_CLIENT  g_web_http_client0;
/* IP instance */
NX_IP g_ip0;
/* DHCP instance. */
NX_DHCP g_dhcp_client0;
/* DNS instance. */
NX_DNS g_dns0;

NX_PACKET *response_ptr = NULL;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* Packet Pool Memory for g_packet_pool0. */
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

/* Buffer to read Response from server. */
extern UCHAR g_receive_buffer[DATA_SIZE];
/* variable to capture invalid input state*/
volatile bool b_invalid_input = false;

/* IPv4 Address structure for server. */
static NXD_ADDRESS         server_ip_address;
static ULONG server_addr     = RESET_VALUE;
static ULONG dns_server_addr = RESET_VALUE;

static void nx_common_init0(void);
static UINT web_http_client0_init(void);
static UINT ip0_init(void);
static UINT packet_pool0_init0(void);
static UINT dhcpv4_client_init(void);
static UINT dns_client_init(void);
static UINT Process_InputRequest(uint8_t input_data);

/*******************************************************************************************************************//**
 * @brief      This is the User Thread Entry for the EP.
 * @param[in]  Thread specific parameters
 * @retval     None
 **********************************************************************************************************************/
void https_client_thread_entry(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT status             = NX_SUCCESS;
    UINT err                = TX_SUCCESS;
    fsp_err_t adc_err       = FSP_SUCCESS;
    /* Client IP address and Network Mask. */
    ULONG client_ip_address = RESET_VALUE;
    ULONG network_mask      = RESET_VALUE;
    uint8_t read_data       = RESET_VALUE;
    float init_adc_val      = RESET_VALUE;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();
    /* Print EP and Banner info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the ADC. */
    adc_err = hal_adc_init();
    if(FSP_SUCCESS != adc_err)
    {
        PRINT_ERR_STR("hal_adc_init() Function failed.");
        ERROR_TRAP(adc_err);
    }

    /* Initialize NetX. */
    nx_common_init0();

    /* Initialize Packet Pool. */
    status = packet_pool0_init0();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool0_init0() Function failed.");
        ERROR_TRAP(status);
    }

    /* Initialize IP. */
    status = ip0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip0_init() Function failed.");
        ERROR_TRAP(status);
    }

    /* Initialize DHCP v4 Client. */
    status = dhcpv4_client_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_client_init() Function failed.");
        ERROR_TRAP(status);
    }

    /* Initialize DNS Client.  */
    status = dns_client_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dns_client_init() Function failed.");
        ERROR_TRAP(status);
    }

    /* Initialize Web HTTPS Client. */
    status = web_http_client0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("web_http_client0_init() Function failed.");
        ERROR_TRAP(status);
    }

    /* Initialize the TLS Component. */
    nx_secure_tls_initialize();

    /* Add Server IP address and IPv4 info in structure. */
    server_ip_address.nxd_ip_address.v4 = server_addr;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V4;

    /* Use the 'NetX' service to connect to the HTTPS server (Adafuit.io server in our case). */
    status = nx_web_http_client_secure_connect (&g_web_http_client0, &server_ip_address, NX_WEB_HTTPS_SERVER_PORT,
                                                tls_setup_callback, SECURE_CONNECT_WAIT_TIME);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_secure_connect API failed.");
        deinit_https_client();
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("HTTPS Client is connected to the server successfully!");

    /* Retrieve the IP Address. */
    status = nx_ip_address_get (&g_ip0, &client_ip_address, &network_mask);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_address_get API failed.");
        deinit_https_client();
        ERROR_TRAP(status);
    }

    /* Print the client IP address. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip_address);

    /*Add an initial data on an empty feed. */
    PRINT_INFO_STR("Adding an initial data on the feed...");
    /* Read the temperature value from the ADC. */
    init_adc_val = adc_data_read();
    /* Post Request. */
    status = processPostRequest(init_adc_val);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("processPostRequest() Function failed.");
        deinit_https_client();
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Posted initial data on the feed successfully!");

    /* Print the Menu Options */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    /* Process the data as per the user selected option from the menu. */
    while (true)
    {
        /* Wait for user input. */
        do
        {
            err = check_for_rtt_user_ip(&read_data);
        }while (err != TX_SUCCESS);

        /* Processes the Input Request from the User. */
        status = Process_InputRequest (read_data);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("Process_InputRequest() function failed.");
            deinit_https_client();
            ERROR_TRAP(status);
        }

        if (true != b_invalid_input)
        {
            /* Read data from the HTTPS server. */
            status = readResponsefromServer ();
            if(NX_SUCCESS != status)
            {
                PRINT_ERR_STR("readResponsefromServer() function failed.");
                deinit_https_client();
                ERROR_TRAP(status);
            }
            else
            {
                /* Print the received feed data from the server. */
                APP_PRINT("Received data: %s\n", g_receive_buffer);
                PRINT_INFO_STR("Read the response from server Successfully!");
            }
        }
        /*Reset the flag*/
        b_invalid_input = false;
        /* Print the Menu Options */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the NetX System.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void nx_common_init0(void)
{
    /* Initialize the NetX system. */
    nx_system_initialize ();
}

/*******************************************************************************************************************//**
 * @brief     This function creates the packet pool.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT packet_pool0_init0(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT status = NX_SUCCESS;

    /* Create the packet pool. */
    status = nx_packet_pool_create(&g_packet_pool0,
                                   "g_packet_pool0 Packet Pool",
                                   G_PACKET_POOL0_PACKET_SIZE,
                                   &g_packet_pool0_pool_memory[0],
                                   G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create API failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the Web HTTPS Client.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT web_http_client0_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT status = NX_SUCCESS;

    /* Create the Web HTTP Client instance. */
    status = nx_web_http_client_create(&g_web_http_client0,
                                       "g_web_http_client0",
                                       &g_ip0,
                                       &g_packet_pool0,
                                       G_WEB_HTTP_CLIENT0_WINDOW_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_create API failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the ip instance and enables the ARP, UPD, ICMP.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT ip0_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT status         = NX_SUCCESS;
    ULONG current_state = RESET_VALUE;

    /* Create the IP instance. */
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
        PRINT_ERR_STR("nx_ip_create API failed.\n");
        return status;
    }

    /* Enable the ARP */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[RESET_VALUE], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable API failed.\n");
        return status;
    }

    /* Enable the TCP */
    status = nx_tcp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_tcp_enable API failed.");
        return status;
    }

    /* Enable the UDP */
    status = nx_udp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable API failed.");
        return status;
    }

    /* Enable the ping using Internet Control Message Protocol */
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable API failed.");
        return status;
    }
    PRINT_INFO_STR("Checking for Ethernet link...");

    /* Check the connection status. */
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_status_check API failed.");
    }
    return status;
    PRINT_INFO_STR("Ethernet Link is Up!");
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the dhcpv4 client by creating the dhcp client instance
 *            and allocates required the packet pool for it. DHCP server assigns the client ip
 *            address after server starts running successfully.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT dhcpv4_client_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT  status                         = NX_SUCCESS;
    ULONG requested_status               = RESET_VALUE;
    UCHAR dns_buffer[4*MAX_DNS_SERVERS]  = {NULL_CHAR};
    UINT dns_buffer_size                 = IP_V4_SIZE;
    ULONG *dns_server_ptr                = NULL;

    /* Create the DHCP instance. */
    status = nx_dhcp_create (&g_dhcp_client0, &g_ip0, "g_dhcp_client0");
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_create API failed.");
        return status;
    }

    /* Set the DHCP Packet Pool */
    status = nx_dhcp_packet_pool_set (&g_dhcp_client0, &g_packet_pool0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_packet_pool_set API failed.");
        return status;
    }

    /* Start DHCP service. */
    status = nx_dhcp_start (&g_dhcp_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_start API failed.");
        return status;
    }

    /* Wait until an IP address is acquired via DHCP. */
    status = nx_ip_status_check (&g_ip0, NX_IP_ADDRESS_RESOLVED, &requested_status, LINK_ENABLE_WAIT_TIME);
    if ((NX_SUCCESS != status) || (NX_IP_ADDRESS_RESOLVED != requested_status))
    {
        PRINT_ERR_STR("nx_ip_status_check API failed.");
        return status;
    }
    PRINT_INFO_STR("Acquired the IP address via DHCP successfully!");

    /* Query the DHCP Client for the DNS Server address. */
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DNS_SVR, dns_buffer, &dns_buffer_size);
    while(status == NX_DHCP_DEST_TO_SMALL)
    {
        if(dns_buffer_size < (MAX_DNS_SERVERS*IP_V4_SIZE))
        {
            dns_buffer_size = dns_buffer_size+IP_V4_SIZE;
            status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DNS_SVR, dns_buffer, &dns_buffer_size);
        }
        else
        {
            break;
        }
    }

    if(NX_SUCCESS == status)
    {
        dns_server_ptr = (ULONG *)(dns_buffer);
        for(uint8_t cnt = RESET_VALUE; cnt < (dns_buffer_size/IP_V4_SIZE); cnt++, dns_server_ptr++)
        {
            /* Print DNS server address.*/
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DNS_SERVER_IP, sizeof(ULONG *), dns_server_ptr);
            /* Retrieve the DNS Server Address. */
            dns_server_addr = *dns_server_ptr;
        }
    }
    return status;
}

    /*******************************************************************************************************************//**
     * @brief     This function initializes the dns client by creating instance and
     *            allocate the required packet pool. It process the dns server ip address.
     * @param[IN] None
     * @retval    None
     **********************************************************************************************************************/
    static UINT dns_client_init(void)
    {
        /* To capture the status(Success/Failure) of each Function/API. */
        UINT status = NX_SUCCESS;

        /* Create the DNS instance. */
        status = nx_dns_create(&g_dns0, &g_ip0, (UCHAR *)"g_dns0");
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dns_create API failed.");
            return status;
        }

        /* Create the DNS packet pool. */
        status = nx_dns_packet_pool_set(&g_dns0, &g_packet_pool0);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dns_packet_pool_set API failed.");
            return status;
        }

        /* Add an IPv4 server address to the Client list. */
        status = nx_dns_server_add(&g_dns0, dns_server_addr);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dns_server_add API failed.");
            return status;
        }

        PRINT_INFO_STR("Getting IP from Adafruit Server...");

        /* Get the IP address from Adafruit.io server. */
        status = nx_dns_host_by_name_get(&g_dns0, (UCHAR *)ADAFRUIT_IO_SERVER,
                                         &server_addr,NETWORK_TIMEOUT);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_dns_host_by_name_get API failed.");
        }
        else
        {
            /* Print the success message and IP acquired. */
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_IP, sizeof(ULONG *), &server_addr);
        }
        return status;
    }

    /*******************************************************************************************************************//**
     * @brief      Process User Input request and send or receive the temperature data to the
     *             Adafruit Server.
     *
     * @param[in]  None
     * @retval     NX_SUCCESS                 Upon successful processing of user input(s).
     * @retval     Any other Error Code       Upon unsuccessful processing of user input(s).
     **********************************************************************************************************************/
   static UINT Process_InputRequest(uint8_t input_data)
    {
        UINT status         = NX_SUCCESS;
        float mcu_die_temp  = RESET_VALUE;

        /* Read the temperature value from the ADC. */
        mcu_die_temp = adc_data_read();
        /* Process requests as per user input. */
        switch(input_data)
        {
            case GET_REQUEST:
            {
                status = processGetRequest();
            }
            break;

            case PUT_REQUEST:
            {
                status = processPutRequest(mcu_die_temp);
            }
            break;

            case POST_REQUEST:
            {
                status = processPostRequest(mcu_die_temp);
            }
            break;

            default:
            {
                PRINT_INFO_STR("Invalid Menu Option Selected");
                /* Set the invalid input flag.*/
                b_invalid_input = true;
            }
            break;
        }
        return status;
    }

    /*******************************************************************************************************************//**
     * @} (end addtogroup NetX_https_client_ep)
     **********************************************************************************************************************/
