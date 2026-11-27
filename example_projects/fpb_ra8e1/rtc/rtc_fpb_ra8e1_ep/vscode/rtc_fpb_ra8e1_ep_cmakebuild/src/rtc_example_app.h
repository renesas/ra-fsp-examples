/***********************************************************************************************************************
 * File Name    : rtc_example_app.h
 * Description  : Public interface for the RTC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef RTC_EXAMPLE_APP_H_
#define RTC_EXAMPLE_APP_H_

#include "rtc_ep.h"

#define EP_VERSION      "1.1"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"     *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the typical use of the RTC HAL module APIs."\
                        "\r\nThe project initializes the RTC module and allows the user to set RTC calendar"\
                        "\r\ntime and calendar alarm. The user can enable periodic interrupt and can also view"\
                        "\r\nthe current RTC calendar time. On enabling periodic interrupt, on-board LED"\
                        "\r\ntoggles every 1 second and on occurrence of calendar alarm, LED is turned ON."

#define EP_MENU         "\r\n\r\nMain menu:"\
                        "\r\n1: Set RTC calendar date and time"\
                        "\r\n2: Set RTC calendar alarm date and time"\
                        "\r\n3: Set RTC periodic IRQ rate and start it"\
                        "\r\n4: Get current RTC date and time"\
                        "\r\nUser input: "

#define EP_GUIDE        "\r\n\r\nEnter any key (other than 1,2,3,4) to go back to the menu"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void rtc_example_app_run(void);

#endif /* RTC_EXAMPLE_APP_H_ */
