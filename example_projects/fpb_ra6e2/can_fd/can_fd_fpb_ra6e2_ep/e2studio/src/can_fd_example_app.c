/***********************************************************************************************************************
 * File Name    : can_fd_example_app.c
 * Description  : Example application layer for the CAN FD EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: can_fd_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "can_fd_change_baudrate.h"
#include "can_fd_example_app.h"
#include "console/uart_console.h"

/* CANFD error events */
static const can_error_event_t can_error_events[] =
{
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

/* Flag to indicate that CANFD transmission has been configured and started */
static bool s_transmission_start = false;

/* Private functions */
static void canfd_baudrate_set(void);
static baudrate_set_status_t canfd_baudrate_get_from_input(menu_t menu_option, baudrate_set_status_t current_status,
                                                           uint32_t * nominal_rate_req, uint32_t * data_rate_req,
                                                           uint16_t * sample_pnt_req);
static void transmission_menu_operation(void);
static void can_read_operation(void);
static void check_can_error_events(void);
static void canfd_operation(void);
static void can_data_check_operation(void);
static void user_input(char * buf, int len);
static uint32_t get_user_input(void);
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: can_fd_example_app_run.
 * Description  : Single entry-point for the CAN FD example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void can_fd_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        /* UART failed to open — cannot print anything, signal via LED instead */
        led_update(LED_CASE_3);
        return;
    }

    /* Initialization of LED pins */
    led_pin_initialization();

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize timer to measure CANFD operation */
    err = timer_init();
    handle_error(err, "timer_init failed\r\n");

    /* Initialize CANFD module */
    err = can_fd_init();
    handle_error(err, "can_fd_init failed\r\n");

    while (true)
    {
        /* Configure CANFD baud rate */
        canfd_baudrate_set();

        while (true == s_transmission_start)
        {
            /* Check for user input */
            if (uart_console_has_char())
            {
                transmission_menu_operation();
            }

            /* Get the status of transmitted frame and read the data */
            can_read_operation();

            /* Handle all occurred error events */
            check_can_error_events();
        }
    }
}
/***********************************************************************************************************************
* End of function can_fd_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: transmission_menu_operation.
 * Description  : This function reads the user's menu selection and either starts a CAN transmission or exits back
 *                to the baud rate configuration menu.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void transmission_menu_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    transmission_menu_t selection = (transmission_menu_t) get_user_input();

    switch (selection)
    {
        case TRANSMIT_DATA:
        {
            /* Start measure for Board 1 */
            err = timer_start_measure();
            handle_error(err, "timer_start_measure failed\r\n");

            /* Update transmit frame and initiate transmission on CAN frame */
            canfd_operation();
            break;
        }

        case RETURN_TO_BAUDRATE_MENU:
        {
            /* Exit the operation loop to re-configure the CANFD baud rate */
            s_transmission_start = false;
            break;
        }

        default:
        {
            printf("\r\nInvalid input. Please enter valid input\r\n");
            printf(SUB_MENU);
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function transmission_menu_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: check_can_error_events.
 * Description  : This function checks for and reports CANFD error events.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void check_can_error_events(void)
{
    bool is_error = false;

    for (uint8_t i = 0; i < sizeof(can_error_events) / sizeof(can_error_events[0]); i++)
    {
        if (g_canfd_event_flag & can_error_events[i].err_event)
        {
            printf("\r\n[ERR] CAN error event: **%s**\r\n", can_error_events[i].err_log);
            is_error = true;
        }
    }

    if (true == is_error)
    {
        handle_error(FSP_ERR_ASSERTION, "CAN communication error detected\r\n");
    }
}
/***********************************************************************************************************************
* End of function check_can_error_events
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: canfd_baudrate_set.
 * Description  : This function configures the CANFD baud rate via the user menu.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void canfd_baudrate_set(void)
{
    menu_t menu_option = (menu_t) RESET_VALUE;
    static uint32_t nominal_rate_req = NOMINAL_BAUDRATE_DEFAULT;
    static uint32_t data_rate_req = DATA_BAUDRATE_DEFAULT;
    static uint16_t sample_pnt_req = SAMPLE_POINT_DEFAULT;
    static baudrate_set_status_t baudrate_set_status = (baudrate_set_status_t) RESET_VALUE;

    while (true != s_transmission_start)
    {
        /* Reset baudrate set flag */
        baudrate_set_status = (baudrate_set_status_t) RESET_VALUE;

        /* Print requested CANFD baudrate information */
        printf("\r\nThe requested CANFD baud rate:");
        printf(CANFD_BAUDRATE_INFO, nominal_rate_req, data_rate_req, sample_pnt_req);

        /* Print menu option */
        printf(EP_MENU);

        /* Get selected menu */
        menu_option = (menu_t) get_user_input();
        switch (menu_option)
        {
            case NOMINAL_BAUDRATE_SET:
            {
                while (RESET_VALUE == (baudrate_set_status & NOMINAL_SET_SUCCESS))
                {
                    baudrate_set_status = canfd_baudrate_get_from_input(menu_option, baudrate_set_status,
                                                                        &nominal_rate_req, &data_rate_req,
                                                                        &sample_pnt_req);
                }
                break;
            }

            case DATA_BAUDRATE_SET:
            {
                while (RESET_VALUE == (baudrate_set_status & DATA_SET_SUCCESS))
                {
                    baudrate_set_status = canfd_baudrate_get_from_input(menu_option, baudrate_set_status,
                                                                        &nominal_rate_req, &data_rate_req,
                                                                        &sample_pnt_req);
                }
                break;
            }

            case SAMPLE_POINT_SET:
            {
                while (RESET_VALUE == (baudrate_set_status & SAMPLE_POINT_SET_SUCCESS))
                {
                    printf("\r\nEnter sample point (%%): Valid value from %d to %d"
                           "\r\nUser input: ", SAMPLE_POINT_MIN, SAMPLE_POINT_MAX);
                    baudrate_set_status = canfd_baudrate_get_from_input(menu_option, baudrate_set_status,
                                                                        &nominal_rate_req, &data_rate_req,
                                                                        &sample_pnt_req);
                }
                break;
            }

            case START_TRANSMISSION:
            {
                can_timing_t nominal_actual = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};
                can_timing_t data_actual    = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

                fsp_err_t err = canfd_change_baudrate(nominal_rate_req, data_rate_req, sample_pnt_req,
                                                      &nominal_actual, &data_actual);
                handle_error(err, "canfd_change_baudrate failed\r\n");

                /* Print actual CANFD baud rate */
                printf("\r\nThe actual CANFD baud rate:");
                printf(CANFD_BAUDRATE_INFO, nominal_actual.baudrate, data_actual.baudrate, sample_pnt_req);

                s_transmission_start = true;
                printf(SUB_MENU);
                break;
            }

            default:
            {
                printf("\r\nInvalid input. Please enter valid input\r\n");
                break;
            }
        }
    }
}
/***********************************************************************************************************************
* End of function canfd_baudrate_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: canfd_baudrate_get_from_input.
 * Description  : This function gets the requested CANFD baudrate from user input.
 * Arguments    : menu_option               Selected option.
 *                current_status            Current baudrate set status, updated and returned by this function.
 *                nominal_rate_req          Nominal baudrate obtained from input.
 *                data_rate_req             Data baudrate obtained from input.
 *                sample_pnt_req            Sample point obtained from input.
 * Return Value : baudrate_set_status       Status of getting the baudrate from user input.
 **********************************************************************************************************************/
static baudrate_set_status_t canfd_baudrate_get_from_input(menu_t menu_option, baudrate_set_status_t current_status,
                                                           uint32_t * nominal_rate_req, uint32_t * data_rate_req,
                                                           uint16_t * sample_pnt_req)
{
    baudrate_set_status_t baudrate_set_status = current_status;
    uint32_t custom_input = RESET_VALUE;

    switch (menu_option)
    {
        case NOMINAL_BAUDRATE_SET:
        {
            baudrate_set_status |= NOMINAL_SET_SUCCESS;

            printf(NOMINAL_RATE_OPTION);

            nominal_rate_opt_t nominal_opt = (nominal_rate_opt_t) get_user_input();
            switch (nominal_opt)
            {
                case KBPS_250:
                {
                    *nominal_rate_req = KBPS_250_VAL;
                    break;
                }

                case KBPS_500:
                {
                    *nominal_rate_req = KBPS_500_VAL;
                    break;
                }

                case KBPS_1000:
                {
                    *nominal_rate_req = MBPS_1_VAL;
                    break;
                }

                case NOMINAL_CUSTOM:
                {
                    printf("\r\nEnter nominal rate(bits/s): Valid value from %d to %d"
                           "\r\nUser input: ", NOMINAL_BAUDRATE_MIN, NOMINAL_BAUDRATE_MAX);
                    custom_input = get_user_input();
                    if (NOMINAL_BAUDRATE_MIN <= custom_input && NOMINAL_BAUDRATE_MAX >= custom_input)
                    {
                        *nominal_rate_req = custom_input;
                    }
                    else
                    {
                        printf("\r\nThe input is out of range\r\n");
                        baudrate_set_status &= (baudrate_set_status_t) ~NOMINAL_SET_SUCCESS;
                    }
                    break;
                }

                default:
                {
                    printf("\r\nInvalid option\r\n");
                    baudrate_set_status &= (baudrate_set_status_t) ~NOMINAL_SET_SUCCESS;
                    break;
                }
            }
            break;
        }

        case DATA_BAUDRATE_SET:
        {
            baudrate_set_status |= DATA_SET_SUCCESS;

            printf(DATA_RATE_OPTION);

            data_rate_opt_t data_opt = (data_rate_opt_t) get_user_input();
            switch (data_opt)
            {
                case MBPS_1:
                {
                    *data_rate_req = MBPS_1_VAL;
                    break;
                }

                case MBPS_2:
                {
                    *data_rate_req = MBPS_2_VAL;
                    break;
                }

#if defined (SUPPORT_MAX_DATA_RATE_5MBPS)
                case MBPS_5:
                {
                    *data_rate_req = MBPS_5_VAL;
                    break;
                }
#elif defined (SUPPORT_MAX_DATA_RATE_8MBPS)
                case MBPS_8:
                {
                    *data_rate_req = MBPS_8_VAL;
                    break;
                }
#endif /* SUPPORT_MAX_DATA_RATE */

                case DATA_CUSTOM:
                {
                    printf("\r\nEnter data rate(bits/s): Valid value from %d to %d"
                           "\r\nUser input: ", DATA_BAUDRATE_MIN, DATA_BAUDRATE_MAX);
                    custom_input = get_user_input();
                    if (DATA_BAUDRATE_MIN <= custom_input && DATA_BAUDRATE_MAX >= custom_input)
                    {
                        *data_rate_req = custom_input;
                        baudrate_set_status |= DATA_SET_SUCCESS;
                    }
                    else
                    {
                        printf("\r\nThe input is out of range\r\n");
                    }
                    break;
                }

                default:
                {
                    printf("\r\nInvalid option\r\n");
                    baudrate_set_status &= (baudrate_set_status_t) ~DATA_SET_SUCCESS;
                    break;
                }
            }
            break;
        }

        case SAMPLE_POINT_SET:
        {
            custom_input = get_user_input();
            if (SAMPLE_POINT_MIN <= custom_input && SAMPLE_POINT_MAX >= custom_input)
            {
                *sample_pnt_req = (uint16_t) custom_input;
                baudrate_set_status |= SAMPLE_POINT_SET_SUCCESS;
            }
            else
            {
                printf("\r\nThe input is out of range\r\n");
            }
            break;
        }

        case START_TRANSMISSION:
        {
            break;
        }

        default:
        {
            printf("\r\nInvalid option\r\n");
            break;
        }
    }
    return baudrate_set_status;
}
/***********************************************************************************************************************
* End of function canfd_baudrate_get_from_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: canfd_operation.
 * Description  : This function transmits data after receiving user input and performs subsequent operations.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void canfd_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update transmit frame parameters */
    g_canfd_tx_frame.id = CAN_ID;
    g_canfd_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    g_canfd_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_canfd_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    g_canfd_tx_frame.options = 0U;

    /* Update transmit frame data with message */
    memcpy((uint8_t *) &g_canfd_tx_frame.data[0U], (uint8_t *) &g_tx_data[0U], CAN_CLASSIC_FRAME_DATA_BYTES);

    printf("\r\nTransmission of data over classic CAN Frame\r\n");

    /* Transmission of data over classic CAN frame */
    err = can_fd_write_frame(g_canfd_tx_frame);
    handle_error(err, "can_fd_write_frame failed\r\n");

    printf("\r\nClassic CAN transmission is successful\n");
}
/***********************************************************************************************************************
* End of function canfd_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: can_data_check_operation.
 * Description  : This function compares transmitted/received data and sends ACK accordingly.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void can_data_check_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t execute_time_mcu = RESET_VALUE;

    /* Update data to be compared with data transmitted/received over FD frame */
    can_fd_data_update();

    /* Impact to Board 2: Compare received data with transmitted classic CAN data from Board 1 */
    if (RESET_VALUE == strncmp((char *) &g_canfd_rx_frame.data[0U], (char *) &g_tx_data[0U],
                               CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        printf("\r\nReceived 'TX__MESG' on classic frame, responding with 'RX__MESG' using classic CAN frame\r\n");

        /* Update the receive frame parameters */
        g_canfd_rx_frame.id = CAN_ID;
        g_canfd_rx_frame.type = CAN_FRAME_TYPE_DATA;
        g_canfd_rx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
        g_canfd_rx_frame.options = 0U;

        /* Update receive frame data with message */
        memcpy(&g_canfd_rx_frame.data, &g_rx_data, CAN_CLASSIC_FRAME_DATA_BYTES);

        /* Start measure for Board 2 */
        err = timer_start_measure();
        handle_error(err, "timer_start_measure failed\r\n");

        /* Transmission of data as acknowledgment */
        err = can_fd_write_frame(g_canfd_rx_frame);
        handle_error(err, "can_fd_write_frame failed\r\n");

        printf("\r\nCAN transmission after receive is successful. Sent back the ACK using classic CAN frame\r\n");
    }

    /* Impact to Board 1: Compare received data with the responded classic CAN data from Board 2 */
    else if (RESET_VALUE == strncmp((char *) &g_canfd_rx_frame.data[0U], (char *) &g_rx_data[0U],
                                    CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        printf("\r\nReceived Acknowledgment for Classic CAN Frame transmission.\r\n"
               "CAN operation Successful length = %d\n", g_canfd_rx_frame.data_length_code);
        printf("\r\nData transmission over FD frame");

        /* Update FD frame parameters */
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

        /* Fill frame data that is to be sent in FD frame */
        for (uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 1);
        }

        /* Transmission of data over FD frame */
        err = can_fd_write_frame(g_canfd_rx_frame);
        handle_error(err, "can_fd_write_frame failed\r\n");

        printf("\r\nCAN transmission on FD Frame after receiving classic CAN frame ACK is successful\r\n");
    }

    /* Impact to Board 2: Compare received data with transmitted CANFD data from Board 1 */
    else if (RESET_VALUE == strncmp((char *) &g_canfd_rx_frame.data[0U], (char *) &g_tx_fd_data[0U],
                                    CAN_FD_DATA_LENGTH_CODE))
    {
        printf("\r\nReceived data over FD Frame.\r\nCAN operation Successful, Data length = %d\r\n",
               g_canfd_rx_frame.data_length_code);
        led_update(LED_CASE_2);

        printf("\r\nSending modified data over FD Frame now as acknowledgment for received FD data.\r\n");

        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;

        /* Fill frame data that is to be sent in FD frame */
        for (uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
        {
            g_canfd_rx_frame.data[j] = (uint8_t) (j + 5);
        }

        /* Transmission of data as acknowledgment */
        err = can_fd_write_frame(g_canfd_rx_frame);
        handle_error(err, "can_fd_write_frame failed\r\n");

        /* Get execution time */
        err = timer_get_measure(&execute_time_mcu);
        handle_error(err, "timer_get_measure failed\r\n");

        /* Print CANFD execution time of Board 2 */
        printf("\r\nPerform CANFD operation on Board 2 successfully in %u microseconds\r\n", execute_time_mcu);

        printf("\r\nCAN transmission on FD Frame as acknowledgment is successful\r\n");
        printf(SUB_MENU);
    }

    /* Impact to Board 1: Compare received data with the responded CANFD data from Board 2 */
    else if (RESET_VALUE == strncmp((char *) &g_canfd_rx_frame.data[0U], (char *) &g_rx_fd_data[0U],
                                    CAN_FD_DATA_LENGTH_CODE))
    {
        /* Get execution time */
        err = timer_get_measure(&execute_time_mcu);
        handle_error(err, "timer_get_measure failed\r\n");

        printf("\r\nReceived Acknowledgment for FD Frame.\r\nCAN operation Successful, Data length = %d\r\n",
               g_canfd_rx_frame.data_length_code);

        led_update(LED_CASE_2);

        /* Print CANFD execution time of Board 1 */
        printf("\r\nPerform CANFD operation on Board 1 successfully in %u microseconds\r\n", execute_time_mcu);

        printf(SUB_MENU);
    }
    else /* Wrong MSG received */
    {
        handle_error(FSP_ERR_ASSERTION, "CAN data mismatch\r\nCAN operation failed\r\n");
    }
}
/***********************************************************************************************************************
* End of function can_data_check_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: can_read_operation.
 * Description  : This function reads channel status info and data.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void can_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    bool data_received = false;

    /* Read the input frame, if any is available */
    err = can_fd_read_frame(&g_canfd_rx_frame, &data_received);
    handle_error(err, "can_fd_read_frame failed\r\n");

    if (data_received)
    {
        /* Check if the transmitted and received data are the same and send ACK accordingly */
        can_data_check_operation();
    }
}
/***********************************************************************************************************************
* End of function can_read_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: user_input.
 * Description  : This function reads a line of user input from stdin and removes trailing newline characters.
 * Arguments    : buf                       Pointer to buffer where the input string is stored.
 *                len                       Maximum number of characters to read (buffer size).
 * Return Value : None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}
/***********************************************************************************************************************
* End of function user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_user_input.
 * Description  : This function waits for and reads a line of user input, converted to an integer value.
 * Arguments    : None.
 * Return Value : input_value               User input value.
 **********************************************************************************************************************/
static uint32_t get_user_input(void)
{
    char input_buf[BUFFER_SIZE] = {NULL_CHAR};

    /* Check for user input */
    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    user_input(input_buf, (int) sizeof(input_buf));

    /* Convert input to integer */
    return (uint32_t) atoi(input_buf);
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, turning ON the LED, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        can_fd_deinit();
        timer_deinit();
        led_update(LED_CASE_3);
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
