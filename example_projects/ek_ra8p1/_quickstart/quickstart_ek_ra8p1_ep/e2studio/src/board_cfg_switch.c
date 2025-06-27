/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_cfg_switch.c
 * Version      : 1.0
 * Description  : Handlier for I/F to sw4, greenpak and associated i2c channel used to communicate with devices.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"
#include "r_typedefs.h"
#include "board_cfg.h"
#include "board_hw_cfg.h"
#include "board_i2c_master.h"
#include "board_cfg_switch.h"
#include "common_utils.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define BSP_I2C_SLAVE_ADDR_SWITCH          (0x43)   /* Slave address for SWITCH */

#define DEVICE_PI4IOE5V6408_ALTERNATE_ID   (0xA0)
#define DEVICE_PI4IOE5V6408_ID             (0xA2)

#define IIC_SWITCH_OUTPUT_REG              (0x05)
#define IIC_SWITCH_OUTPUT_HI_Z_REG         (0x07)
#define IIC_SWITCH_IP_DIR_REG              (0x03)
#define IIC_SWITCH_DEV_ID_REG              (0x01)
#define IIC_SWITCH_PUP_D_SLT_REG           (0x0D)
#define IIC_SWITCH_INPUT_STATE_REG         (0x0F)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
char_t g_switch_warning_str[256];

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static bool_t switch_read_block (uint8_t *pdata);

/* Public Functions */

/**********************************************************************************************************************
 * Function Name: board_cfg_switch_init
 * Description  : Initialise PI4IOE5V6408 Low-voltage translating 8-bit I2C-bus I/O Expander.
 *                Note by setting IIC_SWITCH_OUTPUT_REG
 * Argument     : NONE
 * Return Value : Error code on open
 *                FSP_SUCCESS      - PI4IOE5V6408 device located, identified correctly and interface initialised.
 *                FSP_ERR_NOT_OPEN - PI4IOE5V6408 device not located.
 *********************************************************************************************************************/
fsp_err_t board_cfg_switch_init(void)
{
    i2c_master_status_t i2c_status;
    uint8_t reg_val1 = 0;

    fsp_err_t err = FSP_ERR_NOT_OPEN;

    if(g_board_i2c_master_ctrl.open == 0)
    {
        if(!R_IIC_MASTER_Open(&g_board_i2c_master_ctrl, &g_board_i2c_master_cfg))
        {
            return(err);
        }
    }

    R_IIC_MASTER_StatusGet (&g_board_i2c_master_ctrl, &i2c_status);

    R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, BSP_I2C_SLAVE_ADDR_SWITCH, I2C_MASTER_ADDR_MODE_7BIT);

    err = rd_sensor_reg8_8(IIC_SWITCH_DEV_ID_REG, &reg_val1);

    /* Confirm compatible device (using Device ID) is fitted */
    if ((reg_val1 == DEVICE_PI4IOE5V6408_ID) || (reg_val1 == DEVICE_PI4IOE5V6408_ALTERNATE_ID))
    {
        /* All Output HIGH except OPMOD10MD0SEL & OPMOD10MD0SEL which must be LOW */
        wr_sensor_reg8_8(IIC_SWITCH_OUTPUT_REG      ,   0xFC);
		
        /* None as Hi-Z */
        wr_sensor_reg8_8(IIC_SWITCH_OUTPUT_HI_Z_REG ,   0x00);
		
		/* All Output (vs input) */
        wr_sensor_reg8_8(IIC_SWITCH_IP_DIR_REG      ,   0xFF);
    }
    else
    {
        err = FSP_SUCCESS;
    }

    switch_set_status_msg();
    return(err);
}
/**********************************************************************************************************************
 End of function switch_init
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: switch_set_status_msg
 * Description  : Sets global string g_switch_warning_str based on result of check of configuration switch
 *                i2c device
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void switch_set_status_msg(void)
{
    uint8_t data = 0;

    switch_read_block(&data);

    if(SWITCH_EXPECTED_VALUE != data)
    {
        sprintf(g_switch_warning_str,"\r\nEnsure all of switch SW4 is set to OFF\r\n");
    }
    else
    {
        g_switch_warning_str[0] = '\0';
    }
}
/**********************************************************************************************************************
 End of function switch_set_status_msg
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_cfg_switch_funct_off
 * Description  : Set state of sub switch to use a pull DOWN on the board
 * Argument     : sw_pin - in the form SW4_1 to SW4_5
 * Return Value : did operation succeed
 *                TEST_SUCCESS update not required or update applied
 *                FSP_ERR_ASSERTION internal error when updating switch
 *********************************************************************************************************************/
fsp_err_t board_cfg_switch_funct_off (int sw_pin)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t target_sw = 0;
    uint8_t reg_read = 0;

    /* make the number from 1-8 to 0-7 */
    sw_pin--;

    rd_sensor_reg8_8 (IIC_SWITCH_OUTPUT_REG , &reg_read);

    target_sw = reg_read | ((0x01 << sw_pin));

    if(target_sw != reg_read)
    {
        err = wr_sensor_reg8_8(IIC_SWITCH_OUTPUT_REG  ,target_sw);
    }

    return (err);
}
/**********************************************************************************************************************
 End of function board_cfg_switch_funct_off
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_cfg_switch_funct_on
 * Description  : Set state of sub switch to use a pull UP on the board
 * Argument     : sw_pin - in the form SW4_1 to SW4_5
 * Return Value : did operation succeed
 *                TEST_SUCCESS update not required or update applied
 *                FSP_ERR_ASSERTION internal error when updating switch
 *********************************************************************************************************************/
fsp_err_t board_cfg_switch_funct_on (int sw_pin)
{
    fsp_err_t err = FSP_ERR_INVALID_MODE;
    uint8_t target_sw = 0;
    uint8_t reg_read = 0;

    /* make the number from 1-8 to 0-7 */
    sw_pin--;

    rd_sensor_reg8_8 (IIC_SWITCH_OUTPUT_REG , &reg_read);

    target_sw = reg_read & (0xFF ^ (0x01 << sw_pin));

    if(target_sw != reg_read)
    {
        err = wr_sensor_reg8_8(IIC_SWITCH_OUTPUT_REG  ,target_sw);
        err = FSP_SUCCESS;
    }

    return (err);
}
/**********************************************************************************************************************
 End of function board_cfg_switch_funct_on
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_cfg_switch_func_read
 * Description  : Read state of sub switch
 * Argument     : sw_pin - in the form SW4_1 to SW4_5
 * Return Value : state  - current state read from the i2c device
 *********************************************************************************************************************/
fsp_err_t board_cfg_switch_func_read (int sw_pin, bool_t *state)
{
    volatile uint8_t target_sw = 0;
    uint8_t reg_read  = 0;
    uint8_t reg_write = 0;
    fsp_err_t err = FSP_ERR_INVALID_MODE;

    target_sw = 0;

    /* set state as input */
    reg_write = 0x00;
    wr_sensor_reg8_8 (IIC_SWITCH_IP_DIR_REG , reg_write);

    /* read the state of switches (using Input Status Register now device is set as input */
    err = rd_sensor_reg8_8 (IIC_SWITCH_INPUT_STATE_REG , &reg_read);
    target_sw = reg_read & (0x01 << sw_pin);

    /* set the function return value */
    *state = target_sw == 0 ? true : false;

    /* restore state as output may not be needed */
    reg_write = 0xFF;
    err = wr_sensor_reg8_8 (IIC_SWITCH_IP_DIR_REG , reg_write);

    return (err);
}
/**********************************************************************************************************************
 End of function board_cfg_switch_func_read
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private Functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: switch_read_block
 * Description  : Read IIC_SWITCH_INPUT_STATE_REG state of all 8 ports
 * Argument     : pdata - repository for port status.
 * Return Value : success - for future use, current set as true
 *********************************************************************************************************************/
static bool_t switch_read_block (uint8_t *pdata)
{
    bool_t success = true;
    uint8_t reg_write = 0;

    R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, BSP_I2C_SLAVE_ADDR_SWITCH, I2C_MASTER_ADDR_MODE_7BIT);

    /* set state as input */
    reg_write = 0x00;
    wr_sensor_reg8_8 (IIC_SWITCH_IP_DIR_REG , reg_write);

    /* read the state of switches (using Input Status Register now device is set as input */
    rd_sensor_reg8_8 (IIC_SWITCH_INPUT_STATE_REG , pdata);

    /* restore state as output may not be needed */
    reg_write = 0xFF;
    wr_sensor_reg8_8 (IIC_SWITCH_IP_DIR_REG , reg_write);

    return (success);
}
/**********************************************************************************************************************
 End of function switch_read_block
 *********************************************************************************************************************/
