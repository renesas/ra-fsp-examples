/***********************************************************************************************************************
 * File Name    : iwdt_example_app.h
 * Description  : Public interface for the IWDT example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IWDT_EXAMPLE_APP_H_
#define IWDT_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_iwdt"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n\r\n"

#define EP_INFO         "This example project demonstrates the typical use of the IWDT HAL module.\r\n"\
                        "In the terminal, the user can press '1' to initialize the IWDT and start\r\n"\
                        "the "USED_TIMER" timer. The IWDT counter is refreshed periodically every 1 second\r\n"\
                        "when the "USED_TIMER" timer expires. Refresh status will be printed every 2 seconds.\r\n"\
                        "Once the user presses '2', the IWDT counter stops refreshing and resets the MCU.\r\n"

#define MENU            "\r\nPress '1' to enable IWDT"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void iwdt_example_app_run(void);

#endif /* IWDT_EXAMPLE_APP_H_ */
