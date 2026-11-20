/***********************************************************************************************************************
 * File Name    : lin_master_ep.c
 * Description  : Contains data structures and functions used in lin_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lin_master_ep.h"

/* Private variables */
static uint8_t g_master_tx_buf_id_20h[FRAME_ID_20H_DATA_LENGTH] = {0x01, 0x02, 0x03};
static uint8_t g_master_tx_buf_id_21h[FRAME_ID_21H_DATA_LENGTH] = {0x11, 0x12, 0x13};
static uint8_t g_master_tx_buf_id_22h[FRAME_ID_22H_DATA_LENGTH] = {0x21, 0x22, 0x23};
static uint8_t g_master_tx_buf_id_23h[FRAME_ID_23H_DATA_LENGTH] = {0x31, 0x32, 0x33};

/* Global variables */
uint32_t g_baudrate = LIN_DEFAULT_BAUDRATE;
uint8_t g_master_rx_buf[MAX_TRANSFER_LENGTH] = {RESET_VALUE};
volatile uint8_t g_received_id = RESET_VALUE;
volatile uint32_t g_lin_event_flags = RESET_VALUE;
volatile bool g_lin_timeout_flag = false;

lin_transfer_params_t g_lin_master_tx_transfer_info[MAX_NUMBER_OF_TX_FRAME_ID] =
{
  {WRITE_FRAME_ID_20H, {.p_data = g_master_tx_buf_id_20h}, FRAME_ID_20H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
  {WRITE_FRAME_ID_21H, {.p_data = g_master_tx_buf_id_21h}, FRAME_ID_21H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
  {WRITE_FRAME_ID_22H, {.p_data = g_master_tx_buf_id_22h}, FRAME_ID_22H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
  {WRITE_FRAME_ID_23H, {.p_data = g_master_tx_buf_id_23h}, FRAME_ID_23H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
};
lin_transfer_params_t g_lin_master_tx_header_transfer[MAX_NUMBER_OF_RX_FRAME_ID] =
{
  {READ_FRAME_ID_10H, {.p_data = NULL}, RESET_VALUE, LIN_CHECKSUM_TYPE_NONE},
  {READ_FRAME_ID_11H, {.p_data = NULL}, RESET_VALUE, LIN_CHECKSUM_TYPE_NONE},
  {READ_FRAME_ID_12H, {.p_data = NULL}, RESET_VALUE, LIN_CHECKSUM_TYPE_NONE},
  {READ_FRAME_ID_13H, {.p_data = NULL}, RESET_VALUE, LIN_CHECKSUM_TYPE_NONE},
};
lin_transfer_params_t g_lin_master_rx_transfer_info[MAX_NUMBER_OF_RX_FRAME_ID] =
{
  {READ_FRAME_ID_10H, {.p_data = g_master_rx_buf}, FRAME_ID_10H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
  {READ_FRAME_ID_11H, {.p_data = g_master_rx_buf}, FRAME_ID_11H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
  {READ_FRAME_ID_12H, {.p_data = g_master_rx_buf}, FRAME_ID_12H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
  {READ_FRAME_ID_13H, {.p_data = g_master_rx_buf}, FRAME_ID_13H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
};

#if BSP_PERIPHERAL_SCI_B_PRESENT
lin_cfg_t lin_master_cfg;
sci_b_lin_extended_cfg_t lin_master_cfg_extend;
#elif BSP_PERIPHERAL_SCI_PRESENT
lin_cfg_t lin_master_cfg;
sci_lin_extended_cfg_t lin_master_cfg_extend;
#endif

/* Private functions */
static fsp_err_t lin_master_write(const lin_transfer_params_t * const p_transfer_params, uint32_t expected_event);
static fsp_err_t lin_master_read(lin_transfer_params_t * const p_transfer_params);
static fsp_err_t wait_for_event(uint32_t expected_event);

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
static inline lin_timing_t lin_master_calculate_timings(uint32_t baudrate, uint8_t n_data);
static fsp_err_t lin_master_set_timeout(uint32_t timeout_us);
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

/***********************************************************************************************************************
 * @brief     This function initializes LIN Master module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t lin_master_open(void)
{
    return LIN_OPEN(&g_master_ctrl, &g_master_cfg);
}

/***********************************************************************************************************************
 * @brief     This function closes the opened LIN Master module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lin_master_close(void)
{
    if (g_master_ctrl.open)
    {
        LIN_CLOSE(&g_master_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function transmits a LIN frame.
 * @param[in] p_transfer_params         Pointer to the LIN transfer parameters.
 * @param[in] expected_event Expected LIN event to wait for.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
static fsp_err_t lin_master_write(const lin_transfer_params_t * const p_transfer_params, uint32_t expected_event)
{
    fsp_err_t err = FSP_SUCCESS;

    err = LIN_WRITE(&g_master_ctrl, p_transfer_params);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return wait_for_event(expected_event);
}

/*******************************************************************************************************************//**
 * @brief     This function receives a LIN frame.
 * @param[in] p_transfer_params         Pointer to the LIN transfer parameters.
 * @retval    FSP_SUCCESS               Received successfully.
 * @retval    Any other error codes     Failed to receive.
 **********************************************************************************************************************/
static fsp_err_t lin_master_read(lin_transfer_params_t * const p_transfer_params)
{
    fsp_err_t err = FSP_SUCCESS;

    err = LIN_READ(&g_master_ctrl, p_transfer_params);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return wait_for_event(LIN_EVENT_RX_DATA_COMPLETE);
}

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
/*******************************************************************************************************************//**
 * @brief     This function initializes the timer used for timeout management.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t timer_open(void)
{
    return TIMER_OPEN(&g_lin_master_timeout_ctrl, &g_lin_master_timeout_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function calculates LIN timing values.
 * @param[in] baudrate                  LIN communication baud rate.
 * @param[in] n_data                    Number of data bytes.
 * @retval    timing                    Calculated timing values.
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

/*******************************************************************************************************************//**
 * @brief     This function sets the timeout and starts the timer.
 * @param[in] timeout_us                Timeout period in microseconds.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
static fsp_err_t lin_master_set_timeout(uint32_t timeout_us)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t period_counts = RESET_VALUE;
    timer_info_t lin_master_timer_info;

    err = TIMER_INFO_GET(&g_lin_master_timeout_ctrl, &lin_master_timer_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    period_counts = (uint32_t)((uint64_t)timeout_us * lin_master_timer_info.clock_frequency / 1000000);

    g_lin_timeout_flag = false;

    err = TIMER_RESET(&g_lin_master_timeout_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = TIMER_PERIOD_SET(&g_lin_master_timeout_ctrl, period_counts);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return TIMER_START(&g_lin_master_timeout_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function closes the timeout timer.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void timer_close(void)
{
    if (g_lin_master_timeout_ctrl.open)
    {
        TIMER_CLOSE(&g_lin_master_timeout_ctrl);
    }
}
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

/*******************************************************************************************************************//**
 * @brief     This function transmits a LIN frame with the specified frame ID.
 * @param[in] frame_id_index            Frame ID index.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
fsp_err_t lin_master_transmit(uint8_t frame_id_index)
{
    fsp_err_t err = FSP_SUCCESS;

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    lin_timing_t lin_timing = lin_master_calculate_timings(g_baudrate,
                                                           g_lin_master_tx_transfer_info[frame_id_index].num_bytes);
    err = lin_master_set_timeout(lin_timing.header_timeout_us + lin_timing.response_timeout_us);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Send the header and data */
    g_lin_event_flags &= (uint32_t)(~LIN_EVENT_TX_DATA_COMPLETE);

    return lin_master_write(&g_lin_master_tx_transfer_info[frame_id_index], LIN_EVENT_TX_DATA_COMPLETE);
}

/*******************************************************************************************************************//**
 * @brief     This function receives a LIN frame with the specified frame ID.
 * @param[in] frame_id_index            Frame ID index.
 * @retval    FSP_SUCCESS               Received successfully.
 * @retval    Any other error codes     Failed to receive.
 **********************************************************************************************************************/
fsp_err_t lin_master_receive(uint8_t frame_id_index)
{
    fsp_err_t err = FSP_SUCCESS;

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    lin_timing_t lin_timing = lin_master_calculate_timings(g_baudrate,
                                                           g_lin_master_rx_transfer_info[frame_id_index].num_bytes);
    err = lin_master_set_timeout(lin_timing.header_timeout_us);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Send the header only */
    g_lin_event_flags &= (uint32_t)(~LIN_EVENT_TX_HEADER_COMPLETE);
    err = lin_master_write(&g_lin_master_tx_header_transfer[frame_id_index], LIN_EVENT_TX_HEADER_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
    err = lin_master_set_timeout(lin_timing.response_timeout_us);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    /* Begin reception of the data frame data */
    g_lin_event_flags &= (uint32_t)(~LIN_EVENT_RX_DATA_COMPLETE);

    return lin_master_read(&g_lin_master_rx_transfer_info[frame_id_index]);
}

/*******************************************************************************************************************//**
 * @brief     This function updates the LIN communication baud rate.
 * @param[in] baud_rate                 LIN communication baud rate.
 * @retval    FSP_SUCCESS               Updated successfully.
 * @retval    Any other error codes     Failed to update.
 **********************************************************************************************************************/
fsp_err_t lin_master_baudset(uint32_t baud_rate)
{
    fsp_err_t err = FSP_SUCCESS;

#if BSP_PERIPHERAL_SAU_PRESENT
    uint32_t breakBaudRate = (9 * baud_rate) / 13;
    /* Calculate baud rate for normal communication */
    err = LIN_BAUD_CALCULATE(&g_uart0_ctrl, baud_rate, &g_uart0_baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Calculate baud rate for break field */
    err = LIN_BAUD_CALCULATE(&g_uart0_ctrl, breakBaudRate, &g_master_break_field_baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set calculated baud rate */
    err = R_SAU_UART_BaudSet(&g_uart0_ctrl, &g_uart0_baud_setting);

#elif BSP_PERIPHERAL_SCI_B_PRESENT

    /* Copy LIN configuration */
    memcpy(&lin_master_cfg, &g_master_cfg, sizeof(lin_cfg_t));
    memcpy(&lin_master_cfg_extend, &g_master_cfg_extend, sizeof(lin_master_cfg_extend));
    lin_master_cfg.p_extend = &lin_master_cfg_extend;

    lin_master_close();

    /* Set baud parameters */
    sci_b_lin_baud_params_t user_baud_params = {
        .baudrate = baud_rate,
        .clock_source = g_master_cfg_extend.sci_b_settings_b.clock_source,
        .break_bits = g_master_cfg_extend.break_bits,
        .bus_conflict_clock = g_master_cfg_extend.sci_b_settings_b.bus_conflict_clock
    };

    /* Calculate the baud rate */
    err = LIN_BAUD_CALCULATE(&user_baud_params, &lin_master_cfg_extend.baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reinitialize LIN */
    err = LIN_OPEN(&g_master_ctrl, &lin_master_cfg);

#elif BSP_PERIPHERAL_SCI_PRESENT

    /* Copy LIN configuration */
    memcpy(&lin_master_cfg, &g_master_cfg, sizeof(lin_cfg_t));
    memcpy(&lin_master_cfg_extend, &g_master_cfg_extend, sizeof(lin_master_cfg_extend));
    lin_master_cfg.p_extend = &lin_master_cfg_extend;

    lin_master_close();

    /* Set baud parameters */
    sci_lin_baud_params_t user_baud_params = {
        .baudrate = baud_rate,
        .break_bits = g_master_cfg_extend.break_bits,
        .delimiter_bits = g_master_cfg_extend.delimiter_bits,
    };

    /* Calculate the baud rate */
    err = LIN_BAUD_CALCULATE(&user_baud_params, lin_master_cfg_extend.p_baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reinitialize LIN */
    err = LIN_OPEN(&g_master_ctrl, &lin_master_cfg);

#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

    return err;
}

#if BSP_PERIPHERAL_SAU_PRESENT
/*******************************************************************************************************************//**
 * @brief     This function sends a LIN wake-up signal.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
fsp_err_t lin_master_send_wakeup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = RESET_VALUE;

    /* Send the LIN wake up signal */
    g_lin_event_flags &= (uint32_t)(~LIN_EVENT_TX_WAKEUP_COMPLETE);
    err = R_SAU_LIN_WakeupSend(&g_master_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

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

/*******************************************************************************************************************//**
 * @brief     This function waits for the specified LIN event.
 * @param[in] expected_event            LIN event to wait for.
 * @retval    FSP_SUCCESS               Completed successfully.
 * @retval    FSP_ERR_TIMEOUT           Timed out.
 * @retval    Any other error codes     Failed to complete.
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
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            return FSP_ERR_TIMEOUT;
        }
    }

    /* Stop the timeout timer */
    err = TIMER_STOP(&g_lin_master_timeout_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#else
    uint32_t timeout_counter = RESET_VALUE;

    while (!(g_lin_event_flags & expected_event))
    {
        timeout_counter++;
        if (timeout_counter >= TIMEOUT_LIMIT)
        {
            err = LIN_COMMUNICATION_ABORT(&g_master_ctrl);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            return FSP_ERR_TIMEOUT;
        }
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);
    }
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

    return err;
}

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
/*******************************************************************************************************************//**
 * @brief     This function handles timer overflow events.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
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

/*******************************************************************************************************************//**
 * @brief     This function handles LIN communication events.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void lin_master_callback(lin_callback_args_t *p_args)
{
    if (p_args != NULL)
    {
        /* Store the event */
        g_lin_event_flags |= p_args->event;
        if (g_lin_event_flags & LIN_EVENT_RX_DATA_COMPLETE)
        {
            /* Store the received ID */
            g_received_id = p_args->pid & LIN_PID_MASK_ID;
        }
    }
}
