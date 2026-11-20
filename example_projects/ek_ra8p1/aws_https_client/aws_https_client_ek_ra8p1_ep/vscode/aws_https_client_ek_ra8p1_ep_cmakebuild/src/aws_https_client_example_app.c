/***********************************************************************************************************************
 * File Name    : aws_https_client_example_app.c
 * Description  : Example application layer for the AWS HTTPS Client EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - user_app_thread_entry.c calls a single function: aws_https_client_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "aws_https_client_example_app.h"
#include "adc_ep.h"
#include "littlefs_ep.h"
#include "console/uart_console.h"

/* API paths of the requests, built from the username and the feed name entered by the user */
static char s_get_api[URL_SIZE] = {RESET_VALUE};
static char s_put_post_api[URL_SIZE] = {RESET_VALUE};

/* Extracted ID from GET request to be appended to s_put_post_api */
static char s_feed_id[SIZE_32] = {RESET_VALUE};
static feed_id_state_t s_feed_id_state = FEED_ID_UNAVAILABLE;

/* Private functions */
static void credentials_get(void);
static void validate_input(const char * p_prompt, char * p_buf, uint32_t buf_size);
static void user_input(char * buf, int len);
static void print_ipconfig(void);
static void process_user_input(user_input_t user_input, float temp_c);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the AWS HTTPS Client example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void aws_https_client_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t status = RESET_VALUE;
    fsp_pack_version_t version = {RESET_VALUE};
    TlsTransportStatus_t tls_status = TLS_TRANSPORT_SUCCESS;
    char ip_str[IPV4_STR_SIZE] = {RESET_VALUE};
    char input_buff[INPUT_BUFFER_SIZE] = {RESET_VALUE};
    uint32_t ping_count = RESET_VALUE;
    uint32_t conn_attempt = RESET_VALUE;
    float mcu_die_temp = RESET_VALUE;

    /* Initialize the UART console used by printf */
    if (FSP_SUCCESS != uart_console_init())
    {
        /* UART failed to open - cannot print anything */
        return;
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    uart_console_flush();
    printf(EP_INFO);
    uart_console_flush();

    /* Initialize LittleFS port */
    err = littlefs_init();
    handle_error(err, "littlefs_init failed.");
    printf("\r\n\r\nLittleFS flash module initialized successfully");

    /* Initialize ADC in continuous mode */
    err = adc_init();
    handle_error(err, "adc_init failed.");
    printf("\r\n\r\nADC module initialized successfully");

    /* Initialize the crypto hardware acceleration and mbedtls */
    status = crypto_init();
    handle_error((fsp_err_t) status, "crypto_init failed.");
    printf("\r\n\r\nmbedtls platform set up successfully");

    /* Print the pre-initialization Ethernet IP configuration */
    printf(ETH_PREINIT);
    print_ipconfig();

    /* FreeRTOS IP initializes the IP stack */
    err = ip_stack_init();
    handle_error(err, "ip_stack_init failed.");

    printf("\r\n\r\nWaiting for network link up...");
    err = network_link_wait();
    handle_error(err, "network_link_wait failed.");

    /* Print the post-initialization Ethernet IP configuration */
    printf(ETH_POSTINIT);
    print_ipconfig();

    /* DNS lookup for the Domain name requested */
    err = dns_lookup(domain_name, ip_str);
    if (FSP_SUCCESS == err)
    {
        printf("\r\n\r\nDNS lookup for %s is: %s", domain_name, ip_str);
    }
    else
    {
        printf("\r\n\r\n[ERR] DNS lookup failed for %s", domain_name);
    }

    printf("\r\n\r\nPinging %s:", remote_ip_address);

    while (USR_PING_COUNT > ping_count)
    {
        /* Send an ICMP Ping request to the requested IP address
         * USR_PING_COUNT is used in this example project
         * For continuous testing the count can be increased to a bigger number */
        ping_request_send();
        ping_count++;

        /* Add some delay between pings */
        vTaskDelay(PING_DELAY);
    }
    printf("\r\n\r\nPackets: Sent  = %02d, Received = %02d, Lost = %02d", (int)g_ping_data.sent,
           (int)g_ping_data.received, (int)g_ping_data.lost);

    /* Get the Adafruit IO username, feed name and active key from the user */
    credentials_get();

    /* Perform device provisioning using the specified TLS client credentials */
    status = provision_alt_key();
    handle_error((fsp_err_t) status, "provision_alt_key failed.");
    printf("\r\n\r\nDevice provisioned successfully with client certificate and client key");

    /* Connect to the HTTPS server */
    printf("\r\n\r\nConnecting to the server, please wait...");
    for (conn_attempt = 1; conn_attempt <= HTTPS_CONNECTION_NUM_RETRY; conn_attempt++)
    {
        tls_status = connect_aws_https_client();
        if (TLS_TRANSPORT_SUCCESS == tls_status)
        {
            break;
        }

        if (HTTPS_CONNECTION_NUM_RETRY > conn_attempt)
        {
            printf("\r\n\r\n[ERR] Failed to connect the server, retrying after %d ms", CONNECTION_RETRY_DELAY);
            vTaskDelay(CONNECTION_RETRY_DELAY);
        }
    }
    handle_error((fsp_err_t) tls_status, "connect_aws_https_client failed.");
    printf("\r\n\r\nConnected to the server");

    /* Menu for user selection */
    printf(PRINT_MENU);

    /* Process the user input */
    while (true)
    {
        if (uart_console_has_char())
        {
            user_input(input_buff, (int) sizeof(input_buff));

            /* Read the internal MCU die temperature to upload the value using PUT/POST request */
            err = adc_data_read(&mcu_die_temp);
            handle_error(err, "adc_data_read failed.");

            process_user_input((user_input_t)atoi(input_buff), mcu_die_temp);

            /* Repeat the menu to display for user selection */
            printf(PRINT_MENU);
        }

        /* Yield the CPU to the other tasks until the next user input */
        vTaskDelay(INPUT_POLL_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function gets the Adafruit IO username, feed name and active key from the user, and builds the API
 *            paths used by the GET, PUT and POST requests.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void credentials_get(void)
{
    char username[SIZE_64] = {RESET_VALUE};
    char feed_name[SIZE_64] = {RESET_VALUE};

    /* Get the username, feed name and active key, the active key is used in the header of the requests */
    validate_input("\r\n\r\nEnter the feed name: ", feed_name, sizeof(feed_name));
    validate_input("\r\n\r\nEnter the Adafruit IO username: ", username, sizeof(username));
    validate_input("\r\n\r\nEnter the active key: ", g_aio_key, sizeof(g_aio_key));

    /* Build the API paths of the GET, PUT and POST requests */
    snprintf(s_get_api, sizeof(s_get_api), HTTPS_GET_API_FORMAT, username, feed_name);
    snprintf(s_put_post_api, sizeof(s_put_post_api), HTTPS_PUT_POST_API_FORMAT, username, feed_name);
}

/*******************************************************************************************************************//**
 * @brief      This function prompts the user and reads a line from the terminal.
 * @param[in]  p_prompt                 Prompt to display.
 * @param[out] p_buf                    Buffer to store the input string.
 * @param[in]  buf_size                 Size of the output buffer, including the null terminator.
 * @retval     None.
 **********************************************************************************************************************/
static void validate_input(const char * p_prompt, char * p_buf, uint32_t buf_size)
{
    char line[UART_CONSOLE_RX_BUFFER_SIZE + 1U] = {RESET_VALUE};
    uint32_t len = RESET_VALUE;
    uint32_t index = RESET_VALUE;
    bool is_valid = false;

    while (!is_valid)
    {
        printf("%s", p_prompt);

        /* Wait until the user starts typing */
        while (!uart_console_has_char())
        {
            vTaskDelay(INPUT_POLL_DELAY);
        }

        /* Read the typed line, user_input waits until the user presses ENTER */
        user_input(line, (int) sizeof(line));
        len = strlen(line);

        /* The input must not be empty and must fit in the output buffer */
        is_valid = ((RESET_VALUE != len) && (len < buf_size));

        /* Validate every character must not be space or control characters */
        for (index = RESET_VALUE; (index < len) && is_valid; index++)
        {
            is_valid = (0 != isgraph((unsigned char) line[index]));
        }

        if (!is_valid)
        {
            printf("\r\n\r\nThe input must be 1 to %u characters without spaces. Please try again.",
                   (unsigned int) (buf_size - 1U));
        }
    }

    /* Store the accepted input */
    strcpy(p_buf, line);
}

/*******************************************************************************************************************//**
 * @brief     This function updates the IP configuration with the values provided by the DHCP server, if used,
 *            and prints it.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void print_ipconfig(void)
{
    ip_config_update();

    printf("\r\n\r\nEthernet adapter for Renesas "KIT_NAME":");
    printf("\r\n\tDescription                       : Renesas "KIT_NAME" Ethernet");
    printf("\r\n\tPhysical Address                  : %02x-%02x-%02x-%02x-%02x-%02x",
           ucMACAddress[0], ucMACAddress[1], ucMACAddress[2], ucMACAddress[3], ucMACAddress[4], ucMACAddress[5]);
    printf("\r\n\tDHCP Enabled                      : %s", g_dhcp_in_use ? "Yes" : "No");
    printf("\r\n\tIPv4 Address                      : %d.%d.%d.%d", ucIPAddress[0], ucIPAddress[1], ucIPAddress[2],
           ucIPAddress[3]);
    printf("\r\n\tSubnet Mask                       : %d.%d.%d.%d", ucNetMask[0], ucNetMask[1], ucNetMask[2],
           ucNetMask[3]);
    printf("\r\n\tDefault Gateway                   : %d.%d.%d.%d", ucGatewayAddress[0], ucGatewayAddress[1],
           ucGatewayAddress[2], ucGatewayAddress[3]);
    printf("\r\n\tDNS Servers                       : %d.%d.%d.%d", ucDNSServerAddress[0], ucDNSServerAddress[1],
           ucDNSServerAddress[2], ucDNSServerAddress[3]);
    uart_console_flush();
}

/*******************************************************************************************************************//**
 * @brief     This function processes the request selected by the user from the menu and prints its response.
 * @param[in] user_input                User input option.
 * @param[in] temp_c                    MCU die temperature in degrees Celsius to upload.
 * @retval    None.
 **********************************************************************************************************************/
static void process_user_input(user_input_t user_input, float temp_c)
{
    HTTPStatus_t status = HTTPSuccess;
    char put_path[URL_SIZE] = {RESET_VALUE};
    const char * p_request = NULL;

    switch (user_input)
    {
        case GET:
        {
            printf("\r\n\r\nProcessing GET request");
            status = http_request_send(HTTP_METHOD_GET, s_get_api, RESET_VALUE);
            if (HTTPSuccess == status)
            {
                /* Fetch the feed ID from the response body, which is appended to s_put_post_api.
                 * The synchronous response body starts with a string in the format like
                 * [{"id":"0ENQG7RYQA40W17G2A2SFH8E9Q","..."}].
                 * So, the other portion of the string is skipped to fetch only the ID */
                if (NULL != g_response.pBody)
                {
                    strncpy(s_feed_id, (const char *) &g_response.pBody[ID_START_INDEX], ID_LEN);
                }

                /* The feed ID is just fetched, so the PUT request can use the latest one */
                s_feed_id_state = FEED_ID_LATEST;
            }
            p_request = "GET";
            break;
        }

        case PUT:
        {
            printf("\r\n\r\nProcessing PUT request");

            /* The PUT request needs the feed ID, which is fetched by the GET request */
            if (FEED_ID_UNAVAILABLE == s_feed_id_state)
            {
                printf("\r\n** The ID for the URL is not available. Run the GET method first to get the ID **");
                return;
            }

            if (FEED_ID_STALE == s_feed_id_state)
            {
                printf("\r\n** The ID for the URL being used may not be the latest data point's ID **");
                printf("\r\n** You can run the GET method again to update the latest ID **");
            }

            printf("\r\nValue of ID: %s", s_feed_id);

            /* The feed ID is used by this request, so it may not be the latest one for the next PUT request */
            s_feed_id_state = FEED_ID_STALE;

            /* The feed ID is appended to s_put_post_api for processing the PUT request */
            snprintf(put_path, sizeof(put_path), "%s%s", s_put_post_api, s_feed_id);
            status = http_request_send(HTTP_METHOD_PUT, put_path, temp_c);
            p_request = "PUT";
            break;
        }

        case POST:
        {
            printf("\r\n\r\nProcessing POST request");
            status = http_request_send(HTTP_METHOD_POST, s_put_post_api, temp_c);
            p_request = "POST";
            break;
        }

        default:
        {
            printf("\r\n\r\nInvalid menu option selected");
            return;
        }
    }

#if DEBUG_HTTPS
    if (HTTPSuccess == status)
    {
        printf("\r\nReceived HTTP response from %s%s...", HTTPS_HOST_ADDRESS, g_request_info.pPath);
        printf("\r\nResponse Headers:\r\n%s", g_response.pHeaders);
        uart_console_flush();
        printf("\r\nStatus Code:\r\n%u", g_response.statusCode);
        printf("\r\nResponse Body:\r\n%s", g_response.pBody);
        uart_console_flush();
    }
    else
    {
        printf("Failed to send HTTP %s request to %s%s: Error = %s.", g_request_info.pMethod,
               HTTPS_HOST_ADDRESS, g_request_info.pPath, http_error_string_get(status));
    }
#endif /* DEBUG_HTTPS */

    if (HTTPSuccess != status)
    {
        printf("\r\n\r\nHTTP Client error: %s", http_error_string_get(status));
    }
    handle_error((fsp_err_t) status, "http_request_send failed.\r\n");

    printf("\r\nReceived data using %s request = %s", p_request, (const char *)g_response.pBody);
    uart_console_flush();
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("\r\nReturned Error Code: 0x%x", (unsigned int)err);
        uart_console_flush();
        deinit_aws_https_client();
        littlefs_deinit();
        adc_deinit();
        __BKPT(0);
    }
}
