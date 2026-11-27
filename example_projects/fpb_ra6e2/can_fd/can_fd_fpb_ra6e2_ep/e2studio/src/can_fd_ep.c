/***********************************************************************************************************************
 * File Name    : can_fd_ep.c
 * Description  : Contains data structures and functions used in can_fd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "can_fd_ep.h"

/* CANFD event flag, updated in the callback */
volatile uint32_t g_canfd_event_flag = RESET_VALUE;

/* CANFD transmit and receive frames */
can_frame_t g_canfd_tx_frame = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, {RESET_VALUE}};
can_frame_t g_canfd_rx_frame = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, {RESET_VALUE}};

/* Data to be loaded in Classic CAN and FD frames for transmission and acknowledgment */
uint8_t g_tx_data[CAN_CLASSIC_FRAME_DATA_BYTES] = "TX_MESG";
uint8_t g_rx_data[CAN_CLASSIC_FRAME_DATA_BYTES] = "RX_MESG";
uint8_t g_tx_fd_data[CAN_FD_DATA_LENGTH_CODE]   = {RESET_VALUE};
uint8_t g_rx_fd_data[CAN_FD_DATA_LENGTH_CODE]   = {RESET_VALUE};

/* Acceptance filter array parameters */
#if defined (BOARD_RA8M2_EK) || defined (BOARD_RA8T2_EK) || defined (BOARD_RA8T1_MCK)
const canfd_afl_entry_t g_canfd0_afl[CANFD_CFG_AFL_CH1_RULE_NUM] =
#else
const canfd_afl_entry_t g_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] =
#endif /* BOARD_RA8M2_EK || BOARD_RA8T2_EK || BOARD_RA8T1_MCK */
{
    {
        /* Accept all messages with Extended ID 0x1000-0x1FFF */
        .id =
        {
            /* Specify the ID, ID type and frame type to accept */
            .id         = CANFD_FILTER_ID,
            .frame_type = CAN_FRAME_TYPE_DATA,
            .id_mode    = CAN_ID_MODE_EXTENDED
        },

        .mask =
        {
            /* These values mask which ID/mode bits to compare when filtering messages */
            .mask_id         = MASK_ID,
            .mask_frame_type = 0U,
            .mask_id_mode    = MASK_ID_MODE
        },

        .destination =
        {
            /* If DLC checking is enabled, any messages shorter than the below setting will be rejected */
            .minimum_dlc = (canfd_minimum_dlc_t) 0U,

            /* Optionally specify a Receive Message Buffer (RX MB) to store accepted frames. RX MBs do not have an
             * interrupt or overwrite protection and must be checked with R_CANFD_InfoGet and R_CANFD_Read. */
            .rx_buffer = CANFD_RX_MB_0,

            /* Specify which FIFO(s) to send filtered messages to. Multiple FIFOs can be OR'd together. */
            .fifo_select_flags = CANFD_RX_FIFO_0,
        }
    },
};

/***********************************************************************************************************************
 * Function Name: can_fd_init.
 * Description  : This function opens the CANFD module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Opened successfully.
 *                Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t can_fd_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function can_fd_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: can_fd_deinit.
 * Description  : This function closes the opened CANFD module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void can_fd_deinit(void)
{
    if (g_canfd0_ctrl.open)
    {
        R_CANFD_Close(&g_canfd0_ctrl);
    }
}
/***********************************************************************************************************************
* End of function can_fd_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_init.
 * Description  : This function initializes the GPT timer used to measure CANFD operation execution time.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function timer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_deinit.
 * Description  : This function closes the opened GPT timer module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void timer_deinit(void)
{
    if (g_timer_ctrl.open)
    {
        R_GPT_Close(&g_timer_ctrl);
    }
}
/***********************************************************************************************************************
* End of function timer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: can_fd_read_frame.
 * Description  : This function reads one CAN frame from the CANFD channel, if one is available.
 * Arguments    : p_rx_frame                Pointer to store the received frame.
 *                p_data_received           Set to true if a frame was available and read, false otherwise.
 * Return Value : FSP_SUCCESS               Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t can_fd_read_frame(can_frame_t * p_rx_frame, bool * p_data_received)
{
    fsp_err_t err = FSP_SUCCESS;
    can_info_t rx_info =
    {
        .error_code = RESET_VALUE,
        .error_count_receive = RESET_VALUE,
        .error_count_transmit = RESET_VALUE,
        .rx_fifo_status = RESET_VALUE,
        .rx_mb_status = RESET_VALUE,
        .status = RESET_VALUE,
    };
    uint32_t time_out = WAIT_TIME;

    *p_data_received = false;

    /* Get the status information for CAN transmission */
    err = R_CANFD_InfoGet(&g_canfd0_ctrl, &rx_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check if the data is received in FIFO */
    if (rx_info.rx_mb_status)
    {
        /* Read the input frame received */
        err = R_CANFD_Read(&g_canfd0_ctrl, 0U, p_rx_frame);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        led_update(LED_CASE_1);

        /* Wait here for an event from callback */
        while (!(g_canfd_event_flag & CAN_EVENT_RX_COMPLETE) && (--time_out));
        if (RESET_VALUE == time_out)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Clear CAN_EVENT_RX_COMPLETE bit of g_canfd_event_flag */
        g_canfd_event_flag &= (uint32_t)(~CAN_EVENT_RX_COMPLETE);

        led_update(LED_CASE_2);

        *p_data_received = true;
    }

    return err;
}
/***********************************************************************************************************************
* End of function can_fd_read_frame
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: can_fd_write_frame.
 * Description  : This function transmits one CAN frame on classic CAN or CANFD frame.
 * Arguments    : can_transmit_frame        Data frame to be transmitted.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t can_fd_write_frame(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_out = WAIT_TIME;

    /* Transmit the data from mail box #0 with tx_frame */
    err = R_CANFD_Write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    led_update(LED_CASE_1);

    /* Wait here for an event from callback */
    while (!(g_canfd_event_flag & CAN_EVENT_TX_COMPLETE) && (--time_out));
    if (RESET_VALUE == time_out)
    {
        return FSP_ERR_TIMEOUT;
    }

    /* Clear CAN_EVENT_TX_COMPLETE bit of g_canfd_event_flag */
    g_canfd_event_flag &= (uint32_t)(~CAN_EVENT_TX_COMPLETE);

    led_update(LED_CASE_2);

    return err;
}
/***********************************************************************************************************************
* End of function can_fd_write_frame
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: can_fd_data_update.
 * Description  : This function updates the data buffers compared against transmitted/received FD frame data.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void can_fd_data_update(void)
{
    /* Fill frame data to be compared with data transmitted on CANFD frame */
    for (uint16_t i = 0; i < CAN_FD_DATA_LENGTH_CODE; i++)
    {
        g_tx_fd_data[i] = (uint8_t) (i + 1);
    }
    for (uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
    {
        g_rx_fd_data[j] = (uint8_t) (j + 5);
    }
}
/***********************************************************************************************************************
* End of function can_fd_data_update
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: led_update.
 * Description  : This function updates LED state as per operation status.
 * Arguments    : led_state                 Selects which LED has to be made high.
 * Return Value : None.
 **********************************************************************************************************************/
void led_update(led_state_t led_state)
{
    if (g_bsp_leds.led_count == 2U)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* Turn ON LED1 to indicate an operation in progress */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);

                /* Delay */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }

            case LED_CASE_2:
            {
                /* Turn ON LED2 to indicate successful completion */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);

                /* Delay */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }

            case LED_CASE_3:
            {
                /* Turn ON LED1 and LED2 to indicate an error */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);

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
    else if (g_bsp_leds.led_count >= 3U)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* Turn ON LED1 to indicate an operation in progress */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_OFF);

                /* Delay */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }

            case LED_CASE_2:
            {
                /* Turn ON LED2 to indicate successful completion */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_OFF);

                /* Delay */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }

            case LED_CASE_3:
            {
                /* Turn ON LED3 to indicate an error */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_ON);

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
    else
    {
        /* Do nothing */
    }
}
/***********************************************************************************************************************
* End of function led_update
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: led_pin_initialization.
 * Description  : This function initializes state of LED pins.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void led_pin_initialization(void)
{
    for (uint32_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        /* Set the LED pin state high */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[i], BSP_IO_LEVEL_HIGH);
    }

    R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);

    for (uint32_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        /* Set the LED pin state low */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[i], BSP_IO_LEVEL_LOW);
    }
}
/***********************************************************************************************************************
* End of function led_pin_initialization
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_start_measure.
 * Description  : This function starts the GPT timer to measure execution time of a CANFD operation.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Started successfully.
 *                Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t timer_start_measure(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear timer counter */
    err = R_GPT_Reset(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start timer */
    err = R_GPT_Start(&g_timer_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function timer_start_measure
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_get_measure.
 * Description  : This function measures the timing info by reading the timer.
 * Arguments    : p_time                    Pointer to store the CANFD operation execution time, in microseconds.
 * Return Value : FSP_SUCCESS               Measured successfully.
 *                Any other error codes     Failed to measure.
 **********************************************************************************************************************/
fsp_err_t timer_get_measure(uint32_t * p_time)
{
    fsp_err_t      err          = FSP_SUCCESS;
    timer_status_t timer_status = {RESET_VALUE, RESET_VALUE};
    timer_info_t   timer_info   = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get status of timer */
    err = R_GPT_StatusGet(&g_timer_ctrl, &timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get info of timer */
    err = R_GPT_InfoGet(&g_timer_ctrl, &timer_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Stop timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Convert count value to microseconds unit */
    *p_time = (uint32_t) (((uint64_t) timer_status.counter * 1000000ULL) / timer_info.clock_frequency);

    return err;
}
/***********************************************************************************************************************
* End of function timer_get_measure
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: canfd0_callback.
 * Description  : This function is a callback for the CANFD module.
 * Arguments    : p_args                    Callback function parameter data.
 * Return Value : None.
 **********************************************************************************************************************/
void canfd0_callback(can_callback_args_t * p_args)
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
