/***********************************************************************************************************************
 * File Name    : i2c_control.h
 * Description  : Contains data structures and functions used in i2c_control.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef I2C_CONTROL_H_
#define I2C_CONTROL_H_

#include "common_utils.h"

#define I2C_TIMEOUT_UNIT    (10U)

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
fsp_err_t i2c_control_init (void);
fsp_err_t write_reg_16bit (uint16_t address, uint8_t data);
fsp_err_t read_reg_16bit (uint16_t address, uint8_t * p_data);
fsp_err_t write_reg_8bit (uint8_t address, uint8_t data);
fsp_err_t read_reg_8bit (uint8_t address, uint8_t * p_data);

#endif /* I2C_CONTROL_H_ */
