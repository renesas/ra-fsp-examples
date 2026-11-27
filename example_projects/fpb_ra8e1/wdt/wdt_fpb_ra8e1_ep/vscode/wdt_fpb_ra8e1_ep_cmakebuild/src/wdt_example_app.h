/***********************************************************************************************************************
 * File Name    : wdt_example_app.h
 * Description  : Contains data structures and functions used in wdt_example_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef WDT_EXAMPLE_APP_H_
#define WDT_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_wdt"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
						"\r\n*   Example Project Version "EP_VERSION"                                  *"\
						"\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
						"\r\n******************************************************************"\
						"\r\nRefer to readme.txt file for more details on Example Project and"\
						"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the typical use of the WDT HAL module APIs."\
                        "\r\nThe user can enter '1' through terminal to initialize the WDT and start the"\
                        "\r\nGPT timer. The WDT counter is refreshed periodically every 1 second when"\
                        "\r\nthe GPT timer expires. Refresh status will be printed every 3 seconds."\
                        "\r\nPress the push button "BUTTON" to stop the GPT timer which in turn stops"\
                        "\r\nrefreshing the WDT timer. After approximately 2 seconds, the WDT resets the"\
                        "\r\nMCU and turns the LED ON until the user provides the input once again.\r\n"

#define EP_MENU         "\r\nEnter '1' to enable WDT"\
		                "\r\nUser input: "

/* Number of counts for printing WDT refresh status */
#define WDT_REFRESH_COUNTER_VALUE       (3U)

/* The user command input value */
#define ENABLE_WDT                      ('1')

/* Macro for reset button */
#if defined (BOARD_RA2E2_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK)\
    || defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA2A2_EK)\
    || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define BUTTON                          "S2"
#elif defined (BOARD_RA4W1_EK) || defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK)\
    || defined (BOARD_RA4C1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
#define BUTTON                          "SW1"
#elif defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
#define BUTTON                          "on Pmod BTN"
#else
#define BUTTON                          "S1"
#endif

/* Single entry-point called in hal_entry.c */
void wdt_example_app_run(void);

#endif /* WDT_EXAMPLE_APP_H_ */
