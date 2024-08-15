/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : menu_eth_emb.c
 * Version      : .
 * Description  : The Ethernet Server Demo
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
#include "menu_eth_emb.h"
#include "usr_app.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)
#define MENU_ENTER_RESPONSE_CRTL (0x09)
#define INPUT_BUFFER             (0x05)
#define CARRAGE_RETURN           (0x0D)

#define BLOCK_LIMIT              (0x80)


#define ETH_EMB_MODULE  "\r\n%d. WEB SERVER\r\n"

#define SUB_OPTIONS     "\r\nHost a small web-server on the kit and use a web browser to control the"\
                        "\r\nkit and view kit status\r\n"

#define SUB_SECTION_1   "\r\n> Connect the kit to an Ethernet switch/router using the provided "     \
                        "\r\nEthernet patch cable\r\n" \
                        "\r\n> Press tab upon connecting the Ethernet cable to continue\r\n"

#define CONNECTING      "Connecting..."

#define DHCP_CONNECTED  "\r\nConnection successful\r\n"                                              \
                        "\r\nKit IP address: <%d.%d.%d.%d>\r\n"

#define CONTINUE        "\r\n> Open the web browser\r\n"                                             \
                        "Type the kit IP address in the address bar\r\n"                             \
                        "The kit information will be displayed in the web browser"

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
                        "\r\nnetwork\r\n\r\n"

extern uint8_t g_ip_address[];
extern uint8_t g_net_mask[];
extern uint8_t g_gateway_address[];

/* Each thread must have a separate print buffer, to avoid clashes on task switching */
static char_t s_print_buffer[1024]    = {};
static uint32_t  s_eth_timeout        = 1500;

/**********************************************************************************************************************
 * Function Name: eth_emb_display_menu
 * Description  : Display and control the Ethernet Server Demo Screen.
 * Return Value : None.
 *********************************************************************************************************************/
test_fn eth_emb_display_menu(void)
{
    int32_t c = -1;
    EventBits_t uxBits;

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    sprintf(s_print_buffer, ETH_EMB_MODULE, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    sprintf(s_print_buffer, SUB_OPTIONS);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    sprintf(s_print_buffer, SUB_SECTION_1);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    while (CONNECTION_ABORT_CRTL != c)
    {
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c) || (MENU_ENTER_RESPONSE_CRTL == c))
        {
            break;
        }
        c = input_from_console();
    }

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)"\r\n");

    if (MENU_ENTER_RESPONSE_CRTL == c)
    {
        /* User has pressed TAB key so signal Ethernet to start */
        xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
        s_eth_timeout = 1500;

        xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);

        if (ipconfigUSE_DHCP == 0)
        {
            update_ipconfig_to_static_ip();

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
                update_ipconfig_to_static_ip();

                sprintf(s_print_buffer, DIRECT_CONNECT,
                        g_ip_address[0],      g_ip_address[1],      g_ip_address[2],      g_ip_address[3],
                        g_net_mask[0],        g_net_mask[1],        g_net_mask[2],        g_net_mask[3],
                        g_gateway_address[0], g_gateway_address[1], g_gateway_address[2], g_gateway_address[3]);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
            }

            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)CONNECTING);
            s_eth_timeout = 1500;

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
            sprintf(s_print_buffer, "\r\nConnection failed");

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console((void*)s_print_buffer);
        }
        else
        {
            uint8_t ipv4a = 0;
            uint8_t ipv4b = 0;
            uint8_t ipv4c = 0;
            uint8_t ipv4d = 0;


            if (true == get_resolved_ip_address(&ipv4a, &ipv4b, &ipv4c, &ipv4d))
            {
                sprintf(s_print_buffer, DHCP_CONNECTED, ipv4a, ipv4b, ipv4c, ipv4d);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);

                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)CONTINUE);
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
 End of function eth_emb_display_menu
 *********************************************************************************************************************/
