/***********************************************************************************************************************
 * File Name    : baremetal_example_app.h
 * Description  : Public interface for the Baremetal example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef BAREMETAL_EXAMPLE_APP_H_
#define BAREMETAL_EXAMPLE_APP_H_

#include "baremetal_ep.h"

#define EP_VERSION          "1.0"

#define MODULE_NAME         "Baremetal"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" operation          *"\
							"\r\n*   Example Project Version "EP_VERSION"                                  *"\
							"\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
							"\r\n******************************************************************"\
							"\r\nRefer to readme.txt file for more details on Example Project\r\n"

#define EP_INFO             "\r\nThis project demonstrates the direct access, configuration, and operation"\
                            "\r\nof DOC SFRs. The SFRs and interrupts are configured directly to perform"\
							"\r\na comparison match. The ISR for the DOC is configured through the Interrupts tab"\
							"\r\nof the FSP Configurator.\r\n"

#define EP_GUIDE_2_LEDS     "\r\nLED1 is turned ON while program control waits for the user defined ISR to execute."\
                            "\r\nLED2 is turned ON to indicate the successful conclusion of application execution."\
                            "\r\nLED1 and LED2 are turned ON if there is an expected error encountered during "\
                            "operation.\r\n"

#define EP_GUIDE_3_LEDS     "\r\nLED1 is turned ON while program control waits for the user defined ISR to execute."\
                            "\r\nLED2 is turned ON to indicate the successful conclusion of application execution."\
							"\r\nLED3 is turned ON if there is an expected error encountered during operation.\r\n"

/* Single entry-point called in hal_entry.c */
void baremetal_example_app_run(void);

#endif /* BAREMETAL_EXAMPLE_APP_H_ */
