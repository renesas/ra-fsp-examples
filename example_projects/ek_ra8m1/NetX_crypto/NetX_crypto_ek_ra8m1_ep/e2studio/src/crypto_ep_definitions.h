/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*
 * crypto_ep_definitions.h
 *
 *  Created on: May 7, 2021
 *      Author: xianghui.wang
 */

#ifndef CRYPTO_EP_DEFINITIONS_H_
#define CRYPTO_EP_DEFINITIONS_H_

#define RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE        (512U)
#define RM_NETX_SECURE_CRYPTO_EXAMPLE_SHA256_HASH_SIZE_BYTES    (512U)
#define RM_NETX_SECURE_CRYPTO_EXAMPLE_SHA256_HASH_SIZE_LEN      (32U)
/* Event flags macros */
#define AES_EVENT_FLAG  ((ULONG) 0x01)
#define ECC_EVENT_FLAG  ((ULONG) 0x02)
#define RSA_EVENT_FLAG  ((ULONG) 0x04)
/* TRNG data size */
#define AES_TRNG_DATA_SIZE  (128)

#define THREAD_SLEEP_TIME   (5)
#define TX_WAIT_TIME        (0xFFFF)

#define ZERO    (0)
#define ONE     (1)
#define TWO     (2)
#define THREE   (3)
#define FOUR    (4)
#define FIVE    (5)

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

#define EP_INFO     "\r\nThis project demonstrates the basic functionality of NetX secure crypto on Renesas\r\n"\
                    "RA MCUs based on Renesas FSP supporting SCE9/RSIP7 Hardware acceleration using AzureRTOS.\r\n"\
                    "The example project executes AES, ECC, RSA algorithms(including different chaining modes)\r\n"\
                    "in multi-threaded environment using mutex to enable access to SCE. The status of the \r\n"\
                    "execution will be displayed on Jlink RTT viewer.\r\n\n"\

/* User Menu Options Info. */
#define MENUOPTIONS "Enter any key to start the multi-threaded execution of the algorithms \r\n"\

#endif /* CRYPTO_EP_DEFINITIONS_H_ */
