/***********************************************************************************************************************
 * File Name    : freertos_network_example_app.c
 * Description  : Example application layer for the Getting started with FreeRTOS Network EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - net_thread_entry.c calls a single function: freertos_network_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "freertos_network_http_ep.h"
#include "freertos_network_example_app.h"
#include "console/uart_console.h"

/* Private variable S*/
static uint32_t s_usr_print_ability = RESET_VALUE;

/* Private functions */
static void print_ipconfig(void);
static void process_user_input(void);
static void ping_operation(char * destination);
static status_code_t tcp_client_operation(char * destination);
static status_code_t tcp_data_transfer(void);
static status_code_t udp_client_operation(char * destination);
static status_code_t udp_data_transfer(struct freertos_sockaddr * p_remote_address);
static void handle_error(status_code_t err, const char * err_str);
static void dns_query(char * domain);
static void http_server_operation(void);
static status_code_t update_network_info(void);
static void get_user_input(char * buf, int len);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Getting started with FreeRTOS Network example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void freertos_network_example_app_run(void)
{
    BaseType_t status = pdFALSE;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status_cons = uart_console_init();
    if (FSP_SUCCESS != status_cons)
    {
        return;     /* UART failed to open - cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Prints the Ethernet configuration prior to the IP initialization */
    printf(ETH_PREINIT);
    print_ipconfig();

    /* FreeRTOS IP initialization */
    status = ethernet_init();
    if (pdFALSE == status)
    {
        printf("ethernet_init failed\r\nReturned Error Code: 0x%x\r\n", (unsigned int)status);
        uart_console_flush();
        __BKPT(0);
    }

    printf(ETH_POSTINIT);

    while (true)
    {
        /* Check if both the Ethernet Link and IP link are UP */
        if ((IP_LINK_UP | ETHERNET_LINK_UP) ==
            (is_network_up() & (IP_LINK_UP | ETHERNET_LINK_UP)))
        {
            /* usr_print_ability is added to avoid multiple UP messages or DOWN messages repeating */
            if (!(PRINT_UP_MSG_DISABLE & s_usr_print_ability))
            {
                printf("\r\nNetwork is Up");
                s_usr_print_ability |= PRINT_UP_MSG_DISABLE;
            }

            if (!(PRINT_NWK_USR_MSG_DISABLE & s_usr_print_ability))
            {
#if (ipconfigUSE_DHCP != 0)
                /* Display the new IP credentials obtained from the DHCP server */
                update_dhcp_info();
#endif /* ipconfigUSE_DHCP != 0 */
                /* Update IP credentials on to the console */
                print_ipconfig();
                s_usr_print_ability |= PRINT_NWK_USR_MSG_DISABLE;
            }

            /* Handler for user input */
            process_user_input();
        }
        else
        {
            if (!(PRINT_DOWN_MSG_DISABLE & s_usr_print_ability))
            {
                printf("\r\nNetwork is Down");
                s_usr_print_ability |= PRINT_DOWN_MSG_DISABLE;
            }
            else
            {
                printf(".");
            }
        }
        vTaskDelay(100);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function creates and prints the the IP configuration.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void print_ipconfig(void)
{
#if (ipconfigUSE_DHCP != 0)
    if (g_dhcp_in_use)
    {
        g_netmask[3] = (uint8_t)((g_xnd.ulNetMask & 0xFF000000) >> 24);
        g_netmask[2] = (uint8_t)((g_xnd.ulNetMask & 0x00FF0000) >> 16);
        g_netmask[1] = (uint8_t)((g_xnd.ulNetMask & 0x0000FF00) >> 8);
        g_netmask[0] = (uint8_t)(g_xnd.ulNetMask & 0x000000FF);

        g_gateway_address[3] = (uint8_t)((g_xnd.ulGatewayAddress & 0xFF000000) >> 24);
        g_gateway_address[2] = (uint8_t)((g_xnd.ulGatewayAddress & 0x00FF0000) >> 16);
        g_gateway_address[1] = (uint8_t)((g_xnd.ulGatewayAddress & 0x0000FF00) >> 8);
        g_gateway_address[0] = (uint8_t)(g_xnd.ulGatewayAddress & 0x000000FF);

        g_dns_server_address[3] = (uint8_t)((g_xnd.ulDNSServerAddresses[0] & 0xFF000000) >> 24);
        g_dns_server_address[2] = (uint8_t)((g_xnd.ulDNSServerAddresses[0] & 0x00FF0000) >> 16);
        g_dns_server_address[1] = (uint8_t)((g_xnd.ulDNSServerAddresses[0] & 0x0000FF00) >> 8);
        g_dns_server_address[0] = (uint8_t)(g_xnd.ulDNSServerAddresses[0] & 0x000000FF);

        g_ip_address[3] = (uint8_t)((g_xnd.ulIPAddress & 0xFF000000) >> 24);
        g_ip_address[2] = (uint8_t)((g_xnd.ulIPAddress & 0x00FF0000) >> 16);
        g_ip_address[1] = (uint8_t)((g_xnd.ulIPAddress & 0x0000FF00) >> 8);
        g_ip_address[0] = (uint8_t)(g_xnd.ulIPAddress & 0x000000FF);
    }
#endif /*ipconfigUSE_DHCP != 0 */
    printf("\r\nEthernet adapter for Renesas "KIT_NAME":\r\n");
    printf("\tDescription . . . . . . . . . . . : Renesas "KIT_NAME" Ethernet\r\n");
    printf("\tPhysical Address. . . . . . . . . : %02x-%02x-%02x-%02x-%02x-%02x\r\n", g_mac_address[0],
            g_mac_address[1], g_mac_address[2], g_mac_address[3], g_mac_address[4], g_mac_address[5]);
    printf("\tDHCP Enabled. . . . . . . . . . . : %s\r\n", g_dhcp_in_use ? "Yes" : "No");
    printf("\tIPv4 Address. . . . . . . . . . . : %d.%d.%d.%d\r\n", g_ip_address[0], g_ip_address[1],
              g_ip_address[2], g_ip_address[3]);
    printf("\tSubnet Mask . . . . . . . . . . . : %d.%d.%d.%d\r\n", g_netmask[0], g_netmask[1],
              g_netmask[2], g_netmask[3]);
    printf("\tDefault Gateway . . . . . . . . . : %d.%d.%d.%d\r\n", g_gateway_address[0], g_gateway_address[1],
              g_gateway_address[2], g_gateway_address[3]);
    printf("\tDNS Servers . . . . . . . . . . . : %d.%d.%d.%d\r\n", g_dns_server_address[0], g_dns_server_address[1],
              g_dns_server_address[2], g_dns_server_address[3]);
}

/*******************************************************************************************************************//**
 * @brief     This function processes the user input.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_user_input(void)
{
    status_code_t err = STATUS_SUCCESS;
    char key_index[KEY_INDEX_SIZE] = {RESET_VALUE};
    char remote_info[REMOTE_INFO_SIZE] = {RESET_VALUE};
    char user_input[USER_INPUT_SIZE] = {NULL_CHAR};

    /* Print the main menu */
    printf(MAIN_MENU);

    /* Wait for user input */
    while (!uart_console_has_char())
    {
        vTaskDelay(DELAY_10_TICKS);
    }

    /* Read user input. */
    get_user_input(user_input, (int)sizeof(user_input));

    /* Parse user input */
    sscanf((char *) &user_input, "%2s %64s", key_index, remote_info);

    /* Process the user input */
    switch (key_index[0])
    {
        case PING_OPERATION:
        {
            ping_operation((char *)remote_info);
            break;
        }

        case TCP_CLIENT_OPERATION:
        {
            err = tcp_client_operation((char *)remote_info);
            if (STATUS_SUCCESS != err)
            {
                printf("\r\nTCP client handle operation failed. Please try again.\r\n");
            }
            break;
        }

        case UDP_CLIENT_OPERATION:
        {
            err = udp_client_operation((char *)remote_info);
            if (STATUS_SUCCESS != err)
            {
                printf("\r\nUDP client handle operation failed. Please try again.\r\n");
            }
            break;
        }

        case DNS_OPERATION:
        {
            dns_query((char *)&remote_info);
            break;
        }

        case HTTP_SERVER_OPERATION:
        {
            http_server_operation();
            break;
        }

        default:
        {
            printf("\r\nInvalid input !\r\n");
            break;
        }
    }

    /* Reset the key index after processing */
    key_index[0] = NULL_CHAR;
}

/*******************************************************************************************************************//**
 * @brief     This function send a ping request to the specified IP address.
 * @param[in] destination               Specify the IP address to ping.
 * @retval    None.
 **********************************************************************************************************************/
static void ping_operation(char * destination)
{
    uint32_t pingcount = RESET_VALUE;
    ping_status_t ping_status = PING_ERROR;
    g_ping_data.sent = RESET_VALUE;
    g_ping_data.received = RESET_VALUE;
    g_ping_data.lost = RESET_VALUE;

    printf("\r\n\r\nPinging to %s:\r\n", destination);

    while (USR_PING_COUNT > pingcount)
    {
        /* Send a ICMP Ping request to the requested IP address
         * USR_PING_COUNT (100) is used in this example project
         * For Continuous testing the count can be increased to bigger number */
        ping_status = send_ping_request((char*)destination);
        if (PING_ERROR != ping_status)
        {
            printf("!");
            g_ping_data.sent++;
            if (PING_SUCCESS == ping_status)
            {
                g_ping_data.received++;
            }
            else if (PING_TIMEOUT == ping_status)
            {
                g_ping_data.lost++;
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            g_ping_data.lost++;
            printf(".");
        }
        pingcount++;
        vTaskDelay(100);
    }

    /* Print the ping response on to the console */
    printf("\r\n\r\nPing statistics for %s:", (char *)destination);
    printf("\r\nPackets: Sent  = %d, Received = %d, Lost = %d \r\n", g_ping_data.sent,
            g_ping_data.received, g_ping_data.lost);
}

/***********************************************************************************************************************
 * @brief     This function performs TCP client operation.
 * @param[in] destination               Destination IP address.
 * @retval    STATUS_SUCCESS            Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static status_code_t tcp_client_operation(char * destination)
{
    status_code_t err = STATUS_SUCCESS;
    struct freertos_sockaddr remote_address;

    int ipv4_address[4] = {RESET_VALUE};
    int port = RESET_VALUE;

    char * remote_ip_address = NULL;

    /* Parse the destination IP address and port. */
    sscanf(destination, "%d.%d.%d.%d:%d", &ipv4_address[0], &ipv4_address[1], &ipv4_address[2],
           &ipv4_address[3], &port);

    /* Extract the IP address part. */
    remote_ip_address = strtok(destination, ":");

    /* Configure the remote socket address. */
    remote_address_configure(ipv4_address, port, &remote_address);

    /* Create and configure the TCP socket. */
    if (STATUS_SUCCESS != socket_create(FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP))
    {
        return STATUS_ERROR;
    }
    printf("\r\n\r\nCreated new TCP socket");

    /* Check connectivity with the server. */
    if (PING_SUCCESS == ping_server(remote_ip_address))
    {
        printf("\r\nConnecting to server...");

        if (STATUS_SUCCESS == tcp_server_connect(&remote_address))
        {
            printf("\r\nConnected to server at %d.%d.%d.%d:%d", ipv4_address[0], ipv4_address[1], ipv4_address[2],
                                                                               ipv4_address[3], port);

            /* Send data to the TCP server and receives the server response */
            if (STATUS_SUCCESS != tcp_data_transfer())
            {
                err = STATUS_ERROR;
            }
        }
        else
        {
            err = STATUS_ERROR;
        }
    }
    else
    {
        err = STATUS_ERROR;
    }
    tcp_socket_shutdown();
    socket_close();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function transfers data in TCP client operation.
 * @param[in] None.
 * @retval    STATUS_SUCCESS            Transfered successfully.
 * @retval    Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static status_code_t tcp_data_transfer(void)
{
    BaseType_t send_result = RESET_VALUE;
    BaseType_t receive_result = RESET_VALUE;

    char recv_buffer[RECV_BUFFER_SIZE] = {RESET_VALUE};

    /* Send a message to the server. */
    send_result = tcp_send();
    if (send_result > RESET_VALUE)
    {
        printf("\r\nSent %ld bytes message to TCP server", send_result);
    }
    else
    {
        printf("\r\nNo data sent or error occurred\r\n");
        return STATUS_ERROR;
    }

    /* Receive a response from the server. */
    memset(recv_buffer, NULL_CHAR, sizeof(recv_buffer));
    printf("\r\nWaiting for server response...");

    receive_result = tcp_receive(recv_buffer, (sizeof(recv_buffer) - 1));

    if (receive_result > RESET_VALUE)
    {
        printf("\r\nReceived response: %s\r\n", recv_buffer);
    }
    else
    {
        printf("\r\nNo response received or error occurred.\r\n");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function performs UDP client operation.
 * @param[in] destination               Destination IP address.
 * @retval    STATUS_SUCCESS            Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static status_code_t udp_client_operation(char * destination)
{
    status_code_t err = STATUS_SUCCESS;
    struct freertos_sockaddr remote_address;

    int ipv4_address[4] = {RESET_VALUE};
    int port = RESET_VALUE;

    char * remote_ip_address = NULL;

    /* Parse the destination IP address and port. */
    sscanf(destination, "%d.%d.%d.%d:%d", &ipv4_address[0], &ipv4_address[1], &ipv4_address[2],
           &ipv4_address[3], &port);

    /* Extract the IP address part. */
    remote_ip_address = strtok(destination, ":");

    /* Configure the remote socket address. */
    remote_address_configure(ipv4_address, port, &remote_address);

    /* Create and configure the UDP socket. */
    if (STATUS_SUCCESS != socket_create(FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP))
    {
        return STATUS_ERROR;
    }
    printf("\r\n\r\nCreated new UDP socket");

    if (PING_SUCCESS == ping_server(remote_ip_address))
    {
        /* Send data to and receive data from the UDP server. */
        if (STATUS_SUCCESS != udp_data_transfer(&remote_address))
        {
            err = STATUS_ERROR;
        }
    }
    else
    {
        err = STATUS_ERROR;
    }

    /* Close the UDP socket. */
    socket_close();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function transfers data in UDP client operation.
 * @param[in] p_remote_address          Pointer to the remote UDP server address.
 * @retval    STATUS_SUCCESS            Transfered successfully.
 * @retval    Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static status_code_t udp_data_transfer(struct freertos_sockaddr * p_remote_address)
{
    int32_t bytes_sent = RESET_VALUE;
    int32_t bytes_received = RESET_VALUE;

    char recv_buffer[RECV_BUFFER_SIZE] = {RESET_VALUE};

    /* Send a message to the UDP server. */
    bytes_sent = udp_send(p_remote_address);
    if (bytes_sent > RESET_VALUE)
    {
        printf("\r\nSent %d bytes message to UDP server", bytes_sent);
    }
    else
    {
        printf("\r\nNo data sent or error occurred\r\n");
        return STATUS_ERROR;
    }

    /* Receive a response from the UDP server. */
    memset(recv_buffer, NULL_CHAR, sizeof(recv_buffer));
    printf("\r\nWaiting for server response...");

    bytes_received = udp_receive(p_remote_address, recv_buffer, sizeof(recv_buffer));
    if (bytes_received > RESET_VALUE)
    {
        printf("\r\nReceived UDP message from server: %s\r\n", recv_buffer);
    }
    else
    {
        printf("\r\nNo response received or error occurred.\r\n");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function resolves the specified domain name and displays the corresponding IP address.
 * @param[in] domain                    Domain name to resolve.
 * @retval    None.
 **********************************************************************************************************************/
static void dns_query(char * domain)
{
    int8_t buffer[16] = {RESET_VALUE};
    uint32_t ulIPAddress = RESET_VALUE;

    printf("\r\n\r\nLooking up........");

    /* Lookup the IP address of the FreeRTOS.org website */
    ulIPAddress = get_host_by_name((char *)domain);

    if (RESET_VALUE != ulIPAddress)
    {
        /* Convert the IP address to a string */
        inet_ntoa(ulIPAddress, (char *)buffer);

        /* Print out the IP address obtained from the DNS lookup */
        printf("\r\nDNS Lookup for \"%s\" is: %s\r\n", domain, buffer);
    }
    else
    {
        printf("\r\nDNS Lookup failed for \"%s\"\r\n", domain);
    }
}

/***********************************************************************************************************************
 * @brief     This function initializes and runs the HTTP server until a key is pressed.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void http_server_operation(void)
{
    status_code_t status = STATUS_SUCCESS;
    TCPServer_t * p_tcp_server = NULL;

    /* Update IP configuration */
    status = update_network_info();
    handle_error(status, "update_network_info failed\r\n");

    /* Create the HTTP server. */
    status = http_server_create(&p_tcp_server);
    handle_error(status, "http_server_create failed\r\n");
    printf("\r\n\r\nTCP server created successfully");
    printf("\r\nPlease open a browser and enter \"%d.%d.%d.%d/index.html\" to go to the web page.",
              g_ip_address[0], g_ip_address[1], g_ip_address[2], g_ip_address[3]);
    printf("\r\nPress any key (excluding Enter) to exit the HTTP server...");

    while (true)
    {
        /* Process the HTTP server. */
        http_server_process(p_tcp_server);

        /* Check if the received TCP command is requesting the LED status update */
        if (RESET_VALUE == strncmp((char*)p_tcp_server->pcCommandBuffer, GET_LED_COMMAND, GET_LED_COMMAND_LENGTH))
        {
            /* Get the status of the LEDs */
            status = update_status_leds();
            handle_error(status, "update_status_leds failed\r\n");
        }

        /* Check if the received TCP command is for toggling Switch 1 (GET/switch1) */
        if (RESET_VALUE == strncmp((char*)p_tcp_server->pcCommandBuffer,
                                   GET_SWITCH1_COMMAND, GET_SWITCH1_COMMAND_LENGTH))
        {
            /* Toggle the state of LED1 */
            toggleLED((bsp_io_port_pin_t)g_bsp_leds.p_leds[0U]);
            printf("\r\nLED1 state has been toggled.\r\n");
        }

        /* Check if the received TCP command is for toggling Switch 2 (GET/switch2) */
        if (RESET_VALUE == strncmp((char*)p_tcp_server->pcCommandBuffer,
                                   GET_SWITCH2_COMMAND, GET_SWITCH2_COMMAND_LENGTH))
        {
            /* Toggle the state of LED2 */
            toggleLED((bsp_io_port_pin_t)g_bsp_leds.p_leds[1U]);
            printf("\r\nLED2 state has been toggled.\r\n");
        }

        memset((char*)p_tcp_server->pcCommandBuffer, RESET_VALUE, ipconfigTCP_COMMAND_BUFFER_SIZE);

        /* Check if a key has been pressed */
        if (uart_console_has_char())
        {
            http_server_close(p_tcp_server);
            break;
        }
        vTaskDelay(1);
    }
}

/***********************************************************************************************************************
 * @brief     This function initializes FreeRTOS+FAT and updates the network information file.
 * @param[in] None.
 * @retval    STATUS_SUCCESS            Initialized and updated successfully.
 * @retval    Any other error codes     Failed to initialize or update.
 **********************************************************************************************************************/
static status_code_t update_network_info(void)
{
    char ip_content_body[NETWORK_BUFFER_SIZE];
    status_code_t write_status = STATUS_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the FreeRTOS+FAT */
    err = freertos_fat_init();
    if (FSP_SUCCESS != err)
    {
        return STATUS_ERROR;
    }

    write_status = file_system_mount();
    if (STATUS_SUCCESS != write_status)
    {
        return write_status;
    }

    snprintf(ip_content_body, sizeof(ip_content_body), INNER_HTML_NET_ID, g_mac_address[0],
             g_mac_address[1], g_mac_address[2], g_mac_address[3], g_mac_address[4], g_mac_address[5], g_ip_address[0],
             g_ip_address[1], g_ip_address[2], g_ip_address[3], g_netmask[0], g_netmask[1], g_netmask[2],
             g_netmask[3], g_gateway_address[0], g_gateway_address[1], g_gateway_address[2], g_gateway_address[3]);

    write_status = write_file(NETWORK_INFO_FILE_NAME, ip_content_body);
    if (STATUS_SUCCESS != write_status)
    {
        printf(" %d\r\n", stdioGET_ERRNO());
        printf("Failed to write all data to %s\n", NETWORK_INFO_FILE_NAME);
    }
    return write_status;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and remove trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void get_user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing the error message and code, de-initializing
 *            FreeRTOS+FAT, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(status_code_t err, const char * err_str)
{
    if (STATUS_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        freertos_fat_deinit();
        uart_console_flush();
        __BKPT(0);
    }
}
