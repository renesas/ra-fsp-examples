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


typedef enum
{
	CREATE_AES_KEY_INJECT_PLAINTEXT	=		(1),
	ENCRYPT_USER_DATA   				= 		(2),
	DECRYPT_USER_DATA   				= 		(3),
	DESTROY_AES_KEY			  			= 		(4),
} tests_t;

/* Macros for menu options to be displayed */
#define MENUOPTION_REGION		  "\r\n Code Running in RA6M4 Non-secure Region\r\n"

#define LFS_INITIALIZATION_SUCCESS        "\r\n LFS initialization successful\r\n"
#define LFS_INITIALIZATION_FAILED         "\r\n LFS initialization failed\r\n"

#define BANNER_1                                "\r\n******************************************************************"
#define APPLICATION_NAME                        "\r\n Utilizing the AES service which resides in secure region of RA6M4"
#define BANNER_2                                "\r\n******************************************************************\r\n"

#define SYSTEM_CAME_OUT_OF_RESET                "\r\n system came out of reset, up running"
#define AES_KEY_CREATION_SUCCESSFUL             "\r\n Plaintext key injected as wrapped AES key successfully.\r\n"
#define AES_KEY_CREATION_FAILED                 "\r\n Plaintext key injected as wrapped AES key failed.\r\n"
#define ENCRYPTION_SUCCESSFUL                   "\r\n Encryption is successful. Encrypted data matches NIST cipher text.\r\n"
#define ENCRYPTION_FAILED                  		 "\r\n Encryption failed.\r\n"
#define DECRYPTION_SUCCESSFUL					"\r\n Decryption is successful. Decrypted data matches NIST plaintext message.\r\n"
#define DECRYPTION_FAILED						"\r\n Decryption failed.\r\n"
#define AES_KEY_DESTROYED						"\r\n AES key destroyed \r\n"
#define AES_KEY_DESTROY_FAILED					"\r\n Destroyed AES key failed \r\n"
#define NOT_SUPPORTED							"\r\n not supported! \r\n"
#define NEED_CLEANING                             "\r\nToo many keys, restart the program to clean the key storage\r\n"
#define EXAMPLE_AES_OPERATION_FAILED             "\r\n AES key injection operation failed\r\n"

#endif /* APP_DEFINITIONS_H_ */
