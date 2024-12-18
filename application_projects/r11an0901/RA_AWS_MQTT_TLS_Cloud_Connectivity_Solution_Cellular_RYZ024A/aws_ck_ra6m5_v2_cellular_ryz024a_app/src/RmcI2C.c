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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "sensors_thread.h"
#include "RmcI2C.h"

#define I2C_TRANSMISSION_IN_PROGRESS        (0)
#define I2C_TRANSMISSION_COMPLETE           (1)
#define I2C_TRANSMISSION_ABORTED            (2)

extern volatile uint32_t g_icp_irq_val;
static uint8_t ICM_transmit_complete_flag;
static uint8_t ICP_transmit_complete_flag;

/*******************************************************************************************************************//**
 * @brief       Initialization of communication layer for ICP
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void RmComDevice_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device4.p_api->open (g_comms_i2c_device4.p_ctrl, g_comms_i2c_device4.p_cfg);
    if (FSP_SUCCESS == err)
    {
        APP_PRINT("\r\nICP sensor setup success\r\n");
    }
    else
    {
        APP_DBG_PRINT("\r\nICP open sensor instance failed: %d\n", err);
    }
}

/*******************************************************************************************************************//**
 * @brief       Initialization of communication layer for ICM
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void RmComDevice_init_Icm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device5.p_api->open (g_comms_i2c_device5.p_ctrl, g_comms_i2c_device5.p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM42605 sensor open failed: %u\r\n", err);
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\nICM42605 sensor setup success\r\n");
    }
}


/*******************************************************************************************************************//**
 * @brief       RMComm I2C read for ICM sensor
 * @param[in]   register address, buffer pointer to store read data and number of to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t RmCom_I2C_r_ICM(uint8_t reg, uint8_t *val, uint32_t num)
{
    fsp_err_t err;
    ICM_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    rm_comms_write_read_params_t write_read_params;
    write_read_params.p_src = &reg;
    write_read_params.src_bytes = 1;
    write_read_params.p_dest = val;
    write_read_params.dest_bytes = (uint8_t) num;
    err = g_comms_i2c_device5.p_api->writeRead (g_comms_i2c_device5.p_ctrl, write_read_params);
    if (err == FSP_SUCCESS)
    {
        while (ICM_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            vTaskDelay (1);
            break;
        }

        ICM_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (ICM_transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       RMComm I2C write for ICM sensor
 * @param[in]   register address, buffer pointer to store read data and number of bytes to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t RmCom_I2C_w_ICM(uint8_t reg, uint8_t *val, uint32_t num)
{
    fsp_err_t err;
    static uint8_t data[50];
    data[0] = reg;
    memcpy (&(data[1]), val, (size_t) num);
    ICM_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    err = g_comms_i2c_device5.p_api->write (g_comms_i2c_device5.p_ctrl, data, (uint32_t) (num + 1));
    if (err == FSP_SUCCESS)
    {
        while (ICM_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            vTaskDelay (1);
            break;
        }

        ICM_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (ICM_transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       I2C callback for ICM sensor
 * @param[in]   p_args
 * @retval      None
 ***********************************************************************************************************************/
void ICM_comms_i2c_callback(rm_comms_callback_args_t *p_args)
{
    if (p_args->event == RM_COMMS_EVENT_OPERATION_COMPLETE)
    {
        ICM_transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
    }
    else
    {
        ICM_transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }
}

/*******************************************************************************************************************//**
 * @brief       RMComm I2C read for ICP sensor
 * @param[in]   register address, buffer pointer to store read data and number of to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t RmCom_I2C_r(uint8_t reg, uint8_t *val, uint32_t num)
{
    fsp_err_t err;
    ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    rm_comms_write_read_params_t write_read_params;
    write_read_params.p_src = &reg;
    write_read_params.src_bytes = 1;
    write_read_params.p_dest = val;
    write_read_params.dest_bytes = (uint8_t) num;
    err = g_comms_i2c_device4.p_api->writeRead (g_comms_i2c_device4.p_ctrl, write_read_params);
    if (err == FSP_SUCCESS)
    {
        while (ICP_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            vTaskDelay (1);
            break;
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
 * @param[in]   register address, buffer pointer to store read data and number of bytes to read
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t RmCom_I2C_w(uint8_t reg, uint8_t *val, uint32_t num)
{
    fsp_err_t err;
    static uint8_t data[50];
    data[0] = reg;
    memcpy (&(data[1]), val, (size_t) num);
    ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    err = g_comms_i2c_device4.p_api->write (g_comms_i2c_device4.p_ctrl, data, (uint32_t) (num + 1));
    if (err == FSP_SUCCESS)
    {
        while (ICP_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            vTaskDelay (1);
            break;
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
 * @retval      None
 ***********************************************************************************************************************/
void ICP_comms_i2c_callback(rm_comms_callback_args_t *p_args)
{
    if (p_args->event == RM_COMMS_EVENT_OPERATION_COMPLETE)
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
        g_icp_irq_val = 1;
    }
    else
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }
}

/*******************************************************************************************************************//**
 * @brief       Re-cover the I2C bus if it is busy 
 * @param[in]   SCL SCL pin of the IIC bus
 * @param[in]   SDA SDA pin of the IIC bus
 * @retval      None
 ***********************************************************************************************************************/
void bsp_recover_iic(const bsp_io_port_pin_t SCL, const bsp_io_port_pin_t SDA)
{
    const bsp_io_port_pin_t PIN_SCL = SCL;
    const bsp_io_port_pin_t PIN_SDA = SDA;
    const uint32_t pinToggleDelay = 5;

    /* switch to GPIO mode, N-channel open-drain with pull-up */
    R_IOPORT_PinCfg(&g_ioport_ctrl, PIN_SCL, (uint32_t)IOPORT_CFG_NMOS_ENABLE | (uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH | (uint32_t)IOPORT_CFG_PULLUP_ENABLE);
    R_IOPORT_PinCfg(&g_ioport_ctrl, PIN_SDA, (uint32_t)IOPORT_CFG_NMOS_ENABLE | (uint32_t)IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t)IOPORT_CFG_PORT_OUTPUT_HIGH | (uint32_t)IOPORT_CFG_PULLUP_ENABLE);

    /* toggle SCL 9+ times */
    for (uint32_t i = 0; i< 10; i++) {
        R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_SCL, BSP_IO_LEVEL_LOW);
        R_BSP_SoftwareDelay(pinToggleDelay, BSP_DELAY_UNITS_MICROSECONDS);
        R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_SCL, BSP_IO_LEVEL_HIGH);
        R_BSP_SoftwareDelay(pinToggleDelay, BSP_DELAY_UNITS_MICROSECONDS);
    }

    /* generate STOP condition (SDA going from LOW to HIGH when SCL is HIGH) */
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_SCL, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(pinToggleDelay, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_SDA, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(pinToggleDelay, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_SCL, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(pinToggleDelay, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_SDA, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(pinToggleDelay, BSP_DELAY_UNITS_MICROSECONDS);

    /* switch back to peripheral mode */
    R_IOPORT_PinCfg(&g_ioport_ctrl, PIN_SCL, (uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_IIC);
    R_IOPORT_PinCfg(&g_ioport_ctrl, PIN_SDA, (uint32_t)IOPORT_CFG_PERIPHERAL_PIN | (uint32_t)IOPORT_PERIPHERAL_IIC);
}
