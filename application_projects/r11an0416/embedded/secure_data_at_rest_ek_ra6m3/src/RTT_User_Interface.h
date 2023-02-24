/***********************************************************************************************************************
 * File Name    : RTT_User_Interface.h
 * Description  : Contains RTT Viewer input and output related macros and function definition
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

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
