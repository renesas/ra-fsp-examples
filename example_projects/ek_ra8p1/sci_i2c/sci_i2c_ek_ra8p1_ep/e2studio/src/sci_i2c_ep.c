/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.c
 * Description  : Contains data structures and functions used in sci_i2c_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sci_i2c_ep.h"

/* Private variables */
static volatile i2c_master_event_t s_master_event = (i2c_master_event_t)RESET_VALUE;
static volatile i2c_slave_event_t s_slave_event = (i2c_slave_event_t)RESET_VALUE;
static volatile fsp_err_t s_slave_api_ret_err = FSP_SUCCESS;
static uint8_t s_slave_tx_buf[BUFF_LEN] = {RESET_VALUE};
static uint8_t s_slave_rx_buf[BUFF_LEN] = {RESET_VALUE};

/* Private function */
static fsp_err_t sci_i2c_wait_for_event(i2c_master_event_t master_event, i2c_slave_event_t slave_event);

/*******************************************************************************************************************//**
 * @brief     This function initializes the SCI I2C Master and IIC Slave modules.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_i2c(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the SCI I2C Master module */
    err = SCI_I2C_OPEN(&g_sci_i2c_master_ctrl, &g_sci_i2c_master_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize the IIC Slave module */
    err = IIC_SLAVE_OPEN(&g_i2c_slave_ctrl, &g_i2c_slave_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs the Master write operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t sci_i2c_master_write(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update the Master transmit buffer */
    uint8_t write_buffer[BUFF_LEN] = {0x10, 0x20, 0x30, 0x40, 0x50};

    /* Clear the Slave receive buffer before starting the transfer */
    memset(s_slave_rx_buf, RESET_VALUE, BUFF_LEN);

    /* Reset callback event */
    s_master_event = (i2c_master_event_t)RESET_VALUE;
    s_slave_event = (i2c_slave_event_t)RESET_VALUE;

    /* Start Master write operation */
    err = SCI_I2C_WRITE(&g_sci_i2c_master_ctrl, write_buffer, BUFF_LEN, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the Master write and Slave read operations to complete */
    err = sci_i2c_wait_for_event(I2C_MASTER_EVENT_TX_COMPLETE, I2C_SLAVE_EVENT_RX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (BUFF_EQUAL != memcmp(s_slave_rx_buf, write_buffer, BUFF_LEN))
    {
        err = FSP_ERR_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs the Master read operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t sci_i2c_master_read(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_buffer[BUFF_LEN] = {RESET_VALUE};

    /* Update the Slave transmit buffer with the data received during the previous write operation */
    memset(s_slave_tx_buf, RESET_VALUE, BUFF_LEN);
    memcpy(s_slave_tx_buf, s_slave_rx_buf, BUFF_LEN);

    /* Reset callback event */
    s_master_event = (i2c_master_event_t)RESET_VALUE;
    s_slave_event = (i2c_slave_event_t)RESET_VALUE;

    /* Start the Master read operation to initiate the transfer */
    err = SCI_I2C_READ(&g_sci_i2c_master_ctrl, read_buffer, BUFF_LEN, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the Slave write and Master read operations to complete */
    err = sci_i2c_wait_for_event(I2C_MASTER_EVENT_RX_COMPLETE, I2C_SLAVE_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (BUFF_EQUAL != memcmp(read_buffer, s_slave_tx_buf, BUFF_LEN))
    {
        err = FSP_ERR_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the expected Master and Slave events to occur.
 * @param[in] master_event              Master event to wait for.
 * @param[in] slave_event               Slave event to wait for.
 * @retval    FSP_SUCCESS               Both events occurred successfully.
 * @retval    Any other error codes     Failed to receive the expected events.
 **********************************************************************************************************************/
static fsp_err_t sci_i2c_wait_for_event(i2c_master_event_t master_event, i2c_slave_event_t slave_event)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t time_out = UINT16_MAX;

    while ((master_event != s_master_event) || (slave_event != s_slave_event))
    {
        /* Check for aborted event */
        if ((I2C_SLAVE_EVENT_ABORTED == s_slave_event) || (I2C_MASTER_EVENT_ABORTED == s_master_event))
        {
            /* I2C transaction failure */
            return FSP_ERR_TRANSFER_ABORTED;
        }
        else if (FSP_SUCCESS != s_slave_api_ret_err)
        {
            /* Slave API returned an error inside the callback, capture and reset it */
            err = s_slave_api_ret_err;
            s_slave_api_ret_err = FSP_SUCCESS;
            return err;
        }
        else
        {
            /* Start checking for time out to avoid infinite loop */
            time_out--;
            if (RESET_VALUE == time_out)
            {
                return FSP_ERR_TIMEOUT;
            }
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is the callback for the SCI I2C Master module.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void sci_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        s_master_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is the callback for the I2C Slave module.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void i2c_slave_callback(i2c_slave_callback_args_t * p_args)
{
    fsp_err_t err = FSP_SUCCESS;

    if (NULL != p_args)
    {
        switch (p_args->event)
        {
            case I2C_SLAVE_EVENT_RX_REQUEST:
            {
                err = IIC_SLAVE_READ(&g_i2c_slave_ctrl, s_slave_rx_buf, BUFF_LEN);
                if (FSP_SUCCESS != err)
                {
                    s_slave_api_ret_err = err;
                }
                break;
            }

            case I2C_SLAVE_EVENT_TX_REQUEST:
            {
                err = IIC_SLAVE_WRITE(&g_i2c_slave_ctrl, s_slave_tx_buf, BUFF_LEN);
                if (FSP_SUCCESS != err)
                {
                    s_slave_api_ret_err = err;
                }
                break;
            }

            default:
            {
                /* Store the event in the global variable for any other Slave event */
                s_slave_event = p_args->event;
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function toggles the on-board LEDs.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void toggle_led(void)
{
    bsp_leds_t leds = g_bsp_leds;

    for (uint8_t cnt = RESET_VALUE; leds.led_count > cnt; cnt++)
    {
        R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], (bsp_io_level_t)LED_ON);
        R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], (bsp_io_level_t)LED_OFF);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function turns the on-board LEDs ON or OFF.
 * @param[in] led_status                Expected LED status (ON or OFF).
 * @retval    None.
 **********************************************************************************************************************/
void set_led(bsp_io_level_t led_status)
{
    bsp_leds_t leds = g_bsp_leds;

    for (uint8_t cnt = RESET_VALUE; leds.led_count > cnt; cnt++)
    {
        R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], led_status);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened SCI I2C Master module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_sci_i2c(void)
{
    if (g_sci_i2c_master_ctrl.open)
    {
        SCI_I2C_CLOSE(&g_sci_i2c_master_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened IIC Slave module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_iic(void)
{
    if (g_i2c_slave_ctrl.open)
    {
        IIC_SLAVE_CLOSE(&g_i2c_slave_ctrl);
    }
}
