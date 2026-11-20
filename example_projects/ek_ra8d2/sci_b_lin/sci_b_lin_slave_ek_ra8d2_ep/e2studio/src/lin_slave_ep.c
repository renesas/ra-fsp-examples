/***********************************************************************************************************************
 * File Name    : lin_slave_ep.c
 * Description  : Contains data structures and functions used in lin_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lin_slave_ep.h"

/* Private variables */
static uint8_t g_slave_tx_buf_id_10h[FRAME_ID_10H_DATA_LENGTH] = {0x01, 0x02};
static uint8_t g_slave_tx_buf_id_11h[FRAME_ID_11H_DATA_LENGTH] = {0x03, 0x04};
static uint8_t g_slave_tx_buf_id_12h[FRAME_ID_12H_DATA_LENGTH] = {0x05, 0x06};
static uint8_t g_slave_tx_buf_id_13h[FRAME_ID_13H_DATA_LENGTH] = {0x07, 0x08};

/* Global variables */
uint32_t g_slave_baudrate = LIN_DEFAULT_BAUDRATE;
uint8_t g_slave_rx_buf[MAX_TRANSFER_LENGTH] = {RESET_VALUE};
volatile uint8_t g_received_id = RESET_VALUE;
volatile uint32_t g_lin_event_flags = RESET_VALUE;

lin_transfer_params_t g_lin_slave_transfer_info[MAX_NUMBER_OF_FRAME_ID] =
{
    {READ_FRAME_ID_10H, {.p_data = g_slave_tx_buf_id_10h}, FRAME_ID_10H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
    {READ_FRAME_ID_11H, {.p_data = g_slave_tx_buf_id_11h}, FRAME_ID_11H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
    {READ_FRAME_ID_12H, {.p_data = g_slave_tx_buf_id_12h}, FRAME_ID_12H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
    {READ_FRAME_ID_13H, {.p_data = g_slave_tx_buf_id_13h}, FRAME_ID_13H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},

    {READ_FRAME_ID_20H, {.p_data = g_slave_rx_buf}, FRAME_ID_20H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
    {READ_FRAME_ID_21H, {.p_data = g_slave_rx_buf}, FRAME_ID_21H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
    {READ_FRAME_ID_22H, {.p_data = g_slave_rx_buf}, FRAME_ID_22H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
    {READ_FRAME_ID_23H, {.p_data = g_slave_rx_buf}, FRAME_ID_23H_DATA_LENGTH, LIN_CHECKSUM_TYPE_ENHANCED},
};

/* Private functions */
static fsp_err_t lin_slave_write(const lin_transfer_params_t * const p_transfer_params);
static fsp_err_t lin_slave_read(lin_transfer_params_t * const p_transfer_params);
static fsp_err_t wait_for_event(uint32_t expected_event);

/*******************************************************************************************************************//**
 * @brief     This function initializes LIN Slave module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t lin_slave_open(void)
{
    return LIN_OPEN(&g_slave_ctrl, &g_slave_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened LIN Slave module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lin_slave_close(void)
{
    if (g_slave_ctrl.open)
    {
        LIN_CLOSE(&g_slave_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function transmits a LIN frame.
 * @param[in] p_transfer_params         Pointer to the LIN transfer parameters.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
static fsp_err_t lin_slave_write(const lin_transfer_params_t * const p_transfer_params)
{
    fsp_err_t err = FSP_SUCCESS;

    err = LIN_WRITE(&g_slave_ctrl, p_transfer_params);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return wait_for_event(LIN_EVENT_TX_DATA_COMPLETE);
}

/*******************************************************************************************************************//**
 * @brief      This function receives a LIN frame.
 * @param[out] p_transfer_params         Pointer to the LIN transfer parameters.
 * @retval     FSP_SUCCESS               Received successfully.
 * @retval     Any other error codes     Failed to receive.
 **********************************************************************************************************************/
static fsp_err_t lin_slave_read(lin_transfer_params_t * const p_transfer_params)
{
    fsp_err_t err = FSP_SUCCESS;

    err = LIN_READ(&g_slave_ctrl, p_transfer_params);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return wait_for_event(LIN_EVENT_RX_DATA_COMPLETE);
}

/*******************************************************************************************************************//**
 * @brief     This function transmits a LIN frame with the specified frame ID.
 * @param[in] frame_id_index            Frame ID index.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
fsp_err_t lin_slave_transmit(uint8_t frame_id_index)
{
    if (frame_id_index >= MAX_NUMBER_OF_FRAME_ID)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_TX_DATA_COMPLETE);

    return lin_slave_write(&g_lin_slave_transfer_info[frame_id_index]);
}

/*******************************************************************************************************************//**
 * @brief     This function receives a LIN frame with the specified frame ID.
 * @param[in] frame_id_index            Frame ID index.
 * @retval    FSP_SUCCESS               Received successfully.
 * @retval    Any other error codes     Failed to receive.
 **********************************************************************************************************************/
fsp_err_t lin_slave_receive(uint8_t frame_id_index)
{
    if (frame_id_index >= MAX_NUMBER_OF_FRAME_ID)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    memset(g_slave_rx_buf, RESET_VALUE, sizeof(g_slave_rx_buf));

    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_DATA_COMPLETE);

    return lin_slave_read(&g_lin_slave_transfer_info[frame_id_index]);
}

/*******************************************************************************************************************//**
 * @brief     This function finds the transfer index of the specified frame ID.
 * @param[in] frame_id                  LIN frame ID.
 * @retval    Transfer index            Frame ID found.
 * @retval    -1                        Frame ID not found.
 **********************************************************************************************************************/
int32_t lin_slave_find_transfer_index(uint8_t frame_id)
{
    for (int32_t index = 0; index < (int32_t) MAX_NUMBER_OF_FRAME_ID; index++)
    {
        if (g_lin_slave_transfer_info[index].id == frame_id)
        {
            return index;
        }
    }

    return -1;
}

/*******************************************************************************************************************//**
 * @brief     This function updates the LIN communication baud rate.
 * @param[in] baud_rate                 LIN communication baud rate.
 * @retval    FSP_SUCCESS               Updated successfully.
 * @retval    Any other error codes     Failed to update.
 **********************************************************************************************************************/
fsp_err_t lin_slave_baudset(uint32_t baud_rate)
{
    fsp_err_t err = FSP_SUCCESS;

#if BSP_PERIPHERAL_SAU_PRESENT

    uint32_t break_baud_rate = (9U * baud_rate) / 13U;

    /* Calculate baud rate for normal communication */
    err = LIN_BAUD_CALCULATE(&g_uart0_ctrl,
                             baud_rate,
                             &g_uart0_baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Calculate baud rate for break field */
    err = LIN_BAUD_CALCULATE(&g_uart0_ctrl,
                             break_baud_rate,
                             &g_slave_break_field_baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set calculated baud rate */
    err = R_SAU_UART_BaudSet(&g_uart0_ctrl,
                             &g_uart0_baud_setting);

#elif BSP_PERIPHERAL_SCI_B_PRESENT

    lin_cfg_t lin_slave_cfg;
    sci_b_lin_extended_cfg_t lin_slave_cfg_extend;

    /* Copy LIN configuration */
    memcpy(&lin_slave_cfg,
           &g_slave_cfg,
           sizeof(lin_cfg_t));

    memcpy(&lin_slave_cfg_extend,
           &g_slave_cfg_extend,
           sizeof(sci_b_lin_extended_cfg_t));

    lin_slave_cfg.p_extend = &lin_slave_cfg_extend;

    lin_slave_close();

    /* Set baud parameters */
    sci_b_lin_baud_params_t user_baud_params =
    {
        .baudrate           = baud_rate,
        .clock_source       = g_slave_cfg_extend.sci_b_settings_b.clock_source,
        .break_bits         = g_slave_cfg_extend.break_bits,
        .bus_conflict_clock = g_slave_cfg_extend.sci_b_settings_b.bus_conflict_clock
    };

    /* Calculate the baud rate */
    err = LIN_BAUD_CALCULATE(&user_baud_params,
                             &lin_slave_cfg_extend.baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reinitialize LIN */
    err = LIN_OPEN(&g_slave_ctrl, &lin_slave_cfg);

#elif BSP_PERIPHERAL_SCI_PRESENT

    lin_cfg_t lin_slave_cfg;
    sci_lin_extended_cfg_t lin_slave_cfg_extend;

    /* Copy LIN configuration */
    memcpy(&lin_slave_cfg,
           &g_slave_cfg,
           sizeof(lin_cfg_t));

    memcpy(&lin_slave_cfg_extend,
           &g_slave_cfg_extend,
           sizeof(sci_lin_extended_cfg_t));

    lin_slave_cfg.p_extend = &lin_slave_cfg_extend;

    lin_slave_close();

    /* Set baud parameters */
    sci_lin_baud_params_t user_baud_params =
    {
        .baudrate       = baud_rate,
        .break_bits     = g_slave_cfg_extend.break_bits,
        .delimiter_bits = g_slave_cfg_extend.delimiter_bits,
    };

    /* Calculate the baud rate */
    err = LIN_BAUD_CALCULATE(&user_baud_params,
                             lin_slave_cfg_extend.p_baud_setting);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reinitialize LIN */
    err = LIN_OPEN(&g_slave_ctrl, &lin_slave_cfg);

#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

    if (FSP_SUCCESS == err)
    {
        g_slave_baudrate = baud_rate;
    }

    return err;
}

#if BSP_PERIPHERAL_SAU_PRESENT

/*******************************************************************************************************************//**
 * @brief     This function initializes the low-power mode module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t lin_slave_lpm_open(void)
{
    return R_LPM_Open(&g_lpm_ctrl, &g_lpm_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function closes the low-power mode module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lin_slave_lpm_close(void)
{
    if (g_lpm_ctrl.lpm_open)
    {
        R_LPM_Close(&g_lpm_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function puts the LIN Slave into sleep mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Entered sleep mode successfully.
 * @retval    Any other error codes     Failed to enter sleep mode.
 **********************************************************************************************************************/
fsp_err_t lin_slave_sleep_enter(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear the wake-up event flag */
    g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_WAKEUP_COMPLETE);

    /* Enable LIN wake-up detection */
    err = R_SAU_LIN_SleepEnter(&g_slave_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    R_BSP_SoftwareDelay(50U, BSP_DELAY_UNITS_MILLISECONDS);

    /* Wait for pending UART transmissions to complete */
    uart_console_flush();

    /* Enter software standby mode */
    return R_LPM_LowPowerModeEnter(&g_lpm_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function exits the LIN Slave sleep mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Exited sleep mode successfully.
 * @retval    Any other error codes     Failed to exit sleep mode.
 **********************************************************************************************************************/
fsp_err_t lin_slave_sleep_exit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (g_lin_event_flags & LIN_EVENT_RX_WAKEUP_COMPLETE)
    {
        /* Clear the wake-up event flag */
        g_lin_event_flags &= (uint32_t) (~LIN_EVENT_RX_WAKEUP_COMPLETE);

        /* Exit the LIN bus sleep mode */
        err = R_SAU_LIN_SleepExit(&g_slave_ctrl);
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
    uint32_t timeout_counter = RESET_VALUE;

    while (!(g_lin_event_flags & expected_event))
    {
        timeout_counter++;

        if (timeout_counter >= TIMEOUT_LIMIT)
        {
            err = LIN_COMMUNICATION_ABORT(&g_slave_ctrl);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            return FSP_ERR_TIMEOUT;
        }

        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles LIN communication events.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void lin_slave_callback(lin_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        /* Store the LIN event */
        g_lin_event_flags |= p_args->event;

        if (g_lin_event_flags & LIN_EVENT_RX_HEADER_COMPLETE)
        {
            /* Store the received frame ID */
            g_received_id = p_args->pid & LIN_PID_MASK_ID;
        }
    }
}
