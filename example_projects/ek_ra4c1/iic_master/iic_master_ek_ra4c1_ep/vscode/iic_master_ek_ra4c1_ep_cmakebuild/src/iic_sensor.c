/***********************************************************************************************************************
 * File Name    : iic_sensor.c
 * Description  : Contains data structures and functions used in iic_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "iic_sensor.h"

/* I2C event flag, set in callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

/* Private functions */
static fsp_err_t iic_init(void);
static fsp_err_t iic_write(uint8_t * p_data, uint32_t bytes, bool restart);
static fsp_err_t iic_read(uint8_t * p_data, uint32_t bytes, bool restart);
static fsp_err_t get_device_id(uint8_t * dev_id);
static fsp_err_t validate_iic_event(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes IIC Master module and configures Pmod ACL sensor.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized and configured successfully.
 * @retval    Any other error codes     Failed to initialize or configure.
 **********************************************************************************************************************/
fsp_err_t init_sensor(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t device_id = RESET_VALUE;
    uint8_t measure_enable_payload[MEASURE_PAYLOAD_SIZE] = {POWER_CTL_REG, ENABLE_BIT};

    /* Open IIC Master module */
    err = iic_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get device ID as a start of communication */
    err = get_device_id(&device_id);

    if ((FSP_SUCCESS == err) && (DEVICE_SIGNATURE == device_id))
    {
        /* Access to POWER_CTL register (0x2D) and enable measurement */
        err = iic_write(measure_enable_payload, TWO_BYTE, false);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function reads X, Y, Z axis data from the Pmod ACL sensor.
 * @param[in] xyz_data                  Pointer to buffer where sensor data will be stored (6 bytes).
 * @retval    FSP_SUCCESS               Read sensor data successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t read_sensor_data(uint8_t * xyz_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t adxl_data[ADXL_DATA_SIZE] = {RESET_VALUE};
    uint8_t internal_reg = AXIS_DATA;

    if (NULL == xyz_data)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Write to accelerometer data register */
    err = iic_write(&internal_reg, ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read 6 bytes of axis data */
    err = iic_read(adxl_data, DATA_REGISTERS, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Update the X, Y, Z values to output buffer */
    for (uint8_t itr = RESET_VALUE; itr < SENSOR_DATA_SIZE; itr++)
    {
        *(xyz_data + itr) = adxl_data[itr];
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function reads the device signature/ID from the sensor.
 * @param[in] dev_id                    Pointer to store the device ID.
 * @retval    FSP_SUCCESS               Got device ID successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
static fsp_err_t get_device_id(uint8_t * dev_id)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_address = DEVICE_ID_REG;

    /* Parameter checking */
    if (NULL == dev_id)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Write register address to sensor */
    err = iic_write(&reg_address, ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read device ID */
    err = iic_read(dev_id, ONE_BYTE, false);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the IIC Master module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
static fsp_err_t iic_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open IIC Master module */
    err = IIC_MASTER_OPEN(&g_i2c_master_ctrl, &g_i2c_master_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes bytes over IIC and waits for the transfer to complete.
 * @param[in] p_data                    Pointer to transmit buffer.
 * @param[in] bytes                     Number of bytes to write.
 * @param[in] restart                   true = repeated-start; false = stop condition.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t iic_write(uint8_t * p_data, uint32_t bytes, bool restart)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Write data over IIC */
    err = IIC_MASTER_WRITE(&g_i2c_master_ctrl, p_data, bytes, restart);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return validate_iic_event();
}

/*******************************************************************************************************************//**
 * @brief     This function reads bytes over IIC and waits for the transfer to complete.
 * @param[in] p_data                    Pointer to receive buffer.
 * @param[in] bytes                     Number of bytes to read.
 * @param[in] restart                   true = repeated-start; false = stop condition.
 * @retval    FSP_SUCCESS               Transfer completed successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
static fsp_err_t iic_read(uint8_t * p_data, uint32_t bytes, bool restart)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read data over IIC */
    err = IIC_MASTER_READ(&g_i2c_master_ctrl, p_data, bytes, restart);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return validate_iic_event();
}

/*******************************************************************************************************************//**
 * @brief     This function validates I2C receive/transmit completion based on expected write/read event.
 * @param[in] None.
 * @retval    FSP_SUCCESS                 Transferred successfully.
 * @retval    FSP_ERR_TRANSFER_ABORTED    Returned on timeout or when I2C_MASTER_EVENT_ABORTED is received.
 **********************************************************************************************************************/
static fsp_err_t validate_iic_event(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t local_time_out = UINT16_MAX;

    /* Reset event flag before polling */
    i2c_event = (i2c_master_event_t) RESET_VALUE;

    do
    {
        /* Prevent infinite loop */
        --local_time_out;
        if (RESET_VALUE == local_time_out)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }
    }
    while (i2c_event == RESET_VALUE);

    if (i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        /* Always reset before returning */
        i2c_event = (i2c_master_event_t) RESET_VALUE;
        err = FSP_SUCCESS;
    }
    else
    {
        /* Always reset before returning */
        i2c_event = (i2c_master_event_t) RESET_VALUE;
        err = FSP_ERR_TRANSFER_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened IIC Master module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_sensor(void)
{
    /* Close the opened IIC Master module */
    if (g_i2c_master_ctrl.open)
    {
        IIC_MASTER_CLOSE(&g_i2c_master_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the IIC Master module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        i2c_event = p_args->event;
    }
}
