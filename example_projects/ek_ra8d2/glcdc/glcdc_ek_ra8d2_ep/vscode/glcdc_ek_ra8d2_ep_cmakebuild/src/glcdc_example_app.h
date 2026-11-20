/***********************************************************************************************************************
 * File Name    : glcdc_example_app.h
 * Description  : Public interface for the GLCDC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GLCDC_EXAMPLE_APP_H_
#define GLCDC_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_glcdc"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module               *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of the GLCDC"\
                        "\r\ndevice driver on Renesas RA MCUs based on Renesas FSP. On successful"\
                        "\r\ninitialization of the GLCDC module, the EP will read the display resolution"\
                        "\r\nand frame buffer set in the GLCDC module and draw color bands to cover the"\
                        "\r\nentire screen of external LCD connected to RA MCU. The user can also view"\
                        "\r\nthe raw image in the e2studio IDE. Additionally, the EP information and"\
                        "\r\nany failure messages will be displayed on the terminal.\r\n"

/* Single entry-point called in hal_entry.c */
void glcdc_example_app_run(void);

#endif /* GLCDC_EXAMPLE_APP_H_ */
