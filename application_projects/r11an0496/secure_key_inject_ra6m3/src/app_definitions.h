/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))


#define CRYPTO_OPERATION_WITH_WRAPPED_KEY_SUCCESS " \r\nResult: Cryptographic operation is successful with wrapped AES 128 key\r\n"
#define CRYPTO_OPERATION_WITH_WRAPPED_KEY_FAILED " \r\nResult: Cryptographic operation failed with wrapped AES 128 key\r\n"
#define KEY_WRAP_SUCCESS  " \r\nResult: AES 128 Key Wrap is successful\r\n"
#define KEY_WRAP_FAILED  " \r\nResult: AES 128 Key Wrap failed\r\n"


#endif /* APP_DEFINITIONS_H_ */
