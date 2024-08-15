/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*
 * iso7816_pps.c
 *
 *  Created on: Jan 17, 2022
 *      Author: a5123412
 */
#include "common_utils.h"
#include "iso7816_3_main.h"
#include "iso7816_pps.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MAX_PPS_WAIT_TIME                      (6*9600) // etu base

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static fsp_err_t PPS_Transmit(uint8_t *request, uint8_t len_request);
static fsp_err_t PPS_Receive(uint8_t * const response, uint8_t * len_response);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static uint8_t  pps_received_buffer[PPS_MAX_LENGTH]={0};
static uint8_t  pps_received_count = 0;
static uint8_t  pps_response_length = 0;

/**
  * @brief  apply the PPS procedure.
  * @param  request: a pointer to the PPS request buffer.
  * @param  resp_length: a pointer to a variable that will contain the PPS
  *         response length.
  * @param  pps1: pointer to a PPS1 value.
  * @param  etu: elementary time unit in us.
  * @retval The status of the PPS exchange. The returned value can
  *   be one of the following:
  *     - PPS_OK: PPS response matched with the PPS request.
  *     - PPS_HANDSAKE_ERROR: PPS response not matched with the PPS request.
  *     - PPS_COMMUNICATION_ERROR: communication failed.
  */
fsp_err_t PPS_Exchange(uint8_t * request, uint8_t * resp_length, uint8_t* pps1)
{
    fsp_err_t ret;
    uint8_t response[PPS_MAX_LENGTH];
    uint8_t len_request;
    uint8_t len_response = 0;

    request[0] = 0xFF;

    len_request = PPS_GetLength (request);

    /* Compute the check Character (PCK) */
    request[len_request - 1] = PPS_GetPCK(request, len_request - 1);

    /* ---PPS: Sending request: request, len_request--- */
    ret = PPS_Transmit(request, len_request);
    if(ret != FSP_SUCCESS)
    {
        return ret;
    }

    ret = PPS_Receive(response, &len_response);
    if(ret != FSP_SUCCESS)
    {
        /* Communication error: Response timeout or parity error */
        return ret;
    }

    /* ---PPS: Receiving response: ", response, len_response--- */
    if (!PPS_Match (request, len_request, response, len_response))
    {
        /* The response does not match with the request */
        ret = FSP_ERR_ASSERTION;
    }
    else
    {
        ret = FSP_SUCCESS;  /* PPS response matched with the PPS request */
    }

    /* If PPS1 is echoed */
    if (PPS_HAS_PPS1 (request) && PPS_HAS_PPS1 (response))
    {
        *pps1 = response[PPS1];
    }
    else
    {
        /* ---PPS1 is not present, set to 0 to use default value --- */
        *pps1 = 0;
    }

    /* Copy PPS handshake in request buffer */
    memcpy (request, response, len_response);

    (*resp_length) = len_response;

    return ret;
}

bool PPS_receive_character(uint8_t received_data)
{

    pps_received_buffer[pps_received_count] = received_data;
    pps_received_count++;
    if(pps_received_count == 2)
    {
        /* receive PPS0 to get indicated length and protocol type */
        pps_response_length = (uint8_t)(((received_data & 0x40)>>6) + ((received_data & 0x20)>>5) + ((received_data & 0x10)>>4)) + 3;
    }
    else if(pps_received_count == pps_response_length)
    {
        /* receive whole PPS response */
        return true;
    }

    return false;
}

/**
  * @brief  Compare the PPS request to the PPS response.
  * @param  request: a pointer to the PPS request buffer.
  * @param  len_request: the length of the PPS request buffer.
  * @param  response: pointer to a PPS response buffer.
  * @param  len_response: the length of the PPS request buffer.
  * @retval The status of the PPS comparaison. The returned value can
  *   be one of the following:
  *     - 1: PPS response matched with PPS request.
  *     - 0: PPS response did not match with the PPS request.
  */
uint8_t PPS_Match (uint8_t * request, uint8_t len_request, uint8_t * response, uint8_t len_response)
{
    uint8_t pck = 0;
    uint8_t len_resp=0;

    /* Response longer than request */
    if (len_request < len_response)
    {
        return 0;
    }

    if (response[PPSS] != 0xFF)
    {
        return 0;
    }

    /* The bits b1 to b4 of PPS0 response should echoes b1 to b4 PPS0 request */
    if ((request[PPS0] & 0xF) != (response[PPS0] & 0xF))
    {
        return 0;
    }

    /* See if the reply differs from request: */
    /* Same length  and different contents */
    if ((len_request == len_response) && (memcmp(request, response, len_request)))
    {
        return 0;
    }

    /* See if the card specifies other than default FI and DI */
    if ((PPS_HAS_PPS1 (response)) && (response[PPS1] != request[PPS1]))
    {
        return 0;
    }

    len_resp = PPS_GetLength (response);

    pck = response[0];
    /* Compute PCK */
    for(uint8_t i = 1; i<(len_resp-1); i++)
    {
        pck ^= response[i];
    }

    if(pck != response[len_resp-1])
    {
        return 0;
    }

    /* The PPS response matches with PPS request */
    return 1;
}

/**
  * @brief  compute the length of the PPS frame.
  * @param  pps_buffer: pointer to the PPS buffer.
  * @retval the length of the PPS frame.
  */
uint8_t PPS_GetLength (uint8_t* pps_buffer)
{
    uint8_t length = 3; /* Minimum 3bytes: PPSS, PPS0 and PCK */

    if (PPS_HAS_PPS1 (pps_buffer))
    {
        length++;
    }

    if (PPS_HAS_PPS2 (pps_buffer))
    {
        length++;
    }

    if (PPS_HAS_PPS3 (pps_buffer))
    {
        length++;
    }

    return length;
}

/**
  * @brief  Compute PCK of the PPS frame.
  * @param  pps_buffer: pointer to the PPS buffer.
  * @param  length: the length of the PPS buffer.
  * @retval the PCK value.
  */
uint8_t PPS_GetPCK (uint8_t* pps_buffer, uint8_t length)
{
    uint8_t pck;
    uint8_t i;

    pck = pps_buffer[0];

    for (i = 1; i < length; i++)
    {
        pck ^= pps_buffer[i];
    }

    return pck;
}

/**
  * @brief  Transmit PPS request via USART.
  * @param  request: a pointer to the PPS request buffer.
  * @param  len_request: the lenght of the PPS request buffer.
  * @param  etu_usec: etu in us.
  * @retval None
  */
static fsp_err_t PPS_Transmit(uint8_t *request, uint8_t len_request)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t event_flag = 0;

    memset(pps_received_buffer, 0x00, PPS_MAX_LENGTH);
    pps_received_count = 0;

    err = R_SCI_SMCI_Write(&g_smci0_ctrl, request, (uint32_t)len_request);
    if(err != FSP_SUCCESS)
    {
        __asm("BKPT #0\n");
        return err;
    }

    event_flag = smart_card_interface_event_notify(SMCI_EVENT_TX_COMPLETE, MAX_PPS_WAIT_TIME);
    assert(0 != event_flag);

    return err;
}

/**
  * @brief  Receive PPS response via SCI SMCI
  * @param  response: a pointer to the PPS response buffer.
  * @param  len_response: a pointer to a variable that contains the lenght of
  *         the PPS response buffer.
  * @param  etu_usec: etu in us.
  * @retval None.
  */
static fsp_err_t PPS_Receive(uint8_t * const response, uint8_t * len_response)
{
    /* We don't call SCI_SMCI_Read here because the length of returning PPS response is unknown.
     * We will handle the byte reception in user's callback directly */
    smart_card_interface_event_notify(SMCI_EVENT_RX_COMPLETE, MAX_PPS_WAIT_TIME);
    memcpy(response, pps_received_buffer, pps_received_count);
    *len_response = pps_received_count;
    return FSP_SUCCESS;
}

