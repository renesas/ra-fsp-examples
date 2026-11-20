/***********************************************************************************************************************
 * File Name    : switch_init.c
 * Description  : Contains data structures and functions used in switch_init.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "i2c_control.h"
#include "switch_init.h"

/* Private function */
static fsp_err_t set_switch_reg(uint8_t reg_addr, uint8_t bit_position, uint8_t bit_value);

/***********************************************************************************************************************
 * Function Name: set_switch_state.
 * Description  : This function sets the state of the selected on-board switch.
 * Arguments    : sw_num                    Switch number.
 *                target_state              Target output state.
 * Return Value : FSP_SUCCESS               Set successfully.
 *                Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t set_switch_state(uint8_t sw_num, uint8_t target_state)
{
    fsp_err_t err = FSP_SUCCESS;

    if (sw_num < 1 || sw_num > 8)
    {
        return FSP_ERR_ABORTED;
    }

    uint8_t bit_position = sw_num - 1;

    /* Change the I2C Slave address to control the on-board switch */
    err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_for_peripheral_ctrl, SWITCH_ADDR, I2C_MASTER_ADDR_MODE_7BIT);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the selected switch pin as output */
    err = set_switch_reg(PIN_DIR_REG, bit_position, OUTPUT_DIR);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable the output state for the selected switch port */
    err = set_switch_reg(OUTPUT_ENABLE_REG, bit_position, OUTPUT_ENABLE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the target state for the selected switch port */
    err = set_switch_reg(OUTPUT_STATE_REG, bit_position, target_state);

    return err;
}
/***********************************************************************************************************************
* End of function set_switch_state
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: set_switch_reg.
 * Description  : This function sets the value of a single bit in an 8-bit register.
 * Arguments    : reg_addr                  Register address.
 *                bit_position              Bit position to update.
 *                bit_value                 Value to write.
 * Return Value : FSP_SUCCESS               Set successfully.
 *                Any other error codes     Failed to set.
 **********************************************************************************************************************/
static fsp_err_t set_switch_reg(uint8_t reg_addr, uint8_t bit_position, uint8_t bit_value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_data = RESET_VALUE;

    /* Read the current data of switch register */
    err = read_reg_8bit(reg_addr, &read_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Write the expected value to specific bit of switch register */
    err = write_reg_8bit(reg_addr, (uint8_t) ((read_data & ~ (1 << bit_position)) | (bit_value << bit_position)));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back to confirm */
    err = read_reg_8bit(reg_addr, &read_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (bit_value != ((read_data >> bit_position) & 1))
    {
        return FSP_ERR_ABORTED;
    }

    return err;
}
/***********************************************************************************************************************
* End of function set_switch_reg
***********************************************************************************************************************/
