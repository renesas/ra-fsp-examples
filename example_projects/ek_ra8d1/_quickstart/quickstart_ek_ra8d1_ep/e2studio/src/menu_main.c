/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : menu_main.c
 * Description  : The main menu controller.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "menu_main.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"
#include "jlink_console.h"

#define MODULE_NAME              "\r\n\x1b[2m\x1b[37mWelcome to QSEP Project for %s!\r\n"

#define SUB_OPTIONS              "\r\n> Select from the options in the menu below:\r\n" \
                                 "\r\nMENU"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

typedef struct menu_fn_tbl
{
    char_t * p_name;                   /*<! Name of Test */
    test_fn (* p_func)(void);          /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;

extern bool_t  is_ethernet_mode(void);
extern bool_t  erase_stored_keys(void);
extern test_fn camview_display_menu(bool_t first_call);

/* Table of menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Kit Information",
     kis_display_menu},
    {"Interactive AI, Connectivity & HMI Demos",
     camview_display_menu},
    {"Next Steps",
     ns_display_menu},
    {"",
     NULL}
};

#define ETH_TIMEOUT    (8000)

int8_t g_selected_menu = 0;

static char_t   s_print_buffer[BUFFER_LINE_LENGTH] = {};
static bool_t   s_bool_screen_test                 = true;
static uint32_t timeout = ETH_TIMEOUT;

/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : .
 * Return Value : The main menu controller.
 *********************************************************************************************************************/
int8_t main_display_menu (void)
{
    int8_t c          = -1;
    int8_t menu_limit = 0;

#if 0

    /* Start AI Demo */
    xEventGroupSetBits(g_update_console_event, STATUS_START_ENABLE_AI_DEMO);
#endif

    if (s_bool_screen_test == true)
    {
        /*         automatically run graphics */
        sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

        g_selected_menu = 2;

        camview_display_menu(s_bool_screen_test);

        s_bool_screen_test = false;

        /* When this workflow has run drop to end of function to froce menu redraw */
    }

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void *) s_print_buffer);
    sprintf(s_print_buffer, MODULE_NAME, FULL_NAME);
    print_to_console((void *) s_print_buffer);

    print_to_console((void *) "\r\nImportant:");
    print_to_console((void *) "\r\n 1. Set Configuration Switches (SW1) 3 and 7 to ON and all others to OFF");
    print_to_console((void *) "\r\n    prior to running AI applications (Face Detection and Image Classification)");
    print_to_console((void *) "\r\n 2. Set Configuration Switches (SW1) 5 and 7 to ON and all others to OFF");
    print_to_console((void *) "\r\n    prior to running connectivity applications (Currency Converter, World Time");
    print_to_console((void *) "\r\n    and World Weather).\r\n");

// print_to_console((void*)" (Currency Converter, World Time, and World Weather).");
// print_to_console((void*)"\r\n");

#if 0
    sprintf(s_print_buffer, "%s\r\n", FSP_VERSION_BUILD_STRING);
    print_to_console((void *) s_print_buffer);

    sprintf(s_print_buffer, "%s\r\n", __TIMESTAMP__);
    print_to_console((void *) s_print_buffer);

    sprintf(s_print_buffer, "Version 0.61 - AI Demo, MIPI Display using double buffered background Frame \r\n");
    print_to_console((void *) s_print_buffer);
#endif

    sprintf(s_print_buffer, SUB_OPTIONS);

    /*  */
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void *) s_print_buffer);

    int8_t test_active = 0;
    int8_t limit       = 2;            // append next steps dynamically

    for ( ; test_active < limit; test_active++)
    {
        sprintf(s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[menu_limit++].p_name);
        print_to_console((void *) s_print_buffer);
    }

    if ((true == is_ethernet_mode()) && (false == is_camera_mode()))
    {
        test_active++;
        menu_limit++;
        sprintf(s_print_buffer, "\r\n %d. %s", (test_active++), "Currency Converter Refresh ");
        print_to_console((void *) s_print_buffer);

        menu_limit++;
        sprintf(s_print_buffer, "\r\n %d. %s", (test_active++), "World Time Zones Refresh ");
        print_to_console((void *) s_print_buffer);

        menu_limit++;
        sprintf(s_print_buffer, "\r\n %d. %s", (test_active++), "World Weather Refresh ");
        print_to_console((void *) s_print_buffer);

        menu_limit++;
        sprintf(s_print_buffer, "\r\n %d. %s", (test_active), "API Keys Help & Management");
        print_to_console((void *) s_print_buffer);
    }

    menu_limit++;
    test_active++;
    sprintf(s_print_buffer, "\r\n %d. %s", (test_active), s_menu_items[2].p_name);
    print_to_console((void *) s_print_buffer);

    print_to_console("\r\n");

    while ((0 != c))
    {
        c = input_from_console();
        if (0 != c)
        {
            /* Cast, as compiler will assume calc is int */
            c               = (int8_t) (c - '0');
            g_selected_menu = c;

            bool_t esc_sequence = false;
            if ((c > 0) && (c <= menu_limit))
            {
                if (c == 2)
                {
                    camview_display_menu(s_bool_screen_test);
                }
                else
                {
                    if (true == is_ethernet_mode())
                    {
                        if (3 == c)
                        {
                            sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
                            print_to_console((void *) s_print_buffer);

                            sprintf(s_print_buffer, " %d. %s", 3, "CURRENCY CONVERTER REFRESH\r\n\r\n");
                            print_to_console(s_print_buffer);

                            print_to_console(
                                "While refreshing the Currency Converter application for the first time, QSEP");
                            print_to_console(
                                "\r\nwill prompt the user to enter the API key. After the API key is provided by");
                            print_to_console(
                                "\r\nthe user, it is stored in the Data Flash of the RA8D1 MCU. For subsequent");
                            print_to_console(
                                "\r\nrefreshes, the QSEP will use the stored API key to get the current data from");
                            print_to_console("\r\napp.currencyapi.com\r\n");

                            print_to_console(
                                "\r\nEnsure the Ethernet cable is securely connected and Internet access (without");
                            print_to_console("\r\nfirewalls, etc.) is available.\r\n");

                            print_to_console(
                                (void *) "\r\n> Press any key to continue refreshing. Or press space bar to return to MENU.\r\n");
                            esc_sequence = false;

                            while (1)
                            {
                                c = input_from_console();
                                if (c == MENU_EXIT_CRTL) // MENU_EXIT_CRTL
                                {
                                    esc_sequence = true;
                                }

                                break;
                            }

                            if (esc_sequence == false)
                            {
                                /* User has pressed signal Ethernet to start Can be set many times but should only be needed once */
                                xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);

                                print_to_console(
                                    "\r\nData refresh in progress. This may take about 2 minutes. Please be patient.\r\n");
                                vTaskDelay(500);
                                xEventGroupSetBits(g_update_console_event, STATUS_IOT_REQUEST_CURRENCY);

                                timeout = ETH_TIMEOUT;

                                EventBits_t uxBits;
                                while (timeout)
                                {
                                    uxBits = xEventGroupWaitBits(g_update_console_event,
                                                                 STATUS_IOT_RESPONSE_COMPLETE,
                                                                 pdFALSE,
                                                                 pdTRUE,
                                                                 1);
                                    vTaskDelay(10);
                                    timeout--;

                                    if ((uxBits & (STATUS_IOT_RESPONSE_COMPLETE)) == (STATUS_IOT_RESPONSE_COMPLETE))
                                    {
                                        xEventGroupClearBits(g_update_console_event, STATUS_IOT_RESPONSE_COMPLETE);

                                        break;
                                    }
                                }

                                if (timeout == 0)
                                {
                                    esc_sequence = false;

                                    /* Let interested parties know timout has occurred */
                                    eth_escape();

                                    print_to_console(
                                        "\r\nCannot connect to the Internet. Check the following before trying again:\r\n");
                                    print_to_console("\r\n1. Ethernet cable is securely connected\r\n");
                                    print_to_console("\r\n2. Internet connectivity is present\r\n");
                                    print_to_console("\r\n3. Firewall settings are not blocking Internet access\r\n");

                                    print_to_console("\r\nPress space bar to return to MENU.\r\n");
                                }
                            }

                            if (did_data_abort_from_eth() == false)
                            {
                                if (esc_sequence == false)
                                {
                                    while (CONNECTION_ABORT_CRTL != c)
                                    {
                                        c = input_from_console();
                                        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
                                        {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        else if (4 == c)
                        {
                            sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
                            print_to_console((void *) s_print_buffer);

                            sprintf(s_print_buffer, " %d. %s", 4, "WORLD TIME ZONES REFRESH\r\n\r\n");
                            print_to_console(s_print_buffer);

                            print_to_console("While refreshing the World Time Zone application for the first time, QSEP");
                            print_to_console(
                                "\r\nwill prompt the user to enter the API key. After the API key is provided by");
                            print_to_console(
                                "\r\nthe user, it is stored in the Data Flash of the RA8D1 MCU. For subsequent");
                            print_to_console(
                                "\r\nrefreshes, the QSEP will use the stored API key to get the current data from");
                            print_to_console("\r\nwww.weatherapi.com\r\n");

                            print_to_console(
                                "\r\nEnsure the Ethernet cable is securely connected and Internet access (without");
                            print_to_console("\r\nfirewalls, etc.) is available.\r\n");

                            print_to_console(
                                (void *) "\r\n> Press any key to continue refreshing. Or press space bar to return to MENU.\r\n");
                            esc_sequence = false;

                            while (1)
                            {
                                c = input_from_console();
                                if (c == MENU_EXIT_CRTL) // MENU_EXIT_CRTL
                                {
                                    esc_sequence = true;
                                }

                                break;
                            }

                            if (esc_sequence == false)
                            {
                                /* User has pressed signal Ethernet to start Can be set many times but should only be needed once */
                                xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);

                                print_to_console(
                                    "\r\nData refresh in progress. This may take about 2 minutes. Please be patient.\r\n");

                                vTaskDelay(500);
                                xEventGroupSetBits(g_update_console_event, STATUS_IOT_REQUEST_TIME);

                                timeout = ETH_TIMEOUT;

                                EventBits_t uxBits;
                                while (timeout)
                                {
                                    uxBits = xEventGroupWaitBits(g_update_console_event,
                                                                 STATUS_IOT_RESPONSE_COMPLETE,
                                                                 pdFALSE,
                                                                 pdTRUE,
                                                                 1);
                                    vTaskDelay(10);
                                    timeout--;

                                    if ((uxBits & (STATUS_IOT_RESPONSE_COMPLETE)) == (STATUS_IOT_RESPONSE_COMPLETE))
                                    {
                                        xEventGroupClearBits(g_update_console_event, STATUS_IOT_RESPONSE_COMPLETE);

                                        break;
                                    }
                                }

                                if (timeout == 0)
                                {
                                    esc_sequence = false;

                                    /* Let interested parties know timout has occurred */
                                    eth_escape();

                                    print_to_console(
                                        "\r\nCannot connect to the Internet. Check the following before trying again:\r\n");
                                    print_to_console("\r\n1. Ethernet cable is securely connected\r\n");
                                    print_to_console("\r\n2. Internet connectivity is present\r\n");
                                    print_to_console("\r\n3. Firewall settings are not blocking Internet access\r\n");

                                    print_to_console("\r\nPress space bar to return to MENU.\r\n");
                                }
                            }

                            if (did_data_abort_from_eth() == false)
                            {
                                if (esc_sequence == false)
                                {
                                    while (CONNECTION_ABORT_CRTL != c)
                                    {
                                        c = input_from_console();
                                        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
                                        {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        else if (5 == c)
                        {
                            sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
                            print_to_console((void *) s_print_buffer);

                            sprintf(s_print_buffer, " %d. %s", 5, "WORLD WEATHER REFRESH\r\n\r\n");
                            print_to_console(s_print_buffer);

                            print_to_console("While refreshing the World Weather application for the first time, QSEP");
                            print_to_console(
                                "\r\nwill prompt the user to enter the API key. After the API key is provided by");
                            print_to_console(
                                "\r\nthe user, it is stored in the Data Flash of the RA8D1 MCU. For subsequent");
                            print_to_console(
                                "\r\nrefreshes, the QSEP will use the stored API key to get the current data from");
                            print_to_console("\r\nwww.weatherapi.com\r\n");

                            print_to_console(
                                "\r\nEnsure the Ethernet cable is securely connected and Internet access (without");
                            print_to_console("\r\nfirewalls, etc.) is available.\r\n");

                            print_to_console(
                                (void *) "\r\n> Press any key to continue refreshing. Or press space bar to return to MENU.\r\n");
                            esc_sequence = false;

                            while (1)
                            {
                                c = input_from_console();
                                if (c == MENU_EXIT_CRTL) // MENU_EXIT_CRTL
                                {
                                    esc_sequence = true;
                                }

                                break;
                            }

                            if (esc_sequence == false)
                            {
                                /* User has pressed signal Ethernet to start Can be set many times but should only be needed once */
                                xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);

                                print_to_console(
                                    "\r\nData refresh in progress. This may take about 2 minutes. Please be patient.\r\n");

                                vTaskDelay(500);
                                xEventGroupSetBits(g_update_console_event, STATUS_IOT_REQUEST_WEATHER);
                                EventBits_t uxBits;

                                timeout = ETH_TIMEOUT;

                                while (timeout)
                                {
                                    uxBits = xEventGroupWaitBits(g_update_console_event,
                                                                 STATUS_IOT_RESPONSE_COMPLETE,
                                                                 pdFALSE,
                                                                 pdTRUE,
                                                                 1);
                                    vTaskDelay(10);
                                    timeout--;

                                    if ((uxBits & (STATUS_IOT_RESPONSE_COMPLETE)) == (STATUS_IOT_RESPONSE_COMPLETE))
                                    {
                                        xEventGroupClearBits(g_update_console_event, STATUS_IOT_RESPONSE_COMPLETE);

                                        break;
                                    }
                                }

                                if (timeout == 0)
                                {
                                    esc_sequence = false;

                                    /* Let interested parties know timout has occurred */
                                    eth_escape();

                                    print_to_console(
                                        "\r\nCannot connect to the Internet. Check the following before trying again:\r\n");
                                    print_to_console("\r\n1. Ethernet cable is securely connected\r\n");
                                    print_to_console("\r\n2. Internet connectivity is present\r\n");
                                    print_to_console("\r\n3. Firewall settings are not blocking Internet access\r\n");

                                    print_to_console("\r\nPress space bar to return to MENU.\r\n");
                                }
                            }

                            if (did_data_abort_from_eth() == false)
                            {
                                if (esc_sequence == false)
                                {
                                    while (CONNECTION_ABORT_CRTL != c)
                                    {
                                        c = input_from_console();
                                        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
                                        {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        else if (6 == c)
                        {
                            bool_t  res = true;
                            uint8_t c   = (uint8_t) -1;

                            sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
                            print_to_console((void *) s_print_buffer);

                            sprintf(s_print_buffer, " %d. %s", 6, "API KEYS HELP & MANAGEMENT\r\n\r\n");
                            print_to_console(s_print_buffer);

                            print_to_console("API keys are required by the connectivity applications to obtain near\r\n");
                            print_to_console(
                                "real-time data from third-party services. The users are required to sign\r\n");
                            print_to_console(
                                "up/create an account with the service providers to get the API keys.\r\n\r\n");

                            print_to_console(
                                " 1. World Weather and World Time Zones applications require an API key\r\n    from weatherapi.com\r\n");
                            print_to_console(
                                " 2. Currency Converter application requires an API key\r\n    from app.currencyapi.com\r\n");

                            print_to_console(
                                "\r\nWhile refreshing the connectivity applications for the first time,\r\n");
                            print_to_console(
                                "this QSEP will prompt the user to enter the API keys. After the API keys are\r\n");
                            print_to_console(
                                "provided by the user, they are stored in the Data Flash of the RA8D1 MCU.\r\n");
                            print_to_console(
                                "Upon subsequent refreshes, the QSEP will look for the API keys stored in the MCU\r\n");
                            print_to_console(
                                "and,if found, will use them to get the current data from third parties.\r\n");

                            print_to_console(
                                "\r\n> Press 'E' to erase the existing API keys (if present)\r\n  or press space bar to return to MENU.\r\n\r\n");

                            start_key_check();

                            while (1)
                            {
                                if (true == key_pressed())
                                {
                                    c = get_detected_key();

                                    if (('E' == c) || ('e' == c))
                                    {
                                        res = true;
                                        break;
                                    }

                                    if (MENU_EXIT_CRTL == c) // SPACE Key
                                    {
                                        res = false;
                                        break;
                                    }

                                    start_key_check();
                                }
                            }

                            if (false == res)
                            {
                            }
                            else
                            {
                                erase_stored_keys();
                                print_to_console("Completed - Data erased.\r\n");

                                sprintf(s_print_buffer, MENU_RETURN_INFO);
                                print_to_console((void *) s_print_buffer);

                                while (CONNECTION_ABORT_CRTL != c)
                                {
                                    c = input_from_console();
                                    if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (c == menu_limit)
                            {
                                s_menu_items[2].p_func();
                            }
                            else
                            {
                                s_menu_items[c - 1].p_func();
                            }
                        }
                    }
                    else
                    {
                        if (c == menu_limit)
                        {
                            s_menu_items[2].p_func();
                        }
                        else
                        {
                            s_menu_items[c - 1].p_func();
                        }
                    }
                }

                break;
            }
        }
    }

    /* Cast, as compiler will assume calc is int */
    return (int8_t) (c - '0');
}

/**********************************************************************************************************************
 * End of function main_display_menu
 *********************************************************************************************************************/
