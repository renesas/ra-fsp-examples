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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup CAN_EP
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define ICU_CHANNEL    	                (0U)              	//switch channel for ek-ra6m2
#define WAIT_TIME                       (500U)             //wait time value
#define CAN_DESTINATION_MAILBOX_3       (3U)               //destination mail box number
#define CAN_MAILBOX_NUMBER_0            (0U)               //mail box number
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (6U)               //data length
#define ZERO                            (0U)
/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/* Flags, set from Callback function */
static volatile bool b_switch_pressed = false;          //user button status
static volatile bool b_can_tx = false;                  //CAN transmission status
static volatile bool b_can_rx = false;                  //CAN receive status
static volatile bool b_can_err = false;                 //CAN error status
/* CAN frames for tx and rx */
static can_frame_t g_can_tx_frame;                      //CAN transmit frame
static can_frame_t g_can_rx_frame;                      //CAN receive frame
static uint32_t rx_id;                                  //rx_id to get in callback

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
    uint8_t can_tx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "TX_MSG";       //data to be load in tx_frame while transmitting
    uint8_t can_rx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "RX_MSG";       //data to be load in rx_frame while acknowledging

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
    APP_PRINT("\r\nThis project demonstrates the basic functionality of CAN running on Renesas RA MCUs using 2 RA boards where On "
            "\r\npressing the user button on Board1, data is transmitted to Board2. On receiving the data,  Board2 displays the received data on RTTViewer,"
            "\r\nOn successful comparison of data, Board 2 transmits the framed data to Board1 as received acknowledgment. Also,"
            "\r\nwhenever the transmission is completed, status is printed on the RTTViewer.\r\n");

    /* Initialize CAN module */
    err = R_CAN_Open(&g_can_ctrl, &g_can_cfg);
    /* Error trap */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCAN Open API failed");
        APP_ERR_TRAP(err);
    }

    /* Initialize external irq module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    /* Error trap */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nIRQ Open API failed");
        can_deinit();
        APP_ERR_TRAP(err);
    }
    /* enable user push button */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    /* Error trap */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nIRQ Enable API failed");
        can_deinit();
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\n To start CAN transmission please press the user button");

    g_can_tx_frame.id = CAN_DESTINATION_MAILBOX_3;
    g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;

    /* copy the tx data frame with TX_MSG */
    memcpy((uint8_t*)&g_can_tx_frame.data[ZERO], (uint8_t*)&can_tx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES);

    while (true)
    {
        /* check for user button pressed */
        if (true == b_switch_pressed)
        {
            /* reset flag bit */
            b_switch_pressed = false;
            APP_PRINT("\r\n Transmitting the data");

            /* transmit the data from mail box #0 with tx_frame */
            err = R_CAN_Write(&g_can_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_tx_frame);
            /* Error trap */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n CAN Write API FAILED");
                can_deinit();
                if (FSP_SUCCESS != R_ICU_ExternalIrqClose(&g_external_irq_ctrl))
                {
                    APP_ERR_PRINT("\r\n **ExternalIrq Close API failed**");
                }
                APP_ERR_TRAP(err);
            }

            /* wait for transmit flag bit to set */
            while ((true != b_can_tx) && (time_out--));
            if (RESET_VALUE == time_out)
            {
                APP_ERR_PRINT("CAN transmission failed due to timeout");
                APP_ERR_TRAP(true);
            }
            /* Reset flag bit */
            b_can_tx = false;
            APP_PRINT("\r\n CAN transmission is successful");
        }
        /* check if receive flag is set */
        if (true == b_can_rx)
        {
            /* Reset flag bit */
            b_can_rx = false;
            APP_PRINT("\r\n CAN received the data : %s", &g_can_rx_frame.data);

            /* if received data is same as transmitted data then acknowledge with RX_MSG as received successful*/
            if (RESET_VALUE == strncmp((char*)&g_can_rx_frame.data[ZERO], (char*)&can_tx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES))
            {
                APP_PRINT("\r\n Received 'TX_MSG', responding with 'RX_MSG'\r\n");
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
                    if (FSP_SUCCESS != R_ICU_ExternalIrqClose(&g_external_irq_ctrl))
                    {
                        APP_ERR_PRINT("\r\n **ExternalIrq Close API failed**");
                    }
                    APP_ERR_TRAP(err);
                }
                /* wait for transmit flag bit to set */
                while ((true != b_can_tx) && (time_out--));
                if (RESET_VALUE == time_out)
                {
                    APP_ERR_PRINT("CAN transmission failed due to timeout");
                    APP_ERR_TRAP(true);
                }
                APP_PRINT("\r\n CAN transmission after recv is successful");
                /* Reset flag bit */
                b_can_tx = false;

            }
            /* check if received data is same as rx_msg then received an acknowledge for the transfer of data successful */
            else if (RESET_VALUE == strncmp((char*)&g_can_rx_frame.data[ZERO],(char*)&can_rx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES))
            {
                APP_PRINT("\r\n Received Acknowledge.\r\nCAN operation Successful\r\n");
            }
            /* if no data match then data transfer failed */
            else
            {
                APP_ERR_PRINT("\r\nCAN data mismatch\r\nCAN operation failed\r\n");
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
            rx_id = p_args->p_frame->id;        //copy the rx frame id
            memcpy(&g_can_rx_frame, p_args->p_frame, sizeof(can_frame_t));  //copy the received data to rx_frame
            break;
        }

        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    //overwrite/overrun error event
        case CAN_EVENT_BUS_RECOVERY:            //Bus recovery error event
        case CAN_EVENT_ERR_BUS_OFF:             //error Bus Off event
        case CAN_EVENT_ERR_PASSIVE:             //error passive event
        case CAN_EVENT_ERR_WARNING:             //error warning event
        {
            b_can_err = true;                   //set flag bit
            break;
        }

    }
}

/*******************************************************************************************************************//**
 * This function is called when an user pressed the push button as an event and set the flag bit
 **********************************************************************************************************************/
void irq_callback(external_irq_callback_args_t *p_args)
{
    if(p_args->channel == ICU_CHANNEL)
    {
        b_switch_pressed = true;	//set flag bit
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
