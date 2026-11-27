/***********************************************************************************************************************
 * File Name    : lpm_example_app.h
 * Description  : Public interface for the LPM example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LPM_EXAMPLE_APP_H_
#define LPM_EXAMPLE_APP_H_

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_lpm"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                               *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to the README.md for the Example Project documentation and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver.\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of LPM driver on Renesas"\
                        "\r\nRA MCUs based on FSP. On successful initialization of necessary hardware modules,"\
                        "\r\nthe MCU will automatically enter each LPM mode and then wait for a wake-up source"\
                        "\r\ntrigger, either manually by an external interrupt or automatically by timer.\r\n"

#define BANNER_RESET    "\r\n********************************************************************************"\
                        "\r\n*   MCU has been reset from Deep SW Standby mode by internal reset             *"\
                        "\r\n********************************************************************************\r\n"

/* Single entry-point called in hal_entry.c */
void lpm_example_app_run(void);
void source_cancel_lpm_detect(void);

#endif /* LPM_EXAMPLE_APP_H_ */
