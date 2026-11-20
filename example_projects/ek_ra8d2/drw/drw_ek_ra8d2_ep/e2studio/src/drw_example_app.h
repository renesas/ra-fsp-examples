/***********************************************************************************************************************
 * File Name    : drw_example_app.h
 * Description  : Public interface for the DRW example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DRW_EXAMPLE_APP_H_
#define DRW_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_drw"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of the 2D Drawing Engine (DRW)"\
                        "\r\ndevice driver on Renesas RA MCUs based on Renesas FSP. On successful initialization of"\
                        "\r\nthe DRW and GLCDC modules, the EP draws and fills various shapes with animation and alpha"\
                        "\r\nblending effects on an external LCD connected to the RA MCU. Additionally, the EP"\
                        "\r\ninformation and any failure messages will be displayed on the terminal.\r\n"

/* Single entry-point called in hal_entry.c */
void drw_example_app_run(void);

#endif /* DRW_EXAMPLE_APP_H_ */
