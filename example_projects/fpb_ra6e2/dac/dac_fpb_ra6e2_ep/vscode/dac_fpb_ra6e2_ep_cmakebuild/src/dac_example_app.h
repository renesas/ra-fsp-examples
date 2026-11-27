/***********************************************************************************************************************
 * File Name    : dac_example_app.h
 * Description  : Public interface for the DAC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DAC_EXAMPLE_APP_H_
#define DAC_EXAMPLE_APP_H_

#include "dac_ep.h"

#define EP_VERSION  "1.1"

#define BANNER      "\r\n********************************************************************************"\
                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"                 *"\
                    "\r\n*   Example Project Version "EP_VERSION"                                                *"\
                    "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                    "\r\n********************************************************************************"\
                    "\r\nRefer to readme.txt file for more details on Example Project and"\
                    "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO     "\r\nThe project initializes and demonstrates DAC module on Renesas RA MCUs"\
                    "\r\nusing ADC. The user can enter DAC value within valid range (1-4095)"\
                    "\r\nand the converted output is given as input to ADC channel 0. ADC output"\
                    "\r\nvalue is printed on the terminal.\r\n"

/* Single entry-point called in hal_entry.c */
void dac_example_app_run(void);

#endif /* DAC_EXAMPLE_APP_H_ */
