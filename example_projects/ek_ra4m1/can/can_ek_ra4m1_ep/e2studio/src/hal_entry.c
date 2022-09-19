/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup CAN_EP
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define WAIT_TIME                       (5000U)            //wait time value
#define CAN_DESTINATION_MAILBOX_3       (3U)               //destination mail box number
#define CAN_MAILBOX_NUMBER_0            (0U)               //mail box number
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)               //data length
#define ZERO                            (0U)
#define NULL_CHAR                       ('\0')
/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/* Flags, set from Callback function */
static volatile bool b_can_tx = false;                  //CAN transmission status
static volatile bool b_can_rx = false;                  //CAN receive status
static volatile bool b_can_err = false;                 //CAN error status
/* CAN frames for tx and rx */
static can_frame_t g_can_tx_frame = {RESET_VALUE};      //CAN transmit frame
static can_frame_t g_can_rx_frame = {RESET_VALUE};      //CAN receive frame

/***********************************************************************************************************************
 * Private local functions
 **********************************************************************************************************************/
static void can_deinit(void);
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_out = WAIT_TIME;                                      // time out
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t can_tx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "TX__MESG";       //data to be load in tx_frame while transmitting
    uint8_t can_rx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "RX__MESG";       //data to be load in rx_frame while acknowledging
    char rtt_input_buf[BUFFER_SIZE_DOWN] = {NULL_CHAR,};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT("\r\nThis project demonstrates the basic functionality of CAN module on Renesas RA MCUs using 2 RA boards."
            "\r\nOn pressing any key on the RTTViewer, data is transmitted from Board1 to Board2."
            "\r\nOn reception, Board2 displays the received data on the RTTViewer. Board2, then, transmits the"
            "\r\nframed data back to Board1. On successful transmission, Board1 prints the data on to the RTTViewer.\r\n");

#if defined (BOARD_RA2A1_EK)
    err = R_CGC_Open (&g_cgc0_ctrl, &g_cgc0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCGC Open API failed");
        APP_ERR_TRAP(err);
    }
    err = R_CGC_ClockStart (&g_cgc0_ctrl, CGC_CLOCK_MAIN_OSC, NULL);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCGC ClockStart API failed");
        APP_ERR_TRAP(err);
    }
#endif
    /* Initialize CAN module */
    err = R_CAN_Open(&g_can_ctrl, &g_can_cfg);
    /* Error trap */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCAN Open API failed");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nTo start CAN transmission, please enter any key on RTTViewer\r\n");

    g_can_tx_frame.id = CAN_DESTINATION_MAILBOX_3;
    g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;

    /* copy the tx data frame with TX_MSG */
    memcpy((uint8_t*)&g_can_tx_frame.data[ZERO], (uint8_t*)&can_tx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES);

    while (true)
    {
        /* check for user input */
        if (APP_CHECK_DATA)
        {
            /* read data from RTT */
            APP_READ(rtt_input_buf);

            APP_PRINT("\r\n Transmitting the data");

            /* transmit the data from mail box #0 with tx_frame */
            err = R_CAN_Write(&g_can_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_tx_frame);
            /* Error trap */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n CAN Write API FAILED");
                can_deinit();
                APP_ERR_TRAP(err);
            }

            /* wait for transmit flag bit to set */
            while ((true != b_can_tx) && (--time_out));
            if (RESET_VALUE == time_out)
            {
                APP_ERR_PRINT("CAN transmission failed due to timeout");
                APP_ERR_TRAP(true);
            }
            /* Reset flag bit */
            b_can_tx = false;
            APP_PRINT("\r\n CAN transmission is successful");
            APP_PRINT("\r\n To start CAN transmission, please enter any key on RTTViewer\n");
            memset(rtt_input_buf,NULL_CHAR,BUFFER_SIZE_DOWN);

        }
        /* check if receive flag is set */
        if (true == b_can_rx)
        {
            /* Reset flag bit */
            b_can_rx = false;
            APP_PRINT("\r\n CAN received the data : %s\r\n", &g_can_rx_frame.data);

            /* if received data is same as transmitted data then acknowledge with RX_MSG as received successful*/
            if (RESET_VALUE == strncmp((char*)&g_can_rx_frame.data[ZERO], (char*)&can_tx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES))
            {
                APP_PRINT("\r\n Received 'TX__MESG', responding with 'RX__MESG'\r\n");
                g_can_rx_frame.id = CAN_DESTINATION_MAILBOX_3;
                g_can_rx_frame.type = CAN_FRAME_TYPE_DATA;
                g_can_rx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;

                /* copy the rx_data frame as RX_MSG */
                memcpy((uint8_t*)&g_can_rx_frame.data[ZERO], (uint8_t*)&can_rx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES);

                /* Transmit the rx data frame as acknowledging the data transfer is successful */
                err = R_CAN_Write (&g_can_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_rx_frame);
                /* Error trap */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\n CAN Write API FAILED");
                    can_deinit();
                    APP_ERR_TRAP(err);
                }
                /* wait for transmit flag bit to set */
                while ((true != b_can_tx) && (--time_out));
                if (RESET_VALUE == time_out)
                {
                    APP_ERR_PRINT("CAN transmission failed due to timeout");
                    APP_ERR_TRAP(true);
                }
                APP_PRINT("\r\n CAN transmission after receive is successful");
                /* Reset flag bit */
                b_can_tx = false;

            }
            /* check if received data is same as rx_msg then received an acknowledge for the transfer of data successful */
            else if (RESET_VALUE == strncmp((char*)&g_can_rx_frame.data[ZERO],(char*)&can_rx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES))
            {
                APP_PRINT("\r\n Received Acknowledge.\r\n CAN operation Successful\r\n");
            }
            /* if no data match then data transfer failed */
            else
            {
                APP_ERR_PRINT("\r\nCAN data mismatch\r\n CAN operation failed\r\n");
                APP_ERR_TRAP(true);
            }
            /* Reset the rx frame data */
            memset((uint8_t*)&g_can_rx_frame.data[ZERO],RESET_VALUE,CAN_FRAME_TRANSMIT_DATA_BYTES);
        }
        /* re initializing time out value */
        time_out = WAIT_TIME;
    }
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
            b_can_tx = true;        //set flag bit
            break;
        }

        case CAN_EVENT_RX_COMPLETE:
        {
            b_can_rx = true;
            memcpy(&g_can_rx_frame, &p_args->frame, sizeof(can_frame_t));  //copy the received data to rx_frame
            break;
        }

        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    //overwrite/overrun error event
        case CAN_EVENT_BUS_RECOVERY:            //Bus recovery error event
        case CAN_EVENT_ERR_BUS_OFF:             //error Bus Off event
        case CAN_EVENT_ERR_PASSIVE:             //error passive event
        case CAN_EVENT_ERR_WARNING:             //error warning event
        case CAN_EVENT_ERR_BUS_LOCK:            //error bus lock
        case CAN_EVENT_ERR_CHANNEL:             //error channel
        case CAN_EVENT_ERR_GLOBAL:              //error global
        case CAN_EVENT_TX_ABORTED:              //error transmit abort
        case CAN_EVENT_TX_FIFO_EMPTY:           //error transmit FIFO is empty
        {
            b_can_err = true;                   //set flag bit
            break;
        }

    }
}


/*******************************************************************************************************************//**
 * @brief       This function is to de-initializes the CAN module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void can_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_CAN_Close(&g_can_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n **CAN Close API failed**");
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end defgroup CAN_EP)
 **********************************************************************************************************************/
