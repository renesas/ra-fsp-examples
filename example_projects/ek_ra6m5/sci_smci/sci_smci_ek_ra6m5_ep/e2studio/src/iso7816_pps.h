/*
 * iso7816_pps.h
 *
 *  Created on: Jan 17, 2022
 *      Author: a5123412
 */
/***********************************************************************************************************************

File Name    : iso7816_pps.c/h
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


#ifndef ISO7816_PPS_H_
#define ISO7816_PPS_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define PPSS                                    0   /* PPS initial character */
#define PPS0                                    1   /* PPS format character */
#define PPS1                                    2   /* PPS parameter character 1 */
#define PPS2                                    3   /* PPS parameter character 2 */
#define PPS3                                    4   /* PPS parameter character 3 */
#define PCK                                     5   /* PPS check */
#define PPS_OK                                  0 /* Negotiation OK */
#define PPS_HANDSAKE_ERROR                      -1 /* Agreement not reached */
#define PPS_COMMUNICATION_ERROR                 -2 /* Communication error */
#define PPS_MAX_LENGTH                          6   /* Max length of the PPS frame */
#define PPS_HAS_PPS1(pps_buffer)                ((pps_buffer[1] & 0x10) == 0x10)
#define PPS_HAS_PPS2(pps_buffer)                ((pps_buffer[1] & 0x20) == 0x20)
#define PPS_HAS_PPS3(pps_buffer)                ((pps_buffer[1] & 0x40) == 0x40)

/***********************************************************************************************************************
 * Function Prototypes
 **********************************************************************************************************************/
fsp_err_t PPS_Exchange(uint8_t * request, uint8_t * resp_length, uint8_t* pps1);
bool PPS_receive_character(uint8_t received_data);
uint8_t PPS_Match (uint8_t * request, uint8_t len_request, uint8_t * response, uint8_t len_response);
uint8_t PPS_GetLength (uint8_t* pps_buffer);
uint8_t PPS_GetPCK (uint8_t* pps_buffer, uint8_t length);
#endif /* ISO7816_PPS_H_ */
