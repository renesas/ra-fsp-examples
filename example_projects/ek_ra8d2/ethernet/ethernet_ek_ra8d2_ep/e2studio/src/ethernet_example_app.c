/***********************************************************************************************************************
 * File Name    : ethernet_example_app.c
 * Description  : Example application layer for the Ethernet EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - net_thread_entry.c calls a single function: ethernet_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "ethernet_example_app.h"
#include "console/uart_console.h"

/* Domain for the DNS host lookup is used in this example project
 * The project can be built with different *gp_domain_name to validate the DNS client */
char * gp_domain_name = USR_TEST_DOMAIN_NAME;

/* IP address of the PC or any Device on the LAN/WAN where the Ping request is sent.
 * Note: Users needs to change this according to the LAN settings of your test PC or device
 * when running this project */
char * gp_remote_ip_address = USR_TEST_PING_IP;

#if (ipconfigUSE_DHCP != 0)
    /* DHCP populates these IP address, Subnet mask and Gateway Address. So start with this is zeroed out values
     * The MAC address is Test MAC address */
    static uint8_t ucMACAddress[6]       = {0x74, 0x90, 0x50, 0x65, 0x82, 0x64};
    static uint8_t ucIPAddress[4]        = {RESET_VALUE};
    static uint8_t ucNetMask[4]          = {255, 255, 255, 128};
    static uint8_t ucGatewayAddress[4]   = {132, 158, 124, 1};
    static uint8_t ucDNSServerAddress[4] = {RESET_VALUE};
#else
    /* Static IP configuration, when DHCP mode is not used for the example project.
     * This needs to be populated by the user according to the Network settings of your LAN.
     * This sample address taken from the LAN where it is tested. This is different for different LAN.
     * Get the Address using the PC IPconfig details */
    static uint8_t ucMACAddress[6]       = {0x74, 0x90, 0x50, 0x65, 0x82, 0x64};
    static uint8_t ucIPAddress[4]        = {192, 168, 0, 52};
    static uint8_t ucNetMask[4]          = {255, 255, 255, 0};
    static uint8_t ucGatewayAddress[4]   = {192, 168, 0, 3};
    static uint8_t ucDNSServerAddress[4] = {10, 60, 1, 2};
#endif /* ipconfigUSE_DHCP */

static uint32_t s_usr_ping_count = RESET_VALUE;
static uint32_t s_usr_print_ability = RESET_VALUE;

/* Private functions */
static void print_ipconfig(void);
static void handle_error(BaseType_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Ethernet example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ethernet_example_app_run(void)
{
    BaseType_t status = pdFALSE;
    fsp_pack_version_t version = {RESET_VALUE};
    char buffer[16] = {RESET_VALUE};
    uint32_t ulIPAddress = RESET_VALUE;

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    uint32_t status_cons = uart_console_init();
    if (FSP_SUCCESS != status_cons)
    {
        return;     /* UART failed to open - cannot print anything */
    }

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Print the Ethernet configuration prior to the IP initializes */
    printf(ETH_PREINIT);
    print_ipconfig();

    /* FreeRTOS IP initialization */
    status = ethernet_init(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
    handle_error(status, "\r\nethernet_init failed");

    /* Print the Ethernet configuration after IP initialization */
    printf(ETH_POSTINIT);

    while (true)
    {
        /* Check if both the Ethernet link and IP link are UP */
        if (SUCCESS == is_network_up())
        {
            /* s_usr_print_ability is added to avoid multiple UP messages or DOWN messages repeating */
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
#endif /* ipconfigUSE_DHCP */
                /* Update IP credentials on to the console */
                print_ipconfig();

                /* DNS lookup for the Domain name requested. This is synchronous activity */
                ulIPAddress = get_host_by_name(gp_domain_name);

                if (RESET_VALUE != ulIPAddress)
                {
                    /* Convert the IP address to a string */
                    inet_ntoa(ulIPAddress, buffer);

                    /* Print out the IP address obtained from the DNS lookup */
                    printf("\r\nDNS lookup for \"www.freertos.org\" is: %s", buffer);
                }
                else
                {
                    printf("\r\nDNS lookup failed for \"www.freertos.org\"");
                }
            }

            if (!(PRINT_NWK_USR_MSG_DISABLE & s_usr_print_ability))
            {
                /* Pinging IP */
                printf("\r\nPinging %s:", gp_remote_ip_address);
            }

            while (s_usr_ping_count < USR_PING_COUNT)
            {
                /* Send a ICMP Ping request to the requested IP address
                 * USR_PING_COUNT (100) is used in this example project
                 * For continuous testing the count can be increased to bigger number */
                status = send_ping_request(gp_remote_ip_address);

                if (pdFALSE != status)
                {
                    g_ping_data.sent++;
                    printf("!");
                }
                else
                {
                    g_ping_data.lost++;
                    printf(".");
                }
                s_usr_ping_count++;

                vTaskDelay(10);
            }

            if (!(PRINT_NWK_USR_MSG_DISABLE & s_usr_print_ability))
            {
                printf("\r\nPing Statistics for %s:", gp_remote_ip_address);
                printf("\r\nPackets: Sent = %02d, Received = %02d, Lost = %02d", g_ping_data.sent,
                          g_ping_data.received, g_ping_data.lost);
                s_usr_print_ability |= PRINT_NWK_USR_MSG_DISABLE;
            }
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
 * @brief      This function prints the IP configuration.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
static void print_ipconfig(void)
{
#if (ipconfigUSE_DHCP != 0)
    if (g_dhcp_in_use)
    {
        ucNetMask[3] = (uint8_t)((g_xnd.ulNetMask & 0xFF000000) >> 24);
        ucNetMask[2] = (uint8_t)((g_xnd.ulNetMask & 0x00FF0000) >> 16);
        ucNetMask[1] = (uint8_t)((g_xnd.ulNetMask & 0x0000FF00) >> 8);
        ucNetMask[0] = (uint8_t)(g_xnd.ulNetMask & 0x000000FF);

        ucGatewayAddress[3] = (uint8_t)((g_xnd.ulGatewayAddress & 0xFF000000) >> 24);
        ucGatewayAddress[2] = (uint8_t)((g_xnd.ulGatewayAddress & 0x00FF0000) >> 16);
        ucGatewayAddress[1] = (uint8_t)((g_xnd.ulGatewayAddress & 0x0000FF00) >> 8);
        ucGatewayAddress[0] = (uint8_t)(g_xnd.ulGatewayAddress & 0x000000FF);

        ucDNSServerAddress[3] = (uint8_t)((g_xnd.ulDNSServerAddresses[0] & 0xFF000000) >> 24);
        ucDNSServerAddress[2] = (uint8_t)((g_xnd.ulDNSServerAddresses[0] & 0x00FF0000) >> 16);
        ucDNSServerAddress[1] = (uint8_t)((g_xnd.ulDNSServerAddresses[0] & 0x0000FF00) >> 8);
        ucDNSServerAddress[0] = (uint8_t)(g_xnd.ulDNSServerAddresses[0] & 0x000000FF);

        ucIPAddress[3] = (uint8_t)((g_xnd.ulIPAddress & 0xFF000000) >> 24);
        ucIPAddress[2] = (uint8_t)((g_xnd.ulIPAddress & 0x00FF0000) >> 16);
        ucIPAddress[1] = (uint8_t)((g_xnd.ulIPAddress & 0x0000FF00) >> 8);
        ucIPAddress[0] = (uint8_t)(g_xnd.ulIPAddress & 0x000000FF);
    }
#endif /* ipconfigUSE_DHCP */
    printf("\r\nEthernet adapter for Renesas "KIT_NAME":");
    printf("\r\n\tDescription . . . . . . . . . . . : Renesas "KIT_NAME" Ethernet");
    printf("\r\n\tPhysical Address. . . . . . . . . : %02x-%02x-%02x-%02x-%02x-%02x",
              ucMACAddress[0], ucMACAddress[1], ucMACAddress[2], ucMACAddress[3],
              ucMACAddress[4], ucMACAddress[5]);
    printf("\r\n\tDHCP Enabled. . . . . . . . . . . : %s",g_dhcp_in_use?"Yes":"No");
    printf("\r\n\tIPv4 Address. . . . . . . . . . . : %d.%d.%d.%d", ucIPAddress[0], ucIPAddress[1],
                                                                       ucIPAddress[2], ucIPAddress[3]);
    printf("\r\n\tSubnet Mask . . . . . . . . . . . : %d.%d.%d.%d", ucNetMask[0], ucNetMask[1],
                                                                       ucNetMask[2], ucNetMask[3]);
    printf("\r\n\tDefault Gateway . . . . . . . . . : %d.%d.%d.%d", ucGatewayAddress[0], ucGatewayAddress[1],
                                                                       ucGatewayAddress[2], ucGatewayAddress[3]);
    printf("\r\n\tDNS Servers . . . . . . . . . . . : %d.%d.%d.%d", ucDNSServerAddress[0], ucDNSServerAddress[1],
                                                                       ucDNSServerAddress[2], ucDNSServerAddress[3]);
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(BaseType_t err, const char * err_str)
{
    if (pdPASS != err)
    {
        printf("\r\n\r\n[ERR] %s", err_str);
        printf("\r\nReturned Error Code: 0x%x", (unsigned int) err);
        uart_console_flush();
        __BKPT(0);
    }
}
