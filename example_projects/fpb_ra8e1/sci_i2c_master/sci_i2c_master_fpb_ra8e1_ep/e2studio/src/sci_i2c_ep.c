/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.c
 * Description  : Contains data structures and functions for sci_i2c_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sci_i2c_ep.h"

/* Global variables */
volatile i2c_master_event_t g_sci_i2c_event = I2C_MASTER_EVENT_ABORTED;
volatile bool g_b_sensor_ready_flag = false;

/* Private functions */
static fsp_err_t sensor_init_hardware(void);
static fsp_err_t sensor_init_software(void);
static fsp_err_t sensor_write_reg(uint8_t addr, uint8_t value);
static fsp_err_t sensor_read_reg(uint8_t addr, uint8_t * p_value);
static fsp_err_t sensor_set_reg(uint8_t addr, uint8_t value);
static fsp_err_t sensor_read_data(uint8_t * p_data);
static fsp_err_t sci_i2c_wait_event(i2c_master_event_t event);
static fsp_err_t sensor_wait_data_ready(void);
static fsp_err_t sensor_convert_to_g_unit(uint8_t * p_data, float * p_x_axis, float * p_y_axis, float * p_z_axis);

/*******************************************************************************************************************//**
 * @brief      This function reads and retrieves sensor axis data in g units.
 * @param[out] p_x_axis                 Pointer to X-axis value.
 * @param[out] p_y_axis                 Pointer to Y-axis value.
 * @param[out] p_z_axis                 Pointer to Z-axis value.
 * @retval     FSP_SUCCESS              Read and retrieved successfully.
 * @retval     Any other error codes    Failed to read or retrieve.
 **********************************************************************************************************************/
fsp_err_t sensor_get_axis_data(float * p_x_axis, float * p_y_axis, float * p_z_axis)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t xyz_axis[SENSOR_DATA_SIZE] = {RESET_VALUE};

    err = sensor_wait_data_ready();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sensor_read_data(xyz_axis);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return sensor_convert_to_g_unit(xyz_axis, p_x_axis, p_y_axis, p_z_axis);
}

/*******************************************************************************************************************//**
 * @brief     This function initializes sensor hardware and software configuration.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t sensor_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t value = RESET_VALUE;

    err = sensor_init_hardware();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sensor_init_software();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sensor_read_reg(SENSOR_INT_SOURCE_REG, &value);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (SENSOR_INT_SOURCE_DATA == (value & SENSOR_INT_SOURCE_DATA))
    {
        g_b_sensor_ready_flag = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes sensor hardware including external IRQ and I2C interface.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t sensor_init_hardware(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return SCI_I2C_OPEN(&g_sci_i2c_master_ctrl, &g_sci_i2c_master_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function configures sensor registers.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
static fsp_err_t sensor_init_software(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t sensor_id = RESET_VALUE;

    err = sensor_read_reg(SENSOR_ID_REG, &sensor_id);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (SENSOR_ID_DATA != sensor_id)
    {
        return FSP_ERR_INVALID_DATA;
    }

    err = sensor_set_reg(SENSOR_BW_RATE_REG, SENSOR_BW_RATE_DATA);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sensor_set_reg(SENSOR_INT_ENABLE_REG, SENSOR_INT_ENABLE_DATA);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return sensor_set_reg(SENSOR_POWER_CTL_REG, SENSOR_POWER_CTL_DATA);
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to a sensor register via I2C.
 * @param[in] addr                      Register address.
 * @param[in] value                     Value to write.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t sensor_write_reg(uint8_t addr, uint8_t value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i2c_buf[I2C_TWO_BYTE] = {addr, value};

    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    err = SCI_I2C_WRITE(&g_sci_i2c_master_ctrl, i2c_buf, I2C_TWO_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return sci_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from a sensor register via I2C.
 * @param[in]  addr                     Register address.
 * @param[out] p_value                  Pointer to store the read value.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
static fsp_err_t sensor_read_reg(uint8_t addr, uint8_t * p_value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_addr = addr;

    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    err = SCI_I2C_WRITE(&g_sci_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sci_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    err = SCI_I2C_READ(&g_sci_i2c_master_ctrl, p_value, I2C_ONE_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return sci_i2c_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);
}

/*******************************************************************************************************************//**
 * @brief     This function writes and verifies a sensor register value.
 * @param[in] addr                      Register address.
 * @param[in] value                     Expected value to write.
 * @retval    FSP_SUCCESS               Wrote and verified successfully.
 * @retval    Any other error codes     Failed to write or verify.
 **********************************************************************************************************************/
static fsp_err_t sensor_set_reg(uint8_t addr, uint8_t value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_value = RESET_VALUE;

    err = sensor_write_reg(addr, value);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sensor_read_reg(addr, &read_value);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (value != read_value)
    {
        return FSP_ERR_WRITE_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for a specific I2C event with timeout.
 * @param[in] event                     Expected I2C master event.
 * @retval    FSP_SUCCESS               Received event successfully.
 * @retval    Any other error codes     Timeout occurred.
 **********************************************************************************************************************/
static fsp_err_t sci_i2c_wait_event(i2c_master_event_t event)
{
    uint8_t timeout = UINT8_MAX;

    while (event != g_sci_i2c_event)
    {
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
        timeout--;
        R_BSP_SoftwareDelay(I2C_TIMEOUT_DIV, I2C_TIMEOUT_UNIT);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for sensor data ready signal via external interrupt.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Received data successfully.
 * @retval    Any other error codes     Timeout occurred.
 **********************************************************************************************************************/
static fsp_err_t sensor_wait_data_ready(void)
{
    uint8_t timeout = UINT8_MAX;

    while (true != g_b_sensor_ready_flag)
    {
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
        timeout--;
        R_BSP_SoftwareDelay(DATA_TIMEOUT_DIV, DATA_TIMEOUT_UNIT);
    }

    g_b_sensor_ready_flag = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This function reads sensor axis data via I2C.
 * @param[out] p_data                   Pointer to buffer for raw sensor data.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
static fsp_err_t sensor_read_data(uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_addr = SENSOR_AXIS_DATA_REG;

    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    err = SCI_I2C_WRITE(&g_sci_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = sci_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    g_sci_i2c_event = (i2c_master_event_t)RESET_VALUE;

    err = SCI_I2C_READ(&g_sci_i2c_master_ctrl, p_data, I2C_DATA_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return sci_i2c_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);
}

/*******************************************************************************************************************//**
 * @brief      This function converts raw sensor data to acceleration values in g units.
 * @param[in]  p_data                   Pointer to raw data buffer.
 * @param[out] p_x_axis                 Pointer to X-axis value.
 * @param[out] p_y_axis                 Pointer to Y-axis value.
 * @param[out] p_z_axis                 Pointer to Z-axis value.
 * @retval     FSP_SUCCESS              Converted successfully.
 * @retval     Any other error codes    Failed to convert.
 **********************************************************************************************************************/
static fsp_err_t sensor_convert_to_g_unit(uint8_t * p_data, float * p_x_axis, float * p_y_axis, float * p_z_axis)
{
    if (NULL == p_data || NULL == p_x_axis || NULL == p_y_axis || NULL == p_z_axis)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    int16_t * p_axis_data = (int16_t *)p_data;

    *p_x_axis = (float)(*(p_axis_data + X_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;
    *p_y_axis = (float)(*(p_axis_data + Y_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;
    *p_z_axis = (float)(*(p_axis_data + Z_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened SCI I2C and external IRQ modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sensor_deinit(void)
{
    if (g_external_irq_ctrl.open)
    {
        /* Close the opened external IRQ */
        R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    }

    if (g_sci_i2c_master_ctrl.open)
    {
        /* Close the opened SCI I2C */
        SCI_I2C_CLOSE(&g_sci_i2c_master_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     The callback function for SCI I2C master.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void sci_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_sci_i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 * @brief     The callback function for external IRQ.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        if (g_external_irq_cfg.channel == p_args->channel)
        {
            g_b_sensor_ready_flag = true;
        }
    }
}
