/***********************************************************************************************************************
 * File Name    : lin_slave_ep.c
 * Description  : This file implements the LIN Slave functionality for the example project.
 *                It handles LIN frame transmission, reception, and error handling.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lin_slave_ep.h"

/***********************************************************************************************************************
 * Private global variable
 **********************************************************************************************************************/
static volatile uint32_t g_lin_event_flags = RESET_VALUE;

#if BSP_FEATURE_SAU_IS_AVAILABLE
extern sau_uart_baudrate_setting_t g_uart0_baud_setting;
extern sau_uart_baudrate_setting_t g_slave_break_field_baud_setting;
#endif /* BSP_FEATURE_SAU_IS_AVAILABLE */
static volatile uint8_t g_received_id    = RESET_VALUE;
static uint8_t g_rx_buf[TRANSFER_LENGTH] ={RESET_VALUE};

static const uint32_t lin_baudrate_options[LIN_BAUDRATE_COUNT] = {2400, 4800, 9600, 10400, 14400, 19200 };

static uint8_t g_slave_tx_buf_id_10h[FRAME_ID_10H_DATA_LENGTH] = { 0x01, 0x02 };
static uint8_t g_slave_tx_buf_id_11h[FRAME_ID_11H_DATA_LENGTH] = { 0x03, 0x04 };
static uint8_t g_slave_tx_buf_id_12h[FRAME_ID_12H_DATA_LENGTH] = { 0x05, 0x06 };
static uint8_t g_slave_tx_buf_id_13h[FRAME_ID_13H_DATA_LENGTH] = { 0x07, 0x08 };

lin_transfer_params_t lin_slave_transfer_info[] =
{
  { READ_FRAME_ID_10H, {g_slave_tx_buf_id_10h}, FRAME_ID_10H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
  { READ_FRAME_ID_11H, {g_slave_tx_buf_id_11h}, FRAME_ID_11H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
  { READ_FRAME_ID_12H, {g_slave_tx_buf_id_12h}, FRAME_ID_12H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
  { READ_FRAME_ID_13H, {g_slave_tx_buf_id_13h}, FRAME_ID_13H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },

  { READ_FRAME_ID_20H, {g_rx_buf}, FRAME_ID_20H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
  { READ_FRAME_ID_21H, {g_rx_buf}, FRAME_ID_21H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
  { READ_FRAME_ID_22H, {g_rx_buf}, FRAME_ID_22H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
  { READ_FRAME_ID_23H, {g_rx_buf}, FRAME_ID_23H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED },
};

static const uint8_t g_expected_data_id_20h[] = { 0x01, 0x02, 0x03 };
static const uint8_t g_expected_data_id_21h[] = { 0x11, 0x12, 0x13 };
static const uint8_t g_expected_data_id_22h[] = { 0x21, 0x22, 0x23 };
static const uint8_t g_expected_data_id_23h[] = { 0x31, 0x32, 0x33 };

static const lin_frame_expectation_t g_expected_frames[] =
{
    { READ_FRAME_ID_20H, g_expected_data_id_20h, sizeof(g_expected_data_id_20h) },
    { READ_FRAME_ID_21H, g_expected_data_id_21h, sizeof(g_expected_data_id_21h) },
    { READ_FRAME_ID_22H, g_expected_data_id_22h, sizeof(g_expected_data_id_22h) },
    { READ_FRAME_ID_23H, g_expected_data_id_23h, sizeof(g_expected_data_id_23h) },
};

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static fsp_err_t lin_slave_baudset(uint32_t baud_rate);
static int find_lin_transfer_index(uint8_t id);
static fsp_err_t lin_slave_configure_baudrate(void);
static fsp_err_t wait_for_event(uint32_t expected_event);
static void handle_error(fsp_err_t err, char *err_str);
static void lin_validate_frame(uint8_t id, const uint8_t *p_data, size_t len);
#if BSP_FEATURE_SAU_IS_AVAILABLE
static fsp_err_t lin_slave_sleep_enter(void);
static fsp_err_t lin_slave_sleep_exit(void);
#endif /* BSP_FEATURE_SAU_IS_AVAILABLE */

/**********************************************************************************************************************
 * @brief       This function initializes the necessary peripherals and enables the LIN Slave for receiving and
 *              transmitting frames.
 * @param[IN]   None.
 * @retval      None.
 *********************************************************************************************************************/
void lin_slave_operation(void)
{
    fsp_pack_version_t version              = { RESET_VALUE };
    fsp_err_t err                           = FSP_SUCCESS;
    char recv_str[32]                       = {NULL_CHAR};
    char trans_str[32]                      = {NULL_CHAR};
    char *ptr                               = NULL;
    uint8_t terminal_read[TERM_BUFFER_SIZE] = { RESET_VALUE };
    int id_index                            = RESET_VALUE;

#if (USE_VIRTUAL_COM == 1U)
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        ERROR_TRAP;
    }
#endif /* USE_VIRTUAL_COM */

    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open the LIN instance with initial configuration. */
    err = LIN_OPEN (&g_slave_ctrl, &g_slave_cfg);
    handle_error (err, "Error: LIN initialization failed.\r\n");

#if BSP_FEATURE_SAU_IS_AVAILABLE
    err = R_LPM_Open (&g_lpm_ctrl, &g_lpm_cfg);
    handle_error (err, "Error: LPM initialization failed.\r\n");
#endif /* BSP_FEATURE_SAU_IS_AVAILABLE */

    /* Print Main Menu option */
    APP_PRINT(MAIN_MENU);
    while (true)
    {
        /* Check for new input data from terminal */
        if (APP_CHECK_KEY)
        {
            /* Get user input */
            memset(terminal_read, NULL_CHAR, sizeof(terminal_read));
            APP_READ(terminal_read, TERMINAL_READ_SIZE);

            /* Handle Main Menu */
            switch (terminal_read[MENU_INDEX])
            {
                /* Transmit Start Frame and Receive Info Frame */
                case SLAVE_SET_BAUDRATE:
                {
                    /* Configure the LIN baud rate */
                    err = lin_slave_configure_baudrate ();
                    handle_error (err, "get_user_baudrate FAILED\r\n");
                    break;
                }
                #if BSP_FEATURE_SAU_IS_AVAILABLE
                case SLAVE_ENTER_LPM:
                {
                    err = lin_slave_sleep_enter();
                    handle_error (err, "lin_slave_sleep_enter FAILED\r\n");

                    err = lin_slave_sleep_exit();
                    handle_error (err, "lin_slave_sleep_exit FAILED\r\n");
                    break;
                }
                #endif /* BSP_FEATURE_SAU_IS_AVAILABLE */

                    /* Invalid input */
                default:
                {
                    APP_PRINT("\r\nInvalid input. Provide a valid input\r\n");
                    break;
                }
            }
            /* Reprint the main menu */
            APP_PRINT(MAIN_MENU);

        }

        if (g_lin_event_flags & LIN_EVENT_RX_START_FRAME_COMPLETE)
        {
            g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_START_FRAME_COMPLETE);
            id_index = find_lin_transfer_index (g_received_id);

            if (id_index >= MIN_INDEX_OF_FRAME_ID_WRITE && id_index <= MAX_INDEX_OF_FRAME_ID_WRITE)
            {
                memset (g_rx_buf, RESET_VALUE, sizeof(g_rx_buf));

                /* Begin reception of the information frame data */
                g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_INFORMATION_FRAME_COMPLETE);
                err = LIN_INFO_FRAME_READ (&g_slave_ctrl, &lin_slave_transfer_info[id_index]);
                handle_error (err, "Error: Reading LIN information frame failed.\r\n");


                /* Wait for information frame reception to complete */
                err = wait_for_event (LIN_EVENT_RX_INFORMATION_FRAME_COMPLETE);
                handle_error (err, "Error: RX Event timeout!\r\n");

                ptr = recv_str;
                for (size_t i = 0; i < lin_slave_transfer_info[id_index].num_bytes; i++)
                {
                    ptr += sprintf (ptr, "0x%02x ", g_rx_buf[i]);
                }

                /* Display Slave's received data on RTT */
                APP_PRINT("\r\nSlave received data with ID=0x%x: %s\r\n", g_received_id, recv_str);
                lin_validate_frame(g_received_id,g_rx_buf,lin_slave_transfer_info[id_index].num_bytes);

            }
            else if (id_index >= MIN_INDEX_OF_FRAME_ID_READ && id_index <= MAX_INDEX_OF_FRAME_ID_READ)
            {
                g_lin_event_flags &= (uint32_t) (~LIN_EVENT_TX_INFORMATION_FRAME_COMPLETE);
                err = LIN_INFO_FRAME_WRITE (&g_slave_ctrl, &lin_slave_transfer_info[id_index]);
                handle_error (err, "Error: Writing LIN information frame failed.\r\n");

                /* Wait for information frame transmission to complete */
                err = wait_for_event (LIN_EVENT_TX_INFORMATION_FRAME_COMPLETE);
                handle_error (err, "Error: TX Event timeout!\r\n");

                ptr = trans_str;
                for (size_t i = 0; i < lin_slave_transfer_info[id_index].num_bytes; i++)
                {
                    ptr += sprintf (ptr, "0x%02x ", lin_slave_transfer_info[id_index].p_information[i]);
                }

                /* Display Slave's transmitted data on terminal */
                APP_PRINT("\r\nSlave transmitted data with ID=0x%x: %s\r\n", g_received_id, trans_str);
            }
            else
            {
                /* Skip for this frame ID */
            }
        }
    }
}

/**********************************************************************************************************************
 * @brief       Validate the received LIN frame against the expected frame data.
 * @param[IN]   id       The ID of the received frame.
 * @param[IN]   p_data   Pointer to the received frame data.
 * @param[IN]   len      The length of the received frame data.
 * @retval      None
 *********************************************************************************************************************/
static void lin_validate_frame(uint8_t id, const uint8_t *p_data, size_t len)
{
    for (size_t i = 0; i < sizeof(g_expected_frames) / sizeof(g_expected_frames[0]); ++i)
    {
        const lin_frame_expectation_t *p_frame = &g_expected_frames[i];

        /* Check for length mismatch */
        if (p_frame->id == id)
        {
            if (len != p_frame->length)
            {
                APP_PRINT("\r\nFrame 0x%02X: Length mismatch. "
                          "Expected %d bytes, received %d bytes\r\n", id, p_frame->length, len);
                return;
            }

            /* Validate the received data against the expected data */
            if (memcmp(p_data, p_frame->expected, len) == 0)
            {
                APP_PRINT("Frame 0x%02X: Data validation successful\r\n", id);
            }
            else
            {
                APP_PRINT("Frame 0x%02X: Data mismatch detected\r\n", id);
            }
            return;  /* Exit after processing the relevant frame */
        }
    }
}

/**********************************************************************************************************************
 * @brief       This function retrieves the LIN baud rate and updates the baud rate configuration in the LIN driver.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS if the baud rate is successfully configured, otherwise an error code is returned.
 *********************************************************************************************************************/
static fsp_err_t lin_slave_configure_baudrate(void)
{
    uint8_t terminal_read[TERM_BUFFER_SIZE] = { RESET_VALUE };
    uint8_t selection = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;

    APP_PRINT(BAUDRATE_OPTION);
    while (true)
    {
        /* Check for new input data from terminal */
        if (APP_CHECK_KEY)
        {
            /* Get user input */
            memset(terminal_read, NULL_CHAR, sizeof(terminal_read));
            APP_READ(terminal_read, TERMINAL_READ_SIZE);

            selection = (uint8_t) (terminal_read[0] - '0');

            if (selection >= MIN_BAUDRATE_INDEX && selection <= LIN_BAUDRATE_COUNT)
            {
                APP_PRINT("\r\nSelected baud rate: %d bps\r\n", lin_baudrate_options[selection - 1]);
                err = lin_slave_baudset (lin_baudrate_options[selection - 1]);
                APP_ERR_RET(err, err, "Failed to set LIN baud rate.\r\n");

                APP_PRINT("\r\nLIN baud rate successfully updated.\r\n");
                break;
            }
            else
            {
                APP_PRINT("\r\nInvalid selection. Please enter a number between 1 and 6.\r\n");
            }

        }
    }

    return err;
}

/**********************************************************************************************************************
 * @brief       This function finds the index of a LIN transfer based on the given frame ID.
 *              and receiving frames.
 * @param[IN]   id: The LIN frame ID to search for.
 * @retval      Index of the LIN transfer if found; otherwise, returns -1.
 *********************************************************************************************************************/
static int find_lin_transfer_index(uint8_t id)
{
    for (int i = 0; i < MAX_NUMBER_OF_FRAME_ID; i++)
    {
        if (lin_slave_transfer_info[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

/***********************************************************************************************************************
 * @brief       Callback function to handle LIN communication events.
 * @param[IN]   p_args: Pointer to the callback arguments
 * @retval      None
 ***********************************************************************************************************************/
void lin_slave_callback(lin_callback_args_t *p_args)
{
    if (p_args != NULL)
    {
        /* Store the event */
        g_lin_event_flags |= p_args->event;
        if (g_lin_event_flags & LIN_EVENT_RX_START_FRAME_COMPLETE)
        {
            /* Store the received ID */
            g_received_id = p_args->pid & LIN_PID_MASK_ID;
        }
    }
}


#if BSP_FEATURE_SAU_IS_AVAILABLE

/***********************************************************************************************************************
 * @brief       Put the LIN slave into sleep mode and enter low-power mode.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned
 ***********************************************************************************************************************/
static fsp_err_t lin_slave_sleep_enter(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open IRQ to prepare getting LIN_EVENT_RX_WAKEUP_COMPLETE */
    err = R_SAU_LIN_SleepEnter (&g_slave_ctrl);
    APP_ERR_RET(err, err, "\r\nR_SAU_LIN_SleepEnter FAILED\r\n");

    APP_PRINT("\r\nEntering software standby mode...\r\n");
    R_BSP_SoftwareDelay (50, BSP_DELAY_UNITS_MILLISECONDS);

    err = R_LPM_LowPowerModeEnter (&g_lpm_ctrl);
    APP_ERR_RET(err, err, "\r\nR_LPM_LowPowerModeEnter FAILED\r\n");

    return err;
}

/***********************************************************************************************************************
 * @brief       Exit the bus sleep mode for LIN device.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned
 ***********************************************************************************************************************/
static fsp_err_t lin_slave_sleep_exit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (g_lin_event_flags & LIN_EVENT_RX_WAKEUP_COMPLETE)
    {
        g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_WAKEUP_COMPLETE);
        APP_PRINT("\r\nWake-up signal detected. Exiting sleep mode.\r\n");

        /* Exit the bus sleep mode for LIN device.*/
        err = R_SAU_LIN_SleepExit (&g_slave_ctrl);
        APP_ERR_RET(err, err, "\r\nR_SAU_LIN_SleepExit FAILED\r\n");
    }

    return err;

}
#endif /* BSP_FEATURE_SAU_IS_AVAILABLE */

/***********************************************************************************************************************
 * @brief       This function waits for an event flag to be set with timeout handling.
 * @param[IN]   expected_event The LIN event bitmask to wait for
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned
 ***********************************************************************************************************************/
static fsp_err_t wait_for_event(uint32_t expected_event)
{
    uint32_t timeout = RESET_VALUE;
    fsp_err_t err    = FSP_SUCCESS;

    while (!(g_lin_event_flags & expected_event))
    {
        timeout++;
        if (timeout >= TIMEOUT_LIMIT)
        {
            err = LIN_COMMUNICATION_ABORT(&g_slave_ctrl);
            APP_ERR_RET(err, err, "\r\nError: Failed to Abort LIN communication\r\n");

            return FSP_ERR_TIMEOUT;
        }

        R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
    }

    return err;
}

/***********************************************************************************************************************
 * @brief       Calculates the LIN baud rate and restarts the LIN module.
 * @param[IN]   baud_rate: The desired LIN baud rate.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned
 ***********************************************************************************************************************/
static fsp_err_t lin_slave_baudset(uint32_t baud_rate)
{
    fsp_err_t err = FSP_SUCCESS;

#if BSP_FEATURE_SAU_IS_AVAILABLE
    uint32_t breakBaudRate = (9 * baud_rate) / 13;
    /* Calculate baud rate for normal communication */
    err = LIN_BAUD_CALCULATE (&g_uart0_ctrl, baud_rate, &g_uart0_baud_setting);
    APP_ERR_RET(err, err, "Error: Failed to calculate LIN baud rate\r\n");

    /* Calculate baud rate for break field */
    err = LIN_BAUD_CALCULATE (&g_uart0_ctrl, breakBaudRate, &g_slave_break_field_baud_setting);
    APP_ERR_RET(err, err, "Error: Failed to calculate LIN baud rate\r\n");

    /* Set calculated baud rate */
    err = R_SAU_UART_BaudSet(&g_uart0_ctrl, &g_slave_break_field_baud_setting);
    APP_ERR_RET(err, err, "Error: Failed to set LIN baud rate\r\n");

#elif defined(BSP_FEATURE_SCI_IS_AVAILABLE)

    lin_cfg_t lin_slave_cfg;
    sci_b_lin_extended_cfg_t lin_slave_cfg_extend;

    /* Copy LIN configuration */
    memcpy (&lin_slave_cfg, &g_slave_cfg, sizeof(lin_cfg_t));
    memcpy (&lin_slave_cfg_extend, &g_slave_cfg_extend, sizeof(lin_slave_cfg_extend));
    lin_slave_cfg.p_extend = &lin_slave_cfg_extend;

    err = LIN_CLOSE(&g_slave_ctrl);
    APP_ERR_RET(err, err, "Error: LIN deinitialization  failed.\r\n");

    /* Set baud parameters */
    sci_b_lin_baud_params_t user_baud_params = {
        .baudrate = baud_rate,
        .clock_source = g_slave_cfg_extend.sci_b_settings_b.clock_source,
        .break_bits = g_slave_cfg_extend.break_bits,
        .bus_conflict_clock = g_slave_cfg_extend.sci_b_settings_b.bus_conflict_clock
    };

    /* Calculate the baud rate */
    err = LIN_BAUD_CALCULATE(&user_baud_params, &lin_slave_cfg_extend.baud_setting);
    APP_ERR_RET(err,err,"Failed to calculate LIN baud rate\r\n");

    /* Reinitialize LIN */
    err = LIN_OPEN(&g_slave_ctrl, &lin_slave_cfg);
    APP_ERR_RET(err, err, "Error: LIN initialization failed.\r\n");

#endif /* BSP_FEATURE_SAU_IS_AVAILABLE || BSP_FEATURE_SCI_IS_AVAILABLE*/
    return err;

}

/***********************************************************************************************************************
 * @brief       Close all modules and perform error trapping if an error occurs.
 * @param[IN]   err     Return values from the API calls.
 * @param[IN]   err_str Error message from the failed API call.
 * @retval      None
 ***********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    fsp_err_t close_status = FSP_SUCCESS;

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(err_str);

        if (RESET_VALUE != g_slave_ctrl.open)
        {
            close_status = LIN_CLOSE (&g_slave_ctrl);
            if (FSP_SUCCESS != close_status)
            {
                APP_ERR_PRINT("Error: LIN close failed.\r\n");
            }
        }

#if BSP_FEATURE_SAU_IS_AVAILABLE
        if (RESET_VALUE != g_lpm_ctrl.lpm_open)
        {
            close_status = R_LPM_Close (&g_lpm_ctrl);
            if (FSP_SUCCESS != close_status)
            {
                APP_ERR_PRINT("Error: LPM close failed.\r\n");
            }

        }
#endif
        APP_ERR_TRAP(err);
    }

}

/***********************************************************************************************************************
* End of lin_slave_ep.c
***********************************************************************************************************************/
