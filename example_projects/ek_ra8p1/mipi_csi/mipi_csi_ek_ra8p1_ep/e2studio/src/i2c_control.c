/***********************************************************************************************************************
 * File Name    : i2c_control.c
 * Description  : Contains data structures and functions used in i2c_control.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "i2c_control.h"

/* I2C Master callback event */
static volatile i2c_master_event_t g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

/* Private function */
static fsp_err_t i2c_master_wait_event(const i2c_master_event_t i2c_event);

/***********************************************************************************************************************
 * Function Name: i2c_control_init.
 * Description  : This function initializes I2C Master module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t i2c_control_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize I2C module */
    err = R_IIC_MASTER_Open(&g_i2c_master_for_peripheral_ctrl, &g_i2c_master_for_peripheral_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function i2c_control_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: write_reg_8bit.
 * Description  : This function writes data to an 8-bit camera register.
 * Arguments    : address                   Register address.
 *                data                      Data to be written.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t write_reg_8bit(uint8_t address, uint8_t data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i2c_buffer[2];

    /* Prepare data for write operation */
    i2c_buffer[0] = address;
    i2c_buffer[1] = data;

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 2, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until write transmission complete */
    err = i2c_master_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);

    return err;
}
/***********************************************************************************************************************
* End of function write_reg_8bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: read_reg_8bit.
 * Description  : This function reads data from an 8-bit camera register.
 * Arguments    : address                   Register address.
 *                p_data                    Pointer used to store register data.
 * Return Value : FSP_SUCCESS               Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t read_reg_8bit(uint8_t address, uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i2c_buffer[1];

    /* Prepare data for write operation */
    i2c_buffer[0] = address;

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 1, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until write transmission complete */
    err = i2c_master_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C read operation */
    err = R_IIC_MASTER_Read(&g_i2c_master_for_peripheral_ctrl, p_data, 1, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until read transmission complete */
    err = i2c_master_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);

    return err;
}
/***********************************************************************************************************************
* End of function read_reg_8bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: write_reg_16bit.
 * Description  : This function writes data to a 16-bit camera register.
 * Arguments    : address                   Register address.
 *                data                      Data to be written.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t write_reg_16bit(uint16_t address, uint8_t data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i2c_buffer[3];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);
    i2c_buffer[2] = data;

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 3, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until write transmission complete */
    err = i2c_master_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);

    return err;
}
/***********************************************************************************************************************
* End of function write_reg_16bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: read_reg_16bit.
 * Description  : This function reads data from a 16-bit camera register.
 * Arguments    : address                   Register address.
 *                p_data                    Pointer used to store register data.
 * Return Value : FSP_SUCCESS               Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t read_reg_16bit(uint16_t address, uint8_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i2c_buffer[2];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 2, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until write transmission complete */
    err = i2c_master_wait_event(I2C_MASTER_EVENT_TX_COMPLETE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C read operation */
    err = R_IIC_MASTER_Read(&g_i2c_master_for_peripheral_ctrl, p_data, 1, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until read transmission complete */
    err = i2c_master_wait_event(I2C_MASTER_EVENT_RX_COMPLETE);

    return err;
}
/***********************************************************************************************************************
* End of function read_reg_16bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i2c_master_wait_event.
 * Description  : This function waits for the specified I2C Master event.
 * Arguments    : i2c_event                 I2C Master event to wait for.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t i2c_master_wait_event(const i2c_master_event_t i2c_event)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t time_out = UINT8_MAX;

    /* Wait until the event occurs */
    while (i2c_event != g_i2c_event_for_peripheral)
    {
        /* Check for aborted event */
        if (I2C_MASTER_EVENT_ABORTED == g_i2c_event_for_peripheral)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }
        else
        {
            /* Check for timeout to avoid an infinite loop */
            time_out--;
            R_BSP_SoftwareDelay(I2C_TIMEOUT_UNIT, BSP_DELAY_UNITS_MICROSECONDS);

            /* Check for timeout */
            if (RESET_VALUE == time_out)
            {
                return FSP_ERR_TIMEOUT;
            }
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function i2c_master_wait_event
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i2c_control_deinit.
 * Description  : This function closes the opened I2C Master module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void i2c_control_deinit(void)
{
    /* Close the opened I2C Master module */
    if (g_i2c_master_for_peripheral_ctrl.open)
    {
        R_IIC_MASTER_Close(&g_i2c_master_for_peripheral_ctrl);
    }
}
/***********************************************************************************************************************
* End of function i2c_control_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: g_i2c_master_for_peripheral_callback.
 * Description  : This function is a callback for I2C Master module.
 * Arguments    : p_args                    Pointer to callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void g_i2c_master_for_peripheral_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_i2c_event_for_peripheral = p_args->event;
    }
}
/***********************************************************************************************************************
* End of function g_i2c_master_for_peripheral_callback
***********************************************************************************************************************/
