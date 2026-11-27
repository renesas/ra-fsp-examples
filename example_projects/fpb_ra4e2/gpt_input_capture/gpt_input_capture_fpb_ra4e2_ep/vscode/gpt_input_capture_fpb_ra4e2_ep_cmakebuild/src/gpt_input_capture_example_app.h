/***********************************************************************************************************************
 * File Name    : gpt_input_capture_example_app.h
 * Description  : Public interface for the GPT Input Capture example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPT_INPUT_CAPTURE_EXAMPLE_APP_H_
#define GPT_INPUT_CAPTURE_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_gpt"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the functionality of GPT input capture"\
                        "\r\nmodule. GPT0 is used to generate periodic pulses of 500 milliseconds duration"\
                        "\r\nand provided as input to GPT input capture (GPT1). GPT1 counts the event pulse"\
                        "\r\nreceived at its input. Based on the period and capture event, the time period"\
                        "\r\nof pulse is calculated and displayed on the terminal.\r\n"

/* Single entry-point called in hal_entry.c */
void gpt_input_capture_example_app_run(void);

#endif /* GPT_INPUT_CAPTURE_EXAMPLE_APP_H_ */
