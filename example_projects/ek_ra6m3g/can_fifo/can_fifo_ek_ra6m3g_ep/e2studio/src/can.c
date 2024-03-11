/***********************************************************************************************************************
 * File Name    : can.c
 * Description  : Contains data structures and functions setup can module used in can.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "hal_data.h"
#include "common_utils.h"
#include "can.h"

/*******************************************************************************************************************//**
 * @addtogroup can
 * @{
 **********************************************************************************************************************/
static volatile bool g_can_tx = false;
static volatile bool g_can_rx = false;
static volatile bool g_err_flag;
static can_frame_t rxMsg ={ RESET_VALUE };
static uint8_t validate_cnt = RESET_VALUE;

/* CAN frames for tx */
static can_frame_t g_can_tx_frame = {RESET_VALUE};
static uint8_t can_tx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES]  = "TX__MESG";
static uint8_t can_ack_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "ACK_MESG";
static fsp_err_t can_write (uint8_t msg_num, can_frame_type_t frame_type,  uint8_t* data);
static void can_handle_error(fsp_err_t err,  const char * err_str);
static fsp_err_t can_validate_result(void);

/*******************************************************************************************************************//**
 *  @brief       This function is used to configure and write a CAN frame.
 *
 *  @param[in]   msg_num     : Number of messages to send.
 *  @param[in]   frame_type  : Type of CAN frame.
 *  @param[in]   data        : Data of CAN frame.
 *  @retval      FSP_SUCCESS : Upon successful operation, otherwise: failed
 **********************************************************************************************************************/
static fsp_err_t can_write (uint8_t msg_num, can_frame_type_t frame_type,  uint8_t* data)
{
    fsp_err_t status = FSP_SUCCESS;
    uint32_t timeout = CAN_BUSY_DELAY * msg_num;

    if (CAN_FRAME_TYPE_REMOTE == frame_type)
    {
        g_can_tx_frame.id = EXTENDED_ID_EXAMPLE;
        g_can_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
        g_can_tx_frame.type = CAN_FRAME_TYPE_REMOTE;
        g_can_tx_frame.data_length_code = 0;
    }
    else
    {
        g_can_tx_frame.id = STANDARD_ID_EXAMPLE;
        g_can_tx_frame.id_mode = CAN_ID_MODE_STANDARD;
        g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;
        g_can_tx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;
        memcpy ((uint8_t*) &g_can_tx_frame.data[ZERO], (uint8_t*) data, CAN_FRAME_TRANSMIT_DATA_BYTES);
    }

    /* reset CAN TX flag */
    g_can_tx = false;
    for (uint32_t i = 0; i < msg_num; i++)
    {
        /* Write message */
        status = R_CAN_Write (&g_can_ctrl, CAN_MAILBOX_ID_TX_FIFO, &g_can_tx_frame);

        /* Handle error if timeout expires*/
        if (FSP_SUCCESS != status)
        {
            APP_ERR_PRINT("\r\n** R_CAN_Write API failed ** \r\n");
            return status;
        }
        /* Increment ID so we can distinguish between messages */
        if (FOUR_MSG == msg_num)
        {
            g_can_tx_frame.id++;
        }
    }

    /* Wait for a TX complete interrupt */
    while (!g_can_tx && --timeout)
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }
    /* Handle error if timeout expires*/
    if (RESET_VALUE == timeout)
    {
        APP_ERR_PRINT("\r\nCAN transmission failed due to timeout\r\n");
        return FSP_ERR_TIMEOUT;
    }

    return status;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to validate received CAN data.
 *
 *  @retval      FSP_SUCCESS : Upon successful operation, otherwise: failed
 **********************************************************************************************************************/
static fsp_err_t can_validate_result(void)
{
    fsp_err_t status = FSP_SUCCESS;

    APP_PRINT("\r\nGot messages from receive FIFO: ID %d, DLC %d, Data (%c%c%c%c%c%c%c%c)", rxMsg.id,
            rxMsg.data_length_code, rxMsg.data[0], rxMsg.data[1], rxMsg.data[2],
            rxMsg.data[3], rxMsg.data[4], rxMsg.data[5], rxMsg.data[6], rxMsg.data[7]);

    /* check if received data is same as tx_msg then received an acknowledge for the transfer of data successful */
    if (RESET_VALUE == strncmp ((char*) &rxMsg.data[ZERO], (char*) &can_tx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES))
    {
        /* Increment the count for each validated message */
        validate_cnt++;
        if (REQUIRED_MSG_COUNT == validate_cnt)
        {
            validate_cnt = ZERO;
            APP_PRINT("\r\n\r\n(3) Received four messages 'TX__MESG', responding with 'ACK_MESG'\r\n");
            can_write (ONE_MSG, CAN_FRAME_TYPE_DATA, (uint8_t*) &can_ack_msg);
            /* Handle error if an error occurs*/
            if (FSP_SUCCESS != status)
            {
                APP_ERR_PRINT("\r\nCAN write operation failed during CAN validate result\r\n");
                return status;
            }
            APP_PRINT("\r\nTo start CAN reception, please enter any key on RTTViewer\r\n");
        }
    }
    else if (RESET_VALUE == strncmp ((char*) &rxMsg.data[ZERO], (char*) &can_ack_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES))
    {
        APP_PRINT("\r\n\r\n(4) Received Ack Message(ACK_MESG). CAN operation Successful\r\n");
        APP_PRINT("\r\nTo start CAN reception, please enter any key on RTTViewer\r\n");
    }
    else
    {
        APP_ERR_PRINT("\r\nCAN data mismatch\r\n CAN operation failed\r\n");
        return FSP_ERR_INVALID_DATA;
    }
    /* Reset the rx frame data */
    memset ((uint8_t*) &rxMsg.data[ZERO], RESET_VALUE, CAN_FRAME_TRANSMIT_DATA_BYTES);

    return status;
}

/*******************************************************************************************************************//**
 * @brief       This function de-initializes the CAN module and performs error trapping.
 * @param[in]   status    : error status.
 * @param[in]   err_str   : error string.
 * @return      None
 **********************************************************************************************************************/
static void can_handle_error(fsp_err_t status,  const char * err_str)
{
    if(FSP_SUCCESS != status)
    {
        /* Print the error */
        APP_ERR_PRINT(err_str);

        /* Close opened CAN module*/
        if(RESET_VALUE != g_can_ctrl.open)
        {
            if(FSP_SUCCESS != R_CAN_Close (&g_can_ctrl))
            {
                APP_ERR_PRINT("\r\nCAN Close API failed\r\n");
            }
        }
        /* Error trap */
        APP_ERR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is called when a CAN event has occurred and to set the respective flags.
 * @param[in]   p_args    : CAN Callback arguments.
 * @return      None
 **********************************************************************************************************************/
void can_callback(can_callback_args_t *p_args)
{
    if ((p_args->event & (CAN_EVENT_TX_COMPLETE | CAN_EVENT_TX_FIFO_EMPTY))
            == (CAN_EVENT_TX_COMPLETE | CAN_EVENT_TX_FIFO_EMPTY))
    {
        /* Set CAN TX flag */
        g_can_tx = true;
    }
    else if (p_args->event == CAN_EVENT_RX_COMPLETE)
    {
        rxMsg = p_args->frame;
        /* Set CAN RX flag */
        g_can_rx=true;
    }
    else if (p_args->event == CAN_EVENT_FIFO_MESSAGE_LOST)
    {
        /* Set error flag */
        g_err_flag = true;
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief      This function initializes the CAN module and starts CAN operation.
 *
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void can_entry (void)
{
    fsp_err_t status = FSP_SUCCESS;
    fsp_pack_version_t version    = {RESET_VALUE};
    char rtt_input_buf[BUFFER_SIZE_DOWN] = { NULL_CHAR, };

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize CAN module */
    status = R_CAN_Open (&g_can_ctrl, &g_can_cfg);
    can_handle_error(status,"\r\n **R_CAN_Open API failed**");

    APP_PRINT("\r\nTo start CAN reception, please enter any key on RTT Viewer\r\n");

    while (true)
    {
        /* check for user input */
        if (APP_CHECK_DATA)
        {
            /* read data from RTT */
            APP_READ(rtt_input_buf);
            /* Display information for Receiver Node */
            APP_PRINT("\r\n(1) This is the receiver Node. Sending a remote frame to request the data from Sender Node.\r\n");

            /* Perform CAN write operation for remote frame */
            status = can_write (ONE_MSG, CAN_FRAME_TYPE_REMOTE, NULL);
            can_handle_error(status,"\r\n **CAN write operation failed during CAN reception**");
        }
        /* Check if receive flag is set */
        if (true == g_can_rx)
        {
            /* Reset g_can_rx flag */
            g_can_rx = false;

            if (CAN_FRAME_TYPE_REMOTE == rxMsg.type)
            {
                /* Display information for Sender Node */
                APP_PRINT("\r\n(2) This is the sender Node. Received the remote frame: ID 0x%x, DLC %d, responding with four 'TX__MESG'\r\n",
                        rxMsg.id, rxMsg.data_length_code);

                /* Perform CAN write operation for data frame */
                status = can_write (FOUR_MSG, CAN_FRAME_TYPE_DATA, (uint8_t*) &can_tx_msg);
                can_handle_error (status, "\r\n **CAN write operation failed during CAN transmission**");
                memset ((uint8_t*) &rxMsg.data[ZERO], RESET_VALUE, CAN_FRAME_TRANSMIT_DATA_BYTES);
            }
            else
            {
                /* Validate CAN result for data reception */
                status = can_validate_result ();
                can_handle_error (status, "\r\n **CAN validate result failed**");
            }
        }

        /* Check if error flag is set */
        if (true == g_err_flag)
        {
            /* Handle error due to receive FIFO overrun */
            can_handle_error(true,"\r\n **CAN operation failed due to receive FIFO overrun**");
        }

        /* Initialize rtt_input_buf with null characters */
        memset (rtt_input_buf, NULL_CHAR, BUFFER_SIZE_DOWN);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup can)
 **********************************************************************************************************************/
