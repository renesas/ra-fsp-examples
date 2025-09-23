/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/
#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))


/* Macros for menu options to be displayed */

#define MENUOPTION_MENU             "\r\n MENU to Select \r\n"
#define MENUOPTION1                 "Press 1 to exercise the cryptographic operation with ECC key pair\r\n"
#define MENUOPTION2                 "Press 2 to create updated new ECC key pair using FSP\r\n"

#define OPERATE_ECC_KEY_PAIR         (1)
#define UPDATE_ECC_KEY_PAIR          (2)

#define MENUEVENT1       " Cryptographic operation with ECC key pair\r\n"
#define MENUEVENT2       " Update the new ECC key pair encrypted with key update key and store the new wrapped key pair in MRAM\r\n"

#define RESULT1_SUCCESS " \r\nResult: Cryptographic operation is successful with ECC key pair\r\n\r\n"
#define RESULT2_SUCCESS " \r\nResult: ECC Key Pair is updated successfully\r\n\r\n"

#define RESULT1_FAILURE " \r\nResult: Cryptographic operation failed with ECC key pair\r\n\r\n"
#define RESULT2_FAILURE " \r\nResult: ECC Key Pair Update Failed\r\n\r\n"
#define RESULT3_FAILURE " \r\n Unsupported Command\r\n"

#endif /* APP_DEFINITIONS_H_ */
