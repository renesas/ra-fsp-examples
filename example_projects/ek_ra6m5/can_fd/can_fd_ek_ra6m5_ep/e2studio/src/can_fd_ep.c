/***********************************************************************************************************************
 * File Name    : can_fd_ep.c
 * Description  : Contains data structures and functions used in can_fd_ep.c
 ***********************************************************************************************************************/
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
#include "can_fd_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup can_fd_ep
 * @{
 **********************************************************************************************************************/
 can_frame_t g_canfd_tx_frame;                      //CAN transmit frame
 can_frame_t g_canfd_rx_frame;
 /* Variable to store rx frame status info*/
 can_info_t can_rx_info =
 {
  .error_code  = RESET_VALUE,
  .error_count_receive = RESET_VALUE,
  .error_count_transmit = RESET_VALUE,
  .rx_fifo_status = RESET_VALUE,
  .rx_mb_status = RESET_VALUE,
  .status = RESET_VALUE,
 };


/* Data to be loaded in Classic CAN and FD frames for transmission and acknowledgement */
uint8_t tx_data[SIZE_64] = "TX_MESG";
uint8_t rx_data[SIZE_64] = "RX_MESG";
uint8_t tx_fd_data[SIZE_64];
uint8_t rx_fd_data[SIZE_64];

extern bool b_canfd_tx_complete ;
extern bool b_canfd_rx_complete ;
extern bool b_canfd_err_status;

extern bsp_leds_t g_bsp_leds;
extern uint32_t g_time_out;

/* User defined functions */
static void can_write_operation(can_frame_t can_transmit_frame);
static void can_fd_data_update(void);
static void can_data_check_operation(void);
static void led_update(led_state_t led_state);


/*******************************************************************************************************************//**
 * @brief       This function is to transmit data after receiving user input and it performs subsequent operations
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void canfd_operation(void)
{
    /* Update transmit frame parameters */
    g_canfd_tx_frame.id = CAN_ID;
    g_canfd_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    g_canfd_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_canfd_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    g_canfd_tx_frame.options = ZERO;

    /* Update transmit frame data with message */
    memcpy((uint8_t*)&g_canfd_tx_frame.data[ZERO], (uint8_t*)&tx_data[ZERO], CAN_FD_DATA_LENGTH_CODE);

    APP_PRINT("\nTransmission of data over classic CAN Frame");

    /* Transmission of data over classic CAN frame */
    can_write_operation(g_canfd_tx_frame);

    APP_PRINT("\nClassic CAN transmission is successful");

}

/*******************************************************************************************************************//**
 * @brief       This function is to  transmit data  on classic CAN or CANFD frame
 * @param[in]   can_transmit_frame        Data frame to be transmitted
 * @return      None
 **********************************************************************************************************************/
static void can_write_operation(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit the data from mail box #0 with tx_frame */
    err = R_CANFD_Write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\nCANFD Write API FAILED");
        led_update(red);
        canfd_deinit();
        APP_ERR_TRAP(err);
    }

    led_update(blue);

    /* Wait here for an event from callback */
    while ((true != b_canfd_tx_complete) && (g_time_out--));
    if (RESET_VALUE == g_time_out)
    {
        APP_ERR_PRINT("CAN transmission failed due to timeout");
        led_update(red);
        APP_ERR_TRAP(true);
    }

    led_update(green);

    /* Reset flag bit */
    b_canfd_tx_complete = false;
}


/*******************************************************************************************************************//**
 * @brief       This function is to compare transmitted/received data and to send ACK accordingly
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void can_data_check_operation(void)
{
    /*Update data to be compared with data transmitted/received over FD frame */
    can_fd_data_update();

    if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&tx_data[ZERO], CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\nReceived 'TX__MESG' on classic frame, responding with 'RX__MESG' using classic CAN frame\n");
        /* Update the receive frame parameters*/
        g_canfd_rx_frame.id = CAN_ID;
        g_canfd_rx_frame.type = CAN_FRAME_TYPE_DATA;
        g_canfd_rx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
        g_canfd_rx_frame.options = ZERO;

        /* Update receive frame data with message */
        memcpy(&g_canfd_rx_frame.data, &rx_data, CAN_CLASSIC_FRAME_DATA_BYTES);

        /* Transmission of data as acknowledgement */
        can_write_operation(g_canfd_rx_frame);

        APP_PRINT("\nCAN transmission after receive is successful. Sent back the ACK using classic CAN frame\n");


    }
    else if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&rx_data[ZERO], CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\nReceived Acknowledgement for Classic CAN Frame transmission.\nCAN operation Successful length = %d\n", g_canfd_rx_frame.data_length_code);
        APP_PRINT("\nData transmission over FD frame\n");
        /* Updating FD frame parameters */
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < SIZE_64; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 1);
        }

        /* Transmission of data as over FD frame */
        can_write_operation(g_canfd_rx_frame);

        APP_PRINT("\nCAN transmission on FD Frame after receiving classic frame ACK is successful\n");

    }
    else if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&tx_fd_data[ZERO], CAN_FD_DATA_LENGTH_CODE)) // acknowledging for second transmission
    {
        APP_PRINT("\nReceived data over FD Frame.\nCAN operation Successful, Data length = %d\n", g_canfd_rx_frame.data_length_code);
        led_update(green);

        APP_PRINT("\nSending modified data over FD Frame now as acknowledgement for received FD data.\n");

        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < SIZE_64; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 5);
        }

        /* Transmission of data as acknowledgement */
        can_write_operation(g_canfd_rx_frame);

        APP_PRINT("\nCAN transmission on FD Frame as acknowledgement is successful\n");

    }
    else if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&rx_fd_data[ZERO], CAN_FD_DATA_LENGTH_CODE)) // acknowledgement for second transmission
    {
        APP_PRINT("\nReceived Acknowledgement for FD Frame.\nCAN operation Successful, Data length = %d\n", g_canfd_rx_frame.data_length_code);

        led_update(green);

        APP_PRINT("\nPlease enter any key on RTT Viewer to initiate CAN transmission.\n");
    }
    else /* Wrong MSG Received */
    {
        APP_ERR_PRINT("\nCAN data mismatch\nCAN operation failed\n");
        led_update(red);
        APP_ERR_TRAP(true);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is to read Channel status info and read data.
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void can_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get the status information for CAN transmission */
    err = R_CANFD_InfoGet(&g_canfd0_ctrl, &can_rx_info);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\nCAN InfoGet API FAILED");
        led_update(red);
        canfd_deinit();
        APP_ERR_TRAP(err);
    }


    /* Check if the data is received in FIFO */
    if(can_rx_info.rx_mb_status)
    {
        /* Read the input frame received */
        err = R_CANFD_Read(&g_canfd0_ctrl, ZERO, &g_canfd_rx_frame);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\nCAN Read API FAILED");
            led_update(red);
            canfd_deinit();
            APP_ERR_TRAP(err);
        }

        /*Check if the transmitted and received data are same and send ACK accordingly */
        can_data_check_operation();
    }
    else
    {
        /* Do Nothing */
    }
}



/*******************************************************************************************************************//**
 * @brief       This function is to update data buffer that is to be compared with transmitted FD frame data
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void can_fd_data_update(void)
{
    /* Fill frame data to be compared with data transmitted on CANFD frame*/
    for( uint16_t i = 0; i < SIZE_64; i++)
    {
        tx_fd_data[i]          = (uint8_t) (i + 1);
    }
    for(uint16_t j = 0; j < SIZE_64; j++)
    {
        rx_fd_data[j] = (uint8_t) (j + 5);
    }
}

/*******************************************************************************************************************//**
 * @brief This function updates led state as per operation status
 * @param[in]  led_state      Selects which led has to be made high
 * @retval     None
 **********************************************************************************************************************/
static void led_update(led_state_t led_state)
{
    switch(led_state)
    {
        case red:
        {
            /* Red LED state is made high to show error state */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
            /* Delay */
            R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
            break;
        }
        case green:
        {
            /* Green LED state is made high to show succesful state */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], BSP_IO_LEVEL_LOW);
            /* Delay */
            R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
            break;
        }
        case blue:
        {
            /* Blue LED state is made high to show operation is in progress */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], BSP_IO_LEVEL_LOW);
            /* Delay */
            R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
            break;
        }
        default:
        {
            break;
        }
    }

}

/*******************************************************************************************************************//**
 * @brief CAN FD callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void canfd0_callback(can_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
        {
            b_canfd_tx_complete = true;        //set flag bit
            break;
        }
        case CAN_EVENT_RX_COMPLETE: // Currently driver don't support this. This is unreachable code for now.
        {
            b_canfd_rx_complete = true;
            break;
        }
        case CAN_EVENT_ERR_WARNING:             //error warning event
        case CAN_EVENT_ERR_PASSIVE:             //error passive event
        case CAN_EVENT_ERR_BUS_OFF:             //error Bus Off event
        case CAN_EVENT_BUS_RECOVERY:            //Bus recovery error event
        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    //overwrite/overrun error event
        case CAN_EVENT_ERR_BUS_LOCK:            // Bus lock detected (32 consecutive dominant bits).
        case CAN_EVENT_ERR_CHANNEL:             // Channel error has occurred.
        case CAN_EVENT_TX_ABORTED:              // Transmit abort event.
        case CAN_EVENT_ERR_GLOBAL:              // Global error has occurred.
        {
            b_canfd_err_status = true;          //set flag bit
            break;
        }

    }
}

/*******************************************************************************************************************//**
 * @brief       This function is to de-initializes the CANFD module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void canfd_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close CANFD channel */
    err = R_CANFD_Close(&g_canfd0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\n **CANFD Close API failed**");
    }
}


/*******************************************************************************************************************//**
 * @} (end addtogroup can_fd_ep)
 **********************************************************************************************************************/


