/***********************************************************************************************************************
 * File Name    : NetX_https_client_example_app.c
 * Description  : Example application layer for the NetX HTTPS Client EP.
 *                - All printing goes through the terminal library (terminal.c), which prints it via printf
 *                  (routed to UART through the console library).
 *                - https_client_thread_entry calls a single function: https_client_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include "https_client_thread.h"
#include "adc_ep.h"
#include "NetX_https_client_example_app.h"

/* Variable to capture invalid input state */
volatile bool g_b_invalid_input = false;

/* Private functions */
static void credentials_get(void);
static void validate_input(const char * p_prompt, char * p_buf, uint32_t buf_size);
static UINT process_input_request(UINT input_data);

/*******************************************************************************************************************//**
 * @brief     HTTPS Client Thread entry-point for the NetX HTTPS Client example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void https_client_thread_entry_run(void)
{
    UINT status = NX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Client IP address and Network Mask */
    ULONG client_ip_address = RESET_VALUE;
    UINT read_data = RESET_VALUE;
    float init_adc_val = RESET_VALUE;
    char input_buff[TERM_IP_MAX_SIZE] = {NULL_CHAR};

    /* Initialize the Terminal Thread */
    term_thread_init_check();

    /* Print EP and banner info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* Get the Adafruit IO username, feed name and active key from the user */
    credentials_get();

    /* Initialize the ADC */
    err = adc_init();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("adc_init failed.");
        ERROR_TRAP(err);
        return;
    }
    PRINT_INFO_STR("ADC module initialization successful.");

    /* Initialize NetX */
    nx_common_init0();

    /* Initialize packet pool */
    status = packet_pool0_init0();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool0_init0 failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Initialize IP */
    status = ip0_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip0_init failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Wait for the Ethernet link to be enabled */
    PRINT_INFO_STR("Checking for Ethernet link...");
    status = ip_link_check();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_link_check failed.");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("Ethernet link is up!");

    /* Initialize DHCP v4 Client */
    status = dhcpv4_client_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_client_init failed.");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("Acquired the IP address via DHCP successfully!");

    /* Print the DNS Server address provided by the DHCP Server */
    term_print_data(TERM_OP_MSG_DNS_SERVER_IP, sizeof(ULONG), &g_dns_server_addr);

    /* Initialize DNS Client and get the IP address of the Adafruit IO Server */
    PRINT_INFO_STR("Getting IP from Adafruit Server...");
    status = dns_client_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dns_client_init failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Print the success message and IP acquired */
    term_print_data(TERM_OP_MSG_SERVER_IP, sizeof(ULONG), &g_server_addr);

    /* Initialize Web HTTPS Client */
    status = web_http_client0_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("web_http_client0_init failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Initialize the TLS Component */
    tls_init();

    /* Connect to the HTTPS server (Adafruit IO Server in our case) */
    status = https_server_secure_connect();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("https_server_secure_connect failed.");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("HTTPS Client is connected to the server successfully!");

    /* Retrieve the IP address */
    status = client_ip_address_get(&client_ip_address);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("client_ip_address_get failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Print the client IP address */
    term_print_data(TERM_OP_MSG_CLIENT_IP, sizeof(ULONG), &client_ip_address);

    /* Add initial data to an empty feed */
    PRINT_INFO_STR("Adding initial data to the feed...");

    /* Read the temperature value from the ADC */
    err = adc_data_read(&init_adc_val);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("adc_data_read failed.");
        ERROR_TRAP(err);
        return;
    }

    /* POST request */
    PRINT_INFO_STR("Processing POST request...");
    status = process_post_request(init_adc_val);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("process_post_request failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Read the response of the initial POST request from the server */
    status = read_response_from_server();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("read_response_from_server failed.");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("Posted initial data on the feed successfully!");

    /* Print the menu options */
    term_print_data(TERM_OP_MSG_MENU, RESET_VALUE, NULL);

    /* Process the data as per the user selected option from the menu */
    while (true)
    {
        /* Wait for user input */
        do
        {
            status = term_ip_get(input_buff, sizeof(input_buff));
        } while (TX_SUCCESS != status);

        /* Conversion from input string to integer value */
        read_data = (UINT)atoi(input_buff);

        /* Processes the input request from the user */
        status = process_input_request(read_data);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("process_input_request failed.");
            ERROR_TRAP(status);
            return;
        }

        if (true != g_b_invalid_input)
        {
            /* Read data from the HTTPS Server */
            status = read_response_from_server();
            if (NX_SUCCESS != status)
            {
                PRINT_ERR_STR("read_response_from_server failed.");
                ERROR_TRAP(status);
                return;
            }

            /* Print the received feed data from the server */
            term_print_data(TERM_OP_MSG_RESPONSE_DATA, (uint32_t)(strlen((const char *)g_receive_buffer) + 1U),
                            g_receive_buffer);
            PRINT_INFO_STR("Read the response from the server successfully!");
        }

        /* Reset the flag */
        g_b_invalid_input = false;

        /* Print the menu options */
        term_print_data(TERM_OP_MSG_MENU, RESET_VALUE, NULL);
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function gets the Adafruit IO username, feed name and active key from the user. They are used to
 *            build the URL paths and the headers of the GET, PUT and POST requests.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void credentials_get(void)
{
    validate_input("Enter the feed name: ", g_feed_name, sizeof(g_feed_name));
    validate_input("Enter the Adafruit IO username: ", g_io_username, sizeof(g_io_username));
    validate_input("Enter the active key: ", g_aio_key, sizeof(g_aio_key));
}

/*******************************************************************************************************************//**
 * @brief      This function prompts the user and reads a line from the terminal. The prompt is repeated until the
 *             input is not empty, fits in the output buffer and contains no space or control characters.
 * @param[in]  p_prompt                 Prompt to display.
 * @param[out] p_buf                    Buffer to store the input string.
 * @param[in]  buf_size                 Size of the output buffer, including the null terminator.
 * @retval     None.
 **********************************************************************************************************************/
static void validate_input(const char * p_prompt, char * p_buf, uint32_t buf_size)
{
    UINT status = TX_SUCCESS;
    char line[TERM_IP_MAX_SIZE] = {NULL_CHAR};
    char retry_str[96] = {NULL_CHAR};
    uint32_t len = RESET_VALUE;
    uint32_t index = RESET_VALUE;
    bool is_valid = false;

    while (!is_valid)
    {
        /* Display the prompt */
        term_print_data(TERM_OP_MSG_PROMPT, (uint32_t)(strlen(p_prompt) + 1U), (void *)p_prompt);

        /* Wait for the user input */
        do
        {
            status = term_ip_get(line, sizeof(line));
        } while (TX_SUCCESS != status);

        len = (uint32_t)strlen(line);

        /* The input must not be empty and must fit in the output buffer */
        is_valid = ((RESET_VALUE != len) && (len < buf_size));

        /* Every character must not be a space or a control character */
        for (index = RESET_VALUE; (index < len) && is_valid; index++)
        {
            is_valid = (0 != isgraph((unsigned char)line[index]));
        }

        if (!is_valid)
        {
            snprintf(retry_str, sizeof(retry_str), "The input must be 1 to %u characters without spaces. Please try again.",
                     (unsigned int)(buf_size - 1U));
            PRINT_INFO_STR(retry_str);
        }
    }

    /* Store the accepted input */
    strcpy(p_buf, line);
}

/*******************************************************************************************************************//**
 * @brief     This function processes the user input request and sends the temperature data to, or requests it
 *            from, the Adafruit Server.
 * @param[in] input_data                 User input option.
 * @retval    NX_SUCCESS                 Processed successfully.
 * @retval    Any other error codes      Failed to process.
 **********************************************************************************************************************/
static UINT process_input_request(UINT input_data)
{
    UINT status = NX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;
    float mcu_die_temp = RESET_VALUE;

    /* Read the temperature value from the ADC */
    err = adc_data_read(&mcu_die_temp);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("adc_data_read failed.");
        ERROR_TRAP(err);
        return (UINT)err;
    }

    /* Process requests as per user input */
    switch (input_data)
    {
        case GET_REQUEST:
        {
            PRINT_INFO_STR("Processing GET request...");
            status = process_get_request();
            break;
        }

        case PUT_REQUEST:
        {
            PRINT_INFO_STR("Processing PUT request...");
            status = process_put_request(mcu_die_temp);
            break;
        }

        case POST_REQUEST:
        {
            PRINT_INFO_STR("Processing POST request...");
            status = process_post_request(mcu_die_temp);
            break;
        }

        default:
        {
            PRINT_INFO_STR("Invalid menu option selected");

            /* Set the invalid input flag */
            g_b_invalid_input = true;
            break;
        }
    }

    return status;
}
