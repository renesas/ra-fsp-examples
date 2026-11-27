/***********************************************************************************************************************
 * File Name    : tau_example_app.h
 * Description  : Public interface for the TAU example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TAU_EXAMPLE_APP_H_
#define TAU_EXAMPLE_APP_H_

#define EP_VERSION          "1.0"

#define MODULE_NAME         "r_tau"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                            "\r\n*   Example Project Version %s                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe example project demonstrates the basic usage of TAU driver on"\
                            "\r\nRenesas RA MCUs based on Renesas FSP. It consists of 7 independent"\
                            "\r\noperations coordinated together to create 3 sequences. They include"\
                            "\r\nsetting the period for LED ON and OFF times, counting external events,"\
                            "\r\nand measuring low-level pulse width. The user will input the required"\
                            "\r\nparameters for each sequence to configure the corresponding independent"\
                            "\r\nchannel operation function of TAU. The EP information and error messages"\
                            "\r\nwill be printed to the terminal.\r\n"

/* Macro for the sequence1 information */
#define SEQUENCE1_INFO      "\r\n******************************************************************"\
                            "\r\n*                 Interval and delay timer modes.                *"\
                            "\r\n* This mode uses interval timer to set OFF time for LED1 and     *"\
                            "\r\n* the delay timer is used to set the ON time for LED1.           *"\
                            "\r\n******************************************************************\r\n"

/* Macro for the sequence2 information */
#define SEQUENCE2_INFO      "\r\n******************************************************************"\
                            "\r\n*                  External event counter mode.                  *"\
                            "\r\n* The user sets the frequency and divider for square wave.       *"\
                            "\r\n* The input capture will measure pulse width of one period and   *"\
                            "\r\n* turn ON LED2. The external event counter counts LED2 ON times  *"\
                            "\r\n* and turn OFF LED2 once it blinks 5 times.                      *"\
                            "\r\n******************************************************************\r\n"

/* Macro for the sequence3 information */
#define SEQUENCE3_INFO      "\r\n******************************************************************"\
                            "\r\n*            Low level pulse width measurement mode.             *"\
                            "\r\n* The user sets the period for square wave, and the low level    *"\
                            "\r\n* pulse width of square wave will be measured.                   *"\
                            "\r\n******************************************************************\r\n"

/* Macro for the measurable input signal information */
#define MEASURABLE_WARNING  "\r\nWarning:"\
                            "\r\nThe input signal cannot be measured because it is outside the measurable range,"\
                            "\r\nlimit values will be displayed instead of the expected value.\r\n\r\n"

/* Macro for the input square wave frequency requirement */
#define FREQUENCY_INPUT     "\r\nNote:"\
                            "\r\nPlease input a frequency that is a divisor of %dHz to generate"\
                            "\r\na square wave as expected.\r\n"

/* Single entry-point called in hal_entry.c */
void tau_example_app_run(void);

#endif /* TAU_EXAMPLE_APP_H_ */
