/***********************************************************************************************************************
 * File Name    : iso7816_T0_protocol.h
 * Description  : As required for the file such as Contains macros, data structures and functions used common to the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ISO7816_T0_PROTOCOL_H_
#define ISO7816_T0_PROTOCOL_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define T0_PROTOCOL                            (0x00U)              /* T0 protocol */
#define LC_MAX                                 (255)                /* Maximum data field length in bytes */
#define LE_MAX                                 (256)                /* Maximum expected response data length in bytes */
#define SC_RECEIVE_TIMEOUT                     ((uint16_t)(0x1000)) /* Direction to reader */

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
    uint8_t            Body[LC_MAX];
} smci_apdu_commands_t;

typedef struct st_smci_apdu_response
{
    uint8_t  recv_data[LE_MAX];              /* Data returned from the card */
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
