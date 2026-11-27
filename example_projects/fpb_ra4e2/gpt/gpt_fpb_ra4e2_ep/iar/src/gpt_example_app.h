/***********************************************************************************************************************
 * File Name    : gpt_example_app.h
 * Description  : Public interface for the GPT example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_EXAMPLE_APP_H_
#define GPT_EXAMPLE_APP_H_

#define EP_VERSION      ("1.1")

#define MODULE_NAME     "r_gpt"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe project initializes GPT module in Periodic, PWM or One-Shot mode based"\
                        "\r\non the user input from the displayed menu options. In Periodic mode, the"\
                        "\r\nuser can enter the time period within the permitted ranges to change the"\
                        "\r\nfrequency of the user LED. In PWM mode, the user can enter the duty cycle"\
                        "\r\nwithin the specified range to adjust the intensity of the user LED."\
                        "\r\nIn One-Shot mode, output will be displayed on the terminal when timer expires.\r\n"

#define EP_MENU         "\r\nMenu Options"\
                        "\r\n1. Enter 1 for Periodic mode"\
                        "\r\n2. Enter 2 for PWM mode"\
                        "\r\n3. Enter 3 for One-Shot mode"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void gpt_example_app_run(void);

#endif /* GPT_EXAMPLE_APP_H_ */
