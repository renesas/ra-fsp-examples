/***********************************************************************************************************************
 * File Name    : gt911.c
 * Description  : Contains data structures and functions setup gt911 used in mipi_dsi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "hal_data.h"
#include "common_utils.h"
#include "gt911.h"

/*******************************************************************************************************************//**
 * @addtogroup gt911
 * @{
 **********************************************************************************************************************/

/* global variable */
volatile i2c_master_event_t    g_i2c_master_event;

/* User defined functions */
static fsp_err_t wait_for_i2c_event (i2c_master_event_t event);
static fsp_err_t gt911_i2c_write(uint8_t *buf, uint32_t len);
static fsp_err_t gt911_i2c_read(uint8_t *buf, uint32_t len);

/*******************************************************************************************************************//**
 *  @brief       This function is used to read data from gt911 device.
 *  @param[out]  status      : Store status register returned over I2C.
 *  @param[out]  points      : Store co-ordinate values returned over I2C.
 *  @param[in]  num_points      : Store buffer data.
 *  @retval      FSP_SUCCESS : Upon successful operation, otherwise: failed
 **********************************************************************************************************************/
fsp_err_t gt911_get_status(uint8_t* status, coord_t * points, uint32_t num_points)
{
    /* Buffer to read status along with co-ordinates for 5 touchpoints as mentioned in
     * GT911 Programming Guide. */
    uint8_t g_i2c_rx_buffer[1 + (sizeof(gt911_point_payload_t)) * 5];
    fsp_err_t err = FSP_SUCCESS;
    uint8_t   g_i2c_tx_buffer[BUFFER_LENGTH] = {RESET_VALUE};

    g_i2c_tx_buffer[0] = (GTP_READ_COOR_ADDR & 0xFF00) >> 8;
    g_i2c_tx_buffer[1] = (GTP_READ_COOR_ADDR & 0xFF);
    g_i2c_tx_buffer[2] = RESET_VALUE;

    memset(&g_i2c_rx_buffer[0], 0, sizeof(g_i2c_rx_buffer));

    /* Read buffer status */
    err = gt911_i2c_write ((uint8_t*) &g_i2c_tx_buffer[0], GTP_ADDR_LENGTH);
    APP_ERR_RETURN(err, " ** gt911_i2c_write function FAILED ** \r\n");

    err = gt911_i2c_read (g_i2c_rx_buffer, sizeof(g_i2c_rx_buffer));
    APP_ERR_RETURN(err, " ** gt911_i2c_read function FAILED ** \r\n");

    /* Copy in the number of points expected by the upper layer */
    *status = g_i2c_rx_buffer[0];

    if (num_points >= 5)
    {
        memcpy(&points[4], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t)*4 + 1], sizeof(coord_t));
    }
    if (num_points >= 4)
    {
        memcpy(&points[3], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t)*3 + 1], sizeof(coord_t));
    }
    if (num_points >= 3)
    {
        memcpy(&points[2], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t)*2 + 1], sizeof(coord_t));
    }
    if (num_points >= 2)
    {
        memcpy(&points[1], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t)*1 + 1], sizeof(coord_t));
    }
    if (num_points >= 1)
    {
        memcpy(&points[0], &g_i2c_rx_buffer[1 + sizeof(gt911_point_payload_t)*0 + 1], sizeof(coord_t));
    }


    /* Clear buffer status*/
    err = gt911_i2c_write ((uint8_t*) &g_i2c_tx_buffer[0], (GTP_ADDR_LENGTH + 1));
    APP_ERR_RETURN(err, " ** gt911_i2c_write function FAILED ** \r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to write data to the i2c slave device.
 *
 *  @param[in]   buf[0~1]    : write start address.
 *  @param[in]   buf[2~len-1]: write data buffer
 *  @param[in]   len         : length of data buffer.
 *  @retval      FSP_SUCCESS : Upon successful operation, otherwise: failed
 **********************************************************************************************************************/
static fsp_err_t gt911_i2c_write(uint8_t *buf, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;

    /* write data to the i2c slave device */
    g_i2c_master_event = I2C_MASTER_EVENT_ABORTED;
    err = R_IIC_MASTER_Write (&g_i2c_master_ctrl, buf, len, false);
    APP_ERR_RETURN(err, " ** IIC MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = wait_for_i2c_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RETURN(err, " ** I2C master event timeout ** \r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to read data from the i2c slave device.
 *
 *  @param[in]   buf[0~1]    : read start address.
 *  @param[in]   buf[2~len-1]: read data buffer.
 *  @param[in]   len         : length of data buffer.
 *  @retval      FSP_SUCCESS : Upon successful operation, otherwise: failed
 **********************************************************************************************************************/
static fsp_err_t gt911_i2c_read(uint8_t *buf, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read data back from the I2C slave */
    g_i2c_master_event = I2C_MASTER_EVENT_ABORTED;
    err = R_IIC_MASTER_Read(&g_i2c_master_ctrl, buf, len, false);
    APP_ERR_RETURN(err, " ** IIC MASTER_Read API FAILED  ** \r\n");

    /* Wait until write receive complete */
    err = wait_for_i2c_event (I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RETURN(err, " ** I2C master event timeout ** \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is used to Wait for i2c event.
 *
 * @param[in]   event       : Expected events
 * @retval      FSP_SUCCESS : Upon successful operation, otherwise: failed
 **********************************************************************************************************************/
static fsp_err_t wait_for_i2c_event (i2c_master_event_t event)
{
    uint32_t timeout = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_ICLK) / 10;
    while (--timeout && (g_i2c_master_event != event))
    {
        ;
    }
    return timeout ? FSP_SUCCESS : FSP_ERR_TIMEOUT;
}

/*******************************************************************************************************************//**
 * @brief      Callback functions for i2c master interrupts
 *
 * @param[in]  p_args    Callback arguments
 * @retval     none
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_i2c_master_event = p_args->event;
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup gt911)
 **********************************************************************************************************************/
