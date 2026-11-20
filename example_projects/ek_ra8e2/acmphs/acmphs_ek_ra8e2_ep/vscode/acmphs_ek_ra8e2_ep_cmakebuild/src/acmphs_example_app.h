/***********************************************************************************************************************
 * File Name    : acmphs_example_app.h
 * Description  : Public interface for the ACMPHS example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ACMPHS_EXAMPLE_APP_H_
#define ACMPHS_EXAMPLE_APP_H_

#include "acmphs_ep.h"

#define EP_VERSION      "1.0"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module "MODULE_PADDING" *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define EP_INFO         "\r\nThe example project demonstrates the typical use of the ACMPHS HAL module APIs."\
                        "\r\nIn this project, the internal reference voltage (Vref) is used as reference"\
                        "\r\nvoltage source and AN000 is used as input voltage source for ACMPHS module,"\
                        "\r\nwhich is supplied by output of DAC0. Internal reference voltage value is set"\
                        "\r\nto 1.18V (equivalent to a digital value 1465). The user can enter DAC0 value"\
                        "\r\nwithin permitted range to provide an analog signal input for AN000. When input"\
                        "\r\nvoltage source is greater than reference voltage, the comparator output status"\
                        "\r\nis HIGH and on-board LED is turned ON. When input voltage source is less than"\
                        "\r\nreference voltage, output status is LOW and the LED is turned OFF.\r\n"
#elif defined (BOARD_RA2T1_FPB)
#define EP_INFO         "\r\nThe example project demonstrates the typical use of the ACMPHS HAL module APIs."\
                        "\r\nIn this project, the internal reference voltage (Vref) is used as reference"\
                        "\r\nvoltage source and AN000 is used as input voltage source for ACMPHS module,"\
                        "\r\nwhich is supplied by external DC power supply. Internal reference voltage value"\
                        "\r\nis set to 1.44V. The user can input DC power value within permitted range to"\
                        "\r\nprovide an analog signal input for AN000. When input voltage source is greater"\
                        "\r\nthan reference voltage, the comparator output status is HIGH and on-board LED is"\
                        "\r\nturned ON. When input voltage source is less than reference voltage, output status"\
                        "\r\nis LOW and the LED is turned OFF.\r\n"
#else
#define EP_INFO         "\r\nThe example project demonstrates the typical use of the ACMPHS HAL module APIs."\
                        "\r\nIn this project, DAC0 is used as reference voltage source and DAC1 is used as input"\
                        "\r\nvoltage source for ACMPHS module. DAC0 value is set to 2048 (i.e., 1.65V). The user"\
                        "\r\ncan enter DAC1 value within permitted range. When DAC1 input value is greater than"\
                        "\r\nset DAC0 reference voltage, the comparator output status is HIGH and on-board LED is"\
                        "\r\nturned ON. When DAC1 input is less than reference voltage, output status is LOW and"\
                        "\r\nthe LED is turned OFF.\r\n"
#endif /* Specific board */

/* Single entry-point called in hal_entry.c */
void acmphs_example_app_run(void);

#endif /* ACMPHS_EXAMPLE_APP_H_ */
