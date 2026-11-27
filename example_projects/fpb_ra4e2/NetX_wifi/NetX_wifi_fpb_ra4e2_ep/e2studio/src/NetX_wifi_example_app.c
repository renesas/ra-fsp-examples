/***********************************************************************************************************************
 * File Name    : NetX_wifi_example_app.c
 * Description  : Example application layer for the NetX Wi-Fi EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - azure_wifi_thread_entry calls a single function: azure_wifi_thread_entry_run().
 *                - terminal_thread_entry calls a single function: terminal_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdlib.h>
#include "NetX_wifi_example_app.h"
#include "terminal.h"

/* Private functions */
static void wifi_setup(void);
static void wifi_scan_ap(void);
static void wifi_manual_ap(void);
static fsp_err_t ping_target(void);
static fsp_err_t connect_tcp_server(void);
static void process_terminal_op_msg(VOID);
static fsp_err_t get_user_input(char * user_buff);
static void terminal_input_read(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);
static void nx_handle_error(UINT err, const char * err_str);
static void tx_handle_error(UINT err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Azure Wi-Fi Thread entry-point for the NetX Wi-Fi example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void azure_wifi_thread_entry_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    UINT nx_err = NX_SUCCESS;

    /* Initialize the terminal Thread */
    terminal_thread_init_check();

    /* Print the banner and EP info */
    app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Setup Access Point connection parameters */
    wifi_setup();
    PRINT_STR("Wi-Fi connected successfully !!!\r\n");

    /* Turn OFF the on-board LED */
    set_led(LED_OFF);

    /* Ping demonstration */
    err = ping_target();
    handle_error(err, "ping_target failed\r\n");
    PRINT_STR("Ping successfully !!!\r\n");

    /* IP layer initialization */
    nx_err = network_init();
    nx_handle_error(nx_err, "network_init failed\r\n");

    /* Start TCP client-server communication */
    PRINT_STR("Please start TCP server in PC and start listening.");

    /* Create the socket */
    nx_err = nx_tcp_create("TCP Client Socket");
    nx_handle_error(nx_err, "nx_tcp_create failed\r\n");
    PRINT_INFO_STR("TCP Socket created in client mode on MCU.");

    /* Enter IP address and port to connect to TCP socket */
    err = connect_tcp_server();
    handle_error(err, "connect_tcp_server failed\r\n");

    nx_err = nx_tcp_bind();
    nx_handle_error(nx_err, "nx_tcp_bind failed\r\n");
    PRINT_INFO_STR("TCP socket bind successful !!!");

    /* Connect to the TCP server */
    nx_err = nx_tcp_connect();
    nx_handle_error(nx_err, "nx_tcp_connect failed\r\n");
    PRINT_INFO_STR("TCP socket connected successfully !!!");

    /* Send message to server */
    nx_err = nx_tcp_send("Hello");
    nx_handle_error(nx_err, "nx_tcp_send failed\r\n");
    PRINT_INFO_STR("Sent Hello to the TCP server");

    while (true)
    {
        /* Wait for a message from the server */
        nx_err = nx_tcp_recv();
        if (NX_NO_PACKET == nx_err)
        {
            PRINT_STR("Waiting for message from server...");
            continue;
        }
        nx_handle_error(nx_err, "nx_tcp_recv failed\r\n");

        nx_err = nx_tcp_retrieve();
        nx_handle_error(nx_err, "nx_tcp_retrieve failed\r\n");

        convert_to_lowercase(g_socket.recv_buff);

        if (RESET_VALUE == strcmp((char *)g_socket.recv_buff, "on"))
        {
            set_led(LED_ON);
            app_terminal_print_data(TERMINAL_OUTPUT_TCP_RECV_LED_STATUS, BUFF_LEN, g_socket.recv_buff);
            nx_err = nx_tcp_send("Success");
            nx_handle_error(nx_err, "nx_tcp_send failed\r\n");
        }
        else if (RESET_VALUE == strcmp((char *)g_socket.recv_buff, "off"))
        {
            set_led(LED_OFF);
            app_terminal_print_data(TERMINAL_OUTPUT_TCP_RECV_LED_STATUS, BUFF_LEN, g_socket.recv_buff);
            nx_err = nx_tcp_send("Success");
            nx_handle_error(nx_err, "nx_tcp_send failed\r\n");
        }
        else
        {
            app_terminal_print_data(TERMINAL_OUTPUT_TCP_RECV_INVALID, BUFF_LEN, g_socket.recv_buff);
        }

        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function configures the Wi-Fi Access Point connection parameters.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_setup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    char msg_buff[BUFF_LEN + 32U] = {RESET_VALUE};
    uint8_t selection = RESET_VALUE;

    /* Initialize Wi-Fi Onchip Silex module */
    err = wifi_open();
    handle_error(err, "wifi_open failed\r\n");

    do
    {
        app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
        get_user_input(input_buff);
        selection = (uint8_t)atoi(input_buff);

        switch (selection)
        {
            case SCAN_AP_SELECTION:
            {
                wifi_scan_ap();
                break;
            }

            case ENTER_SSID_SELECTION:
            {
                wifi_manual_ap();
                break;
            }

            default:
            {
                PRINT_INFO_STR("Invalid input.\r\n");
                break;
            }
        }
    } while ((SCAN_AP_SELECTION != selection) && (ENTER_SSID_SELECTION != selection));

    /* Password for non-open security */
    if (eWiFiSecurityOpen != g_wifi.security)
    {
        snprintf(msg_buff, sizeof(msg_buff), "\r\n\r\nEnter password for %s:", g_wifi.ssid);
        PRINT_STR(msg_buff);
        get_user_input(input_buff);
        memcpy(g_wifi.pwd, input_buff, sizeof(g_wifi.pwd));
    }

    snprintf(msg_buff, sizeof(msg_buff), "\r\nConnecting to %s", g_wifi.ssid);
    PRINT_STR(msg_buff);

    err = wifi_config();
    handle_error(err, "wifi_config failed\r\n");

    app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_MCU_IP_ADDRESS, BUFF_LEN, g_wifi.ip_addr_device);
}

/*******************************************************************************************************************//**
 * @brief     This function scans for available Wi-Fi Access Points and stores the selected network information.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_scan_ap(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint8_t index_ap = RESET_VALUE;
    uint8_t index = RESET_VALUE;

    do
    {
        /* Perform a Wi-Fi network scan */
        PRINT_INFO_STR("Scanning Wi-Fi Access Points...");
        err = wifi_scan();
        handle_error(err, "wifi_scan failed\r\n");

        /* Print result Wi-Fi Access Points scan */
        PRINT_STR("------------List of Wi-Fi Access Points-----------");

        for (index = RESET_VALUE; index < MAX_WIFI_SCAN_RESULTS; index++)
        {
            if (NULL_CHAR == g_scan_data[index].ucSSID[0] || '\t' == g_scan_data[index].ucSSID[0])
            {
                continue;
            }

            app_terminal_print_data(TERMINAL_OUTPUT_PRINT_INDEX, sizeof(index), &index);
            app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_SECURITY, sizeof(g_scan_data[index]), &g_scan_data[index]);

            /* Print Wi-Fi security type */
            switch (g_scan_data[index].xSecurity)
            {
                case eWiFiSecurityOpen:
                {
                    PRINT_STR("Security: Open");
                    break;
                }

                case eWiFiSecurityWPA:
                {
                    PRINT_STR("Security: WPA");
                    break;
                }

                case eWiFiSecurityWPA2:
                {
                    PRINT_STR("Security: WPA2");
                    break;
                }

                default:
                {
                    PRINT_STR("Security: Unknown");
                    break;
                }
            }
        }

        /* Perform the user enter Access Point index */
        PRINT_STR("--------------------------------------------------");
        PRINT_STR("Enter Access Point index to connect, or non-digit to re-scan:");
        memset(input_buff, RESET_VALUE, sizeof(input_buff));
        get_user_input(input_buff);
        index_ap = (uint8_t)atoi(input_buff);

    } while (('0' > input_buff[0]) || ('9' < input_buff[0]) || (MAX_WIFI_SCAN_RESULTS <= index_ap));

    wifi_set_ap_ssid(index_ap);
}

/*******************************************************************************************************************//**
 * @brief     This function allows the user to manually enter the SSID and security type.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_manual_ap(void)
{
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint32_t sec_val = RESET_VALUE;

    /* The user inputs the SSID of the Wi-Fi AP */
    PRINT_STR("\r\n\r\nEnter SSID of Wi-Fi Access Point:");

    get_user_input(input_buff);

    /* Copy user input to Wi-Fi structures member */
    memcpy(g_wifi.ssid, input_buff, sizeof(g_wifi.ssid));

    /* The user selecting security type of Wi-Fi AP */
    do
    {
        app_terminal_print_data(TERMINAL_OUTPUT_PRINT_SECURITY_MENU, RESET_VALUE, NULL);
        get_user_input(input_buff);
        sec_val = (uint32_t)atoi(input_buff);

    } while (('0' > input_buff[0]) || ('2' < input_buff[0]) || (NULL_CHAR != input_buff[1]));

    if (0 == sec_val)
    {
        g_wifi.security = eWiFiSecurityOpen;
    }
    else if (1 == sec_val)
    {
        g_wifi.security = eWiFiSecurityWPA;
    }
    else
    {
        g_wifi.security = eWiFiSecurityWPA2;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function validates the target URL or IP address.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Validated successfully.
 * @retval    Any other error codes     Failed to validate.
 **********************************************************************************************************************/
static fsp_err_t ping_target(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t ip_addr[IP_BUFF_LEN] = {RESET_VALUE};
    char input_buff[BUFF_LEN] = {NULL_CHAR};
    uint32_t dns_retry_count = WIFI_MAX_TRY;
    uint32_t ping_retry_count = WIFI_MAX_TRY;

    do
    {
        /* User input for IP address or URL to ping */
        PRINT_STR("Enter the IP or URL to ping: ");
        get_user_input(input_buff);

        PRINT_STR("\r\nPinging. Please wait...");

        /* DNS lookup for URL or IP */
        err = dns_query(input_buff, ip_addr);

        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("DNS lookup failed. Check IP or URL entered.\r\n");
            if (RESET_VALUE > (dns_retry_count--))
            {
                PRINT_ERR_STR("Failed maximum retry count for DNS lookup. Reset the MCU.\r\n");
                return err;
            }
        }
        
        /* Only if DNS lookup is successful, ping would be performed */
        else
        {
            /* Ping URL or IP address */
            err = ping(ip_addr);
            if (FSP_SUCCESS != err)
            {
                PRINT_ERR_STR("Ping failed. Check IP or URL entered.\r\n");

                /* If ping fails maximum times, then EP fails */
                if (RESET_VALUE > (ping_retry_count--))
                {
                    PRINT_ERR_STR("Failed maximum retry count for ping operation. Reset the MCU.\r\n");
                    return err;
                }

                /* Reset DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    } while (FSP_SUCCESS != err);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function connects to the TCP server.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
static fsp_err_t connect_tcp_server(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {NULL_CHAR};
    uint32_t dns_retry_count  = WIFI_MAX_TRY;
    uint32_t ping_retry_count = WIFI_MAX_TRY;

    do
    {
        /* User input for TCP server IP address */
        PRINT_STR("Enter TCP server IP address to connect:");
        get_user_input(input_buff);

        PRINT_STR("\r\nValidating TCP server IP address. Please wait...");

        /* DNS lookup for TCP server IP address */
        err = dns_query(input_buff, (uint8_t *)&g_socket.ip_addr_server);

        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("DNS lookup failed. Check TCP server IP address entered.\r\n");

            if (RESET_VALUE > (dns_retry_count--))
            {
                PRINT_ERR_STR("Failed maximum retry count for DNS lookup. Reset the MCU.\r\n");
                return err;
            }
        }
        
        /* Only if DNS lookup is successful, ping would be performed */
        else
        {
            /* Ping IP address to validate */
            err = ping((uint8_t *)&g_socket.ip_addr_server);
            if (FSP_SUCCESS != err)
            {
                PRINT_ERR_STR("Ping failed. Check TCP server IP address.\r\n");

                /* If ping fails maximum times, then EP fails */
                if (RESET_VALUE > (ping_retry_count--))
                {
                    PRINT_ERR_STR("Failed maximum retry count for ping operation. Reset the MCU.\r\n");
                    return err;
                }

                /* Reset DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    } while (FSP_SUCCESS != err);

    /* User input for TCP server port number */
    PRINT_STR("\r\nEnter TCP server port number to connect (greater than 10,000): ");
    memset(input_buff, NULL_CHAR, sizeof(input_buff));
    get_user_input(input_buff);
    g_socket.port = (uint32_t)atoi(input_buff);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the terminal thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    UINT err = TX_SUCCESS;
    CHAR terminal_buffer[IP_MSG_MAX_SIZE] = {NULL_CHAR};
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {NULL_CHAR};

    terminal_msg_t * p_data = NULL;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Print the current time */
    snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n", tx_time_get());
    DBG_PRINT(timestamp_info);

    /* Initialize terminal framework */
    err = terminal_framework_init();
    tx_handle_error(err, "terminal_framework_init failed\r\n");

    while (true)
    {
        /* Check for user input */
        if (uart_console_has_char())
        {
            terminal_input_read(terminal_buffer, sizeof(terminal_buffer));
            UINT read_bytes = (UINT)strlen(terminal_buffer);
            err = memory_allocate_terminal(&g_byte_pool, &p_data, sizeof(terminal_msg_t) + read_bytes + 1U);
            if (TX_SUCCESS != err)
            {
                printf("Error in processing, please check again\r\n");
            }
            else
            {
                /* Send data to user thread */
                p_data->msg_id = TERMINAL_INPUT_MESSAGE_TYPE_STR_QUEUED;
                p_data->msg_data_size = strlen(terminal_buffer) + 1U;
                memcpy(p_data->p_msg, terminal_buffer, p_data->msg_data_size);
                tx_queue_send(&g_terminal_ip_data_queue, (terminal_msg_t *)&p_data, TX_WAIT_FOREVER);

                memset(terminal_buffer, NULL_CHAR, sizeof(terminal_buffer));
            }
        }

        process_terminal_op_msg();

        /* 1-tick sleep is needed to signal to higher priority threads */
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function receives messages from the application thread through tx_queue_receive and displays
 *            them on the terminal. Also releases the print message allocated block.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_terminal_op_msg(VOID)
{
    UINT err = TX_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    terminal_msg_t * terminal_op_data = NULL;
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {NULL_CHAR};

    do
    {
        err = tx_queue_receive(&g_terminal_op_data_queue, (VOID *)&terminal_op_data, 1);

        if (TX_SUCCESS == err)
        {
            /* Print the time this message was originally queued (only visible when LOG_LEVEL is raised
             * to LVL_DEBUG in terminal.h) */
            snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n",
                     (ULONG)terminal_op_data->msg_time);
            DBG_PRINT(timestamp_info);

            /* Message received. Take action and release buffer */
            switch (terminal_op_data->msg_id)
            {
                case TERMINAL_OUTPUT_MESSAGE_BANNER:
                {
                    /* Get FLEX version information */
                    R_FSP_VersionGet(&version);

                    /* Example project information printed on the terminal */
                    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor,
                           version.version_id_b.patch);
                    uart_console_flush();

                    printf(EP_INFO);
                    uart_console_flush();
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_INFO_STR:
                {
                    printf("\r\nINFO: %s\r\n", terminal_op_data->p_msg);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_GENERAL_STR:
                {
                    printf("%s\r\n", terminal_op_data->p_msg);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_ERR_STR:
                {
                    printf("\r\nERROR: %s\r\n", terminal_op_data->p_msg);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_ERR_TRAP:
                {
                    fsp_err_t trap_err = *(fsp_err_t *)terminal_op_data->p_msg;
                    printf("Returned Error Code: 0x%x\r\n", (unsigned int)trap_err);
                    uart_console_flush();
                    wifi_deinit();
                    socket_deinit();
                    __BKPT(0);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_NX_ERR_TRAP:
                {
                    UINT trap_err = *(UINT *)terminal_op_data->p_msg;
                    printf("Returned Error Code: 0x%x\r\n", (unsigned int)trap_err);
                    uart_console_flush();
                    wifi_deinit();
                    socket_deinit();
                    __BKPT(0);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_PRINT_MENU:
                {
                    /* Print NetX Wi-Fi menu options to connect */
                    printf(EP_MENU);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_MCU_IP_ADDRESS:
                {
                    uint32_t mcu_ip[IP_BUFF_LEN] = {RESET_VALUE};
                    memcpy(mcu_ip, (uint32_t *)terminal_op_data->p_msg, BUFF_LEN);
                    printf("MCU IP address: %d.%d.%d.%d\r\n", mcu_ip[0U], mcu_ip[1U], mcu_ip[2U], mcu_ip[3U]);
                    break;
                }

                case TERMINAL_OUTPUT_TCP_RECV_LED_STATUS:
                {
                    uint32_t serv_recv_msg[BUFF_LEN] = {RESET_VALUE};
                    memcpy(serv_recv_msg, (uint32_t *)terminal_op_data->p_msg, BUFF_LEN);
                    printf("Received: %s, User LED %s\r\n", (char *)serv_recv_msg, (char *)serv_recv_msg);
                    break;
                }

                case TERMINAL_OUTPUT_TCP_RECV_INVALID:
                {
                    uint32_t serv_recv_msg[BUFF_LEN] = {RESET_VALUE};
                    memcpy(serv_recv_msg, (uint32_t *)terminal_op_data->p_msg, BUFF_LEN);
                    printf("Received: %s, Invalid message. Send 'on' or 'off'\r\n", (char *)serv_recv_msg);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_SECURITY:
                {
                    WIFIScanResult_t scan_data =
                    {
                        .ucSSID       = {RESET_VALUE},
                        .ucSSIDLength = RESET_VALUE,
                        .ucBSSID      = {RESET_VALUE},
                        .xSecurity    = RESET_VALUE,
                        .cRSSI        = RESET_VALUE,
                        .ucChannel    = RESET_VALUE
                    };
                    scan_data = *(WIFIScanResult_t *)terminal_op_data->p_msg;
                    printf("%s: Signal strength: %d, Channel: %d", scan_data.ucSSID, scan_data.cRSSI,
                           scan_data.ucChannel);
                    break;
                }

                case TERMINAL_OUTPUT_PRINT_INDEX:
                {
                    printf("Index:(%d)", *(uint8_t *)terminal_op_data->p_msg);
                    break;
                }

                case TERMINAL_OUTPUT_PRINT_SECURITY_MENU:
                {
                    printf(SUB_MENU);
                    break;
                }

                default:
                {
                    printf("\r\nUnknown message ID");
                    break;
                }
            }

            /* Send a newline and return carriage requesting flushing the buffer */
            printf("\r\n");

            /* Release print message allocated block */
            err = tx_byte_release((void *)terminal_op_data);
        }
    } while (TX_QUEUE_EMPTY != err);
}

/*******************************************************************************************************************//**
 * @brief     This function takes input from the user.
 * @param[in] user_buff                 Pointer to user input buffer.
 * @retval    FSP_SUCCESS               Input taken successfully.
 **********************************************************************************************************************/
static fsp_err_t get_user_input(char * user_buff)
{
    UINT err = TX_SUCCESS;

    do
    {
        err = check_for_terminal_user_IP(user_buff);
    } while (TX_SUCCESS != err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void terminal_input_read(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = NULL_CHAR;
}

/*******************************************************************************************************************//**
 * @brief     This function handles an FSP error by printing error message and code, de-initializing the related
 *            modules, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR(err_str);
        ERROR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an ThreadX error by printing error message and code, de-initializing the related
 *            modules, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void nx_handle_error(UINT err, const char * err_str)
{
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR(err_str);
        NX_ERROR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles a ThreadX error by printing the error message and code, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void tx_handle_error(UINT err, const char * err_str)
{
    if (TX_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        __BKPT(0);
    }
}
