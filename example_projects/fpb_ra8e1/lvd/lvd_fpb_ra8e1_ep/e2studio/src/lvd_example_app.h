/***********************************************************************************************************************
 * File Name    : lvd_example_app.h
 * Description  : Public interface for the LVD example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LVD_EXAMPLE_APP_H_
#define LVD_EXAMPLE_APP_H_

#include "lvd_ep.h"

#define EP_VERSION        "1.0"

#define MODULE_NAME       "r_lvd"

#define BANNER_INFO       "\r\n******************************************************************"\
                          "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                          "\r\n*   Example Project Version %s                                  *"\
                          "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                          "\r\n******************************************************************"\
                          "\r\nRefer to readme.txt file for more details on Example Project and"\
                          "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO           "\r\nThe example project demonstrates the typical use of the LVD HAL module APIs."\
                          "\r\nThe example project enables LVD0 circuit to reset MCU and LVD1 to generate"\
                          "\r\nmaskable interrupt.\r\n"\
                          "\r\nLVD0 resets MCU only when input voltage drops below LVD0 threshold value, "\
                          "causing the MCU to power off."\
                          "\r\nTo detect this, the user has to increase the voltage value back to above LVD0 "\
                          "threshold value."\
                          "\r\nBy doing this, LED will turn ON indicating LVD0 reset detection."\
                          "\r\nEx: 2.8V is LVD0 threshold value. When input voltage is dropped from 2.9V to 2.7V,"\
                          "\r\nLVD0 resets MCU."\
                          "\r\nThe user has to raise input voltage above 2.8V again to detect LVD0 reset.\r\n"\
                          "\r\nLVD1 generates a maskable interrupt only when input voltage is raised above LVD1 "\
                          "threshold value."\
                          "\r\nEx: 2.9V is LVD1 threshold value. When input voltage is raised from 2.8V to 3.0V,"\
                          "\r\nLVD1 generates the maskable interrupt.\r\n"

#define EP_GUIDE          "\r\nNote:"\
                          "\r\n1. To run LVD example project, use external debugger for flashing the code,"\
                          "\r\ndisconnect debugging mode and run the project."\
                          "\r\n2. To detect LVD0 reset, input voltage has to be raised back above threshold"\
                          "\r\nvalue (Vdet0) once it is dropped below threshold value (Vdet0). This will turn"\
                          "\r\nON LED and print LVD0 reset detection message on the terminal. Vdet0 values are"\
                          "\r\ndocumented in readme.txt for specific board.\r\n"

/* Single entry-point called in hal_entry.c */
void lvd_example_app_run(void);

#endif /* LVD_EXAMPLE_APP_H_ */
