#include "i2c_control.h"
#include "switch_init.h"

/* Private function declaration */
static fsp_err_t set_switch_reg(uint8_t reg_addr, uint8_t bit_position, uint8_t bit_value);

/***********************************************************************************************************************
 *  Function Name: set_switch_state
 *  Description  : This function is used to set state of the switch on-board
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t set_switch_state(uint8_t sw_num, uint8_t target_state)
{
    fsp_err_t err = FSP_SUCCESS;

    if (sw_num < 1 || sw_num > 8)
    {
        return FSP_ERR_ABORTED;
    }

    uint8_t bit_position = sw_num - 1;

    /* Change the I2C slave address to control the switch onboard */
    err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_for_peripheral_ctrl, SWITCH_ADDR, I2C_MASTER_ADDR_MODE_7BIT);
    APP_ERR_RET( FSP_SUCCESS != err, err, "** R_IIC_MASTER_SlaveAddressSet API failed **");

    /* Set the selected switch pin as output */
    err = set_switch_reg(PIN_DIR_REG, bit_position, OUTPUT_DIR);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Set switch port as output failed");

    /* Enable the output state for the selected switch port */
    err = set_switch_reg(OUTPUT_ENABLE_REG, bit_position, OUTPUT_ENABLE);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Enable the output state of switch port failed");

    /* Set the target state for the selected switch port */
    err = set_switch_reg(OUTPUT_STATE_REG, bit_position, target_state);
    APP_ERR_RET( FSP_SUCCESS != err, err, "Set the logic state of switch port failed");

    return err;
}
/***********************************************************************************************************************
* End of function set_switch_state
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: set_switch_reg
 *  Description  : This function is used to set the value for 1 bit of the 8-bits register
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t set_switch_reg(uint8_t reg_addr, uint8_t bit_position, uint8_t bit_value)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t read_data = RESET_VALUE;

    /* Read the current data of switch register */
    read_reg_8bit(reg_addr, &read_data);

    /* Write the expected value to specific bit of switch register */
    write_reg_8bit(reg_addr, (uint8_t) ((read_data & ~ (1 << bit_position)) | (bit_value << bit_position)));

    /* Read back to confirm */
    read_reg_8bit(reg_addr, &read_data);

    if (bit_value != ((read_data >> bit_position) & 1))
    {
        return FSP_ERR_ABORTED;
    }

    return err;
}
/***********************************************************************************************************************
* End of function set_switch_reg
***********************************************************************************************************************/
