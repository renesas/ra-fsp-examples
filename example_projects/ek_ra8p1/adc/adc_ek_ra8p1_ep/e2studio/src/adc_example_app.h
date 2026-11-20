/***********************************************************************************************************************
 * File Name    : adc_example_app.h
 * Description  : Public interface for the ADC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EXAMPLE_APP_H_
#define ADC_EXAMPLE_APP_H_

#include "adc_ep.h"

#define EP_VERSION      "1.1"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"               *"\
						"\r\n*   Example Project Version "EP_VERSION"                                  *"\
						"\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
						"\r\n******************************************************************"\
						"\r\nRefer to readme.txt file for more details on Example Project and"\
						"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe project initializes the ADC in "MODE_1" or "MODE_2""\
                        "\r\nbased on the user selection in RA Configurator. Once initialized, the user"\
						"\r\ncan start the ADC scan and also stop the scan (in "MODE_2")"\
						"\r\nusing the terminal by sending commands."\
						"\r\nResults are displayed on the terminal.\r\n"

#define EP_MENU         "\r\nMenu to Select\r\n"\
                        "Press 1 to Start ADC Scan\r\n"\
						"Press 2 to Stop ADC Scan (Only for "MODE_2")\r\n"\
						"User input:\r\n"

/* Single entry-point called in hal_entry.c */
void adc_example_app_run(void);

#endif /* ADC_EXAMPLE_APP_H_ */
