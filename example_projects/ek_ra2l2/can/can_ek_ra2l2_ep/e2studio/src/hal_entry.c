/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup CAN_EP
 * @{
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define WAIT_TIME                       (5000U)             /* Wait time value */
#define CAN_DESTINATION_MAILBOX_3       (3U)                /* Destination mail box number */
#define CAN_MAILBOX_NUMBER_0            (0U)                /* Mail box number */
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)                /* Data length */
#define ZERO                            (0U)
#define NULL_CHAR                       ('\0')
/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/* Flags, set from Callback function */
static volatile bool b_can_tx = false;                      /* CAN transmission status */
static volatile bool b_can_rx = false;                      /* CAN receive status */
static volatile bool b_can_err = false;                     /* CAN error status */
/* CAN frames for TX and RX */
static can_frame_t g_can_tx_frame = {RESET_VALUE};          /* CAN transmit frame */
static can_frame_t g_can_rx_frame = {RESET_VALUE};          /* CAN receive frame */

/***********************************************************************************************************************
 * Private local function
 **********************************************************************************************************************/
static void can_deinit(void);
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used. This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_out = WAIT_TIME;  /* Time out */
    fsp_pack_version_t version = {RESET_VALUE};
    /* Data to be load in tx_frame while transmitting */
    uint8_t can_tx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "TX__MESG";
    /* Data to be load in rx_frame while acknowledging */
    uint8_t can_rx_msg[CAN_FRAME_TRANSMIT_DATA_BYTES] = "RX__MESG";
    char rtt_input_buf[BUFFER_SIZE_DOWN] = {NULL_CHAR,};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch );
    APP_PRINT("\r\nThis project demonstrates the basic functionality of the CAN module on Renesas RA MCUs"
              " using two RA boards."
              "\r\nOn pressing any key on the J-Link RTT Viewer, data is transmitted from Board 1 to Board 2."
              "\r\nOn reception, Board 2 displays the received data on the J-Link RTT Viewer and then transmits the"
              "\r\nframed data back to Board 1. On successful transmission, Board 1 prints the data"
              " onto the J-Link RTT Viewer.\r\n");

#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA2L1_EK) || defined (BOARD_RA2L2_EK)
    err = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_CGC_Open API failed");
        APP_ERR_TRAP(err);
    }
    err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_MAIN_OSC, NULL);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_CGC_ClockStart API failed");
        APP_ERR_TRAP(err);
    }
#endif
    /* Initialize CAN module */
    err = R_CAN_Open(&g_can_ctrl, &g_can_cfg);
    /* Error trap */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_CAN_Open API failed");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nTo start CAN transmission, please enter any key on RTT Viewer\r\n");

    g_can_tx_frame.id = CAN_DESTINATION_MAILBOX_3;
    g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;

    /* Copy the TX data frame with TX_MSG */
    memcpy((uint8_t*)&g_can_tx_frame.data[ZERO], (uint8_t*)&can_tx_msg[ZERO], CAN_FRAME_TRANSMIT_DATA_BYTES);

    while (true)
    {
        /* Check for user input */
        if (APP_CHECK_DATA)
        {
            /* Read data from RTT */
            APP_READ(rtt_input_buf);

            APP_PRINT("\r\nTransmitting the data");

            /* Transmit the data from mail box #0 with tx_frame */
            err = R_CAN_Write(&g_can_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_tx_frame);
            /* Error trap */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_CAN_Write API failed");
                can_deinit();
                APP_ERR_TRAP(err);
            }

            /* Wait for transmit flag bit to set */
            while ((true != b_can_tx) && (--time_out));
            if (RESET_VALUE == time_out)
            {
                APP_ERR_PRINT("\r\nCAN transmission failed due to timeout");
                APP_ERR_TRAP(true);
            }
            /* Reset flag bit */
            b_can_tx = false;
            APP_PRINT("\r\nCAN transmission is successful");
            APP_PRINT("\r\nTo start CAN transmission, please enter any key on RTT Viewer\r\n");
            memset(rtt_input_buf, NULL_CHAR, BUFFER_SIZE_DOWN);
        }
        /* Check if receive flag is set */
        if (true == b_can_rx)
        {
            /* Reset flag bit */
            b_can_rx = false;
            APP_PRINT("\r\nCAN received the data: %s\r\n", &g_can_rx_frame.data);

            /* If received data is same as transmitted data then acknowledge with RX_MSG as received successful */
            if (RESET_VALUE == strncmp((char*)&g_can_rx_frame.data[ZERO], (char*)&can_tx_msg[ZERO],\
                                       CAN_FRAME_TRANSMIT_DATA_BYTES))
            {
                APP_PRINT("\r\n Received 'TX__MESG', responding with 'RX__MESG'\r\n");
                g_can_rx_frame.id = CAN_DESTINATION_MAILBOX_3;
                g_can_rx_frame.type = CAN_FRAME_TYPE_DATA;
                g_can_rx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;

                /* Copy the rx_data frame as RX_MSG */
                memcpy((uint8_t*)&g_can_rx_frame.data[ZERO], (uint8_t*)&can_rx_msg[ZERO],\
                        CAN_FRAME_TRANSMIT_DATA_BYTES);

                /* Transmit the RX data frame as acknowledging the data transfer is successful */
                err = R_CAN_Write(&g_can_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_rx_frame);
                /* Error trap */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_CAN_Write API failed");
                    can_deinit();
                    APP_ERR_TRAP(err);
                }
                /* Wait for transmit flag bit to set */
                while ((true != b_can_tx) && (--time_out));
                if (RESET_VALUE == time_out)
                {
                    APP_ERR_PRINT("\r\nCAN transmission failed due to timeout");
                    APP_ERR_TRAP(true);
                }
                APP_PRINT("\r\nCAN transmission after receive is successful");
                /* Reset flag bit */
                b_can_tx = false;
            }
            /* Check if received data is same as rx_msg then received an acknowledge
             * for the transfer of data successful */
            else if (RESET_VALUE == strncmp((char*)&g_can_rx_frame.data[ZERO],(char*)&can_rx_msg[ZERO],\
                                            CAN_FRAME_TRANSMIT_DATA_BYTES))
            {
                APP_PRINT("\r\nReceived Acknowledge.\r\nCAN operation Successful\r\n");
            }
            /* If no data match then data transfer failed */
            else
            {
                APP_ERR_PRINT("\r\nCAN data mismatch.\r\nCAN operation failed\r\n");
                APP_ERR_TRAP(true);
            }
            /* Reset the RX frame data */
            memset((uint8_t*)&g_can_rx_frame.data[ZERO],RESET_VALUE,CAN_FRAME_TRANSMIT_DATA_BYTES);
        }
        /* Re initializing time out value */
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
static void can_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_CAN_Close(&g_can_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n**R_CAN_Close API failed**");
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process. This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup */

        /* Configure pins */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end defgroup CAN_EP)
 **********************************************************************************************************************/
