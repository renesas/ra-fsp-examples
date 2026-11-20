/***********************************************************************************************************************
 * File Name    : netx_crypto_ep.h
 * Description  : Contains macros used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_CRYPTO_EP_H_
#define NETX_CRYPTO_EP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "NetX_crypto"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                         *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of NetX Secure Crypto on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP supporting SCE/RSIP hardware acceleration"\
                        "\r\nusing AzureRTOS. The example project executes AES, ECC, and RSA algorithms (including"\
                        "\r\ndifferent chaining modes) in a multi-threaded environment using mutex to enable"\
                        "\r\naccess to SCE/RSIP. The user can enter any key and press Enter to start continuous"\
                        "\r\nconcurrent execution of the algorithms. The algorithms will continue running and"\
                        "\r\ndisplay execution status on the terminal until the board is reset.\r\n"

#define EP_GUIDE        "\r\nEnter any key and press Enter to start continuous execution."\
                        "\r\nReset the board to stop execution."\
                        "\r\nUser input: "

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macros for event flags used for thread synchronization */
#define ECC_EVENT_FLAG          ((ULONG)0x02)
#define RSA_EVENT_FLAG          ((ULONG)0x04)

/* Macro for TRNG data size */
#define AES_TRNG_DATA_SIZE      (128)

/* Macro for thread sleep duration in RTOS ticks */
#define THREAD_SLEEP_TIME       (5)

/* Macro for timeout value used when waiting on RTOS objects */
#define TX_WAIT_TIME            (0xFFFF)

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)     (app_term_print_data(TERM_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)      (app_term_print_data(TERM_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)         (app_term_print_data(TERM_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* Macros for the example output buffer sizes */
#define RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE        (512U)
#define RM_NETX_SECURE_CRYPTO_EXAMPLE_SHA256_HASH_SIZE_BYTES    (512U)
#define RM_NETX_SECURE_CRYPTO_EXAMPLE_SHA256_HASH_SIZE_LEN      (32U)

#endif /* NETX_CRYPTO_EP_H_ */
