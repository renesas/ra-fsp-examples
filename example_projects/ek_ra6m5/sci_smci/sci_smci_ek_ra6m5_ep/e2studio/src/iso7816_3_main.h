/*
 * iso7816_3_activation.h
 *
 *  Created on: Jan 17, 2022
 *      Author: a5123412
 */
/***********************************************************************************************************************

File Name    : iso7816_3_main.c/h
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

#ifndef ISO7816_3_MAIN_H_
#define ISO7816_3_MAIN_H_

#include <r_sci_smci.h>
#include <r_sci_smci_cfg.h>

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MIN_COLD_RESET_WAIT_TIME               (400)
#define DEFAULT_ATR_WAIT_TIME                  (9600) // 9600 etu

/* constants for Answer To Reset */
#define ATS_TS_DIRECT                          (0x3BU)  /* Direct bit convention */
#define ATS_TS_INDIRECT                        (0x3FU)  /* Indirect bit convention */
#define ATS_T_SETUP_LENGTH                     (5)
#define ATS_HIST_LENGTH                        (20)

#define EP_INFO1   "\r\nThis project enables Smart Card interface on RA using SCI SMCI module.\r\n" \
                   "In the beginning of operation, application initialize the SCI SMCI module \r\n" \
                   "uses the default asynchronous parameters that are defined by ISO7816-3."
#define EP_INFO2   "\r\nAfter smart card device is connected, the application proceeds the \r\n" \
                   "activation process including Answer To Reset stage and negotiation stage \r\n" \
                   "(PPS exchange). Once the card preferred transmission protocol and asynchronous \r\n"
#define EP_INFO3   "parameters are confirmed, application changes the proper settings for SCI SMCI \r\n" \
		           "module at run-time. Then, the interface is ready to transmit the APDU messages.\r\n"
#define EP_MENU    "\r\n***********THIS MENU ONLY APPLIES TO DEBIT CARD***********\r" \
		           "\r\nFrom the display menu options, user can enter the number to execute the specified operation." \
                   "\r\n \"1\" - Follows Transportation of APDU messages by T-0 to issue a SELECT MF command based on APDU case 3 format. "\
                   "\r\n \"2\" - Follows Transportation of APDU messages by T-0 to issue a GET CHALLENGE command based on APDU case 2 format. "\
                   "\r\n \"3\" - Performs smart card device de-activation. \r\n "

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

typedef enum e_smc_control_status
{
    REGULAR_POOLING        = 0x00,
    SMC_DEVICE_NOT_READY   = 0x01,
    SMC_DEVICE_COLD_RESET  = 0x02,
    SMC_DEVICE_WARM_RESET  = 0x03,
    SMC_DEVICE_ATR         = 0x04,
    SMC_DEVICE_NEGOTIATION = 0x05,
    SMC_DEVICE_ACTIVE      = 0x06,
    SMC_DEVICE_DEACTIVE    = 0x07
} smc_control_status_t;

/* ATR structure */
typedef struct st_smc_atr_message_t
{
    uint8_t TS;                         /* Data Convention */
    uint8_t T0;                         /* High nibble = Number of setup byte; low nibble = Number of historical byte */
    uint8_t TCK;
    uint8_t Ta[ATS_T_SETUP_LENGTH];     /* Setup array for TA */
    uint8_t Tb[ATS_T_SETUP_LENGTH];     /* Setup array for TB */
    uint8_t Tc[ATS_T_SETUP_LENGTH];     /* Setup array for TC */
    uint8_t Td[ATS_T_SETUP_LENGTH];     /* Setup array for TD */
    uint8_t H[ATS_HIST_LENGTH];         /* Historical array */
    uint8_t Tlength;                    /* Setup array dimension */
    uint8_t Hlength;                    /* Historical array dimension */
    uint8_t protocol_type;
    uint8_t data_convention_type;
} smc_atr_message_t;


fsp_err_t Smart_Card_Interface_Init(void);
fsp_err_t Smart_Card_Interface_Card_Detection(void);
fsp_err_t Smart_Card_Interface_F_D_confirmation(smc_atr_message_t * sc_returned_atr);
smc_control_status_t Smart_Card_Interface_Handler(smc_control_status_t set_smc_state);
uint8_t smart_card_interface_event_notify(uint8_t set_event_flag_value, uint32_t timout_etu);
void waiting_timer_enable(uint32_t etu_num);
void waiting_timer_disable(void);

#endif /* ISO7816_3_MAIN_H_ */
