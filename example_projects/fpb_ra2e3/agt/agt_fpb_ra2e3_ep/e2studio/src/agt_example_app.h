/***********************************************************************************************************************
 * File Name    : agt_example_app.h
 * Description  : Public interface for the AGT example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AGT_EXAMPLE_APP_H_
#define AGT_EXAMPLE_APP_H_

#define EP_VERSION          ("1.1")

#define MODULE_NAME         "r_agt"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                            "\r\n*   Example Project Version %s                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThis example project demonstrates the functionality of AGT in Periodic mode and"\
                            "\r\nOne-Shot mode. On providing any input on the terminal, AGT channel 0 starts in"\
						    "\r\nOne-Shot mode. AGT channel 1 starts in Periodic mode when AGT channel 0 expires."\
						    "\r\nTimer in Periodic mode expires periodically at a time period specified by the user"\
						    "\r\nand toggles the on-board LED.\r\n"

#define EP_GUIDE            "\r\nPlease enter time period values for One-Shot and Periodic mode timers in milliseconds"\
                            "\r\nValid range: 1 to 2000\r\n"\
                            "\r\nOne-Shot mode: "

/* Single entry-point called in hal_entry.c */
void agt_example_app_run(void);

#endif /* AGT_EXAMPLE_APP_H_ */
