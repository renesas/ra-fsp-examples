/***********************************************************************************************************************
 * File Name    : iic_slave_ep.c
 * Description  : Contains data structures and functions used in iic_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "iic_slave_ep.h"

/* Private variables */
static uint8_t s_slave_tx_buf[BUF_LEN] = {RESET_VALUE};
static uint8_t s_slave_rx_buf[BUF_LEN] = {RESET_VALUE};
static uint8_t s_master_buf[BUF_LEN] = {RESET_VALUE};
static volatile i2c_master_event_t s_master_event = (i2c_master_event_t)RESET_VALUE;
static volatile i2c_slave_event_t s_slave_event = (i2c_slave_event_t)RESET_VALUE;
static volatile fsp_err_t s_slave_api_ret_err = FSP_SUCCESS;

/* Private function */
static fsp_err_t validate_iic_event(i2c_master_event_t master_expected, i2c_slave_event_t slave_expected);

/*******************************************************************************************************************//**
 * @brief     This function initializes IIC Master and Slave drivers.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_i2c_driver(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open IIC Master module */
    err = IIC_MASTER_OPEN(&g_i2c_master_ctrl, &g_i2c_master_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open IIC Slave module */
    err = IIC_SLAVE_OPEN(&g_i2c_slave_ctrl, g_i2c_slave.p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs IIC Slave write operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t i2c_slave_write(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t write_buffer[BUF_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05};

    /* Update the Slave transmit buffer and clear the Master buffer (recipient) */
    memset(s_master_buf, RESET_VALUE, BUF_LEN);
    memcpy(s_slave_tx_buf, write_buffer, BUF_LEN);

    /* Reset callback events */
    s_master_event = (i2c_master_event_t)RESET_VALUE;
    s_slave_event = (i2c_slave_event_t)RESET_VALUE;

    /* Start a Master read operation to initiate the transfer */
    err = IIC_MASTER_READ(&g_i2c_master_ctrl, s_master_buf, BUF_LEN, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until the Slave write and Master read processes complete */
    err = validate_iic_event(I2C_MASTER_EVENT_RX_COMPLETE, I2C_SLAVE_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Compare the Slave write buffer with the data received by the Master device */
    if (BUFF_EQUAL == memcmp(s_slave_tx_buf, s_master_buf, BUF_LEN))
    {
        return FSP_SUCCESS;
    }
    else
    {
        return FSP_ERR_ABORTED;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs IIC Slave read operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t i2c_slave_read(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_buffer[BUF_LEN] = {0x10, 0x20, 0x30, 0x40, 0x50};

    /* Update the Master transmit buffer and clear the Slave RX buffer (recipient) */
    memset(s_slave_rx_buf, RESET_VALUE, BUF_LEN);
    memcpy(s_master_buf, read_buffer, BUF_LEN);

    /* Reset callback events */
    s_master_event = (i2c_master_event_t)RESET_VALUE;
    s_slave_event = (i2c_slave_event_t)RESET_VALUE;

    /* Master write to Slave */
    err = IIC_MASTER_WRITE(&g_i2c_master_ctrl, s_master_buf, BUF_LEN, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until the Slave read and Master write processes complete */
    err = validate_iic_event(I2C_MASTER_EVENT_TX_COMPLETE, I2C_SLAVE_EVENT_RX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Compare the Slave read buffer with the data written by the Master device */
    if (BUFF_EQUAL == memcmp(s_slave_rx_buf, s_master_buf, BUF_LEN))
    {
        return FSP_SUCCESS;
    }
    else
    {
        return FSP_ERR_ABORTED;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function validates IIC transfer completion using expected Master and Slave events.
 * @param[in] master_expected           Expected IIC Master event.
 * @param[in] slave_expected            Expected IIC Slave event.
 * @retval    FSP_SUCCESS               Transferred successfully.
 * @retval    Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static fsp_err_t validate_iic_event(i2c_master_event_t master_expected, i2c_slave_event_t slave_expected)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t time_out = UINT16_MAX;

    while ((slave_expected != s_slave_event) || (master_expected != s_master_event))
    {
        if ((I2C_MASTER_EVENT_ABORTED == s_master_event) || (I2C_SLAVE_EVENT_ABORTED == s_slave_event))
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

        /* Handle Slave API error stored in s_slave_api_ret_err.
         * This variable is updated only when a Slave API call returns an error. */
        else if (FSP_SUCCESS != s_slave_api_ret_err)
        {
            err = s_slave_api_ret_err;

            s_slave_api_ret_err = FSP_SUCCESS;

            return err;
        }

        else
        {
            --time_out;
            if (RESET_VALUE == time_out)
            {
                return FSP_ERR_TIMEOUT;
            }
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the IIC Master.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        s_master_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the IIC Slave.
 * @param[in] p_args                    Pointer to the callback argument.
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
                err = IIC_SLAVE_READ(&g_i2c_slave_ctrl, s_slave_rx_buf, BUF_LEN);
                if (FSP_SUCCESS != err)
                {
                    s_slave_api_ret_err = err;
                }
                break;
            }

            case I2C_SLAVE_EVENT_TX_REQUEST:
            {
                err = IIC_SLAVE_WRITE(&g_i2c_slave_ctrl, s_slave_tx_buf, BUF_LEN);
                if (FSP_SUCCESS != err)
                {
                    s_slave_api_ret_err = err;
                }
                break;
            }

            default:
            {
                s_slave_event = p_args->event;
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened IIC Master and Slave modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_i2c_driver(void)
{
    /* Close the opened IIC Master module */
    if (g_i2c_master_ctrl.open)
    {
        IIC_MASTER_CLOSE(&g_i2c_master_ctrl);
    }

    /* Close the opened IIC Slave module */
    if (g_i2c_slave_ctrl.open)
    {
        IIC_SLAVE_CLOSE(&g_i2c_slave_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function blinks the on-board LED.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void blink_led(void)
{
    set_led(LED_ON);
    R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    set_led(LED_OFF);
}

/*******************************************************************************************************************//**
 * @brief     This function turns the on-board LED ON or OFF.
 * @param[in] led_state                 LED state.
 * @retval    None.
 **********************************************************************************************************************/
void set_led(bsp_io_level_t led_state)
{
    /* Get LED pin information for this board */
    bsp_leds_t leds = g_bsp_leds;

    for (uint8_t cnt = RESET_VALUE; leds.led_count > cnt; cnt++)
    {
       R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], led_state);
    }
}
