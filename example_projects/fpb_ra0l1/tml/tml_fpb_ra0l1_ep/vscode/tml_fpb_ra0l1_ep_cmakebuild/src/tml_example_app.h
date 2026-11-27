/***********************************************************************************************************************
 * File Name    : tml_example_app.h
 * Description  : Public interface for the TML example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TML_EXAMPLE_APP_H_
#define TML_EXAMPLE_APP_H_

#define EP_VERSION                  "1.1"

#define MODULE_NAME                 "r_tml"

#define BANNER_INFO                 "\r\n******************************************************************"\
                                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                                    "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                                    "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                    "\r\n******************************************************************"\
                                    "\r\nRefer to readme.txt file for more details on Example Project and"\
                                    "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                     "\r\nThis example project demonstrates the TML HAL module on Renesas RA MCUs."\
                                    "\r\nThe user can select different TML operating modes through the terminal,"\
                                    "\r\nincluding counter operation, capture operation, and counter operation in"\
                                    "\r\nlow power mode. In counter operation in normal mode, three TML instances"\
                                    "\r\nare configured as 8-bit, 16-bit, and 32-bit counters. The 16-bit capture"\
                                    "\r\nmode uses an ELC software event to capture the raw timer count value. In"\
                                    "\r\ncounter operation in low power mode, the 32-bit timer is configured as"\
                                    "\r\ncounter mode with input period time, and the RA board wakes up from LPM"\
                                    "\r\nto normal mode when the timer expires. EP information and error messages"\
                                    "\r\nare displayed on the terminal.\r\n"

#define EP_MENU                     "\r\nMain menu:"\
                                    "\r\n1. TML counter mode"\
                                    "\r\n2. TML capture mode"\
                                    "\r\n3. TML counter mode in LPM"\
                                    "\r\nUser input: "

#define SUB_MENU1                   "\r\nPlease enter time period values in seconds"\
                                    "\r\nValid range: 1 to 60"\
							        "\r\nUser input: "

#define SUB_MENU2                   "\r\nCapture menu:"\
		                            "\r\n1. Capture the raw counts value of 16-bit timer"\
		                            "\r\n2. Back to the main menu"\
                                    "\r\nUser input: "

/* Macros for name of TML modules in each mode */
#define NAME_8BIT_MODE              "8-bit counter timer"
#define NAME_16BIT_MODE             "16-bit counter timer"
#define NAME_32BIT_MODE             "32-bit counter timer"
#define NAME_16BIT_CAPTURE_MODE     "16-bit capture timer"

/* Macros for LPM modes name */
#define NAME_LPM_SLEEP_MODE         "\r\nMCU enters Sleep mode"
#define NAME_LPM_SW_STANDBY_MODE    "\r\nMCU enters SW Standby mode"

/* Single entry-point called in hal_entry.c */
void tml_example_app_run(void);

#endif /* TML_EXAMPLE_APP_H_ */
