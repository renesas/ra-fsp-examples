/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
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
#define MENUOPTION1                 "Press 1 to exercise the cryptographic operation with directly injected AES key\r\n"
#define MENUOPTION2                 "Press 2 to create updated new AES key using FSP\r\n"
#define MENUOPTION3                 "Press 3 to exercise the cryptographic operation with newly updated AES key\r\n"
#define MENUOPTION4                 "Press 4 to exercise the cryptographic operation with directly injected ECC public key\r\n"
#define MENUOPTION5                 "Press 5 to create updated new ECC public key using FSP\r\n"
#define MENUOPTION6                 "Press 6 to exercise the cryptographic operation with newly updated ECC public key\r\n"

#define DECRYPT_WITH_INJECTED_AES_KEY             (1)
#define UPDATE_AES_KEY                              (2)
#define DECRYPT_WITH_UPDATED_AES_KEY                (3)
#define OPERATE_DIRECTLY_INJECTED_ECC_PUB_KEY       (4)
#define UPDATE_ECC_PUB_KEY                          (5)
#define OPERATE_UPDATED_ECC_PUB_KEY                 (6)


#define MENUEVENT1       " Cryptographic operation with directly injected AES key, which is injected via the serial interface.\r\n"
#define MENUEVENT2       " Update the new AES key encrypted with key update key and store the new wrapped key in data flash.\r\n"
#define MENUEVENT3       " Cryptographic operation with new wrapped AES key stored in data flash\r\n\r\n"
#define MENUEVENT4       " Cryptographic operation with directly injected ECC public key, which is injected via the serial interface.\r\n"
#define MENUEVENT5       " Update the new ECC public key encrypted with key update key and store the new wrapped key in data flash.\r\n"
#define MENUEVENT6       " Cryptographic operation with new wrapped ECC public key stored in data flash\r\n\r\n"

#define RESULT1_SUCCESS " \r\nResult: Cryptographic operation is successful with directly injected AES key\r\n\r\n"
#define RESULT2_SUCCESS " \r\nResult: AES Key is updated and stored to Data Flash\r\n\r\n"
#define RESULT3_SUCCESS " \r\nResult: Cryptographic operation is successful with Updated AES Key\r\n\r\n"
#define RESULT4_SUCCESS " \r\nResult: Cryptographic operation is successful with directly injected ECC public key\r\n\r\n"
#define RESULT5_SUCCESS " \r\nResult: ECC public Key is updated and stored to Data Flash\r\n\r\n"
#define RESULT6_SUCCESS " \r\nResult: Cryptographic operation is successful with Updated ECC public Key\r\n\r\n"

#define RESULT1_FAILURE " \r\nResult: Cryptographic operation failed with directly injected AES key\r\n\r\n"
#define RESULT2_FAILURE " \r\nResult: AES Key Update Failed\r\n\r\n"
#define RESULT3_FAILURE " \r\nResult: Cryptographic operation failed with Updated AES Key\r\n\r\n"
#define RESULT4_FAILURE " \r\nResult: Cryptographic operation failed with directly injected ECC public key\r\n\r\n"
#define RESULT5_FAILURE " \r\nResult: ECC public Key Update Failed\r\n\r\n"
#define RESULT6_FAILURE " \r\nResult: Cryptographic operation failed with Updated ECC public Key\r\n\r\n"
#define RESULT7_FAILURE " \r\n Unsupported Command\r\n"

#endif /* APP_DEFINITIONS_H_ */
