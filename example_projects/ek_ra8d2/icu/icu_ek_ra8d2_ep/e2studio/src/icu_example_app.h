/***********************************************************************************************************************
 * File Name    : icu_example_app.h
 * Description  : Public interface for the ICU example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ICU_EXAMPLE_APP_H_
#define ICU_EXAMPLE_APP_H_

#define EP_VERSION          "1.0"

#define MODULE_NAME         "r_icu"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                            "\r\n*   Example Project Version %s                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe example project demonstrates the typical use of the ICU HAL module APIs."\
		                    "\r\nThe project initializes the ICU module. On pressing the user push button,"\
		                    "\r\nan external IRQ is triggered, which toggles on-board LED.\r\n"

/* Single entry-point called in hal_entry.c */
void icu_example_app_run(void);

#endif /* ICU_EXAMPLE_APP_H_ */
