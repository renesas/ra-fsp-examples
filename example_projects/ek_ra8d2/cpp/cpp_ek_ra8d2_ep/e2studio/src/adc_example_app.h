/***********************************************************************************************************************
 * File Name    : adc_example_app.h
 * Description  : Public interface for the ADC C++ example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EXAMPLE_APP_H_
#define ADC_EXAMPLE_APP_H_

#include "adc_ep.h"

#define EP_VERSION      "1.0"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for " MODULE_NAME " Module" MODULE_PADDING "      *"\
                        "\r\n*   Example Project Version " EP_VERSION "                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about " MODULE_NAME " driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates some features of C++ such as class, constructors,"\
                        "\r\ndestructor, member functions using ADC HAL module APIs provided by FSP."\
                        "\r\nThe ADC module is wrapped into a class. An ADC object of this class is instantiated"\
                        "\r\nand is used to call respective member functions. The project initializes the ADC in"\
                        "\r\n" MODE_1 " or " MODE_2 " based on user selection in RA Configurator."\
                        "\r\nOnce initialized, the user can initiate the ADC scan and also stop the scan (in the"\
                        "\r\ncase of " MODE_2 ") using the terminal by sending commands. Result and ADC"\
                        "\r\nstatus are displayed on the terminal."

#define EP_MENU         "\r\n\r\nMenu to select"\
                        "\r\nPress 1 to start ADC Scan"\
                        "\r\nPress 2 to stop ADC Scan (Only for " MODE_2 ")"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.cpp */
void adc_example_app_run(void);

#endif /* ADC_EXAMPLE_APP_H_ */
