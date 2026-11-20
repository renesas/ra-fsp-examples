/***********************************************************************************************************************
 * File Name    : mipi_dsi_example_app.h
 * Description  : Public interface for the MIPI DSI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MIPI_DSI_EXAMPLE_APP_H_
#define MIPI_DSI_EXAMPLE_APP_H_

#define EP_VERSION      "1.1"

#define MODULE_NAME     "MIPI DSI"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                            *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of the MIPI DSI device"\
                        "\r\ndriver on Renesas RA MCUs based on Renesas FSP. On successful initialization of the"\
                        "\r\nMIPI DSI module, the EP will display 8-color bars on the MIPI LCD (external LCD"\
                        "\r\nconnected to RA MCU) using the graphics and MIPI-DSI modules in FSP. A user menu will"\
                        "\r\nbe provided over the terminal. The user can select the time to enter Ultra-Low-Power"\
                        "\r\nState (ULPS) and touch the screen to exit this mode. Status information and error"\
                        "\r\nmessages will be printed on the terminal during the execution of the project.\r\n"\
                        "\r\nNote: After entering Ultra Low Power State (ULPS), please touch the screen to exit"\
                        " this mode"\
                        "\r\n      or wait 30s after backlight turn off to automatically exit this mode."

#define EP_MENU         "\r\n\r\nSet time to enter Ultra-Low Power State (ULPS):"\
                        "\r\n1. Input '1' to enter ULPS after 5s."\
                        "\r\n2. Input '2' to enter ULPS after 15s."\
                        "\r\n3. Input '3' to enter ULPS after 30s."\
                        "\r\n4. Input '4' to always display (Default)."\
                        "\r\nUser input: "

#define EP_GUIDE        "\r\n\r\nPress 1, 2, 3, or 4 to change the display time, or press any other key to "\
                        "return to the menu."\
                        "\r\nUser input: "

#define MIPI_DSI_INFO_5S            "\r\nEnter ULPS after 5 seconds of display"
#define MIPI_DSI_INFO_15S           "\r\nEnter ULPS after 15 seconds of display"
#define MIPI_DSI_INFO_30S           "\r\nEnter ULPS after 30 seconds of display"
#define MIPI_DSI_INFO_DISABLE_ULPS  "\r\nAlways display"

/* Single entry-point called in hal_entry.c */
void mipi_dsi_example_app_run(void);

#endif /* MIPI_DSI_EXAMPLE_APP_H_ */
