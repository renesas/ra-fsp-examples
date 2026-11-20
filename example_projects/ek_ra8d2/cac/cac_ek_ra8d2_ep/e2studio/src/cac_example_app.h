/***********************************************************************************************************************
 * File Name    : cac_example_app.h
 * Description  : Public interface for the CAC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CAC_EXAMPLE_APP_H_
#define CAC_EXAMPLE_APP_H_

#include "cac_ep.h"

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_cac"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates basic functionalities of CAC driver."\
                        "\r\nOn any key press from the terminal, CAC module starts measuring the"\
                        "\r\naccuracy of selected target clock against the selected reference clock."\
                        "\r\nResult of the measurement is displayed on the terminal.\r\n"

#define EP_GUIDE        "\r\nEnter any key to perform CAC measurement operation\r\n"

/* Single entry-point called in hal_entry.c */
void cac_example_app_run(void);

#endif /* CAC_EXAMPLE_APP_H_ */
