/***********************************************************************************************************************
 * File Name    : usbx_pcdc_acm_example_app.h
 * Description  : Public interface for the USBX PCDC ACM example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_PCDC_ACM_EXAMPLE_APP_H_
#define USBX_PCDC_ACM_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USBX_PCDC_ACM"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                       *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of USBX Device CDC ACM"\
                        "\r\non Renesas RA MCUs based on Renesas FSP using Azure RTOS. The RA board acts as"\
                        "\r\na CDC device which will be enumerated as a COM port when connected to the host PC."\
                        "\r\nOnce the board is connected, the user can provide input from the USBX PCDC ACM terminal."\
                        "\r\nThe user input will be echoed on the USBX PCDC ACM terminal. The application status"\
                        "\r\nmessages will be displayed on the debug terminal.\r\n"

/* Single entry-point called in usbx_pcdc_acm_thread_entry.c */
void usbx_pcdc_acm_example_app_run(void);

/* Function declaration */
void usbx_pcdc_acm_uninit(void);

#endif /* USBX_PCDC_ACM_EXAMPLE_APP_H_ */
