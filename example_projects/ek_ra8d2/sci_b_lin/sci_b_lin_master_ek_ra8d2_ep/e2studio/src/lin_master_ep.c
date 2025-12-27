/***********************************************************************************************************************
 * File Name    : lin_master_ep.c
 * Description  : This file implements the LIN Master functionality for the example project.
 *                It handles LIN frame transmission, reception, and error handling.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lin_master_ep.h"
#include "common_utils.h"

/***********************************************************************************************************************
 * Private global variable
 **********************************************************************************************************************/
static volatile uint32_t g_lin_event_flags = RESET_VALUE;
static volatile bool g_lin_timeout_flag = false;
static uint32_t g_baudrate = LIN_DEFAULT_BAUDRATE;

static uint8_t g_master_rx_buf[MAX_TRANSFER_LENGTH] = {RESET_VALUE};
static volatile uint8_t g_received_id = RESET_VALUE;

static const uint8_t tx_frame_id[MAX_NUMBER_OF_TX_FRAME_ID] = {WRITE_FRAME_ID_20H, WRITE_FRAME_ID_21H,
                                                               WRITE_FRAME_ID_22H, WRITE_FRAME_ID_23H};

static const uint8_t rx_frame_id[MAX_NUMBER_OF_RX_FRAME_ID] = {READ_FRAME_ID_10H, READ_FRAME_ID_11H,
                                                               READ_FRAME_ID_12H, READ_FRAME_ID_13H};

static const uint32_t lin_baudrate_options[LIN_BAUDRATE_COUNT]  = {2400, 4800, 9600, 10400, 14400, 19200};
static uint8_t g_master_tx_buf_id_20h[FRAME_ID_20H_DATA_LENGTH] = {0x01, 0x02, 0x03};
static uint8_t g_master_tx_buf_id_21h[FRAME_ID_21H_DATA_LENGTH] = {0x11, 0x12, 0x13};
static uint8_t g_master_tx_buf_id_22h[FRAME_ID_22H_DATA_LENGTH] = {0x21, 0x22, 0x23};
static uint8_t g_master_tx_buf_id_23h[FRAME_ID_23H_DATA_LENGTH] = {0x31, 0x32, 0x33};

static lin_transfer_params_t lin_master_tx_transfer_info[MAX_NUMBER_OF_TX_FRAME_ID] =
{
 {WRITE_FRAME_ID_20H, {g_master_tx_buf_id_20h}, FRAME_ID_20H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
 {WRITE_FRAME_ID_21H, {g_master_tx_buf_id_21h}, FRAME_ID_21H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
 {WRITE_FRAME_ID_22H, {g_master_tx_buf_id_22h}, FRAME_ID_22H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
 {WRITE_FRAME_ID_23H, {g_master_tx_buf_id_23h}, FRAME_ID_23H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
};

static lin_transfer_params_t lin_master_rx_transfer_info[MAX_NUMBER_OF_RX_FRAME_ID] =
{
 {READ_FRAME_ID_10H, {g_master_rx_buf}, FRAME_ID_10H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
 {READ_FRAME_ID_11H, {g_master_rx_buf}, FRAME_ID_11H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
 {READ_FRAME_ID_12H, {g_master_rx_buf}, FRAME_ID_12H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
 {READ_FRAME_ID_13H, {g_master_rx_buf}, FRAME_ID_13H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
};

static const uint8_t g_expected_data_id_20h[] = {0x01, 0x02};
static const uint8_t g_expected_data_id_21h[] = {0x03, 0x04};
static const uint8_t g_expected_data_id_22h[] = {0x05, 0x06};
static const uint8_t g_expected_data_id_23h[] = {0x07, 0x08};

static const lin_frame_expectation_t g_expected_frames[] =
{
 {READ_FRAME_ID_10H, g_expected_data_id_20h, sizeof(g_expected_data_id_20h)},
 {READ_FRAME_ID_11H, g_expected_data_id_21h, sizeof(g_expected_data_id_21h)},
 {READ_FRAME_ID_12H, g_expected_data_id_22h, sizeof(g_expected_data_id_22h)},
 {READ_FRAME_ID_13H, g_expected_data_id_23h, sizeof(g_expected_data_id_23h)},
};


#if BSP_PERIPHERAL_SAU_PRESENT
    extern sau_uart_baudrate_setting_t g_uart0_baud_setting;
    extern sau_uart_baudrate_setting_t g_master_break_field_baud_setting;
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str);
static fsp_err_t lin_master_write(uint8_t frame_id);
static fsp_err_t lin_master_read(uint8_t frame_id);
static fsp_err_t lin_master_baudset(uint32_t baud_rate);
static fsp_err_t wait_for_event(uint32_t expected_event);
static fsp_err_t handle_lin_master_transmit(bool is_write);
static void lin_validate_frame(uint8_t id, const uint8_t *p_data, size_t len);
#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
static timer_info_t lin_master_timer_info =
{
    .count_direction = RESET_VALUE,
    .clock_frequency = RESET_VALUE,
    .period_counts   = RESET_VALUE
};
static inline lin_timing_t lin_master_calculate_timings(uint32_t baudrate, uint8_t n_data);
static fsp_err_t lin_master_set_timeout(uint32_t timeout_us);
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */
static fsp_err_t lin_master_configure_baudrate(void);

#if BSP_PERIPHERAL_SAU_PRESENT
static fsp_err_t lin_master_send_wakeup(void);
#elif BSP_PERIPHERAL_SCI_B_PRESENT
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

/***********************************************************************************************************************
 * @brief       This function initializes the necessary peripherals and enables the LIN Master for transmitting 
 *              and receiving frames.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void lin_master_operation(void)
{
    fsp_err_t err                           = FSP_SUCCESS;
    uint8_t terminal_read[TERM_BUFFER_SIZE] = {RESET_VALUE};
    fsp_pack_version_t version              = {RESET_VALUE};

    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        ERROR_TRAP;
    }

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(EP_INFO);

    /* Open LIN Master interface */
    err = LIN_OPEN(&g_master_ctrl, &g_master_cfg);
    handle_error(err, "Error: LIN initialization failed.\r\n");

    /* Configure timeout management if enabled */
#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    err = TIMER_OPEN(&g_lin_master_timeout_ctrl, &g_lin_master_timeout_cfg);
    handle_error(err, "Error: Timer initialization failed.\r\n");
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Print main menu option */
    APP_PRINT(MAIN_MENU);
    while (true)
    {
        /* Check new input data from terminal */
        if (APP_CHECK_KEY)
        {
            /* Get user input */
            memset(terminal_read, NULL_CHAR, sizeof(terminal_read));
            APP_READ(terminal_read, TERMINAL_READ_SIZE);

            /* Handle main menu */
            switch (terminal_read[MENU_INDEX])
            {
                case MASTER_SET_BAUDRATE:
                {
                    /* Configure the LIN baud rate */
                    err = lin_master_configure_baudrate();
                    handle_error(err, "Error: Failed to configure LIN baud rate.\r\n");

                    /* Reprint the main menu */
                    APP_PRINT(MAIN_MENU);
                    break;
                }

                case MASTER_WRITE:
                {
                    err = handle_lin_master_transmit(IS_WRITE);
                    handle_error(err, "Error: Failed to write LIN message.\r\n");

                    /* Reprint the main menu after the write operation */
                    APP_PRINT(MAIN_MENU);
                    break;
                }

                case MASTER_READ:
                {
                    err = handle_lin_master_transmit(IS_READ);
                    handle_error(err, "Error: Failed to read LIN message.\r\n");

                    /* Reprint the main menu after the read operation */
                    APP_PRINT(MAIN_MENU);
                    break;
                }

                #if BSP_PERIPHERAL_SAU_PRESENT
                case WAKE_UP_SLAVE:
                {
                    err = lin_master_send_wakeup();
                    handle_error(err, "Error: Failed to send a wake-up signal.\r\n");
                    /* Reprint the main menu after send a wake-up signal */
                    APP_PRINT(MAIN_MENU);
                    break;
                }
                #endif /* BSP_PERIPHERAL_SAU_PRESENT */

                    /* Invalid input */
                default:
                {
                    APP_PRINT("\r\nInvalid input. Provide a valid input\r\n");
                    APP_PRINT(MAIN_MENU);
                    break;
                }
            }
        }
    }
}

/***********************************************************************************************************************
 * @brief       This function handles LIN frame transmission or reception based on user selection.
 * @param[in]   is_write    Operation mode (1: transmit, 0: receive).
 * @retval      FSP_SUCCESS if operation successful, otherwise an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t handle_lin_master_transmit(bool is_write)
{
    uint8_t terminal_read[TERM_BUFFER_SIZE] = {RESET_VALUE};
    uint8_t selection                       = RESET_VALUE;
    fsp_err_t err                           = FSP_SUCCESS;

    APP_PRINT(is_write ? WRITE_MENU : READ_MENU);
    while (true)
    {
        if (APP_CHECK_KEY)
        {
            /* Get user input */
            memset(terminal_read, NULL_CHAR, sizeof(terminal_read));
            APP_READ(terminal_read, TERMINAL_READ_SIZE);

            selection = terminal_read[MENU_INDEX] - '0';
            /* Exit loop if user selects '0' */
            if (EXIT_INDEX == selection)
                break;

            if (selection >= MIN_NUMBER_OF_RX_FRAME_ID && selection <= MAX_NUMBER_OF_RX_FRAME_ID)
            {
                err = is_write ? lin_master_write (selection - 1) : lin_master_read (selection - 1);
                APP_ERR_RET(FSP_SUCCESS != err, err,
                            is_write ? "lin_master_write failed.\r\n" : "lin_master_read failed.\r\n");
            }
            else
            {
                APP_PRINT("\r\nInvalid selection. Please enter a number between 0 and 4.\r\n");
            }
        }
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       This function retrieves the LIN baud rate and updates the baud rate configuration in the LIN driver.
 * @param[in]   None.
 * @retval      FSP_SUCCESS if the baud rate is successfully configured, otherwise an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t lin_master_configure_baudrate(void)
{
    uint8_t terminal_read[TERM_BUFFER_SIZE] = {RESET_VALUE};
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
                g_baudrate = lin_baudrate_options[selection - 1];
                APP_PRINT("\r\nSelected baud rate: %d bps\r\n", g_baudrate);

                err = lin_master_baudset(g_baudrate);
                APP_ERR_RET(FSP_SUCCESS != err, err, "Failed to set LIN baud rate.\r\n");

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

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
/***********************************************************************************************************************
 * @brief       This function calculates LIN timing values based on the baud rate and data length.
 * @param[in]   baudrate    The LIN communication baud rate.
 * @param[in]   n_data      The data length of the LIN frame.
 * @retval      The calculated LIN timing values.
 **********************************************************************************************************************/
static inline lin_timing_t lin_master_calculate_timings(uint32_t baudrate, uint8_t n_data)
{
    lin_timing_t timing;
    uint32_t t_bit_us = 1000000U / baudrate;

    /* Refer to LIN Protocol Specification, Revision 2.2A, for THeader_Maximum and TResponse_Maximum */
    /* 1.4 × 34 × Tbit */
    timing.header_timeout_us = (34U * t_bit_us * LIN_TIMEOUT_FACTOR) / LIN_TIMEOUT_DIVISOR;
    /* 1.4 × 10 × (Ndata + 1) × Tbit */
    timing.response_timeout_us = (10U * (n_data + 1U) * t_bit_us * LIN_TIMEOUT_FACTOR) / LIN_TIMEOUT_DIVISOR;

    return timing;
}
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

/***********************************************************************************************************************
 * @brief       This function executes the LIN write operation and manages the timeout if enabled.
 * @param[in]   frame_id_index    The ID of the LIN frame to be transmitted.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t lin_master_write(uint8_t frame_id_index)
{
    fsp_err_t err = FSP_SUCCESS;

    #if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    lin_timing_t lin_timing = lin_master_calculate_timings(g_baudrate,
                                                           lin_master_tx_transfer_info[frame_id_index].num_bytes);
    err = lin_master_set_timeout(lin_timing.header_timeout_us);
    APP_ERR_RET(FSP_SUCCESS != err, err, "lin_master_set_timeout failed!\r\n");
    #endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Send the LIN start frame: break, sync, and protected identifier */
    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_TX_START_FRAME_COMPLETE);
    err = LIN_START_FRAME_WRITE(&g_master_ctrl, tx_frame_id[frame_id_index]);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Writing LIN start frame failed.\r\n");

    /* Wait for start frame transmission to complete before sending the information frame */
    err = wait_for_event(LIN_EVENT_TX_START_FRAME_COMPLETE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: TX Event timeout!\r\n");

    #if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    err = lin_master_set_timeout(lin_timing.response_timeout_us);
    APP_ERR_RET(FSP_SUCCESS != err, err, "lin_master_set_timeout failed!\r\n");
    #endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_TX_INFORMATION_FRAME_COMPLETE);
    err = LIN_INFO_FRAME_WRITE(&g_master_ctrl, &lin_master_tx_transfer_info[frame_id_index]);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Writing LIN information frame failed.\r\n");

    /* Wait for information frame transmission to complete */
    err = wait_for_event(LIN_EVENT_TX_INFORMATION_FRAME_COMPLETE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: TX Event timeout!\r\n");

    APP_PRINT("\r\nMaster writes message with frame ID=0x%x successfully\r\n", tx_frame_id[frame_id_index]);

    return err;
}

/***********************************************************************************************************************
 * @brief       This function executes the LIN read operation and manages the timeout if enabled.
 * @param[in]   frame_id_index  The ID of the LIN frame to be received.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t lin_master_read(uint8_t frame_id_index)
{
    fsp_err_t err     = FSP_SUCCESS;
    char recv_str[32] = {'\0'};
    char *ptr         = NULL;

    #if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    lin_timing_t lin_timing = lin_master_calculate_timings(g_baudrate,
                                                           lin_master_rx_transfer_info[frame_id_index].num_bytes);

    err = lin_master_set_timeout(lin_timing.header_timeout_us);
    APP_ERR_RET(FSP_SUCCESS != err, err, "lin_master_set_timeout failed!\r\n");
    #endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Send the LIN start frame: break, sync, and protected identifier */
    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_TX_START_FRAME_COMPLETE);
    err = LIN_START_FRAME_WRITE(&g_master_ctrl, rx_frame_id[frame_id_index]);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Writing LIN start frame failed.\r\n");

    /* Wait for start frame transmission to complete before sending the information frame */
    err = wait_for_event(LIN_EVENT_TX_START_FRAME_COMPLETE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: TX Event timeout!\r\n");

    #if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    err = lin_master_set_timeout(lin_timing.response_timeout_us);
    APP_ERR_RET(FSP_SUCCESS != err, err, "lin_master_set_timeout failed!\r\n");
    #endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Begin reception of the information frame data */
    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_INFORMATION_FRAME_COMPLETE);
    err = LIN_INFO_FRAME_READ(&g_master_ctrl, &lin_master_rx_transfer_info[frame_id_index]);

    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Reading LIN information frame failed.\r\n");

    /* Wait for information frame transmission to complete */
    err = wait_for_event(LIN_EVENT_RX_INFORMATION_FRAME_COMPLETE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: RX Event timeout!\r\n");

    ptr = recv_str;
    for (size_t i = 0; i < lin_master_rx_transfer_info[frame_id_index].num_bytes; i++)
    {
        ptr += sprintf(ptr, "0x%02x ", g_master_rx_buf[i]);
    }

    APP_PRINT("\r\nMaster received data of ID=0x%x: %s\r\n", g_received_id, recv_str);
    lin_validate_frame(rx_frame_id[frame_id_index], g_master_rx_buf,
                       lin_master_rx_transfer_info[frame_id_index].num_bytes);

    return err;

}

/***********************************************************************************************************************
 * @brief       Validate the received LIN frame against the expected frame data.
 * @param[in]   id       The ID of the received frame.
 * @param[in]   p_data   Pointer to the received frame data.
 * @param[in]   len      The length of the received frame data.
 * @retval      None.
 **********************************************************************************************************************/
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

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
/***********************************************************************************************************************
 * @brief       Sets the LIN timeout and starts the timer.
 * @param[in]   timeout_us  The timeout duration in microseconds.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t lin_master_set_timeout(uint32_t timeout_us)
{
    fsp_err_t err          = FSP_SUCCESS;
    uint32_t period_counts = RESET_VALUE;

    err = TIMER_INFO_GET(&g_lin_master_timeout_ctrl, &lin_master_timer_info);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to get timer information\r\n");

    period_counts = (uint32_t) (timeout_us * (lin_master_timer_info.clock_frequency / 1000000));

    g_lin_timeout_flag = false;

    err = TIMER_RESET(&g_lin_master_timeout_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to reset timer\r\n");

    err = TIMER_PERIOD_SET(&g_lin_master_timeout_ctrl, period_counts);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to set timer period\r\n");

    err = TIMER_START(&g_lin_master_timeout_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to start timer\r\n");

    return err;
}
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

/***********************************************************************************************************************
 * @brief       Calculates the LIN baud rate and restarts the LIN module.
 * @param[in]   baud_rate: The desired LIN baud rate.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t lin_master_baudset(uint32_t baud_rate)
{
    fsp_err_t err = FSP_SUCCESS;

#if BSP_PERIPHERAL_SAU_PRESENT

    uint32_t breakBaudRate = (9 * baud_rate) / 13;
    /* Calculate baud rate for normal communication */
    err = LIN_BAUD_CALCULATE(&g_uart0_ctrl, baud_rate, &g_uart0_baud_setting);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to calculate LIN baud rate\r\n");

    /* Calculate baud rate for break field */
    err = LIN_BAUD_CALCULATE(&g_uart0_ctrl, breakBaudRate, &g_master_break_field_baud_setting);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to calculate LIN baud rate\r\n");

    /* Set calculated baud rate */
    err = R_SAU_UART_BaudSet(&g_uart0_ctrl, &g_master_break_field_baud_setting);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to set LIN baud rate\r\n");

#elif BSP_PERIPHERAL_SCI_B_PRESENT

    lin_cfg_t lin_master_cfg;
    sci_b_lin_extended_cfg_t lin_master_cfg_extend;

    /* Copy LIN configuration */
    memcpy(&lin_master_cfg, &g_master_cfg, sizeof(lin_cfg_t));
    memcpy(&lin_master_cfg_extend, &g_master_cfg_extend, sizeof(lin_master_cfg_extend));
    lin_master_cfg.p_extend = &lin_master_cfg_extend;

    err = LIN_CLOSE(&g_master_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: LIN de-initialization failed.\r\n");

    /* Set baud parameters */
    sci_b_lin_baud_params_t user_baud_params = {
        .baudrate = baud_rate,
        .clock_source = g_master_cfg_extend.sci_b_settings_b.clock_source,
        .break_bits = g_master_cfg_extend.break_bits,
        .bus_conflict_clock = g_master_cfg_extend.sci_b_settings_b.bus_conflict_clock
    };

    /* Calculate the baud rate */
    err = LIN_BAUD_CALCULATE(&user_baud_params, &lin_master_cfg_extend.baud_setting);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Failed to calculate LIN baud rate\r\n");

    /* Reinitialize LIN */
    err = LIN_OPEN(&g_master_ctrl, &lin_master_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: LIN initialization failed.\r\n");

#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

    return err;
}

#if BSP_PERIPHERAL_SAU_PRESENT
/***********************************************************************************************************************
 * @brief       This function sends the LIN wake-up signal.
 * @param[in]   None.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned.
 **********************************************************************************************************************/
fsp_err_t lin_master_send_wakeup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = RESET_VALUE;

    /* Send the LIN wake-up signal */
    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_TX_WAKEUP_COMPLETE);
    err = R_SAU_LIN_WakeupSend(&g_master_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to send LIN wake-up signal.\r\n");

    while (!(g_lin_event_flags & LIN_EVENT_TX_WAKEUP_COMPLETE))
    {
        timeout++;
        if (timeout >= TIMEOUT_LIMIT)
        {
            return FSP_ERR_TIMEOUT;
        }

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }

    return err;
}
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

/***********************************************************************************************************************
 * @brief       This function waits for an event flag to be set with timeout handling.
 * @param[in]   expected_event  The LIN event bitmask to wait for.
 * @retval      FSP_SUCCESS if the operation was successful; otherwise, an error code is returned.
 **********************************************************************************************************************/
static fsp_err_t wait_for_event(uint32_t expected_event)
{
    fsp_err_t err = FSP_SUCCESS;
#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    /* Wait for event or timeout flag */
    while (!(g_lin_event_flags & expected_event))
    {
        if (g_lin_timeout_flag)
        {
            err = LIN_COMMUNICATION_ABORT(&g_master_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nError: Failed to Abort LIN communication\r\n");

            return FSP_ERR_TIMEOUT;
        }
    }

    /* Stop the timeout timer */
    err = TIMER_STOP(&g_lin_master_timeout_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Error: Failed to stop Timer\r\n");

#else
    uint32_t timeout_counter = RESET_VALUE;

    while (!(g_lin_event_flags & expected_event))
    {
        timeout_counter++;
        if (timeout_counter >= TIMEOUT_LIMIT)
        {
            err = LIN_COMMUNICATION_ABORT(&g_master_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nError: Failed to Abort LIN communication\r\n");

            return FSP_ERR_TIMEOUT;
        }
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);
    }
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    return err;
}

/***********************************************************************************************************************
 * @brief       Close all modules and perform error trapping if an error occurs.
 * @param[in]   err         Return values from the API calls.
 * @param[in]   err_str     Error message from the failed API call.
 * @retval      None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    fsp_err_t close_status = FSP_SUCCESS;

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(err_str);

        if (RESET_VALUE != g_master_ctrl.open)
        {
            close_status = LIN_CLOSE(&g_master_ctrl);
            if (FSP_SUCCESS != close_status)
            {
                APP_ERR_PRINT("Error: LIN close failed.\r\n");
            }
        }

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
        if (RESET_VALUE != g_lin_master_timeout_ctrl.open)
        {
            close_status = TIMER_CLOSE(&g_lin_master_timeout_ctrl);
            if (FSP_SUCCESS != close_status)
            {
                APP_ERR_PRINT("Error: Timer close failed.\r\n");
            }

        }
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */
        APP_ERR_TRAP(err);
    }

}

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
/***********************************************************************************************************************
 * @brief       Callback function to handle the timer overflow event for the LIN Master.
 * @param[in]   p_args      Pointer to the callback arguments.
 * @retval      None.
 **********************************************************************************************************************/
void lin_master_overflow_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* Set timeout flag when cycle ends */
        g_lin_timeout_flag = true;
    }
}
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

/***********************************************************************************************************************
 * @brief       Callback function to handle LIN communication events.
 * @param[in]   p_args      Pointer to the callback arguments.
 * @retval      None.
 **********************************************************************************************************************/
void lin_master_callback(lin_callback_args_t *p_args)
{
    if (p_args != NULL)
    {
        /* Store the event */
        g_lin_event_flags |= p_args->event;
        if (g_lin_event_flags & LIN_EVENT_RX_INFORMATION_FRAME_COMPLETE)
        {
            /* Store the received ID */
            g_received_id = p_args->pid & LIN_PID_MASK_ID;
        }
    }
}

/***********************************************************************************************************************
* End of lin_master_ep.c
***********************************************************************************************************************/
