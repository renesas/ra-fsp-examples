/***********************************************************************************************************************
 * File Name    : periodic_signal_generator_example_app.h
 * Description  : Public interface for the DAC AGT Periodic Signal Generator example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PERIODIC_SIGNAL_GENERATOR_EXAMPLE_APP_H_
#define PERIODIC_SIGNAL_GENERATOR_EXAMPLE_APP_H_

#include "periodic_signal_generator_ep.h"

#define EP_VERSION      "1.1"

#define MODULE_NAME     "dac_agt_periodic_signal_generator"

#if defined (BOARD_RA8T1_MCK)
#define DADR            "DADR1"
#else
#define DADR            "DADR0"
#endif /* BOARD_RA8T1_MCK */

#define BANNER_INFO     "\r\n*******************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module  *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                               *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                          *"\
                        "\r\n*******************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the configuration of the DAC, AGT, and DTC"\
                        "\r\nto generate a continuous sinusoidal signal. The user will define the samples"\
                        "\r\nper period and frequency in Hz of the desired output wave via the terminal."\
                        "\r\nThe samples per period are used to compute and store one period of the sinusoidal"\
                        "\r\nin a lookup table (LUT), and the frequency is used to set the AGT period to"\
                        "\r\nthe appropriate sampling rate. The AGT generates an interrupt that triggers"\
                        "\r\nthe DTC to transfer the next value of the LUT to the "DADR" register. The output"\
                        "\r\nsinusoidal wave will oscillate between the minimum and maximum DAC values at the"\
                        "\r\nuser-specified samples per period and frequency.\r\n\r\n"

#define EP_GUIDE1       "\r\nEnter the desired number of samples per period for the output sine wave."\
                        "\r\nValid range: %d to %d"\
                        "\r\nUser input: "

#define EP_GUIDE2       "\r\nEnter desired frequency for output sine wave, in Hz."\
                        "\r\nValid range: %u to %u Hz"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void dac_agt_periodic_signal_generator_ep_run(void);

#endif /* PERIODIC_SIGNAL_GENERATOR_EXAMPLE_APP_H_ */
