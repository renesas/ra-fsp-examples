/***********************************************************************************************************************
 * File Name    : clock_output_example_app.h
 * Description  : Public interface for the Clock Output example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CLOCK_OUTPUT_EXAMPLE_APP_H_
#define CLOCK_OUTPUT_EXAMPLE_APP_H_

#include "hal_data.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "Clock Output"

#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                *"\
                        "\r\n*********************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#if !(defined (BOARD_RA6E2_FPB))
#define EP_INFO         "\r\nThis example project demonstrates the typical use of the Clock Output."\
                        "\r\nThe user inputs a key from the terminal application to start the clock output"\
                        "\r\nsource to the clock output pin. Enter number 3 in the terminal application"\
                        "\r\nto stop the clock output from the clock output pin of the MCU. The EP information"\
                        "\r\nor any error messages will be displayed on the terminal application.\r\n"

#define MENU            "\r\nEnter 1 to start Main clock output"\
                        "\r\nEnter 2 to start Sub-clock output"\
                        "\r\nEnter 3 to stop clock output"\
                        "\r\nUser input: "
#else
#define EP_INFO         "\r\nThis example project demonstrates the typical use of the Clock Output."\
                        "\r\nThe user inputs a key from the terminal application to start the clock output"\
                        "\r\nsource to the clock output pin. Enter number 2 in the terminal application"\
                        "\r\nto stop the clock output from the clock output pin of the MCU. The EP information"\
                        "\r\nor any error messages will be displayed on the terminal application.\r\n"

#define MENU            "\r\nEnter 1 to start Sub-clock output"\
                        "\r\nEnter 2 to stop clock output"\
                        "\r\nUser input: "
#endif /* BOARD_RA6E2_FPB */

/* Macro for the error LED */
#define ERR_LED         (0U)

/* Macros commonly used */
#define RESET_VALUE     (0x00)
#define NULL_CHAR       ('\0')
#define BUFFER_SIZE     (16U)

/* Extern variable */
extern bsp_leds_t g_bsp_leds;

/* Single entry-point called in hal_entry.c */
void clock_output_example_app_run(void);

#endif /* CLOCK_OUTPUT_EXAMPLE_APP_H_ */
