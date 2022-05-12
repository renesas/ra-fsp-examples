/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : new_aes_key.c
 * Version      : 1.00
 * Device(s)    : RA,RE,RX,RZ,Synergy Family
 * Description  : sample program of Key data.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 31.12.2021 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "new_aes_key.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const encrypted_user_key_data_t g_encrypted_user_key_data =
{
    /* uint32_t keytype; */
    0x00000007,
    /* uint32_t shared_key_number; */
    0x00000000,
    /* uint8_t wufpk[32]; */
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    /* uint8_t initial_vector[16]; */
    {
        0x45, 0x2B, 0x59, 0x09, 0xC7, 0x12, 0xD9, 0xC4, 0x06, 0x86, 0x9B, 0x0A, 0x35, 0x4D, 0x85, 0x50
    },
    /* uint8_t encrypted_user_key[ENCRYPTED_KEY_BYTE_SIZE]; */
    {
        0xB0, 0xE9, 0x6C, 0x84, 0xDC, 0x7B, 0x6D, 0x04, 0x69, 0xDE, 0xF0, 0x7E, 0x28, 0xFA, 0x88, 0x3F,
        0xDE, 0x60, 0xF6, 0xE1, 0x8B, 0x79, 0x6B, 0xB9, 0x50, 0x72, 0xEF, 0xD1, 0x70, 0x5B, 0x46, 0x19,
        0xC2, 0x87, 0x87, 0xA4, 0x1E, 0x29, 0xA8, 0xD1, 0xA7, 0xCA, 0x65, 0xE0, 0xA1, 0xF1, 0x31, 0xB0
    },
    /* uint8_t crc[4]; */
    {
        0x91, 0xC8, 0x6F, 0xAD
    },
};

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

