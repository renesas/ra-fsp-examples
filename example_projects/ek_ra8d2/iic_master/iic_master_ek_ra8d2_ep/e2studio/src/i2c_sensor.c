/***********************************************************************************************************************
 * File Name    : i2c_sensor.c
 * Description  : Contains data structures and functions used in i2c_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "common_utils.h"
#include "i2c_sensor.h"

/*******************************************************************************************************************//**
 * @addtogroup r_iic_master_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
/* Read I2C callback event through I2C Master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

/* Private function declarations */
static fsp_err_t get_device_id(uint8_t *dev_id);
static fsp_err_t validate_i2c_event(void);

/*******************************************************************************************************************//**
 *  @brief       Initializes IIC Master module and set up Pmod ACL sensor.
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS    Upon successful open and start of timer.
 *  @retval      Any other error code apart from FSP_SUCCESS    Unsuccessful open or start.
 **********************************************************************************************************************/
fsp_err_t init_sensor(void)
{
    uint8_t device_id = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;
    uint8_t measure_enable_payload[MEASURE_PAYLOAD_SIZE] = {POWER_CTL_REG, ENABLE_BIT};

    /* Open IIC Master module */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_MASTER_Open(&g_iica_master_ctrl,&g_iica_master_cfg);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_MASTER_Open(&g_i2c_master_ctrl,&g_i2c_master_cfg);
#else
    err = R_IIC_MASTER_Open(&g_i2c_master_ctrl,&g_i2c_master_cfg);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(OPEN_FAIL);
        return err;
    }

    /* Get device ID as a start of communication */
    err = get_device_id(&device_id);

    /* Verify that the I2C transaction with the Pmod ACL is successful and the correct device ID is received */
    if ((FSP_SUCCESS == err) && (DEVICE_SIGNATURE == device_id))
    {
        /* Access to POWER_CTL register (0x2D) and enable measurement */
#if (BSP_PERIPHERAL_IICA_PRESENT)
        err = R_IICA_MASTER_Write(&g_iica_master_ctrl, measure_enable_payload, TWO_BYTE, false);
#elif defined (BOARD_RA6T2_MCK)
        err = R_IIC_B_MASTER_Write(&g_i2c_master_ctrl, measure_enable_payload, TWO_BYTE, false);
#else
        err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, measure_enable_payload, TWO_BYTE, false);
#endif
        if (FSP_SUCCESS == err)
        {
            err = validate_i2c_event();
            /* Handle error */
            if (FSP_ERR_TRANSFER_ABORTED == err)
            {
                APP_ERR_PRINT("** POWER_CTL register I2C write failed **\r\n");
            }
        }
        else
        {
            /* Handle error when write fails */
            APP_ERR_PRINT(WRITE_FAIL);
        }
    }
    else
    {
        /* Handle error when device ID retrieval fails */
        APP_ERR_PRINT("** Error in reading device ID **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       De-initializes IIC Master module.
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_sensor(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize the IIC Master module if it had already opened */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    if (MODULE_CLOSE != g_iica_master_ctrl.open)
    {
        err = R_IICA_MASTER_Close(&g_iica_master_ctrl);
#else
    if (MODULE_CLOSE != g_i2c_master_ctrl.open)
    {
#if defined (BOARD_RA6T2_MCK)
        err = R_IIC_B_MASTER_Close(&g_i2c_master_ctrl);
#else
        err = R_IIC_MASTER_Close(&g_i2c_master_ctrl);
#endif  /* defined (BOARD_RA6T2_MCK) */
#endif  /* (BSP_PERIPHERAL_IICA_PRESENT) */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(CLOSE_FAIL);
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       Reads ACL sensor data for the X, Y, and Z axes, and prints the raw data to the RTT console.
 *  @param[IN]   xyz_data           Pointer to an array where the sensor data will be stored.
 *  @retval      FSP_SUCCESS        Returned upon successful I2C transaction and data retrieval.
 *  @retval      Any other error code apart from FSP_SUCCESS    Unsuccessful read or write operation.
 **********************************************************************************************************************/
fsp_err_t read_sensor_data(uint8_t *xyz_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t adxl_data[ADXL_DATA_SIZE] = {RESET_VALUE};
    uint8_t internal_reg = AXIS_DATA;

    if (NULL == xyz_data)
    {
        err = FSP_ERR_INVALID_POINTER;
        APP_ERR_PRINT("** NULL pointer check failed **\r\n");
        return err;
    }
    /* Write to accelerometer data register */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_MASTER_Write(&g_iica_master_ctrl, &internal_reg, ONE_BYTE, true);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_MASTER_Write(&g_i2c_master_ctrl, &internal_reg, ONE_BYTE, true);
#else
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, &internal_reg, ONE_BYTE, true);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(WRITE_FAIL);
        return err;
    }
    else
    {
        err = validate_i2c_event();

        /* Perform read operation only if write event is successful; failure is handled below */
        if (FSP_SUCCESS == err)
        {
#if (BSP_PERIPHERAL_IICA_PRESENT)
            err = R_IICA_MASTER_Read(&g_iica_master_ctrl, &adxl_data[0], DATA_REGISTERS, false);
#elif defined (BOARD_RA6T2_MCK)
            err = R_IIC_B_MASTER_Read(&g_i2c_master_ctrl, &adxl_data[0], DATA_REGISTERS, false);
#else
            err = R_IIC_MASTER_Read(&g_i2c_master_ctrl, &adxl_data[0], DATA_REGISTERS, false);
#endif
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT(READ_FAIL);
                return err;
            }
            else
            {
                err = validate_i2c_event();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed **\r\n");
                    return err;
                }
            }
        }
        else
        {
            APP_ERR_PRINT("** Accelerometer Data register, I2C write failed **\r\n");
            return err;
        }

        /* If read is successful, update the X, Y, Z values and return success */
        for (uint8_t itr = RESET_VALUE; itr < SENSOR_DATA_SIZE; itr++)
        {
            *(xyz_data + itr) = adxl_data[itr];
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      Reads the device signature/ID from the sensor.
 *  @param[in]  dev_id                       Pointer to store the retrieved Pmod ACL device ID.
 *  @retval     FSP_SUCCESS                  Returned on a successful I2C transaction.
 *              FSP_ERR_INVALID_POINTER      Returned if an invalid parameter is passed.
 *              err                          Error code returned from the API or I2C event failure.
 **********************************************************************************************************************/
static fsp_err_t get_device_id(uint8_t *dev_id)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_address = DEVICE_ID_REG;

    /* Parameter checking */
    if (NULL == dev_id)
    {
        err = FSP_ERR_INVALID_POINTER;
        APP_ERR_PRINT("** NULL pointer check failed **\r\n");
        return err;
    }

    /* Send register address to sensor */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_MASTER_Write(&g_iica_master_ctrl, &reg_address, ONE_BYTE, true);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_MASTER_Write(&g_i2c_master_ctrl, &reg_address, ONE_BYTE, true);
#else
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, &reg_address, ONE_BYTE, true);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(WRITE_FAIL);
    }
    else
    {
        err = validate_i2c_event();
        /* Handle error */
        if (FSP_ERR_TRANSFER_ABORTED == err)
        {
            APP_ERR_PRINT("** Failed to write to Device ID register **\r\n");
        }
        else
        {
            /* Read only if I2C write and callback are successful */
#if (BSP_PERIPHERAL_IICA_PRESENT)
            err = R_IICA_MASTER_Read(&g_iica_master_ctrl, dev_id, ONE_BYTE, false);
#elif defined (BOARD_RA6T2_MCK)
            err = R_IIC_B_MASTER_Read(&g_i2c_master_ctrl, dev_id, ONE_BYTE, false);
#else
            err = R_IIC_MASTER_Read(&g_i2c_master_ctrl, dev_id, ONE_BYTE, false);
#endif
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT(READ_FAIL);
                /* Do nothing, the error is returned in the end */
            }
            else
            {
                err = validate_i2c_event();
                /* Handle error */
                if (FSP_ERR_TRANSFER_ABORTED == err)
                {
                    APP_ERR_PRINT("** Failed to read from Device ID register **\r\n");
                }
            }
        }
    }
    /* On successful I2C transaction with Pmod ACL return from here */
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      User callback function.
 *  @param[in]  p_args  Callback argument.
 *  @retval     None
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Capture callback event for validating the I2C transfer event */
        i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Validates I2C receive/transmit completion based on expected write/read event.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Operation completed successfully.
 *  @retval      FSP_ERR_TRANSFER_ABORTED   Timeout occurred or I2C_MASTER_EVENT_ABORTED was received.
 **********************************************************************************************************************/
static fsp_err_t validate_i2c_event(void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* Reset callback event capture variable */
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
        i2c_event = (i2c_master_event_t) RESET_VALUE;  /* Always reset before returning */
        return FSP_SUCCESS;
    }

    i2c_event = (i2c_master_event_t) RESET_VALUE;      /* Always reset before returning */
    return FSP_ERR_TRANSFER_ABORTED;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iic_master_ep)
 **********************************************************************************************************************/
