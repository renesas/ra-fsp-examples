/***********************************************************************************************************************
 * File Name    : can.c
 * Description  : Contains can protocol related functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "can.h"
#include "common_utils.h"

can_frame_t g_can_rx_frame = {0x00};          /* CAN receive frame */
/* Flags, set from Callback function */
volatile bool b_can_tx = false;                      /* CAN transmission status */
volatile bool b_can_rx = false;                      /* CAN receive status */
volatile bool b_can_err = false;                     /* CAN error status */

static volatile bool g_timer0_event = false;

const uint8_t g_can_msg_payload[CAN_MSG_MAX][CAN_FRAME_TRANSMIT_DATA_BYTES] =
{
 [CAN_MSG_START]     = { 'S','T','A','R','T' },
 [CAN_MSG_SUCCESS]   = { 'A','C','K' },
 [CAN_MSG_FLASH]     = { 'F','L','A','S','H' },
 [CAN_MSG_DONE]      = { 'D','O','N','E' },
 [CAN_MSG_ERROR]     = { 'N','A','C','K' },
};

static inline const uint8_t * can_msg_bytes(can_msg_t msg)
{
    return (msg < CAN_MSG_MAX) ? g_can_msg_payload[msg] : NULL;
}
fsp_err_t can_open(void)
{
    fsp_err_t err;

    err = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_MAIN_OSC, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Initialize CAN module */
    err = R_CAN_Open(&g_can_ctrl, &g_can_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return FSP_SUCCESS;
}

fsp_err_t can_send(uint8_t * data, uint32_t can_id)
{
    if (NULL == data)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    fsp_err_t err;

    b_can_tx  = false;
    b_can_err = false;

    can_frame_t g_can_tx_frame = {0x00};      /* CAN transmit frame */

    g_can_tx_frame.id               = can_id;
    g_can_tx_frame.type             = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;

    /* Copy the TX data payload (8 bytes) */
    memcpy((uint8_t *) &g_can_tx_frame.data[0], (uint8_t *) &data[0], CAN_FRAME_TRANSMIT_DATA_BYTES);

    err = R_CAN_Write(&g_can_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_tx_frame);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    err = R_GPT_Start( &g_can_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Wait for TX complete or timeout */
    while ((true != b_can_tx))
    {
        if (g_timer0_event == true)
        {
            if (b_can_err == true)
            {
                b_can_err = false;
                return FSP_ERR_ASSERTION;
            }
            else
            {
                g_timer0_event = false;
                return FSP_ERR_TIMEOUT;
            }
        }
    }
    err = R_GPT_Stop(&g_can_timer_ctrl);
    err = R_GPT_Reset( &g_can_timer_ctrl);
    return FSP_SUCCESS;
}


fsp_err_t can_send_msg(can_msg_t msg, uint32_t can_id)
{
    const uint8_t * p = can_msg_bytes(msg);
    if (NULL == p)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }
    return can_send((uint8_t *) p, can_id);
}

fsp_err_t can_receive(void)
{
    fsp_err_t err      = FSP_SUCCESS;

    err = R_GPT_Start( &g_can_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Wait for RX complete with timeout */
    while (true != b_can_rx)
    {
        if (g_timer0_event == true)
        {
            if (b_can_err == true)
            {
                b_can_err = false;
                return FSP_ERR_ASSERTION;
            }
            else
            {
                g_timer0_event = false;
                return FSP_ERR_TIMEOUT;
            }
        }
    }

    err = R_GPT_Stop(&g_can_timer_ctrl);
    err = R_GPT_Reset( &g_can_timer_ctrl);
    b_can_rx = false;

    if (0 == memcmp((uint8_t *) &g_can_rx_frame.data[0], can_msg_bytes(CAN_MSG_SUCCESS), CAN_FRAME_TRANSMIT_DATA_BYTES))
    {
        err = FSP_SUCCESS;
    }
    else if (0 == memcmp((uint8_t *) &g_can_rx_frame.data[0], can_msg_bytes(CAN_MSG_ERROR), CAN_FRAME_TRANSMIT_DATA_BYTES))
    {
        err = FSP_ERR_ASSERTION;
    }
    else
    {
        err = FSP_ERR_ABORTED;;
    }

    memset((uint8_t *) &g_can_rx_frame.data[0], RESET_VALUE, CAN_FRAME_TRANSMIT_DATA_BYTES);
    return err;
}

/*******************************************************************************************************************//**
 * This function is called when an CAN event is occurred and SET the respective flags.
 **********************************************************************************************************************/
void can_callback(can_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
        {
            b_can_tx = true;                    /* Set flag bit */
            break;
        }
        case CAN_EVENT_RX_COMPLETE:
        {
            b_can_rx = true;
            memcpy(&g_can_rx_frame, &p_args->frame, sizeof(can_frame_t));  /* Copy the received data to rx_frame */
            break;
        }
        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    /* Bus recovery error event */
        case CAN_EVENT_BUS_RECOVERY:            /* Bus recovery error event */
        case CAN_EVENT_ERR_BUS_OFF:             /* Error Bus Off event */
        case CAN_EVENT_ERR_PASSIVE:             /* Error passive event */
        case CAN_EVENT_ERR_WARNING:             /* Error warning event */
        case CAN_EVENT_ERR_BUS_LOCK:            /* Error bus lock */
        case CAN_EVENT_ERR_CHANNEL:             /* Error channel */
        case CAN_EVENT_ERR_GLOBAL:              /* Error global */
        case CAN_EVENT_TX_ABORTED:              /* Error transmit abort */
        case CAN_EVENT_TX_FIFO_EMPTY:           /* Error transmit FIFO is empty */
        case CAN_EVENT_FIFO_MESSAGE_LOST:       /* Error FIFO message lost */
        {
            b_can_err = true;                   /* Set flag bit */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is to de-initializes the CAN module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
fsp_err_t can_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_CAN_Close(&g_can_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    return FSP_SUCCESS;
}

/* GPT timer callback */
void can_timer_cb(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_timer0_event = true;
}
