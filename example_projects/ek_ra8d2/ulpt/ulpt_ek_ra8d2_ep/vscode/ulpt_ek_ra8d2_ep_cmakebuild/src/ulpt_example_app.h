/***********************************************************************************************************************
 * File Name    : ulpt_example_app.h
 * Description  : Public interface for the ULPT example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ULPT_EXAMPLE_APP_H_
#define ULPT_EXAMPLE_APP_H_

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_ulpt"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"\

#define EP_INFO         "\r\nThis example project demonstrates the functionality of ULPT in Periodic mode and"\
                        "\r\nOne-Shot mode. The user selects the period for both One-Shot and Periodic timers"\
                        "\r\nas 1 second, 2 seconds, or 3 seconds. After selecting the period, the ULPT1 starts"\
                        "\r\nin Periodic mode and toggles the on-board LED when the ULPT1 underflow occurs. The ULPT1"\
                        "\r\nwill stop after underflow three times, and the ULPT0 will start in One-Shot mode"\
                        "\r\nafter that. The MCU will be put into Deep SW Standby mode and wait for the ULPT0"\
                        "\r\nunderflow to reset internally to normal mode and restart the EP.\r\n"

#define EP_MENU         "\r\nSelect period values for both One-Shot and Periodic timers:"\
                        "\r\n1. Period value is 1 second"\
                        "\r\n2. Period value is 2 seconds"\
                        "\r\n3. Period value is 3 seconds"\
                        "\r\nUser input: "

#define BANNER_RESET    "\r\n******************************************************************"\
                        "\r\n* MCU has been reset from Deep SW Standby mode by internal reset *"\
                        "\r\n******************************************************************\r\n"\
                        "\r\nA sequence has been completed\r\n"

/* Single entry-point called in hal_entry.c */
void ulpt_example_app_run(void);

#endif /* ULPT_EXAMPLE_APP_H_ */
