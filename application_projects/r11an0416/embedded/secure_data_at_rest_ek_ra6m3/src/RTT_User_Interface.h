/***********************************************************************************************************************
 * File Name    : RTT_User_Interface.h
 * Description  : Contains RTT Viewer input and output related macros and function definition
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef RTT_USER_INTERFACE_H_
#define RTT_USER_INTERFACE_H_

#define MAX_NO_TEST_MSG                     (10)

/* Switch Commands */
#define VIEW_SECURITY_SETTING                       '1'
#define SECURE_FLASH_OPERATION                      '2'
#define SECURE_SRAM_OPERATION                       '3'
#define NON_SECURE_FLASH_OPERATION                  '4'
#define SECURE_FLASH_READ			                'a'
#define SECURE_FLASH_WRITE			                'b'
#define SET_UP_FAW			                  		'c'
#define RESET_FAW									'd'
#define SECURE_SRAM_READ							'g'
#define SECURE_SRAM_WRITE							'h'
#define NON_SECURE_FLASH_READ						'm'
#define NON_SECURE_FLASH_WRITE						'n'
#define EXIT                                        '~'
#define BUFF_SIZE                       0x0d
#define BUFF_INDEX                      0x00

extern void start_RTT_user_interface();

#endif /* RTT_USER_INTERFACE_H_ */
