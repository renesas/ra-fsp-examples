/***********************************************************************************************************************
 * File Name    : i2c_control.c
 * Description  : Contains data structures and functions used to control I2C master module.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "i2c_control.h"

/* Private function declaration */
static fsp_err_t i2c_master_wait_event (const i2c_master_event_t i2c_event);

/* Private global variables */
static volatile i2c_master_event_t g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

/***********************************************************************************************************************
 *  Function Name: i2c_control_init
 *  Description  : This function is used to initialize I2C master module
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t i2c_control_init (void)
{
    fsp_err_t err           = FSP_SUCCESS;

    /* Initialize IIC module */
    err = R_IIC_MASTER_Open(&g_i2c_master_for_peripheral_ctrl, &g_i2c_master_for_peripheral_cfg);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Open API FAILED ** \r\n");

    return err;
}
/***********************************************************************************************************************
* End of function i2c_control_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: write_reg_8bit
 *  Description  : This function is used to write data to 8-bits camera register.
 *  Arguments    : address        Register address
 *                 data           Data to be written
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t write_reg_8bit (uint8_t address, uint8_t data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[2];

    /* Prepare data for write operation */
    i2c_buffer[0] = address;
    i2c_buffer[1] = data;

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 2, false);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** i2c_master_wait_event FAILED ** \r\n");

    return err;
}
/***********************************************************************************************************************
* End of function write_reg_8bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: read_reg_8bit
 *  Description  : This function is used to read data from 8-bits camera register.
 *  Arguments    : address        Register address
 *                 p_data         Pointer used to store register data
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t read_reg_8bit (uint8_t address, uint8_t * p_data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[1];

    /* Prepare data for write operation */
    i2c_buffer[0] = address;

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 1, true);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** i2c_master_wait_event FAILED ** \r\n");

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C read operation */
    err = R_IIC_MASTER_Read(&g_i2c_master_for_peripheral_ctrl, p_data, 1, false);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Read API FAILED ** \r\n");

    /* Wait until read transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** i2c_master_wait_event FAILED ** \r\n");
    return err;
}
/***********************************************************************************************************************
* End of function read_reg_8bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: read_reg_16bit
 *  Description  : This function is used to read data from 16-bits camera register.
 *  Arguments    : address        Register address
 *                 p_data         Pointer used to store register data
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t read_reg_16bit (uint16_t address, uint8_t * p_data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[2];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 2, true);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** i2c_master_wait_event FAILED ** \r\n");

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C read operation */
    err = R_IIC_MASTER_Read(&g_i2c_master_for_peripheral_ctrl, p_data, 1, false);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Read API FAILED ** \r\n");

    /* Wait until read transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** i2c_master_wait_event FAILED ** \r\n");

    return err;
}
/***********************************************************************************************************************
* End of function read_reg_16bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: write_reg_16bit
 *  Description  : This function is used to write data to 16-bits camera register.
 *  Arguments    : address        Register address
 *                 data           Data to be written
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t write_reg_16bit (uint16_t address, uint8_t data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[3];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);
    i2c_buffer[2] = data;

    /* Reset callback event */
    g_i2c_event_for_peripheral = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_peripheral_ctrl, i2c_buffer, 3, false);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RET( FSP_SUCCESS != err, err, " ** i2c_master_wait_event FAILED ** \r\n");

    return err;
}
/***********************************************************************************************************************
* End of function write_reg_16bit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: i2c_master_wait_event
 *  Description  : This function is used to wait for i2c master event.
 *  Arguments    : i2c_event      i2c master events need to wait for
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t i2c_master_wait_event (const i2c_master_event_t i2c_event)
{
    fsp_err_t err      = FSP_SUCCESS;
    uint8_t   time_out = UINT8_MAX;

    /* Wait until event occur */
    while(i2c_event != g_i2c_event_for_peripheral)
    {
        /* Check for aborted event */
        if(I2C_MASTER_EVENT_ABORTED == g_i2c_event_for_peripheral)
        {
            APP_ERR_RET( FSP_SUCCESS != err, FSP_ERR_TRANSFER_ABORTED,
                           " ** Error EVENT_ABORTED received during perform i2c operation **\r\n");
        }
        else
        {
            /* Start checking for time out to avoid infinite loop */
            time_out --;
            R_BSP_SoftwareDelay(I2C_TIMEOUT_UNIT, BSP_DELAY_UNITS_MICROSECONDS);

            /* Check for time elapse*/
            if (RESET_VALUE == time_out)
            {
                APP_ERR_RET( FSP_SUCCESS != err, FSP_ERR_TIMEOUT, " ** I2C Master Callback event not received **\r\n");
            }
        }
    }
    return err;
}
/***********************************************************************************************************************
* End of function i2c_master_wait_event
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: g_i2c_master_for_peripheral_callback
 *  Description  : This function is used to get i2c master event
 *  Arguments    : p_args      Pointer to callback argument
 *  Return Value : None
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
