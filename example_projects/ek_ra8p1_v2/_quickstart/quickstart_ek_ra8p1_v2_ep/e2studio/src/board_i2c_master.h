/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_i2c_master.h
 * Version      : 1.0
 * Description  : I2C Bus master driver, I/F .h
  *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef BOARD_I2C_MASTER_CFG_H_
#define BOARD_I2C_MASTER_CFG_H_
/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

#define EK_RA8P1_I2C_SLAVE_ADDR_GREENPAK_RAM        (0x20)   //Slave address for Greenpak RAM access
/* SxxxxAAA--    */
#define EK_RA8P1_I2C_SLAVE_ADDR_GREENPAK_NVRAM      (0x22)   //Slave address for Greenpak EEPROM access

#define EK_RA8P1_I2C_SLAVE_ADDR_GREENPAK_EEPROM     (0x23)   //Slave address for Greenpak EEPROM access


#define GP_I2C_REGISTER_GREENPAK_BLOCK_ERASE        (0xE3)  /* Greenpack block erase register */
#define GP_I2C_COMMAND_GREENPAK_BLOCK_ERASE         (0x80)  /* Greenpack block erase register */
#define GP_I2C_COMMAND_GREENPAK_EEPROM_BLOCK_ERASE  (0x90)  /* Greenpack block erase register */
#define GP_I2C_REGISTER_GREENPAK_C8                 (0xC8)  /* Greenpack ????????????? */
#define GP_I2C_COMMAND_GREENPAK_02                  (0x02)  /* Greenpack ????????????? */

#define RESET_VALUE             (0x00)

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern fsp_err_t board_i2c_master_wait_complete(void);

extern fsp_err_t wr_sensor_reg8_8(int regID, int regDat);
extern fsp_err_t rd_sensor_reg8_8(uint16_t regID, uint8_t* regDat);

extern fsp_err_t wr_sensor_reg16_8(i2c_master_ctrl_t * p_api_ctrl, uint16_t regID, uint8_t regDat);
extern fsp_err_t rd_sensor_reg16_8(i2c_master_ctrl_t * p_api_ctrl, uint16_t regID, uint8_t* regDat);

#endif /* BOARD_I2C_MASTER_CFG_H_ */
