/*
 * iso7816_T0_protocol.h
 *
 *  Created on: Jan 18, 2022
 *      Author: a5123412
 */
/***********************************************************************************************************************

File Name    : iso7816_T0_protocol.c/h
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


#ifndef ISO7816_T0_PROTOCOL_H_
#define ISO7816_T0_PROTOCOL_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define T0_PROTOCOL                            (0x00U)  /* T0 protocol */
#define Lc_Max                                 (255)
#define Le_Max                                 (256)
#define SC_Receive_Timeout                     (uint16_t)0x1000 /* Direction to reader */


/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

typedef struct st_smci_apdu_header
{
    uint8_t CLA;  /* Command class */
    uint8_t INS;  /* Operation code */
    uint8_t P1;   /* Selection Mode */
    uint8_t P2;   /* Selection Option */
    uint8_t Lc;   /* Data field length */
    uint8_t Le;   /* Expected length of data to be returned */
} smci_apdu_header_t;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct st_smci_apdu_commands
{
    smci_apdu_header_t Header;
    uint8_t            Body[Lc_Max];
} smci_apdu_commands_t;

typedef struct st_smci_apdu_response
{
    uint8_t  recv_data[Le_Max];              /* Data returned from the card */
    uint16_t recv_length;
    uint8_t  status_bytes[2];
    uint8_t  Lx;
} smci_apdu_response_t;

typedef struct st_smci_t0_parameters_t
{
    uint32_t                    F;           /* Smart card clock rate conversion integer */
    uint32_t                    D;           /* Smart card baudrate adjustment integer */
    uint32_t                    frequency;   /* Smart card frequency */
    uint32_t                    wi;          /* waiting time integer */
    uint32_t                    wt;          /* waiting time */
    uint32_t                    gti;         /* guard time integer */
    uint32_t                    gt;          /* guard time */
} smci_t0_parameters_t;


fsp_err_t ISO7816_T0_SET_PARAMETERS(smci_t0_parameters_t * t0, smc_atr_message_t * sc_returned_atr, uint32_t etu_us);
fsp_err_t ISO7816_T0_SEND_APDU_CASE1(smci_apdu_header_t * smci_apdu_header, uint16_t * status_word);
fsp_err_t ISO7816_T0_SEND_APDU_CASE2(smci_apdu_header_t * smci_apdu_header, smci_apdu_response_t * smci_apdu_response);
fsp_err_t ISO7816_T0_SEND_APDU_CASE3(smci_apdu_commands_t * smci_apdu_command, uint16_t * status_word);
#endif /* ISO7816_T0_PROTOCOL_H_ */
