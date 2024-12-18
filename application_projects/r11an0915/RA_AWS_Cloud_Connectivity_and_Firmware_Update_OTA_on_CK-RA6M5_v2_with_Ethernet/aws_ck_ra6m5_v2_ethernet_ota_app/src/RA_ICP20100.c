/***********************************************************************************************************************
 * File Name    : RA_ICP20100.c
 * Description  : Contains data structures and function definitions for ICP20100 sensor
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
 * Copyright (C) 2023-2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "icp.h"
#include "ICP_20100.h"
#include "RmcI2C.h"
#include "user_choice.h"
#include "usr_data.h"
#define DEVICE_VERSION_A (0x00)
#define DEVICE_VERSION_B (0xB2)
#define NOTICE_FOR_VERSION_A "\r\nVersion A haven't tested yet."\
    "\r\nPlease contact us via https://www.renesas.com/ if there is any problem\r\n"

volatile uint32_t g_icp_irq_val = 0;
volatile uint8_t g_device_id;
float g_temperature, g_pressure;
usr_icp_data_t g_icp_data;


/*******************************************************************************************************************//**
 * @brief       ICP Callback function
 * @param[in]   p_args
 * @retval      None
 ***********************************************************************************************************************/
void ICP_IRQ_CALLBACK(external_irq_callback_args_t *p_args)
{

    FSP_PARAMETER_NOT_USED (p_args);
}


/*******************************************************************************************************************//**
 * @brief       Identify ICP device and its version
 * @param[in]   None
 * @retval      reg_data : version of the device
 ***********************************************************************************************************************/
static uint8_t ICP_identify_device(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_data = 0;
    uint8_t data = 0;
    int8_t device_error = -1;

    /* dummy write */
    data = DUMMY;
    err = pressure20_write_register(PRESSURE20_REG_DUMMY, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor dummy write fail\r\n");
        APP_ERR_TRAP(err);
    }

    if (ERROR == pressure20_check_communication() )
    {
        return ERROR;
    }
    err = pressure20_read_register(PRESSURE20_REG_DEVICE_ID, &g_device_id, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor read device ID fail\r\n");
        APP_ERR_TRAP(err);
    }
    if (PRESSURE20_DEVICE_ID != g_device_id)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor device ID does not match\r\n");
        APP_ERR_TRAP(device_error);
    }

    err = pressure20_read_register(PRESSURE20_REG_VERSION, &reg_data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor read device version fail\r\n");
        APP_ERR_TRAP(err);
    }
    return reg_data;
}

/*******************************************************************************************************************//**
 * @brief       ICP boot sequence. Depend on version of device to perform boot sequence.
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
static void ICP_boot_sequence(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_data = 0;
    uint8_t device = DEVICE_VERSION_A;
    uint8_t data = 0;

    device = ICP_identify_device();
    if (DEVICE_VERSION_B == device)
    {
        /* Device version B, no further initialization is required */
        APP_PRINT("\r\nICP20100 Sensor Version B\r\n");
        return;
    }
    if (DEVICE_VERSION_A != device)
    {
        APP_DBG_PRINT("\r\nICP detect version fail.\r\n");
        err = FSP_ERR_SENSOR_INVALID_DATA;
        APP_ERR_TRAP(err);
    }
    /* Device version A, need boot sequence */
    APP_PRINT("\r\nICP20100 Sensor Version A\r\n");
    APP_PRINT(NOTICE_FOR_VERSION_A);
    /* Read boot up status and avoid re-running boot up sequence if it is already done */
    err = pressure20_read_register(PRESSURE20_REG_OTP_STATUS2, &reg_data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor read OTP status2 fail\r\n");
        APP_ERR_TRAP(err);
    }
    if ( PRESSURE20_BOOT_UP_STATUS != ( reg_data & PRESSURE20_BOOT_UP_STATUS ) )
    {
        uint8_t offset, gain, hfosc;

        /* Set to power mode to activate the OTP power domain and get access to the main registers */
        err = pressure20_read_register(PRESSURE20_REG_MODE_SELECT, &reg_data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor read mode select fail\r\n");
            APP_ERR_TRAP(err);
        }
        data = reg_data | PRESSURE20_POWER_MODE_ACTIVE;
        err = pressure20_write_register(PRESSURE20_REG_MODE_SELECT, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write mode select fail\r\n");
            APP_ERR_TRAP(err);
        }
        vTaskDelay (4);

        /* Unlock the main registers */
        data = PRESSURE20_MASTER_UNLOCK;
        err = pressure20_write_register(PRESSURE20_REG_MASTER_LOCK, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write Master lock fail\r\n");
            APP_ERR_TRAP(err);
        }

        /* Enable the OTP and the write switch */
        data = (PRESSURE20_OTP_WRITE_SWITCH | PRESSURE20_OTP_ENABLE);
        err = pressure20_write_register(PRESSURE20_REG_OTP_CONFIG1, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP CONFIG1 fail\r\n");
            APP_ERR_TRAP(err);
        }
        delayMicroseconds(10);

        /* Toggle the OTP reset pin */
        data = PRESSURE20_OTP_RESET_SET;
        err = pressure20_write_register(PRESSURE20_REG_OTP_DBG2, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP DBG2 fail\r\n");
            APP_ERR_TRAP(err);
        }
        delayMicroseconds(10);

        data = PRESSURE20_OTP_RESET_CLEAR;
        err = pressure20_write_register(PRESSURE20_REG_OTP_DBG2, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP DBG2 fail\r\n");
            APP_ERR_TRAP(err);
        }
        delayMicroseconds(10);

        /* Program redundant read */
        data = PRESSURE20_OTP_MRA_LSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRA_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_MRA_LSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        data = PRESSURE20_OTP_MRA_MSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRA_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_MRA_MSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        data = PRESSURE20_OTP_MRB_LSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRB_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_MRB_LSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        data = PRESSURE20_OTP_MRB_MSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MRB_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_MRB_MSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        data = PRESSURE20_OTP_MR_LSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MR_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_MR_LSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        data = PRESSURE20_OTP_MR_MSB;
        err = pressure20_write_register(PRESSURE20_REG_OTP_MR_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_MR_MSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        /* Read offset, gain and hfosc */
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_OFFSET, &offset);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor read offset fail\r\n");
            APP_ERR_TRAP(err);
        }
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_GAIN, &gain);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor read gain fail\r\n");
            APP_ERR_TRAP(err);
        }
        err = pressure20_read_otp_data(PRESSURE20_OTP_ADDRESS_HFOSC, &hfosc);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor read hfosc fail\r\n");
            APP_ERR_TRAP(err);
        }

        /* Disable OTP and write switch */
        data = PRESSURE20_OTP_DISABLE;
        err = pressure20_write_register(PRESSURE20_REG_OTP_CONFIG1, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_CONFIG1 fail\r\n");
            APP_ERR_TRAP(err);
        }
        delayMicroseconds(10);

        /* Write OTP values to main registers */
        data = (offset & PRESSURE20_TRIM1_MSB_PEFE_OFFSET_MASK);
        err = pressure20_write_register(PRESSURE20_REG_TRIM1_MSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write TRIM1_MSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        err = pressure20_read_register (PRESSURE20_REG_TRIM2_MSB, &reg_data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor read TRIM2_MSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        reg_data = ( reg_data & ( ~PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK ) ) | ( ( gain << 4 ) & PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK );
        err = pressure20_write_register (PRESSURE20_REG_TRIM2_MSB, &reg_data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write TRIM2_MSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        data = (hfosc & PRESSURE20_TRIM2_LSB_HFOSC_MASK);
        err = pressure20_write_register (PRESSURE20_REG_TRIM2_LSB, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write TRIM2_LSB fail\r\n");
            APP_ERR_TRAP(err);
        }

        /* Lock the main register */
        data = PRESSURE20_MASTER_LOCK;
        err = pressure20_write_register (PRESSURE20_REG_MASTER_LOCK, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write MASTER_LOCK fail\r\n");
            APP_ERR_TRAP(err);
        }

        /* Move to stand by mode */
        err = pressure20_write_mode_select (PRESSURE20_POWER_MODE_NORMAL );
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write POWER_MODE fail\r\n");
            APP_ERR_TRAP(err);
        }

        /* Update boot up status to 1 */
        data = PRESSURE20_BOOT_UP_STATUS;
        err = pressure20_write_register (PRESSURE20_REG_OTP_STATUS2, &data, 1);
        if(FSP_SUCCESS != err)
        {
            APP_DBG_PRINT("\r\nICP20100 Sensor write OTP_STATUS2 fail\r\n");
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       Ignore first 14 samples. It is recommended when enable FIR FILTER
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
static void ICP_ignore_first_14_samples(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg_data = 0;
    uint8_t data = 0;

    /* dummy write */
    data = DUMMY;
    err = pressure20_write_register(PRESSURE20_REG_DUMMY, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor dummy write fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Configure the FIFO watermark high to 14 samples */
    data = 0xE0;
    err = pressure20_write_register(PRESSURE20_REG_FIFO_CONFIG, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor FIFO Config write fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Unmask the watermark high interrupt */
    data = 0xFB;
    err = pressure20_write_register(PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor Mode select write fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Start a measurement */
    err = pressure20_write_mode_select(PRESSURE20_MEAS_MODE_CONTINUOUS);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor Mode select write fail\r\n");
        APP_ERR_TRAP(err);
    }

    delayMicroseconds(10);

    /* Stop the measurement */
    err = pressure20_write_mode_select(PRESSURE20_POWER_MODE_NORMAL);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor Mode select write fail\r\n");
        APP_ERR_TRAP(err);
    }
    delayMicroseconds(10);

    /* Flush the FIFO */
    data = PRESSURE20_FIFO_FLUSH;
    err = pressure20_write_register(PRESSURE20_REG_FIFO_FILL, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor FIFO Flush write fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Mask all interrupts */
    data = PRESSURE20_INT_MASK_ALL;
    err = pressure20_write_register(PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor Interrupt mask write fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Clear interrupts */
    err = pressure20_clear_interrupts();
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor Clear Interrupt fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Start a measurement */
    /* Enable continuous measurement and set ODR to 25 Hz */
    data = PRESSURE20_MEAS_MODE_CONTINUOUS | PRESSURE20_MEAS_CONFIG_MODE0_ODR_25HZ;
    err = pressure20_write_register(PRESSURE20_REG_MODE_SELECT, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor Mode select write fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Wait for the FIFO full indication */
    do
    {
        err = pressure20_read_register (PRESSURE20_REG_FIFO_FILL, &reg_data, 1);
    } while ( PRESSURE20_FIFO_FULL != ( reg_data & PRESSURE20_FIFO_FULL ) );
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor read FIFO fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Set FIFO watermark HIGH level to 1 */
    data = (PRESSURE20_FIFO_LEVEL_8 << 4);       //PRESSURE20_FIFO_LEVEL_1
    err = pressure20_write_register (PRESSURE20_REG_FIFO_CONFIG, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor write FIFO config fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Unmask FIFO watermark HIGH interrupt */
    data = ~(PRESSURE20_INT_MASK_FIFO_WMK_HIGH | PRESSURE20_INT_MASK_FIFO_OVERFLOW);
    err = pressure20_write_register (PRESSURE20_REG_INTERRUPT_MASK, &data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor write Interrupt Mask fail\r\n");
        APP_ERR_TRAP(err);
    }

    err = pressure20_read_register (PRESSURE20_REG_INTERRUPT_MASK, &reg_data, 1);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor read Interrupt Mask fail\r\n");
        APP_ERR_TRAP(err);
    }

    /* Clear interrupts */
    err = pressure20_clear_interrupts();
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor clear interrupts fail\r\n");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief   Set up ICP sensor
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void icp_setup(void)
{
    ICP_boot_sequence();

    /* FIR filter enable, first 14 samples should be ignored */
    ICP_ignore_first_14_samples();
}

/*******************************************************************************************************************//**
 * @brief   Read ICP sensor data
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void icp_get(void)
{
    fsp_err_t err = FSP_SUCCESS;
    while( g_icp_irq_val != 1)
    {
        vTaskDelay(1);
    }
    g_icp_irq_val = 0;

    err = pressure20_clear_interrupts();
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor pressure20_clear_interrupts failed \r\n");
    }

    err = pressure20_read_data(&g_pressure, &g_temperature);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP20100 Sensor pressure20_read_data failed \r\n");
    }
    /* Update value for icp_data variable */
    g_icp_data.temperature_C = g_temperature;
    g_icp_data.pressure_Pa = g_pressure*100; //Convert from mBar to Pa

    xQueueOverwrite(g_icp_queue, &g_icp_data);
    vTaskDelay(pdMS_TO_TICKS(1000));
}


