/***********************************************************************************************************************
 * File Name    : sau_i2c_master_ep.h
 * Description  : Contains macros and functions used in sau_i2c_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_I2C_MASTER_EP_H_
#define SAU_I2C_MASTER_EP_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "hal_data.h"

/* Macros for default values */
#define RESET_VALUE                 (0x00)
#define SENSOR_RESET_VALUE          (0.0F)

/* Macro for Accelerometer sensitivity g/LSB */
#define SENSOR_SENSITIVITY          (0.0039F)

/* Macros for Accelerometer configure data */
#define SENSOR_ID_DATA              (0xE5)
#define SENSOR_BW_RATE_DATA         (0x04)
#define SENSOR_POWER_CTL_DATA       (0x08)
#define SENSOR_INT_ENABLE_DATA      (0x80)
#define SENSOR_INT_SOURCE_DATA      (0x80)

/* Macros for Accelerometer register address */
#define SENSOR_ID_REG               (0x00)
#define SENSOR_BW_RATE_REG          (0x2C)
#define SENSOR_POWER_CTL_REG        (0x2D)
#define SENSOR_INT_ENABLE_REG       (0x2E)
#define SENSOR_INT_SOURCE_REG       (0x30)
#define SENSOR_AXIS_DATA_REG        (0x32)
#define SENSOR_DATA_SIZE            (6U)
#define X_AXIS_ADDR_OFFSET          (0U)
#define Y_AXIS_ADDR_OFFSET          (1U)
#define Z_AXIS_ADDR_OFFSET          (2U)

/* Macro to wait for Accelerometer to activate new data is ready */
#define DATA_TIMEOUT_DELAY          (10U)

/* Macros for I2C transfer */
#define I2C_ONE_BYTE                (1U)
#define I2C_TWO_BYTE                (2U)
#define I2C_DATA_BYTE               (SENSOR_DATA_SIZE)
#define I2C_TIMEOUT_UNIT            (BSP_DELAY_UNITS_MICROSECONDS)
#define I2C_TIMEOUT_DIV             (1U)
#define I2C_TIMEOUT_PERIOD          (8000U)

/* Function declarations */
fsp_err_t sensor_init(void);
fsp_err_t sensor_wait_data_ready(void);
fsp_err_t sensor_read_data(uint8_t * p_data);
fsp_err_t sensor_convert_to_g_unit(uint8_t * p_data, float * p_x_axis, float * p_y_axis, float * p_z_axis);
void sensor_deinit(void);

#endif /* SAU_I2C_MASTER_EP_H_ */
