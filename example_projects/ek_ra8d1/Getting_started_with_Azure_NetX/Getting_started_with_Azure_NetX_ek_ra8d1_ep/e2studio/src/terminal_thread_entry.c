/***********************************************************************************************************************
* File Name    : terminal_thread_entry.c
* Description  : Contains data structures and functions used in the terminal thread
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <terminal_thread.h>
#include "common_app.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern TX_THREAD net_link_thread;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static UINT terminal_input_handle(void);
static UINT terminal_output_handle(void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief Entry function for the terminal thread.
 *
 * This function initializes the terminal communication, starts the network link thread, and
 * enters a loop to handle user input and output for terminal communication.
 **********************************************************************************************************************/
void terminal_thread_entry(void)
{
    UINT status = TX_SUCCESS;

    /* Initialize the terminal communication. */
    /* Initialize terminal */
    status = TERM_INIT();
    if (FSP_SUCCESS != status)
    {
        /* Error trap */
        ERROR_TRAP;
    }

    /* Initialize the necessary resources for the terminal communication. */
    status = term_framework_init();
    if (TX_SUCCESS != status)
    {
        /* Error trap */
        ERROR_TRAP;
    }

    /* Start the network link thread, which does not start automatically during initialization. */
    tx_thread_resume(&net_link_thread);

    while (true)
    {
        /* Handle user input from the terminal. */
        status = terminal_input_handle();
        if (TX_SUCCESS != status)
        {
            /* Error trap */
            ERROR_TRAP;
        }

        /* Handle output to the terminal application via the terminal interface. */
        status = terminal_output_handle();
        if (TX_SUCCESS != status)
        {
            /* Error trap */
            ERROR_TRAP;
        }

        tx_thread_sleep (TERMINAL_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief Handles input from the terminal.
 *
 * This function checks if there is any data input from the terminal, reads it, and sends it to the input queue.
 *
 * @retval TX_SUCCESS Always returns success.
 **********************************************************************************************************************/
static UINT terminal_input_handle(void)
{
    UINT read_len = RESET_VALUE;
    CHAR read_buff [TERM_BUFFER_SIZE];

    if(TERM_HAS_DATA())
    {
        read_len = TERM_READ(read_buff, sizeof(read_buff));
        if (read_len)
        {
            term_send_input_queue(TERMINAL_INPUT_MESSAGE, read_buff, read_len);
        }
    }

    return TX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief Handles output to the terminal based on messages from the output queue.
 *
 * This function retrieves messages from the output queue and prints them to the terminal. It handles
 * various types of messages, including errors, network information, and protocol-specific messages.
 *
 * @retval TX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT terminal_output_handle(void)
{
    UINT status = TX_SUCCESS;
    term_msg_t * p_term_msg = NULL;

    while (true)
    {
        /* Retrieve a message from the output queue. */
        status = term_get_output_queue((void *)&p_term_msg, TX_NO_WAIT);
        ERROR_RET(TX_SUCCESS != status && TX_QUEUE_EMPTY != status, status);

        if (TX_SUCCESS == status)
        {
            switch (p_term_msg->id)
            {
                case TERMINAL_OUTPUT_EP_BANNER:
                    /* Get FSP package information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);

                    /* Print the EP banner to the terminal application on PC. */
                    TERM_PRINTF(BANNER_INFO, EP_VERSION,
                                    version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
                    break;

                case TERMINAL_OUTPUT_EP_INFO:
                    /* Print the EP information to the terminal application on PC. */
                    TERM_PRINTF(EP_INFO);
                    break;

                case TERMINAL_OUTPUT_NET_INFO:
                    /* Retrieve the network configuration. */
                    net_info_t info = *(net_info_t*)p_term_msg->msg;

                    /* Print the physical address to the terminal application on PC. */
                    TERM_PRINTF("\r\nPhysical Address. . . . . . . . . : %.2x-%.2x-%.2x-%.2x-%.2x-%.2x",
                                info.phy_addr.block.a, info.phy_addr.block.b, info.phy_addr.block.c,
                                info.phy_addr.block.d, info.phy_addr.block.e, info.phy_addr.block.f);

                    /* Print the IP address to the terminal application on PC. */
                    TERM_PRINTF("\r\nIPv4 Address. . . . . . . . . . . : %d.%d.%d.%d",
                                info.ipv4_addr.block.a, info.ipv4_addr.block.b,
                                info.ipv4_addr.block.c, info.ipv4_addr.block.d);

                    /* Print the subnet mask to the terminal application on PC. */
                    TERM_PRINTF("\r\nSubnet Mask . . . . . . . . . . . : %d.%d.%d.%d",
                                info.subnet_mask.block.a, info.subnet_mask.block.b,
                                info.subnet_mask.block.c, info.subnet_mask.block.d);

                    /* Print the default gateway address to the terminal application on PC. */
                    TERM_PRINTF("\r\nDefault Gateway . . . . . . . . . : %d.%d.%d.%d\r\n",
                                info.default_gateway.block.a, info.default_gateway.block.b,
                                info.default_gateway.block.c, info.default_gateway.block.d);
                    break;

                case TERMINAL_OUTPUT_INFO_STR:
                    /* Print a message to the terminal application on PC. */
                    TERM_PRINTF("\r\n%s", p_term_msg->msg);
                    break;

                case TERMINAL_OUTPUT_ERR_STR:
                    /* Print an error message to the terminal application on PC. */
                    TERM_PRINTF("\r\nERROR: %s", p_term_msg->msg);
                    break;

                case TERMINAL_OUTPUT_ERR_TRAP:
                    /* Print the error code to the terminal application on PC. */
                    UINT err = *(UINT*)p_term_msg->msg;
                    TERM_PRINTF("\r\nReturned Error Code: 0x%x\r\n\r\n", err);

                    /* trap upon the error */
                    ERROR_TRAP;
                    break;

                case TERMINAL_OUTPUT_EP_MENU:
                    /* Print the menu to the terminal application on PC. */
                    TERM_PRINTF(MENU);
                    break;

                case TERMINAL_OUTPUT_TCP_SEND_DATA:
                    /* Print the TCP request string to the terminal application on PC. */
                    TERM_PRINTF("\r\nTCP Request: %s", p_term_msg->msg);
                    break;

                case TERMINAL_OUTPUT_TCP_RECV_DATA:
                    /* Print the TCP response string to the terminal application on PC. */
                    TERM_PRINTF("\r\nTCP Response: %s", p_term_msg->msg);
                    break;

                case TERMINAL_OUTPUT_UDP_SEND_DATA:
                    /* Print the UDP request string to the terminal application on PC. */
                    TERM_PRINTF("\r\nUDP Request: %s", p_term_msg->msg);
                    break;

                case TERMINAL_OUTPUT_UDP_RECV_DATA:
                    /* Print the UDP response string to the terminal application on PC. */
                    TERM_PRINTF("\r\nUDP Response: %s", p_term_msg->msg);
                    break;

                case TERMINAL_OUTPUT_DNS_HOST_ADDR:
                    /* Print the host IPv4 address to the terminal application on PC. */
                    ipv4_addr_t ip = {.full = *(ULONG*)p_term_msg->msg};
                    TERM_PRINTF("\r\nThe Host IPv4 Address: %d.%d.%d.%d", ip.block.a, ip.block.b, ip.block.c, ip.block.d);
                    break;

                case TERMINAL_OUTPUT_DNS_HOST_NAME:
                    /* Print the host name to the terminal application on PC. */
                    TERM_PRINTF("\r\nThe Host Name: %s", p_term_msg->msg);
                    break;

                default :
                    break;
            }

            /*  Release the print message allocated block. */
            tx_byte_release((VOID *)p_term_msg);
        }

        if (TX_QUEUE_EMPTY == status)
        {
            break;
        }
    }
    return TX_SUCCESS;
}
