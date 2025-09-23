/***********************************************************************************************************************
 * File Name    : can_fd_ep.c
 * Description  : Contains data structures and functions used in can_fd_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "can_fd_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup can_fd_ep
 * @{
 **********************************************************************************************************************/
 can_frame_t g_canfd_tx_frame;                      /* CAN transmit frame */
 can_frame_t g_canfd_rx_frame;
 /* Variable to store RX frame status information */
 can_info_t g_can_rx_info =
 {
  .error_code  = RESET_VALUE,
  .error_count_receive = RESET_VALUE,
  .error_count_transmit = RESET_VALUE,
  .rx_fifo_status = RESET_VALUE,
  .rx_mb_status = RESET_VALUE,
  .status = RESET_VALUE,
 };

/* Data to be loaded into Classic CAN and CAN FD frames for transmission and acknowledgment */
uint8_t g_tx_data[SIZE_64] = "TX_MESG";
uint8_t g_rx_data[SIZE_64] = "RX_MESG";
uint8_t g_tx_fd_data[SIZE_64];
uint8_t g_rx_fd_data[SIZE_64];

extern bool g_canfd_tx_complete ;
extern bool g_canfd_rx_complete ;
extern bool g_canfd_err_status;

extern bsp_leds_t g_bsp_leds;
extern uint32_t g_time_out;

/* User defined functions */
static void can_write_operation(can_frame_t can_transmit_frame);
static void can_fd_data_update(void);
static void can_data_check_operation(void);


/*******************************************************************************************************************//**
 * @brief       Transmits data after receiving user input and performs subsequent operations.
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

    /* Update transmit frame data by copying payload from g_tx_data */
    memcpy((uint8_t*)&g_canfd_tx_frame.data[ZERO], (uint8_t*)&g_tx_data[ZERO], CAN_FD_DATA_LENGTH_CODE);

    APP_PRINT("\r\nTransmitting data over Classic CAN frame...");

    /* Transmit data over Classic CAN frame */
    can_write_operation(g_canfd_tx_frame);

    APP_PRINT("\r\nCAN transmission is successful");
}

/*******************************************************************************************************************//**
 * @brief       Transmits data using either a Classic CAN or CAN FD frame.
 * @param[in]   can_transmit_frame    CAN frame structure to be transmitted.
 * @return      None
 **********************************************************************************************************************/
static void can_write_operation(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit the data from mailbox #0 with the given transmit frame */
    err = R_CANFD_Write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_CANFD_Write API failed");
        led_update(LED_CASE_3);
        canfd_deinit();
        APP_ERR_TRAP(err);
    }

    led_update(LED_CASE_1);

    /* Wait here for the transmission complete event from the callback */
    while ((true != g_canfd_tx_complete) && (--g_time_out));
    if (RESET_VALUE == g_time_out)
    {
        APP_ERR_PRINT("\r\nCAN transmission failed due to timeout");
        led_update(LED_CASE_3);
        APP_ERR_TRAP(true);
    }

    led_update(LED_CASE_2);

    /* Reset transmission complete flag */
    g_canfd_tx_complete = false;
}

/*******************************************************************************************************************//**
 * @brief     Compares transmitted and received CAN/CAN FD frame data, and sends an appropriate acknowledgment.
 * @param[in] None
 * @return    None
 **********************************************************************************************************************/
static void can_data_check_operation(void)
{
    /* Update data to be compared with data transmitted/received over CAN FD frame */
    can_fd_data_update();

    if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&g_tx_data[ZERO],\
                              CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\r\nReceived \"TX__MESG\" via Classic CAN frame, responding with \"RX__MESG\" using Classic"\
                  " CAN frame.\r\n");
        /* Update the receive frame parameters */
        g_canfd_rx_frame.id = CAN_ID;
        g_canfd_rx_frame.type = CAN_FRAME_TYPE_DATA;
        g_canfd_rx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
        g_canfd_rx_frame.options = ZERO;

        /* Update receive frame data with message */
        memcpy(&g_canfd_rx_frame.data, &g_rx_data, CAN_CLASSIC_FRAME_DATA_BYTES);

        /* Transmission of data as acknowledgment */
        can_write_operation(g_canfd_rx_frame);

        APP_PRINT("\r\nCAN transmission successful. ACK sent via Classic CAN frame.\r\n");
    }
    else if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&g_rx_data[ZERO],\
                                   CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\r\nReceived acknowledgment \"RX__MESG\" for Classic CAN transmission.\r\n");
        APP_PRINT("Classic CAN operation successful. Data length = %d bytes\r\n", g_canfd_rx_frame.data_length_code);

        APP_PRINT("\r\nTransmitting data via CAN FD frame...\r\n");
        /* Updating FD frame parameters */
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

        /* Fill frame data that is to be sent in FD frame */
        for(uint16_t j = 0; j < SIZE_64; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 1);
        }

        /* Transmission of data as over FD frame */
        can_write_operation(g_canfd_rx_frame);

        APP_PRINT("\r\nCAN FD transmission successful after receiving Classic CAN ACK.\r\n");
    }
    else if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&g_tx_fd_data[ZERO],\
                                   CAN_FD_DATA_LENGTH_CODE))  /* Acknowledgment for second transmission */
    {
        APP_PRINT("\r\nReceived data via CAN FD frame.\r\nData reception successful."\
                  " Data length = %d bytes\r\n", g_canfd_rx_frame.data_length_code);

        led_update(LED_CASE_2);

        APP_PRINT("\r\nTransmitting modified data via CAN FD frame as acknowledgment...\r\n");

        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < SIZE_64; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 5);
        }

        /* Transmission of data as acknowledgment */
        can_write_operation(g_canfd_rx_frame);

        APP_PRINT("\r\nCAN FD transmission successful. ACK sent via CAN FD frame.\r\n");
    }
    else if(RESET_VALUE == strncmp((char*)&g_canfd_rx_frame.data[ZERO], (char*)&g_rx_fd_data[ZERO],\
                                   CAN_FD_DATA_LENGTH_CODE)) /* Acknowledgment for second transmission */
    {
        APP_PRINT("\r\nReceived acknowledgment via CAN FD frame.\r\nCAN FD operation successful."\
                  " Data length = %d bytes\r\n", g_canfd_rx_frame.data_length_code);

        led_update(LED_CASE_2);

        APP_PRINT("\r\nPlease enter any key on RTT Viewer to initiate CAN transmission\r\n");
    }
    else /* Wrong MSG received */
    {
        APP_ERR_PRINT("\r\nCAN data mismatch\r\nCAN operation failed\r\n");
        led_update(LED_CASE_3);
        APP_ERR_TRAP(true);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function reads CAN channel status and received data frame.
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void can_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get the status information for CAN transmission */
    err = R_CANFD_InfoGet(&g_canfd0_ctrl, &g_can_rx_info);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_CANFD_InfoGet API failed");
        led_update(LED_CASE_3);
        canfd_deinit();
        APP_ERR_TRAP(err);
    }

    /* Check if the data is received in FIFO */
    if(g_can_rx_info.rx_mb_status)
    {
        /* Read the received CAN frame from mailbox 0 */
        err = R_CANFD_Read(&g_canfd0_ctrl, ZERO, &g_canfd_rx_frame);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_CANFD_Read API failed");
            led_update(LED_CASE_3);
            canfd_deinit();
            APP_ERR_TRAP(err);
        }

        /* Verify received data and send response frame accordingly if valid */
        can_data_check_operation();
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief     Updates the data buffers used for comparison with transmitted and received CAN FD frame data.
 * @param[in] None
 * @return    None
 **********************************************************************************************************************/
static void can_fd_data_update(void)
{
    /* Fill frame data to be compared with data transmitted on CAN FD frame */
    for(uint16_t i = 0; i < SIZE_64; i++)
    {
        g_tx_fd_data[i] = (uint8_t) (i + 1);
    }
    for(uint16_t j = 0; j < SIZE_64; j++)
    {
        g_rx_fd_data[j] = (uint8_t) (j + 5);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function updates LED state as per operation status.
 * @param[in]   led_state       Selects which LED should be turned ON.
 * @retval      None
 **********************************************************************************************************************/
void led_update(led_state_t led_state)
{
    if (g_bsp_leds.led_count == 2)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* LED 1 state is made ON to show operation is in progress */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_2:
            {
                /* LED 2 state is made ON to show successful state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_3:
            {
                /* LED 1 & LED 2 states are made ON to show error state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if (g_bsp_leds.led_count >= 3)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* LED 1 state is made ON to show operation is in progress */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_OFF);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_2:
            {
                /* LED 2 state is made ON to show successful state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_OFF);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_3:
            {
                /* LED 3 state is made ON to show error state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_ON);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       Callback function for CAN FD events.
 * @param[in]   p_args  Pointer to the CAN callback arguments structure.
 * @retval      None
 **********************************************************************************************************************/
void canfd0_callback(can_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
        {
            g_canfd_tx_complete = true;         /* Set the transmission complete flag */
            break;
        }
        case CAN_EVENT_RX_COMPLETE:     /* Currently unsupported by the driver. This block is unreachable for now */
        {
            g_canfd_rx_complete = true;
            break;
        }
        case CAN_EVENT_ERR_WARNING:             /* Error warning event */
        case CAN_EVENT_ERR_PASSIVE:             /* Error passive event */
        case CAN_EVENT_ERR_BUS_OFF:             /* Bus-off error event */
        case CAN_EVENT_BUS_RECOVERY:            /* Bus recovery error event */
        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    /* Mailbox message lost (overwrite/overrun) */
        case CAN_EVENT_ERR_BUS_LOCK:            /* Bus lock detected (32 consecutive dominant bits) */
        case CAN_EVENT_ERR_CHANNEL:             /* Channel error has occurred */
        case CAN_EVENT_TX_ABORTED:              /* Transmit abort event */
        case CAN_EVENT_ERR_GLOBAL:              /* Global error has occurred */
        case CAN_EVENT_FIFO_MESSAGE_LOST:       /* Receive FIFO overrun */
        case CAN_EVENT_TX_FIFO_EMPTY:           /* Transmit FIFO is empty */
        {
            g_canfd_err_status = true;          /* Set the error status flag */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function de-initializes the CAN FD module.
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void canfd_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close CAN FD channel */
    err = R_CANFD_Close(&g_canfd0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_CANFD_Close API failed");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup can_fd_ep)
 **********************************************************************************************************************/
