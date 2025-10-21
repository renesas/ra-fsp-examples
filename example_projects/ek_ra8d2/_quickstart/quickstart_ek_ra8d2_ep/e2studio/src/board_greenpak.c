/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_greenpak.c
 * Version      : 1.0
 * Description  : greenpak application functions used by the system to manage sw4.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
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
#include "board_greenpak.h"
#include "common_utils.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define BSP_I2C_SLAVE_ADDR_GREENPAK_RAM    (0x20)   //Slave address for GreenPak RAM access
#define BSP_I2C_SLAVE_ADDR_GREENPAK_NVRAM  (0x21)   //Slave address for GreenPak NVRAM access
#define BSP_I2C_SLAVE_ADDR_GREENPAK_EEPROM (0x22)   //Slave address for GreenPak EEPROM access

#define GREENPAK_IO4_EXPECTED_VALUE   (0x30)
#define GREENPAK_IO4_MASK             (0xCF)

#define GREENPAK_IO5_EXPECTED_VALUE   (0x30)
#define GREENPAK_IO5_MASK             (0xCF)


#define LOCAL_GREENPAK_WARNING_MSG ("\r\nGreenPak device configuration out of date.\r\n" \
                                    "Please look for reprogramming Errata on the Renesas website.\r\n" \
                                    "This code will execute correctly.\r\n")

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
char_t g_greenpak_warning_str[256];

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: board_greenpak_init
 * Description  : 
 * Argument     : 
 * Return Value : Greenpak status on startup
 *                FSP_SUCCESS          - Configured as expected.
 *                FSP_ERR_INVALID_MODE - Configuration not as expected. The  function will attempt to configure
 *
 *********************************************************************************************************************/
fsp_err_t board_greenpak_init(void)
{
    uint8_t reg_val1 = 0;
    uint8_t reg_val2 = 0;

    uint8_t reg_start = 0x66;

    fsp_err_t retval = FSP_ERR_INVALID_MODE;

    uint8_t ram_reg_array[32] = {};
    uint8_t eeprom_reg_array[32] = {};

    if(g_board_i2c_master_ctrl.open == 0)
    {
        if(FSP_SUCCESS != R_IIC_MASTER_Open(&g_board_i2c_master_ctrl, &g_board_i2c_master_cfg))
        {
            return (retval);
        }
    }

    /** Reading 16 EEPROM byte block from sub address */
    R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, BSP_I2C_SLAVE_ADDR_GREENPAK_EEPROM, I2C_MASTER_ADDR_MODE_7BIT);
    for (uint8_t ndx = 0; ndx < 16; ndx++)
    {
        reg_val1 = reg_start + ndx;
        rd_sensor_reg8_8(reg_val1,&reg_val2);
        eeprom_reg_array[ndx] = reg_val2;
    }

    /** Generate warning message (used later) to inform user GreenPak device may need to be updated */
    g_greenpak_warning_str[0] = '\0';

    uint8_t t0 = eeprom_reg_array[0] & GREENPAK_IO4_EXPECTED_VALUE;
    uint8_t t1 = eeprom_reg_array[1] & GREENPAK_IO4_EXPECTED_VALUE;

    if((t0 != (uint8_t)GREENPAK_IO4_EXPECTED_VALUE) || (t1 != (uint8_t)GREENPAK_IO4_EXPECTED_VALUE))
    {
        sprintf(g_greenpak_warning_str,LOCAL_GREENPAK_WARNING_MSG);

        ram_reg_array[0] = ram_reg_array[0] & GREENPAK_IO4_MASK;
        ram_reg_array[0] += GREENPAK_IO4_EXPECTED_VALUE;

        ram_reg_array[1] = ram_reg_array[1] & GREENPAK_IO5_MASK;
        ram_reg_array[1] += GREENPAK_IO5_EXPECTED_VALUE;

        R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, BSP_I2C_SLAVE_ADDR_GREENPAK_RAM, I2C_MASTER_ADDR_MODE_7BIT);
        for (uint8_t ndx = 0; ndx < 16; ndx++)
        {
            reg_val1 = reg_start + ndx;
            reg_val2 = ram_reg_array[ndx];
            wr_sensor_reg8_8(reg_val1,reg_val2);
        }
    }
    else
    {
        /* No update required */
        retval = FSP_SUCCESS;
    }
    return (retval);
}
/**********************************************************************************************************************
 End of function board_greenpak_init
 *********************************************************************************************************************/
