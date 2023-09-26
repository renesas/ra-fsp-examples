/*
 * user_config.h
 *
 *  Created on: Jan 25, 2022
 *      Author: a5123412
 */
/***********************************************************************************************************************

File Name    : user_config.c/h
Description  : As required for the file such as Contains macros, data structures and functions used  common to the EP
/
/

DISCLAIMER
This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
applicable laws, including copyright laws.
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
this software. By using this software, you agree to the additional terms and conditions found by accessing the
following link:
http://www.renesas.com/disclaimer


Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/


#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

#define CARD_DETECTION_PIN_IS_USED             (1)

#define SMCI_VCC                               (BSP_IO_PORT_08_PIN_04)
#define SMCI_RST                               (BSP_IO_PORT_08_PIN_03)
#define SMCI_DETECT                            (BSP_IO_PORT_07_PIN_08)

#endif /* USER_CONFIG_H_ */
