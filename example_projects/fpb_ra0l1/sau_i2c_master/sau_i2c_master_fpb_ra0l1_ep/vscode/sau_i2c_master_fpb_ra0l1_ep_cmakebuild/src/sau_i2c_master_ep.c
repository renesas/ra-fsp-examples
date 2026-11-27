/***********************************************************************************************************************
 * File Name    : sau_i2c_master_ep.c
 * Description  : Contains data structures and functions used in sau_i2c_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sau_i2c_master_ep.h"

/* Private variables */
static volatile i2c_master_event_t s_sau_i2c_event = I2C_MASTER_EVENT_ABORTED;
static volatile bool s_sensor_ready_flag = false;

/* Private function declarations */
static fsp_err_t sensor_write_reg(uint8_t addr, uint8_t value);
static fsp_err_t sensor_read_reg(uint8_t addr, uint8_t * p_value);
static fsp_err_t sensor_set_reg(uint8_t addr, uint8_t value);
static fsp_err_t sensor_init_hardware(void);
static fsp_err_t sensor_init_software(void);
static fsp_err_t sau_i2c_wait_event(const i2c_master_event_t event);

/*******************************************************************************************************************//**
 * @brief     This function initializes the necessary hardware and configures the sensor registers.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized and configured successfully.
 * @retval    Any other error codes     Failed to initialize or configure.
 **********************************************************************************************************************/
fsp_err_t sensor_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t value = RESET_VALUE;

    /* Initialize the necessary hardware to control the sensor */
    err = sensor_init_hardware();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure the sensor registers */
    err = sensor_init_software();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read the INT_SOURCE register to detect data ready */
    err = sensor_read_reg(SENSOR_INT_SOURCE_REG, &value);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* If data is already ready, trigger the data ready flag by software */
    if (SENSOR_INT_SOURCE_DATA == (value & SENSOR_INT_SOURCE_DATA))
    {
        s_sensor_ready_flag = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits until sensor data is ready.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Data ready.
 * @retval    FSP_ERR_TIMEOUT           Timed out waiting for data.
 **********************************************************************************************************************/
fsp_err_t sensor_wait_data_ready(void)
{
    uint8_t timeout = UINT8_MAX;

    /* Wait until new data is ready or timeout occurs */
    while (true != s_sensor_ready_flag)
    {
        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Decrease timeout value then software delay */
        timeout--;
        R_BSP_SoftwareDelay(DATA_TIMEOUT_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    }

    /* Clear the sensor data ready flag */
    s_sensor_ready_flag = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief      This function reads 3-axis raw data from the sensor via the I2C interface.
 * @param[out] p_data                   Pointer to buffer where 3-axis raw data will be stored.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t sensor_read_data(uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_addr = SENSOR_AXIS_DATA_REG;

    /* Reset SAU I2C callback event */
    s_sau_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Write the axis data register address to the sensor */
    err = R_SAU_I2C_Write(&g_sau_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
    err = R_SAU_I2C_Start(&g_sau_i2c_master_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Wait for the TX complete event */
    err = sau_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
#if SAU_I2C_CFG_RESTART_ENABLE
    if (false == g_sau_i2c_master_ctrl.restarted)
#endif /* SAU_I2C_CFG_RESTART_ENABLE */
    {
        err = R_SAU_I2C_Stop(&g_sau_i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Reset SAU I2C callback event */
    s_sau_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Read 6 bytes of axis data */
    err = R_SAU_I2C_Read(&g_sau_i2c_master_ctrl, p_data, I2C_DATA_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
    err = R_SAU_I2C_Start(&g_sau_i2c_master_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Wait for the RX complete event */
    err = sau_i2c_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
#if SAU_I2C_CFG_RESTART_ENABLE
    if (false == g_sau_i2c_master_ctrl.restarted)
#endif /* SAU_I2C_CFG_RESTART_ENABLE */
    {
        err = R_SAU_I2C_Stop(&g_sau_i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function converts raw sensor data to gravitational units (g units).
 * @param[in]  p_data                   Pointer to the buffer containing 3-axis raw data.
 * @param[out] p_x_axis                 Pointer to store the X-axis value in g units.
 * @param[out] p_y_axis                 Pointer to store the Y-axis value in g units.
 * @param[out] p_z_axis                 Pointer to store the Z-axis value in g units.
 * @retval     FSP_SUCCESS              Converted successfully.
 * @retval     Any other error codes    Failed to convert.
 **********************************************************************************************************************/
fsp_err_t sensor_convert_to_g_unit(uint8_t * p_data, float * p_x_axis, float * p_y_axis, float * p_z_axis)
{
    int16_t * p_axis_data = (int16_t *)p_data;

    /* Check for NULL pointers */
    if ((NULL == p_data) || (NULL == p_x_axis) || (NULL == p_y_axis) || (NULL == p_z_axis))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Convert to g units */
    *p_x_axis = (float)(*(p_axis_data + X_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;
    *p_y_axis = (float)(*(p_axis_data + Y_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;
    *p_z_axis = (float)(*(p_axis_data + Z_AXIS_ADDR_OFFSET)) * SENSOR_SENSITIVITY;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to a sensor register via the I2C interface.
 * @param[in] addr                      Register address.
 * @param[in] value                     Value to write to the register.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t sensor_write_reg(uint8_t addr, uint8_t value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i2c_buf[I2C_TWO_BYTE] = {addr, value};

    /* Reset SAU I2C callback event */
    s_sau_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Perform the SAU I2C write operation */
    err = R_SAU_I2C_Write(&g_sau_i2c_master_ctrl, i2c_buf, I2C_TWO_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
    err = R_SAU_I2C_Start(&g_sau_i2c_master_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Wait for the TX complete event */
    err = sau_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
#if SAU_I2C_CFG_RESTART_ENABLE
    if (false == g_sau_i2c_master_ctrl.restarted)
#endif /* SAU_I2C_CFG_RESTART_ENABLE */
    {
        err = R_SAU_I2C_Stop(&g_sau_i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from a sensor register via the I2C interface.
 * @param[in]  addr                     Register address.
 * @param[out] p_value                  Pointer to store the register value.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
static fsp_err_t sensor_read_reg(uint8_t addr, uint8_t * p_value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_addr = addr;

    /* Reset SAU I2C callback event */
    s_sau_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Write the register address to the sensor */
    err = R_SAU_I2C_Write(&g_sau_i2c_master_ctrl, &reg_addr, I2C_ONE_BYTE, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
    err = R_SAU_I2C_Start(&g_sau_i2c_master_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Wait for the TX complete event */
    err = sau_i2c_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
#if SAU_I2C_CFG_RESTART_ENABLE
    if (false == g_sau_i2c_master_ctrl.restarted)
#endif /* SAU_I2C_CFG_RESTART_ENABLE */
    {
        err = R_SAU_I2C_Stop(&g_sau_i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Reset SAU I2C callback event */
    s_sau_i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Read the register value */
    err = R_SAU_I2C_Read(&g_sau_i2c_master_ctrl, p_value, I2C_ONE_BYTE, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
    err = R_SAU_I2C_Start(&g_sau_i2c_master_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    /* Wait for the RX complete event */
    err = sau_i2c_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if SAU_I2C_CFG_MANUAL_START_STOP_ENABLE
#if SAU_I2C_CFG_RESTART_ENABLE
    if (false == g_sau_i2c_master_ctrl.restarted)
#endif /* SAU_I2C_CFG_RESTART_ENABLE */
    {
        err = R_SAU_I2C_Stop(&g_sau_i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* SAU_I2C_CFG_MANUAL_START_STOP_ENABLE */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets data to a sensor register via I2C interface.
 * @param[in] addr                      Register address.
 * @param[in] value                     Value to set register.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
static fsp_err_t sensor_set_reg(uint8_t addr, uint8_t value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_value = RESET_VALUE;

    /* Write data to the register */
    err = sensor_write_reg(addr, value);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back data from the register */
    err = sensor_read_reg(addr, &read_value);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Verify the data */
    if (value != read_value)
    {
        return FSP_ERR_WRITE_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the hardware necessary to communicate with the sensor.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t sensor_init_hardware(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the ICU External IRQ module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable the ICU External IRQ to detect new data from the sensor */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open the SAU I2C Master module to communicate with the sensor */
    err = R_SAU_I2C_Open(&g_sau_i2c_master_ctrl, &g_sau_i2c_master_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function configures the sensor registers to describe the expected sensor behavior.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
static fsp_err_t sensor_init_software(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t sensor_id = RESET_VALUE;

    /* Read the sensor ID */
    err = sensor_read_reg(SENSOR_ID_REG, &sensor_id);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Verify the sensor ID */
    if (SENSOR_ID_DATA != sensor_id)
    {
        return FSP_ERR_INVALID_DATA;
    }

    /* Set up the BW_RATE register to select an output data rate of 1.56 Hz */
    err = sensor_set_reg(SENSOR_BW_RATE_REG, SENSOR_BW_RATE_DATA);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set up the INT_ENABLE register to enable data ready interrupts */
    err = sensor_set_reg(SENSOR_INT_ENABLE_REG, SENSOR_INT_ENABLE_DATA);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set up the POWER_CTL register to select measurement mode */
    err = sensor_set_reg(SENSOR_POWER_CTL_REG, SENSOR_POWER_CTL_DATA);

    return err;
}


/*******************************************************************************************************************//**
 * @brief     This function waits for the expected I2C event from the SAU I2C Master callback.
 * @param[in] event                     I2C Master event to wait for.
 * @retval    FSP_SUCCESS               Event received successfully.
 * @retval    FSP_ERR_TIMEOUT           Timed out waiting for the event.
 **********************************************************************************************************************/
static fsp_err_t sau_i2c_wait_event(const i2c_master_event_t event)
{
    uint16_t timeout = I2C_TIMEOUT_PERIOD;

    /* Wait until the I2C Master event equals the expected event */
    while (event != s_sau_i2c_event)
    {
        /* Check time elapsed to avoid infinite loop */
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }

        /* Decrease timeout value then software delay */
        timeout--;
        R_BSP_SoftwareDelay(I2C_TIMEOUT_DIV, I2C_TIMEOUT_UNIT);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the SAU I2C module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void sau_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        s_sau_i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the External IRQ module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        if (g_external_irq_cfg.channel == p_args->channel)
        {
            s_sensor_ready_flag = true;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened ICU and SAU I2C Master modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sensor_deinit(void)
{
    /* Close the opened ICU module */
    if (g_external_irq_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    }

    /* Close the opened SAU I2C Master module */
    if (g_sau_i2c_master_ctrl.open)
    {
        R_SAU_I2C_Close(&g_sau_i2c_master_ctrl);
    }
}
