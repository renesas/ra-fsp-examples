/***********************************************************************************************************************
 * File Name    : can_fd_ep.c
 * Description  : Contains data structures and functions used in can_fd_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "can_fd_change_baudrate.h"
#include <math.h>

/* External variable */
extern bsp_leds_t g_bsp_leds;

/* User defined functions */
static void led_pin_initialization(void);
static fsp_err_t can_read_operation(void);
static fsp_err_t can_write_operation(can_frame_t can_transmit_frame);
static void can_fd_data_update(void);
static fsp_err_t can_data_check_operation(void);
static fsp_err_t canfd_operation(void);
static void canfd_deinit(void);
static void handle_error(fsp_err_t err, const char *err_str);
static fsp_err_t timer_start_measure (void);
static fsp_err_t timer_get_measure (uint32_t * p_time);
static fsp_err_t timer_init (void);
static void timer_deinit(void);

/* CANFD event flag */
static volatile uint32_t g_canfd_event_flag = RESET_VALUE;

/* CANFD error event */
static const can_error_event_t can_error_events[] = {
    { CAN_EVENT_ERR_WARNING,            LOG_ERR_WARNING },
    { CAN_EVENT_ERR_PASSIVE,            LOG_ERR_PASSIVE },
    { CAN_EVENT_ERR_BUS_OFF,            LOG_ERR_BUS_OFF },
    { CAN_EVENT_BUS_RECOVERY,           LOG_BUS_RECOVERY },
    { CAN_EVENT_MAILBOX_MESSAGE_LOST,   LOG_MAILBOX_MESSAGE_LOST },
    { CAN_EVENT_ERR_BUS_LOCK,           LOG_ERR_BUS_LOCK },
    { CAN_EVENT_ERR_CHANNEL,            LOG_ERR_CHANNEL },
    { CAN_EVENT_TX_ABORTED,             LOG_TX_ABORTED },
    { CAN_EVENT_ERR_GLOBAL,             LOG_ERR_GLOBAL },
    { CAN_EVENT_TX_FIFO_EMPTY,          LOG_TX_FIFO_EMPTY },
    { CAN_EVENT_FIFO_MESSAGE_LOST,      LOG_FIFO_MESSAGE_LOST }
};
/* Global variables */
bool g_transmission_start = false;
volatile uint32_t g_time_out = WAIT_TIME;

/* CANFD transmit and receive frames. */
can_frame_t g_canfd_tx_frame = {RESET_VALUE};
can_frame_t g_canfd_rx_frame = {RESET_VALUE};

/* Data to be loaded in Classic CAN and FD frames for transmission and acknowledgement */
uint8_t g_tx_data[CAN_CLASSIC_FRAME_DATA_BYTES] = "TX_MESG";
uint8_t g_rx_data[CAN_CLASSIC_FRAME_DATA_BYTES] = "RX_MESG";
uint8_t g_tx_fd_data[CAN_FD_DATA_LENGTH_CODE]   = {RESET_VALUE};
uint8_t g_rx_fd_data[CAN_FD_DATA_LENGTH_CODE]   = {RESET_VALUE};

/* Acceptance filter array parameters */
const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] =
{
 { /* Accept all messages with Extended ID 0x1000-0x1FFF */
   .id =
   {
    /* Specify the ID, ID type and frame type to accept. */
    .id         = CANFD_FILTER_ID,
    .frame_type = CAN_FRAME_TYPE_DATA,
    .id_mode    = CAN_ID_MODE_EXTENDED
   },

   .mask =
   {
    /* These values mask which ID/mode bits to compare when filtering messages. */
    .mask_id         = MASK_ID,
    .mask_frame_type = ZERO,
    .mask_id_mode    = MASK_ID_MODE
   },

   .destination =
   {
    /* If DLC checking is enabled, any messages shorter than the below setting will be rejected. */
    .minimum_dlc = (canfd_minimum_dlc_t)ZERO,

    /* Optionally specify a Receive Message Buffer (RX MB) to store accepted frames. RX MBs do not have an
     * interrupt or overwrite protection and must be checked with R_CANFD_InfoGet and R_CANFD_Read. */
    .rx_buffer   = CANFD_RX_MB_0,

    /* Specify which FIFO(s) to send filtered messages to. Multiple FIFOs can be OR'd together. */
    .fifo_select_flags = CANFD_RX_FIFO_0,
   }
 },
};

/* Variable to store rx frame status info*/
can_info_t g_can_rx_info =
{
  .error_code  = RESET_VALUE,
  .error_count_receive = RESET_VALUE,
  .error_count_transmit = RESET_VALUE,
  .rx_fifo_status = RESET_VALUE,
  .rx_mb_status = RESET_VALUE,
  .status = RESET_VALUE,
};

/***********************************************************************************************************************
 *  Function Name: canfd_entry
 *  Description  : This function is used to start the canfd example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void canfd_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version           = {RESET_VALUE};
    char rtt_input_buf[BUFFER_SIZE_DOWN] = {NULL_CHAR};
    bool is_error                        = false;
    char can_err_log[BUFF_SIZE]          = {NULL_CHAR};

    /* Initialization of LED Pins*/
    led_pin_initialization();

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize timer to measure CANFD operation */
    err = timer_init();
    handle_error(err, "timer_init FAILED\r\n");

    /* Initialize CANFD module */
    err = R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);
    handle_error(err, "CANFD Open API failed\r\n");

    while (true)
    {
       /* Configure CANFD baud rate */
       canfd_baudrate_set();

        while (true == g_transmission_start)
        {
            /* Check for user input */
            if (APP_CHECK_DATA)
            {
                /* Clean RTT input buffer */
                memset(rtt_input_buf, NULL_CHAR, BUFFER_SIZE_DOWN);
                APP_READ(rtt_input_buf);

                /* Exit the operation loop to re-configure the CANFD baud rate */
                if (CHAR_LF == rtt_input_buf[0] || CHAR_CR == rtt_input_buf[0])
                {
                    g_transmission_start = false;

                    break;
                }

                /* Start measure for Board 1 */
                err = timer_start_measure();
                handle_error(err, "timer_start_measure FAILED\r\n");

                /* Update transmit frame and initiate transmission on CAN frame */
                canfd_operation();
            }

            /* Get the status of transmitted frame and read the data */
            err = can_read_operation();
            handle_error(err, "can_read_operation failed\r\n");

            /* Handle all occurred error events */
            for (uint8_t i = 0; i < sizeof(can_error_events)/sizeof(can_error_events[0]); i++)
            {
                if (g_canfd_event_flag & can_error_events[i].err_event)
                {
                    snprintf (can_err_log, sizeof(can_err_log), "CAN error event: **%s**\r\n",
                              can_error_events[i].err_log);
                    APP_ERR_PRINT(can_err_log);
                    is_error = true;
                }
            }
            if (true == is_error)
            {
                is_error = false;
                handle_error(FSP_ERR_ASSERTION, "CAN communication error detected\r\n");
            }
        }
    }
}
/***********************************************************************************************************************
* End of function canfd_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: canfd_operation
 *  Description  : This function transmits data after receiving user input and it performs subsequent operations.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t canfd_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update transmit frame parameters */
    g_canfd_tx_frame.id = CAN_ID;
    g_canfd_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    g_canfd_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_canfd_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    g_canfd_tx_frame.options = ZERO;

    /* Update transmit frame data with message */
    memcpy((uint8_t*)&g_canfd_tx_frame.data[ZERO], (uint8_t*)&g_tx_data[ZERO], CAN_CLASSIC_FRAME_DATA_BYTES);

    APP_PRINT("Transmission of data over classic CAN Frame\r\n");

    /* Transmission of data over classic CAN frame */
    err = can_write_operation(g_canfd_tx_frame);
    APP_ERR_RETURN(err, "can_write_operation failed\r\n");

    APP_PRINT("\r\nClassic CAN transmission is successful\n");

    return err;
}
/***********************************************************************************************************************
* End of function canfd_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: can_write_operation
 *  Description  : This function is used to transmit data on classic CAN or CANFD frame.
 *  Arguments    : can_transmit_frame       Data frame to be transmitted
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t can_write_operation(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Re-initialize timeout value */
    g_time_out = WAIT_TIME;

    /* Transmit the data from mail box #0 with tx_frame */
    err = R_CANFD_Write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    APP_ERR_RETURN(err, "CANFD Write API FAILED\r\n");

    led_update(blue);

    /* Wait here for an event from callback */
    while (!(g_canfd_event_flag & CAN_EVENT_TX_COMPLETE) && (--g_time_out));
    if (RESET_VALUE == g_time_out)
    {
        APP_ERR_RETURN(FSP_ERR_TIMEOUT, "CAN transmission failed due to timeout\r\n");
    }

    /* Clear CAN_EVENT_TX_COMPLETE bit of g_canfd_event_flag */
    g_canfd_event_flag &= (uint32_t)(~CAN_EVENT_TX_COMPLETE);

    led_update(green);

    return err;
}
/***********************************************************************************************************************
* End of function can_write_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: can_data_check_operation
 *  Description  : This function compares transmitted/received data and to send ACK accordingly.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t can_data_check_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t execute_time_mcu                = RESET_VALUE;

    /* Update data to be compared with data transmitted/received over FD frame */
    can_fd_data_update();

    /* Impact to Board 2: Compare received data with transmitted classic CAN data from Board 1 */
    if (RESET_VALUE
            == strncmp ((char*) &g_canfd_rx_frame.data[ZERO], (char*) &g_tx_data[ZERO], CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\nReceived 'TX__MESG' on classic frame, responding with 'RX__MESG' using classic CAN frame\n");
        /* Update the receive frame parameters */
        g_canfd_rx_frame.id = CAN_ID;
        g_canfd_rx_frame.type = CAN_FRAME_TYPE_DATA;
        g_canfd_rx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
        g_canfd_rx_frame.options = ZERO;

        /* Update receive frame data with message */
        memcpy(&g_canfd_rx_frame.data, &g_rx_data, CAN_CLASSIC_FRAME_DATA_BYTES);

        /* Start measure for Board 2 */
        err = timer_start_measure();
        APP_ERR_RETURN(err, "\r\ntimer_start_measure FAILED");

        /* Transmission of data as acknowledgement */
        err = can_write_operation(g_canfd_rx_frame);
        APP_ERR_RETURN(err, "can_write_operation failed\r\n");

        APP_PRINT("\nCAN transmission after receive is successful. Sent back the ACK using classic CAN frame\n");

    }
    /* Impact to Board 1: Compare received data with the responded classic CAN data from Board 2 */
    else if (RESET_VALUE
            == strncmp ((char*) &g_canfd_rx_frame.data[ZERO], (char*) &g_rx_data[ZERO], CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\nReceived Acknowledgement for Classic CAN Frame transmission.\nCAN operation Successful length = %d\n", g_canfd_rx_frame.data_length_code);
        APP_PRINT("\nData transmission over FD frame");
        /* Update FD frame parameters */
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 1);
        }

        /* Transmission of data over FD frame */
        err = can_write_operation(g_canfd_rx_frame);
        APP_ERR_RETURN(err, "can_write_operation failed\r\n");

        APP_PRINT("\nCAN transmission on FD Frame after receiving classic CAN frame ACK is successful\n");

    }
    /* Impact to Board 2: Compare received data with transmitted CANFD data from Board 1 */
    else if (RESET_VALUE
            == strncmp ((char*) &g_canfd_rx_frame.data[ZERO], (char*) &g_tx_fd_data[ZERO], CAN_FD_DATA_LENGTH_CODE))
    {
        APP_PRINT("\nReceived data over FD Frame.\nCAN operation Successful, Data length = %d\n", g_canfd_rx_frame.data_length_code);
        led_update(green);

        APP_PRINT("\nSending modified data over FD Frame now as acknowledgement for received FD data.\n");

        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 5);
        }

        /* Transmission of data as acknowledgement */
        err = can_write_operation(g_canfd_rx_frame);
        APP_ERR_RETURN(err, "can_write_operation failed\r\n");

        /* Get execution time */
        err = timer_get_measure(&execute_time_mcu);
        APP_ERR_RETURN(err, "timer_get_measure FAILED\r\n");

        /* Print CANFD execution time of Board 2 */
         APP_PRINT("\r\nPerform CANFD operation on Board 2 successfully in %d microseconds\r\n", execute_time_mcu);

        APP_PRINT("\r\nCAN transmission on FD Frame as acknowledgement is successful\r\n");
        APP_PRINT(PERFORM_TRANSMISSION);

    }
    /* Impact to Board 1: Compare received data with the responded CANFD data from Board 2 */
    else if (RESET_VALUE
            == strncmp ((char*) &g_canfd_rx_frame.data[ZERO], (char*) &g_rx_fd_data[ZERO], CAN_FD_DATA_LENGTH_CODE))
    {
        /* Get execution time  */
        err = timer_get_measure(&execute_time_mcu);
        APP_ERR_RETURN(err, "\r\ntimer_get_measure FAILED");

        APP_PRINT("\r\nReceived Acknowledgement for FD Frame.\r\nCAN operation Successful, Data length = %d\r\n", g_canfd_rx_frame.data_length_code);

        led_update(green);

        /* Print CANFD execution time of Board 1 */
        APP_PRINT("\r\nPerform CANFD on MCU#1 operation successfully in %d microseconds", execute_time_mcu);

        APP_PRINT(PERFORM_TRANSMISSION);

    }
    else /* Wrong MSG Received */
    {
        APP_ERR_RETURN(FSP_ERR_ASSERTION, "CAN data mismatch\r\nCAN operation failed\r\n");
    }
    return err;
}
/***********************************************************************************************************************
* End of function can_data_check_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: can_read_operation
 *  Description  : This function reads channel status info and reads data.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t can_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get the status information for CAN transmission */
    err = R_CANFD_InfoGet(&g_canfd0_ctrl, &g_can_rx_info);
    APP_ERR_RETURN(err, "**R_CANFD_Read API failed\r\n**");

    /* Check if the data is received in FIFO */
    if(g_can_rx_info.rx_mb_status)
    {
        /* Re-initialize timeout value */
        g_time_out = WAIT_TIME;

        /* Read the input frame received */
        err = R_CANFD_Read(&g_canfd0_ctrl, ZERO, &g_canfd_rx_frame);
        APP_ERR_RETURN(err, "**R_CANFD_Read API failed\r\n**");

        led_update(blue);

        /* Wait here for an event from callback */
        while (!(g_canfd_event_flag & CAN_EVENT_RX_COMPLETE) && (--g_time_out));
        if (RESET_VALUE == g_time_out)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "CAN reception failed due to timeout\r\n");
        }

        /* Clear CAN_EVENT_RX_COMPLETE bit of g_canfd_event_flag */
        g_canfd_event_flag &= (uint32_t)(~CAN_EVENT_RX_COMPLETE);

        led_update(green);

        /* Check if the transmitted and received data are the same and send ACK accordingly */
        err = can_data_check_operation();
        APP_ERR_RETURN(err, "can_data_check_operation failed\r\n");
    }
    else
    {
        /* Do Nothing */
    }
    return err;
}
/***********************************************************************************************************************
* End of function can_read_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: can_fd_data_update
 *  Description  : This function updates data buffer that is to be compared with transmitted FD frame data.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void can_fd_data_update(void)
{
    /* Fill frame data to be compared with data transmitted on CANFD frame*/
    for( uint16_t i = 0; i < CAN_FD_DATA_LENGTH_CODE; i++)
    {
        g_tx_fd_data[i]          = (uint8_t) (i + 1);
    }
    for(uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
    {
        g_rx_fd_data[j] = (uint8_t) (j + 5);
    }
}
/***********************************************************************************************************************
* End of function can_fd_data_update
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: timer_start_measure
 *  Description  : This function starts GPT module to measure execution time of an CANFD operation.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_start_measure (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear timer counter */
    err = R_GPT_Reset (&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Reset FAILED\r\n");

    /* Start timer */
    err = R_GPT_Start (&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Start FAILED\r\n");
    return err;
}
/***********************************************************************************************************************
* End of function timer_start_measure
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: timer_get_measure
 *  Description  : This Function measures the timing info by reading the timer.
 *  Arguments    : *p_time          Pointer will be used to store the CANFD operation execution time
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_get_measure (uint32_t * p_time)
{
    fsp_err_t       err             = FSP_SUCCESS;
    timer_status_t  timer_status    = {RESET_VALUE};
    timer_info_t    timer_info      = {RESET_VALUE};

    /* Get status of timer */
    err = R_GPT_StatusGet (&g_timer_ctrl, &timer_status);
    APP_ERR_RETURN(err, "**R_GPT_StatusGet API failed**\r\n");

    /* Get info of timer */
    err = R_GPT_InfoGet (&g_timer_ctrl, &timer_info);
    APP_ERR_RETURN(err, "**R_GPT_InfoGet API failed**\r\n");

    /* Stop timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    APP_ERR_RETURN(err, "**R_GPT_Stop API failed**\r\n");

    /* Convert count value to microseconds unit */
    *p_time = timer_status.counter / (timer_info.clock_frequency / 1000000);
    return err;
}
/***********************************************************************************************************************
* End of function timer_get_measure
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: timer_init
 *  Description  : This functions initializes GPT module used to measure CANFD operation execution time.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_init (void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    APP_ERR_RETURN(err, "**R_GPT_Open API failed**\r\n");
    return err;
}
/***********************************************************************************************************************
* End of function timer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: timer_deinit
 *  Description  : This function de-initializes the opened timer module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void timer_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    if (MODULE_CLOSE != g_timer_ctrl.open)
    {
        /* Close timer module */
        err = R_GPT_Close(&g_timer_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**R_GPT_Close API failed**\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function timer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: led_update
 *  Description  : This function updates led state as per operation status.
 *  Arguments    : led_state        Selects which led has to be made high
 *  Return Value : None
 **********************************************************************************************************************/
void led_update(led_state_t led_state)
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
            /* Green LED state is made high to show successful state */
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
/***********************************************************************************************************************
* End of function led_update
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: canfd0_callback
 *  Description  : CAN FD callback function.
 *  Arguments    : p_args       Pointer to the callback arguments
 *  Return Value : None
 **********************************************************************************************************************/
void canfd0_callback(can_callback_args_t *p_args)
{

    if (NULL != p_args)
    {
        /* Store the events */
        g_canfd_event_flag |= p_args->event;

    }
}
/***********************************************************************************************************************
* End of function canfd0_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: canfd_deinit
 *  Description  : This function de-initializes the opened CANFD module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void canfd_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    if (MODULE_CLOSE != g_canfd0_ctrl.open)
    {
        /* Close CANFD channel */
        err = R_CANFD_Close(&g_canfd0_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**R_CANFD_Close API failed**\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function canfd_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: get_user_input
 *  Description  : This function gets input from terminal.
 *  Arguments    : None
 *  Return Value : input_value      user input value
 **********************************************************************************************************************/
uint32_t get_user_input(void)
{
    char user_input[BUFFER_SIZE_DOWN] = { NULL_CHAR };

    /* Check for user input */
    while (!APP_CHECK_DATA)
    {
        __NOP();
    }
    /* Clean RTT input buffer */
    memset(user_input, NULL_CHAR, BUFFER_SIZE_DOWN);
    APP_READ(user_input);

    /* Convert input to integer */
    uint32_t input_value = (uint32_t) atoi(user_input);

    return input_value;
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: led_pin_initialization
 *  Description  : This function initializes state of LED pins.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void led_pin_initialization(void)
{
    /* Set the LED pin state high */
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH); //Blue LED
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], BSP_IO_LEVEL_HIGH); //Green LED
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], BSP_IO_LEVEL_HIGH); //Red LED

    R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);

    /* Set the LED pin state low */
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], BSP_IO_LEVEL_LOW);
}
/***********************************************************************************************************************
* End of function led_pin_initialization
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  Arguments    : err          error status
 *                 err_str      error string
 *  Return Value : None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err )
    {
        /* Print error information and error code */
        APP_ERR_PRINT(err_str);

        /* De-initialize opened CANFD module */
        canfd_deinit();

        /* De-initialize opened timer module */
        timer_deinit();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
