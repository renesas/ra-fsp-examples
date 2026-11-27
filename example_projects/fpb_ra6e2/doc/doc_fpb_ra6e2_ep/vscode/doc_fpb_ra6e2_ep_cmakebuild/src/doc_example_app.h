/***********************************************************************************************************************
 * File Name    : doc_example_app.h
 * Description  : Public interface for the DOC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DOC_EXAMPLE_APP_H_
#define DOC_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_doc"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the typical use of the DOC HAL module APIs."\
                        "\r\nThe project performs the different event operations selected by the user in"\
                        "\r\nconfigurator properties. One of the following events can be selected: Compare Match,"\
                        "\r\nCompare Mismatch, Addition Overflow, or Subtraction Underflow. Upon success or"\
                        "\r\nfailure of operation, the result will be displayed on the terminal. LED will turn ON"\
                        "\r\nonly if the selected event operation is success and stays OFF if the operation fails.\r\n"

/* Single entry-point called in hal_entry.c */
void doc_example_app_run(void);

#endif /* DOC_EXAMPLE_APP_H_ */
