/***********************************************************************************************************************
 * File Name    : gt911.c
 * Description  : Contains data structures and functions used in gt911.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "gt911.h"

/* Private functions */
static fsp_err_t wait_for_i2c_event(i2c_master_event_t event);
static fsp_err_t gt911_i2c_write(uint8_t * buf, uint32_t len);
static fsp_err_t gt911_i2c_read(uint8_t * buf, uint32_t len);

/* Private variable */
static volatile i2c_master_event_t s_i2c_master_event;

/*******************************************************************************************************************//**
 * @brief      This function reads data from the gt911 touch controller.
 * @param[out] status                   Pointer to store the status register returned over I2C.
 * @param[out] points                   Pointer to store the coordinate values returned over I2C.
 * @param[in]  num_points               Number of coordinate points to read.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t gt911_get_status(uint8_t * status, coord_t * points, uint32_t num_points)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Buffer to read status along with co-ordinates for 5 touch points as mentioned in
     * GT911 Programming Guide */
    uint8_t g_i2c_rx_buffer[1 + (sizeof(gt911_point_payload_t)) * 5];
    uint8_t g_i2c_tx_buffer[BUFFER_LENGTH] = {RESET_VALUE};

    g_i2c_tx_buffer[0] = (GTP_READ_COOR_ADDR & 0xFF00) >> 8;
    g_i2c_tx_buffer[1] = (GTP_READ_COOR_ADDR & 0xFF);
    g_i2c_tx_buffer[2] = RESET_VALUE;

    memset(&g_i2c_rx_buffer[0], 0, sizeof(g_i2c_rx_buffer));

    /* Read buffer status */
    err = gt911_i2c_write((uint8_t *) &g_i2c_tx_buffer[0], GTP_ADDR_LENGTH);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = gt911_i2c_read(g_i2c_rx_buffer, sizeof(g_i2c_rx_buffer));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Copy in the number of points expected by the upper layer */
    *status = g_i2c_rx_buffer[0];

    if (num_points >= 5)
    {
        memcpy(&points[4], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t) * 4 + 1], sizeof(coord_t));
    }
    if (num_points >= 4)
    {
        memcpy(&points[3], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t) * 3 + 1], sizeof(coord_t));
    }
    if (num_points >= 3)
    {
        memcpy(&points[2], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t) * 2 + 1], sizeof(coord_t));
    }
    if (num_points >= 2)
    {
        memcpy(&points[1], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t) * 1 + 1], sizeof(coord_t));
    }
    if (num_points >= 1)
    {
        memcpy(&points[0], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t) * 0 + 1], sizeof(coord_t));
    }

    /* Clear buffer status */
    err = gt911_i2c_write((uint8_t *) &g_i2c_tx_buffer[0], (GTP_ADDR_LENGTH + 1));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to the I2C slave device.
 * @param[in] buf                       Pointer to the write address and data buffer.
 * @param[in] len                       Length of the data buffer.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t gt911_i2c_write(uint8_t * buf, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    s_i2c_master_event = (i2c_master_event_t) RESET_VALUE;

    /* Write data to the I2C Slave device */
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, buf, len, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until write transmission complete */
    err = wait_for_i2c_event(I2C_MASTER_EVENT_TX_COMPLETE);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from the I2C Slave device.
 * @param[out] buf                      Pointer to the buffer used to store the received data.
 * @param[in]  len                      Length of the data buffer.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
static fsp_err_t gt911_i2c_read(uint8_t * buf, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    s_i2c_master_event = (i2c_master_event_t) RESET_VALUE;

    /* Read data back from the I2C Slave */
    err = R_IIC_MASTER_Read(&g_i2c_master_ctrl, buf, len, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until read transmission complete */
    err = wait_for_i2c_event(I2C_MASTER_EVENT_RX_COMPLETE);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for I2C Master event.
 * @param[in] event                     I2C Master event to wait for.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t wait_for_i2c_event(i2c_master_event_t event)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_ICLK) / 10;

    while (event != s_i2c_master_event)
    {
        if (I2C_MASTER_EVENT_ABORTED == s_i2c_master_event)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

        timeout--;

        if (0U == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for I2C Master module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        s_i2c_master_event = p_args->event;
    }
}
