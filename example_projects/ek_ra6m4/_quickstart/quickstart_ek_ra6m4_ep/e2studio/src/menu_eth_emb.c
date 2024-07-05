/**********************************************************************************************************************
 * File Name    : menu_eth_emb.c
 * Version      : .
 * Description  : The Ethernet Server Demo
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "net_thread.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
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

#define INVALID_CHARACTER        ((int16_t) -1)
#define INVALID_BLOCK_SIZE       ((int16_t) -2)
#define INVALID_BLOCK_BOUNDARY   ((int16_t) -3)

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


/* Terminal window escape sequences */
static const char * const sp_clear_screen   = "\x1b[2J";
static const char * const sp_cursor_home    = "\x1b[H";

static char print_buffer [1024] = {};
static uint32_t eth_timeout = 150; // (5 sec)

extern uint8_t ucIPAddress[];
extern uint8_t ucNetMask[];
extern uint8_t ucGatewayAddress[];

extern uint32_t isNetworkUp(void);
extern void update_ipconfig_to_static_ip(void);

/**********************************************************************************************************************
 * Function Name: eth_emb_display_menu
 * Description  : Display and control the Ethernet Server Demo Screen.
 * Return Value : None.
 *********************************************************************************************************************/
test_fn eth_emb_display_menu(void)
{
    int c = -1;
    EventBits_t uxBits;

    sprintf(print_buffer, "%s%s", sp_clear_screen, sp_cursor_home);
    print_to_console(print_buffer);

    sprintf(print_buffer, ETH_EMB_MODULE, g_selected_menu);
    print_to_console(print_buffer);

    sprintf(print_buffer, SUB_OPTIONS);
    print_to_console(print_buffer);

    sprintf(print_buffer, SUB_SECTION_1);
    print_to_console(print_buffer);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c) || (MENU_ENTER_RESPONSE_CRTL == c))
        {
            break;
        }
        c = input_from_console();
    }

    print_to_console("\r\n");

    if (MENU_ENTER_RESPONSE_CRTL == c)
    {
        /* User has pressed TAB key so signal Ethernet to start */
        xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
        eth_timeout = 150;

        xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);

        if (ipconfigUSE_DHCP == 0)
        {
            sprintf(print_buffer, DIRECT_CONNECT,
                    ucIPAddress[0],      ucIPAddress[1],      ucIPAddress[2],      ucIPAddress[3],
                    ucNetMask[0],        ucNetMask[1],        ucNetMask[2],        ucNetMask[3],
                    ucGatewayAddress[0], ucGatewayAddress[1], ucGatewayAddress[2], ucGatewayAddress[3]);
            print_to_console(print_buffer);
        }

        print_to_console(CONNECTING);

        while (--eth_timeout)
        {
            vTaskDelay(100);

            uxBits = xEventGroupWaitBits(g_update_console_event,  STATUS_ETHERNET_LINKUP, pdFALSE, pdTRUE, 1);

            if ((uxBits & (STATUS_ETHERNET_LINKUP)) == (STATUS_ETHERNET_LINKUP))
            {
                xEventGroupClearBits(g_update_console_event, STATUS_ETHERNET_LINKUP);
                break;
            }
        }

        if (0 == eth_timeout)
        {
            if (ipconfigUSE_DHCP == 1)
            {
                sprintf(print_buffer, DIRECT_CONNECT,
                        ucIPAddress[0],      ucIPAddress[1],      ucIPAddress[2],      ucIPAddress[3],
                        ucNetMask[0],        ucNetMask[1],        ucNetMask[2],        ucNetMask[3],
                        ucGatewayAddress[0], ucGatewayAddress[1], ucGatewayAddress[2], ucGatewayAddress[3]);
                print_to_console(print_buffer);
            }

            print_to_console(CONNECTING);
            eth_timeout = 150;

            while (--eth_timeout)
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

        if (0 == eth_timeout)
        {
            sprintf(print_buffer, "\r\nConnection failed");
            print_to_console(print_buffer);
        }
        else
        {
            uint8_t ipv4a = 0;
            uint8_t ipv4b = 0;
            uint8_t ipv4c = 0;
            uint8_t ipv4d = 0;

            char * domain_name = USR_TEST_DOMAIN_NAME;

            dnsQuerryFunc(domain_name);

            if (true == getResolvedIPAddress(&ipv4a, &ipv4b, &ipv4c, &ipv4d))
            {
                BaseType_t status = pdFALSE;

                dnsQuerryFunc(domain_name);

                sprintf(print_buffer, DHCP_CONNECTED, ipv4a, ipv4b, ipv4c, ipv4d);
                print_to_console(print_buffer);

                print_to_console(CONTINUE);
            }
        }
    }

    sprintf(print_buffer, MENU_RETURN_INFO);
    print_to_console(print_buffer);

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
