/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
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
#define MENUOPTION_REGION		  "\r\n Code Running in Non-secure Region\r\n"
#define MENUOPTION_MENU       "\r\n MENU to Select \r\n"
#define MENUOPTION1       "Press 1 to use NIST AES test key as plaintext key, wrap and inject this key\r\n"
#define MENUOPTION2       "Press 2 to use the injected key to encrypt the NIST plaintext message. Compare the encrypted data with NIST cipher text.\r\n"
#define MENUOPTION3       "Press 3 to use the injected key to decrypt the NIST cipher text. Compare the decrypted data with the NIST plaintext message.\r\n"
#define MENUOPTION4		  "Press 4 to destroy the wrapped AES key last created\r\n"


#define MENUEVENT1       	"perform plaintext NIST AES key wrap and inject.\r\n"
#define MENUEVENT2       	"encrypt NIST plaintext message using injected wrapped key.\r\n"
#define MENUEVENT3       	"decrypt NIST cipher data using injected wrapped key.\r\n"
#define MENUEVENT5			"destroy AES key\r\n"

#define BANNER_1                                "\r\n******************************************************************"
#define APPLICATION_NAME                        "\r\n Utilizing the AES service which resides in secure region "
#define BANNER_2                                "\r\n******************************************************************"

#define SYSTEM_CAME_OUT_OF_RESET                "\r\n system came out of reset, up running"
#define AES_KEY_CREATION_SUCCESSFUL             "\r\n plaintext key injected as wrapped AES key successfully.\r\n"
#define AES_KEY_CREATION_FAILED                 "\r\n plaintext key injected as wrapped AES key failed.\r\n"
#define ENCRYPTION_SUCCESSFUL                   "\r\n Encryption is successful.Encrypted data matches NIST cipher text.\r\n"
#define ENCRYPTION_FAILED                  		 "\r\n Encryption failed.\r\n"
#define DECRYPTION_SUCCESSFUL					"\r\n Decryption is successful.Decrypted data matches NIST plaintext message.\r\n"
#define DECRYPTION_FAILED						"\r\n Decryption failed.\r\n"
#define AES_KEY_DESTROYED						"\r\n AES key destroyed \r\n"
#define AES_KEY_DESTROY_FAILED					"\r\n destroyed AES key failed \r\n"
#define NOT_SUPPORTED							"\r\n not supported! \r\n"
#define NEED_CLEANING                             "\r\nToo many keys, restart the program to clean the key storage\r\n"

#endif /* APP_DEFINITIONS_H_ */
