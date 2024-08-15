/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : menu_eth_www.c
 * Version      : .
 * Description  : The Ethernet Client Demo.
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "net_thread.h"

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "bsp_api.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "menu_eth_www.h"
#include "usr_app.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)
#define MENU_ENTER_RESPONSE_CRTL (0x09)
#define INPUT_BUFFER             (0x05)
#define CARRAGE_RETURN           (0x0D)

#define BLOCK_LIMIT              (0x80)


#ifdef MODULE_NAME
    #undef MODULE_NAME
#endif

#define MODULE_NAME     "\r\n%d. NETWORK NAME LOOKUP\r\n"

#define SUB_OPTIONS     "\r\nObtain information from the World Wide Web by performing a DNS lookup\r\n"

#define SUB_SECTION_1   "\r\n> Connect the kit to an Ethernet switch/router using the Ethernet"    \
                        "\r\npatch cable\r\n" \
                        "\r\n> Press tab upon connecting the Ethernet cable to continue\r\n"

#define CONTINUE        "\r\n> Open the web browser on the host PC by typing the above IP address"   \
                        "\r\nin the address bar. The kit information will be displayed in the web"   \
                        "\r\nbrowser" \
                        "\r\n"

#define CONNECTING      "\r\nConnecting..."

#define DHCP_CONNECTED  "\r\nKit IP address: <%d.%d.%d.%d>"

#define WWW_LOOKUP      "\r\nDNS Lookup for \"%s\" returned %s \r\n"                              \
                        "\r\nPinging %s..."

#define WWW_PING_PASS   "\r\nPinging successful"

#define WWW_PING_FAIL   "\r\nPinging failed"

#define DIRECT_CONNECT  "\r\n\r\nDCHP was unsuccessful, using static IP\r\n"                         \
                        "\r\n> To change the Static IP settings, edit the configuration in the"\
                        "\r\nEthernet Stack in FSP configuration source code of this example project"\
                        "\r\nwhich requires rebuilding the code and reprogramming the kit\r\n"       \
                        "\r\nKit IP Address: <%d.%d.%d.%d> "                                         \
                        "\r\nKit IP Mask is: <%d.%d.%d.%d> "                                         \
                        "\r\nKit IP Gateway: <%d.%d.%d.%d> \r\n"                                     \
                        "\r\n> Ensure that the host PC is using the same Gateway address. Check that"\
                        "\r\nat least one bit of the IP address of the PC is different where the "   \
                        "\r\nbinary mask bits are zero. For example, if the Mask is 255.255.255.0,"  \
                        "\r\nthe final number of the IP address must be different from that of the " \
                        "\r\nkit, and both addresses must not match with any other device on the "\
                        "\r\nnetwork\r\n"


extern uint8_t g_ip_address[];
extern uint8_t g_net_mask[];
extern uint8_t g_gateway_address[];

extern char_t g_remote_ip_address[];

/* Each thread must have a separate print buffer, to avoid clashes on task switching */
static char_t    s_print_buffer[1024] = {};
static uint32_t  s_eth_timeout        = 150;

/**********************************************************************************************************************
 * Function Name: eth_www_display_menu
 * Description  : The Ethernet Client Demo.
 * Return Value : .
 *********************************************************************************************************************/
test_fn eth_www_display_menu(void)
{
    int32_t c = -1;
    EventBits_t uxBits;

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);
    sprintf(s_print_buffer, MODULE_NAME, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);
    sprintf(s_print_buffer, SUB_OPTIONS);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);
    sprintf(s_print_buffer, SUB_SECTION_1);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c) || (MENU_ENTER_RESPONSE_CRTL == c))
        {
            break;
        }
        c = input_from_console();
    }

    if (MENU_ENTER_RESPONSE_CRTL == c)
    {
        /* User has pressed TAB key so signal Ethernet to start */
        xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
        s_eth_timeout = 150;

        xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);

        if (ipconfigUSE_DHCP == 0)
        {
            sprintf(s_print_buffer, DIRECT_CONNECT,
                    g_ip_address[0],      g_ip_address[1],      g_ip_address[2],      g_ip_address[3],
                    g_net_mask[0],        g_net_mask[1],        g_net_mask[2],        g_net_mask[3],
                    g_gateway_address[0], g_gateway_address[1], g_gateway_address[2], g_gateway_address[3]);

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console((void*)s_print_buffer);
        }

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)CONNECTING);

        while (--s_eth_timeout)
        {
            vTaskDelay(100);

            uxBits = xEventGroupWaitBits(g_update_console_event,  STATUS_ETHERNET_LINKUP, pdFALSE, pdTRUE, 1);

            if ((uxBits & (STATUS_ETHERNET_LINKUP)) == (STATUS_ETHERNET_LINKUP))
            {
                xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);
                break;
            }
        }

        if (0 == s_eth_timeout)
        {
            if (ipconfigUSE_DHCP == 1)
            {

                sprintf(s_print_buffer, DIRECT_CONNECT,
                        g_ip_address[0],      g_ip_address[1],      g_ip_address[2],      g_ip_address[3],
                        g_net_mask[0],        g_net_mask[1],        g_net_mask[2],        g_net_mask[3],
                        g_gateway_address[0], g_gateway_address[1], g_gateway_address[2], g_gateway_address[3]);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
            }

            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)CONNECTING);
            s_eth_timeout = 150;

            while (--s_eth_timeout)
            {
                vTaskDelay(100);

                uxBits = xEventGroupWaitBits(g_update_console_event,  STATUS_ETHERNET_LINKUP, pdFALSE, pdTRUE, 1);

                if ((uxBits & (STATUS_ETHERNET_LINKUP)) == (STATUS_ETHERNET_LINKUP))
                {
                    xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);
                    break;
                }
            }
        }

        if (0 == s_eth_timeout)
        {
            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)"\r\nConnection failed");
        }
        else
        {
            uint8_t ipv4a = 0;
            uint8_t ipv4b = 0;
            uint8_t ipv4c = 0;
            uint8_t ipv4d = 0;

            char_t * p_domain_name  = USR_TEST_DOMAIN_NAME;
            uint32_t  usrPingCount  = RESET_VALUE;
            st_ping_data_t ping_data   = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

            if (true == get_resolved_ip_address(&ipv4a, &ipv4b, &ipv4c, &ipv4d))
            {
                BaseType_t status = pdFALSE;

                dns_querry_func(p_domain_name);

                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)"\r\nConnection successful\r\n");
                sprintf(s_print_buffer, DHCP_CONNECTED, ipv4a, ipv4b, ipv4c, ipv4d);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                sprintf(s_print_buffer, WWW_LOOKUP, USR_TEST_DOMAIN_NAME, g_remote_ip_address, g_remote_ip_address);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);

                while (usrPingCount < USR_PING_COUNT)
                {
                    /* Send a ICMP Ping request to the requested IP address */
                    status =  send_ping((char *)g_remote_ip_address);

                    if (pdFALSE != status)
                    {
                        ping_data.sent++;
                    }
                    else
                    {
                        ping_data.lost++;
                    }
                    usrPingCount++;

                    /* Add some delay between Pings */
                    vTaskDelay(10);
                }

                if (0 == ping_data.lost)
                {
                    /* ignoring -Wpointer-sign is OK for a constant string */
                    print_to_console((uint8_t *)WWW_PING_PASS);
                }
                else
                {
                    /* ignoring -Wpointer-sign is OK for a constant string */
                    print_to_console((uint8_t *)WWW_PING_FAIL);
                }
            }
        }
    }

    sprintf(s_print_buffer, MENU_RETURN_INFO);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
        c = input_from_console();
    }

return (0);
}
/**********************************************************************************************************************
 End of function eth_www_display_menu
 *********************************************************************************************************************/
