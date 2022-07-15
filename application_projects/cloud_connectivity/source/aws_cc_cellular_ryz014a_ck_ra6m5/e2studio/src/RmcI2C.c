/***********************************************************************************************************************
 * File Name    : RmcI2C.c
 * Description  : Contains data structures and function definitions
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "RmcI2C.h"
#include  "sensor_thread.h"
#include "ICM_20948.h"
#include "ICP_10101.h"
#include "common_utils.h"

#define I2C_TRANSMISSION_IN_PROGRESS        0
#define I2C_TRANSMISSION_COMPLETE           1
#define I2C_TRANSMISSION_ABORTED            2

static uint8_t transmit_complete_flag, ICP_transmit_complete_flag;
/* variable to keep initial pressure as reference point */
float reference_pressure = 0.0;

/*******************************************************************************************************************//**
 * @brief       Initialization of communication layer for ICP
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void RmComDevice_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device4.p_api->open (g_comms_i2c_device4.p_ctrl, g_comms_i2c_device4.p_cfg);
    if (FSP_SUCCESS == err)
    {
        APP_PRINT("\r\nICM30498 sensor setup success\r\n");
    }
    else
    {
        APP_DBG_PRINT("\r\nICP10101 open sensor instance failed: %d\n", err);
    }
}

/*******************************************************************************************************************//**
 * @brief       Initialization of communication layer for ICM
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void RmComDevice_init_Icm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device5.p_api->open (g_comms_i2c_device5.p_ctrl, g_comms_i2c_device5.p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM20948 sensor open failed: %u\r\n", err);
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\nICM20948 sensor setup success\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       Initialization of ICM20948 sensor
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void ICM20948_Sensor_init()
{
    whoAmI ();
    icm20948_init ();
    initMagnetometer ();
    _delay (1000);
    autoOffsets ();
    setMagOpMode (AK09916_CONT_MODE_20HZ);
    setAccRange (ICM20948_ACC_RANGE_4G);
    setAccDLPF (ICM20948_DLPF_6);
    setAccSampleRateDivider (10);
    setGyrRange (ICM20948_GYRO_RANGE_250);
    setGyrDLPF (ICM20948_DLPF_6);
}

/*******************************************************************************************************************//**
 * @brief       RMComm I2C read
 * @param[in]   register address, buffer pointer to store read data and number of to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t RmCom_I2C_r(uint8_t reg, uint8_t *val, uint8_t num)
{
    uint16_t timeout = 1000;
    fsp_err_t err;
    transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    rm_comms_write_read_params_t write_read_params;
    write_read_params.p_src = &reg;
    write_read_params.src_bytes = 1;
    write_read_params.p_dest = val;
    write_read_params.dest_bytes = (uint8_t) num;
    err = g_comms_i2c_device5.p_api->writeRead (g_comms_i2c_device5.p_ctrl, write_read_params);
    if (err == FSP_SUCCESS)
    {
        while (transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            if (--timeout == 0)
            {
                break;
            }
            delay_Microseconds (1);
        }

        transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       RMComm I2C write
 * @param[in]   register address, buffer pointer to store read data and number of bytes to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t RmCom_I2C_w(uint8_t reg, uint8_t *val, uint8_t num)
{
    uint16_t timeout = 1000;
    fsp_err_t err;
    static uint8_t data[50];
    data[0] = reg;
    memcpy (&(data[1]), val, (size_t) num);
    transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    err = g_comms_i2c_device5.p_api->write (g_comms_i2c_device5.p_ctrl, data, (uint32_t) (num + 1));
    if (err == FSP_SUCCESS)
    {
        while (transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            if (--timeout == 0)
            {
                break;
            }
            delay_Microseconds (1);
        }

        transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       I2C callback for ICM sensor
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void ICM_comms_i2c_callback(rm_comms_callback_args_t *p_args)
{
    if (p_args->event == RM_COMMS_EVENT_OPERATION_COMPLETE)
    {
        transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
    }
    else
    {
        transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }
}

/*******************************************************************************************************************//**
 * @brief       RMComm I2C read for ICP sensor
 * @param[in]   buffer pointer to store read data and number of bytes to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t ICP_RmCom_I2C_r(uint8_t *val, uint32_t num)
{
    uint16_t timeout = 1000;
    fsp_err_t err;
    ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    err = g_comms_i2c_device4.p_api->read (g_comms_i2c_device4.p_ctrl, val, num);
    if (err == FSP_SUCCESS)
    {
        while (ICP_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            if (--timeout == 0)
            {
                break;
            }
            delay_Microseconds (1);
        }

        ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (ICP_transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       RMComm I2C write for ICP sensor
 * @param[in]   register address, buffer pointer to store read data and number of to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t ICP_RmCom_I2C_w(uint8_t *val, uint32_t num)
{
    uint16_t timeout = 1000;
    int8_t err;
    static uint8_t data[50];
    memcpy (data, val, (size_t) num);
    ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    err = g_comms_i2c_device4.p_api->write (g_comms_i2c_device4.p_ctrl, data, (uint32_t) (num));
    if (err == FSP_SUCCESS)
    {
        while (ICP_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            if (--timeout == 0)
            {
                break;
            }
            delay_Microseconds (1);
        }

        ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (ICP_transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       I2C callback for ICM sensor
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void ICP_comms_i2c_callback(rm_comms_callback_args_t *p_args)
{
    if (p_args->event == RM_COMMS_EVENT_OPERATION_COMPLETE)
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
    }
    else
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }
}

/**************************************************************************************
 * Name:       delay_Microseconds
 * Function:   delay specified number of microseconds
 * Parameters: us (uint16_t) number of microseconds to delay
 * Return:     None
 **************************************************************************************/
void delay_Microseconds(uint32_t us)
{
    R_BSP_SoftwareDelay (us, BSP_DELAY_UNITS_MICROSECONDS);
}

/**************************************************************************************
 * Name:       Delay
 * Function:   delay specified number of Milliseconds
 * Parameters: ms (uint32_t) number of Milliseconds to delay
 * Return:     None
 **************************************************************************************/
void _delay(uint32_t ms)
{
    R_BSP_SoftwareDelay (ms, BSP_DELAY_UNITS_MILLISECONDS);
}
