/***********************************************************************************************************************
 * File Name    : mbed_crypto_example_app.h
 * Description  : Public interface for the Mbed Crypto example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MBED_CRYPTO_EXAMPLE_APP_H_
#define MBED_CRYPTO_EXAMPLE_APP_H_

#include "mbed_crypto_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "mbed_crypto"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module           *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#if defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)
#define EP_INFO         "\r\nThis example project demonstrates the functionality of Mbed Crypto module on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP. The EP performs AES256 crypto operation for GCM"\
                        "\r\nMode. Error and info messages will be printed on the terminal.\r\n"
#elif defined (BOARD_RA4L1_EK) || defined (BOARD_RA4C1_EK)
#define EP_INFO         "\r\nThis example project demonstrates the functionality of Mbed Crypto module on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP. The EP performs following crypto operations:"\
                        "\r\n1. AES256 for GCM Mode."\
                        "\r\n2. SHA256 hashing."\
                        "\r\n3. ECC - P256R1 operation."\
                        "\r\nError and info messages will be printed on the terminal.\r\n"
#else
#define EP_INFO         "\r\nThis example project demonstrates the functionality of Mbed Crypto module on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP. The EP performs following crypto operations:"\
                        "\r\n1. AES256 for GCM Mode."\
                        "\r\n2. SHA256 hashing."\
                        "\r\n3. ECC - P256R1 operation."\
                        "\r\n4. RSA 2048 operation."\
                        "\r\nError and info messages will be printed on the terminal.\r\n"
#endif /* Specific board */

/* Single entry-point called in hal_entry.c */
void mbed_crypto_example_app_run(void);

#endif /* MBED_CRYPTO_EXAMPLE_APP_H_ */
