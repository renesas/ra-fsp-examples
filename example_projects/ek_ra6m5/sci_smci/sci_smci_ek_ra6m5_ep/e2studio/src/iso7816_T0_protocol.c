/*
 * iso7816_T0_protocol.c
 *
 *  Created on: Jan 18, 2022
 *      Author: a5123412
 */
#include "common_utils.h"
#include "iso7816_3_main.h"
#include "iso7816_T0_protocol.h"
#include "math.h"


#define T0_PROCEDURE_BYTE_NULL          0x60
#define T0_PROCEDURE_BYTE_CASE_4S_2     0x90
#define T0_PROCEDURE_BYTE_CASE_4S_3     0x61
#define INS_CODE_GET_RESPONSE           0xC0


static uint32_t T0_GT = 0;
static uint32_t T0_WT = 0;

/**
 *
 * @param request
 * @param len_request
 * @return
 */
static fsp_err_t iso7816_t0_transmit(uint8_t * request, uint32_t len_request)
{
    fsp_err_t err;
    uint8_t event_flag = 0;

    err = R_SCI_SMCI_Write(&g_smci0_ctrl, request, len_request);
    if(err != FSP_SUCCESS)      
    {
        return err;
    }

    event_flag = smart_card_interface_event_notify(SMCI_EVENT_TX_COMPLETE, ((T0_GT + 10) * len_request));
    if(0 == event_flag)         
    {
        return FSP_ERR_TIMEOUT;
    }

    return FSP_SUCCESS;
}

/**
 *
 * @param response
 * @param len_response
 * @return
 */
static fsp_err_t iso7816_t0_receive(uint8_t * const response, uint32_t len_response)
{
    fsp_err_t err;
    uint8_t event_flag = 0;

    err = R_SCI_SMCI_Read(&g_smci0_ctrl, response, len_response);
    if(err != FSP_SUCCESS)
    {
        return err;
    }

    event_flag = smart_card_interface_event_notify(SMCI_EVENT_RX_COMPLETE, T0_WT);
    if(0 == event_flag)
    {
        return FSP_ERR_TIMEOUT;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Initialize parameters that are used by T=1 protocol.
 *
 * @param   t1: pointer to parameters for T=1 protocol.
 * @param   sc_returned_atr: a pointer to ATR structure.
 * @retval  FSP_SUCCESS                  configuration successfully ends.
 **********************************************************************************************************************/
fsp_err_t ISO7816_T0_SET_PARAMETERS(smci_t0_parameters_t * t0, smc_atr_message_t * sc_returned_atr, uint32_t etu_us)
{
    uint32_t max_clock_frequency = 4000000;

    smci_speed_params_t smci_speed_params = {
                                             .baudrate  = 10000,
                                             .di = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_1,
                                            };

    /* Get the information of current used F, f */
    t0->F = 372;
    t0->D = smci_speed_params.di;
    t0->frequency = max_clock_frequency;

    /* Get the WI from ATR TC2 paramter */
    if(sc_returned_atr->Tc[1] != 0)
    {
        t0->wi = (uint32_t) sc_returned_atr->Tc[1];
    }
    else
    {
        /* set the default WI */
        t0->wi = 10; //etu
    }

    /* Calculate the WT */
    T0_WT = (uint32_t)((((int64_t) t0->wi) * ((int64_t) t0->F) * 960 * 1000000) / t0->frequency); // microsecond base
    T0_WT = (T0_WT/ etu_us);
    t0->wt = T0_WT;

    /* Get the GT integer from ATR TC1 parameter */
    if(sc_returned_atr->Tc[0] != 0)
    {
        t0->gti = (uint32_t) sc_returned_atr->Tc[0];
        /* Calculate the GT */
        T0_GT = (uint32_t)( (((int64_t) t0->F) * ((int64_t) t0->gti) * 1000000) / (((int64_t) t0->D) * ((int64_t) t0->frequency)) ); // microsecond base
        T0_GT = (T0_GT/ etu_us); // extra guard time, etu base
        T0_GT = 12 + T0_GT;
    }
    else
    {
        t0->gti = 0;
        T0_GT = 12;
    }

    t0->gt = T0_GT;

    return FSP_SUCCESS;
}


/**
 * Le is valued from 1 to 256 and coded on byte B1 (B1=’00’ means maximum, i.e. Le=256)
 * The command APDU is mapped onto the T=0 command TPDU without any change
 * Command TPDU: CLA INS P1 P2 P3, P3=’Le’
 * The response TPDU is mapped onto the response APDU according to the acceptance of Le and according to the processing of the command.
 *
 * Case 2S.1 – Le accepted
 * The response TPDU is mapped onto the the response APDU without any change
 * R-TPDU: [Le bytes] [SW1 SW2]
 *
 * Case 2S.2 – Le definitely not accepted
 * Le is not accepted be the card which does not support the service of providing data if the length is wrong
 * The response TPDU from the card indicates that the card aborts the command because of wrong length (SW1=’67’).
 * The response TPDU is mapped onto the response APDU without any change
 * R-TPDU: [SW1='67' SW2]
 *
 * Case 2S.3 – Le not accepted, La indicated
 * Le is not accepted by the card and the card indicates the available length La
 * The response TPDU from the card indicates that the command is aborted due to a wrong length and that the right length is La: (SW1=’6C’ and SW2 codes La).
 * If the transmission system does not support the service of re-issuing the same command, it shall map the response TPDU onto the response APDU without any change
 * R-TPDU: [SW1='6C' SW2='La']
 *
 * Case 2S.4 – SW1-SW2=’9XYZ’, expect ‘9000’
 * The response TPDU is mapped on the response APDU without any change
 *
 * @param smci_apdu_command
 * @param smci_apdu_response
 * @return
 */
fsp_err_t ISO7816_T0_SEND_APDU_CASE2(smci_apdu_header_t * smci_apdu_header, smci_apdu_response_t * smci_apdu_response)
{
    fsp_err_t err;
    uint8_t request[5]={0};
    uint8_t procedure_byte;
    uint8_t procedure_byte_xor;
    uint32_t len_request = 5;
    uint16_t receive_length = 0;
    uint16_t expected_rev_length = 0;

    request[0] = smci_apdu_header->CLA; /* CLA */
    request[1] = smci_apdu_header->INS; /* INS */
    request[2] = smci_apdu_header->P1;  /* P1 */
    request[3] = smci_apdu_header->P2;  /* P2 */
    request[4] = smci_apdu_header->Le;  /* P3 = Expected length of data to be returned */
    expected_rev_length = (uint16_t)((smci_apdu_header->Le != 0) ? (smci_apdu_header->Le) : 256);

    APP_PRINT("Command TPDU: ");
    for(uint8_t i=0; i<len_request;i++)
    {
        APP_PRINT("0x%x ", request[i]);
    }
    APP_PRINT("\r\n");

    err = iso7816_t0_transmit(request, len_request);
    assert(err == FSP_SUCCESS);

    do
    {
        err = iso7816_t0_receive(&procedure_byte, 1);
        assert(err == FSP_SUCCESS);
        procedure_byte_xor = (procedure_byte ^ 0xff);
        if(procedure_byte == T0_PROCEDURE_BYTE_NULL)
        {
            continue;
        }
        else if(procedure_byte == smci_apdu_header->INS)

        {
            /* ACK
             * Receive all remaining data bytes */
            len_request = (uint32_t)((smci_apdu_header->Le != 0) ? (smci_apdu_header->Le) : 256);
            err = iso7816_t0_receive(&smci_apdu_response->recv_data[receive_length], len_request);
            assert(err == FSP_SUCCESS);
            receive_length += (uint16_t)len_request;
        }

        else if(procedure_byte_xor == smci_apdu_header->INS)
        {
            /* ACK
             * Receive the next data byte */
            err = iso7816_t0_receive(&smci_apdu_response->recv_data[receive_length++], 1);
            assert(err == FSP_SUCCESS);
        }
        else if (((procedure_byte & 0xF0) == 0x60) || ((procedure_byte & 0xF0) == 0x90))
        {
            smci_apdu_response->status_bytes[0] = procedure_byte;
            /* Receive SW2 and return */
            err = iso7816_t0_receive(&smci_apdu_response->status_bytes[1], 1);
            assert(err == FSP_SUCCESS);
            smci_apdu_response->recv_length = receive_length;
            return FSP_SUCCESS;
        }
    }while(receive_length != (uint16_t)expected_rev_length);

    /* receive status bytes */
    len_request = 2;
    err = iso7816_t0_receive(&smci_apdu_response->status_bytes[0], len_request);
    assert(err == FSP_SUCCESS);
    smci_apdu_response->recv_length = receive_length;

    APP_PRINT("Response TPDU: ");
    for(uint8_t i=0; i<receive_length;i++)
    {
        APP_PRINT("0x%x ", smci_apdu_response->recv_data[i]);
    }
    APP_PRINT("\r\n");

    APP_PRINT("Status Bytes: ");
    for(uint8_t i=0; i<2;i++)
    {
        APP_PRINT("0x%x ", smci_apdu_response->status_bytes[i]);
    }
    APP_PRINT("\r\n");

    return FSP_SUCCESS;
}


/**
 * In this case, Lc is valued from 1 to 255 and coded on byte B1 (=’00’).
 * The command APDU is mapped onto the T=0 command TPDU without any change.
 *
 * Command TPDU: CLA INS P1 P2 P3, P3=’Lc’, Lc bytes
 *
 * The response TPDU is mapped onto the response APDU without any change.
 * R-TPDU: [SW1 SW2]
 *
 * @param smci_apdu_command
 * @param status_word
 * @return
 */
fsp_err_t ISO7816_T0_SEND_APDU_CASE3(smci_apdu_commands_t * smci_apdu_command, uint16_t * status_word)
{
    fsp_err_t err;
    uint8_t command[260] = {0};
    uint8_t response[2]={0};
    uint32_t len_request = 5;


    if(smci_apdu_command->Header.Lc < 1)
        return FSP_ERR_INVALID_ARGUMENT;

    command[0] = smci_apdu_command->Header.CLA; /* CLA */
    command[1] = smci_apdu_command->Header.INS; /* INS */
    command[2] = smci_apdu_command->Header.P1;  /* P1 */
    command[3] = smci_apdu_command->Header.P2;  /* P2 */
    command[4] = smci_apdu_command->Header.Lc;  /* P3 = Data field length */
    memcpy(&command[5], &smci_apdu_command->Body[0], smci_apdu_command->Header.Lc);

    APP_PRINT("Command TPDU: ");
    for(uint8_t i=0; i<5+smci_apdu_command->Header.Lc;i++)
    {
        APP_PRINT("0x%x ", command[i]);
    }
    APP_PRINT("\r\n");

    len_request += (uint32_t)smci_apdu_command->Header.Lc;
    err = iso7816_t0_transmit(command, len_request);
    assert(err == FSP_SUCCESS);

    len_request = 2;
    err = iso7816_t0_receive(response, len_request);
    assert(err == FSP_SUCCESS);
    APP_PRINT("Response TPDU: ");
    for(uint8_t i=0; i<len_request;i++)
    {
        APP_PRINT("0x%x ", response[i]);
    }
    APP_PRINT("\r\n");

    *status_word = (uint16_t)(response[0]<<8 | response[1]);
    return FSP_SUCCESS;
}
