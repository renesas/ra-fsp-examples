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
 * File Name    : new_ecc_public_key.c
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
#include "new_ecc_public_key.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const encrypted_ecc_key_data_t g_encrypted_ecc_key_data =
{
    /* uint32_t keytype; */
    0x00000016,
    /* uint32_t shared_key_number; */
    0x00000000,
    /* uint8_t wufpk[32]; */
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    /* uint8_t initial_vector[16]; */
    {
        0x6F, 0xD2, 0xB2, 0xBA, 0x82, 0x90, 0x3C, 0x5D, 0x6E, 0xEE, 0xB1, 0x6E, 0x96, 0xF0, 0x1A, 0xA0
    },
    /* uint8_t encrypted_user_key[ENCRYPTED_KEY_BYTE_SIZE]; */
    {
        0xC1, 0x7F, 0xB6, 0x2D, 0x00, 0x23, 0x17, 0x29, 0x7A, 0x42, 0xDB, 0xC5, 0xB9, 0xEC, 0x8D, 0xD0,
        0x74, 0x44, 0x9F, 0x87, 0xFD, 0xB7, 0xC1, 0x93, 0x6B, 0x22, 0xB6, 0x0C, 0x3F, 0x97, 0xA0, 0x03,
        0xA5, 0x8A, 0x5B, 0xE4, 0xA3, 0xB8, 0xA8, 0xE0, 0xD3, 0x82, 0xB0, 0x3E, 0x12, 0x37, 0x60, 0xAB,
        0xAA, 0x5D, 0x44, 0x38, 0xED, 0x66, 0xA4, 0xBC, 0xA0, 0xD6, 0x1F, 0xC8, 0x2D, 0xDB, 0x25, 0xA8,
        0x70, 0x9D, 0x02, 0x9E, 0x44, 0xC3, 0xB4, 0x52, 0x20, 0xAA, 0x27, 0xA6, 0x88, 0x4C, 0xAA, 0xFE
    },
    /* uint8_t crc[4]; */
    {
        0xF0, 0x5B, 0x6C, 0xA1
    },
};

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

