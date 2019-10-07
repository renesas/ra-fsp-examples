/***********************************************************************************************************************
 * File Name    : RTT_User_Interface.h
 * Description  : Contains RTT Viewer input and output related macros and function definition
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
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
