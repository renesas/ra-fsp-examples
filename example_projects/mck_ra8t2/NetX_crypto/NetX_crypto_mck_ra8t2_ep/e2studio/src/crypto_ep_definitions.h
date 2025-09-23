/***********************************************************************************************************************
 * File Name    : crypto_ep_definitions.h
 * Description  : Contains macro used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
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

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

#define EP_INFO "\r\nThis project demonstrates the basic functionality of NetX Secure Crypto on Renesas RA MCUs\r\n"\
                "based on Renesas FSP supporting SCE7/SCE9/RSIP-E51A/RSIP-E11A/RSIP-E50D hardware acceleration\r\n"\
                "using AzureRTOS. The example project executes AES, ECC, and RSA algorithms (including different\r\n"\
                "chaining modes) in a multi-threaded environment using mutex to enable access to SCE/RSIP. The\r\n"\
                "user can enter any key to enable concurrent execution of the different algorithms and status of\r\n"\
                "the execution will be displayed on J-Link RTT Viewer.\r\n\n"

/* The user Menu Options info */
#define MENUOPTIONS "Enter any key to start the multi-threaded execution of the algorithms\r\n"

#endif /* CRYPTO_EP_DEFINITIONS_H_ */
