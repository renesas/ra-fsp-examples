/***********************************************************************************************************************
 * File Name    : crypto_ep_definitions.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

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
/* Enable this flag to execute threads in sequential */
#define EVENT_FLAG_ENABLE   (1)
#define EVENT_FLAG_DISABLE  (2)
#define THREAD_SLEEP_TIME   (5)
#define TX_WAIT_TIME        (0xFFFF)

#define ZERO    0
#define ONE     1
#define TWO     2
#define THREE   3
#define FOUR    4
#define FIVE    5

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

#define EP_INFO        "\r\nThis project demonstrates the basic functionality of Netx secure crypto on Renesas.\r\n"\
                        "RA MCUs based on Renesas FSP supporting SCE7 Hardware acceleration using AzureRTOS\r\n"\
                        "The example project executes AES, ECC, RSA algorithms(including different chaining modes)\r\n"\
                        "in multi threaded environment using round robin mechanism. The multi-threaded environment can\r\n"\
                        "be configured to execute in sequential/concurrent order. The status and other important information \r\n"\
                        "will be displayed on Jlink RTT viewer.\r\n\n"\

/* User Menu Options Info. */
#define MENUOPTIONS  "\r\nNetX Crypto Menu Options: \r\n"\
                     "1.Enter 1 to Enable the sequential execution of the algorithms \r\n"\
                     "2.Enter 2 to Enable the concurrent execution of the algorithms \r\n"\
                     "User Input: \r\n"\

#endif /* CRYPTO_EP_DEFINITIONS_H_ */
