/***********************************************************************************************************************
 * File Name    : i2c_control.h
 * Description  : Contains macros and functions used in i2c_control.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef I2C_CONTROL_H_
#define I2C_CONTROL_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE         (0x00)

/* Macro for I2C timeout unit */
#define I2C_TIMEOUT_UNIT    (10U)

/* Function declarations */
fsp_err_t i2c_control_init(void);
fsp_err_t write_reg_8bit(uint8_t address, uint8_t data);
fsp_err_t read_reg_8bit(uint8_t address, uint8_t * p_data);
fsp_err_t write_reg_16bit(uint16_t address, uint8_t data);
fsp_err_t read_reg_16bit(uint16_t address, uint8_t * p_data);
void i2c_control_deinit(void);

#endif /* I2C_CONTROL_H_ */
