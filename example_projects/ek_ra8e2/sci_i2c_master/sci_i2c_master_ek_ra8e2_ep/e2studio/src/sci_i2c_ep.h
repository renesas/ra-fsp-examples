/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.h
 * Description  : Contains data structures and functions used in sci_i2c_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_

/* Macros for Accelerometer sensitivity g/LSB */
#define SENSOR_SENSITIVITY          (0.0039F)
#define SENSOR_RESET_VALUE          (0.0F)

/* Macros for Accelerometer configure data */
#define SENSOR_ID_DATA              (0xE5)
#define SENSOR_BW_RATE_DATA         (0x04)
#define SENSOR_POWER_CTL_DATA       (0x08)
#define SENSOR_INT_ENABLE_DATA      (0x80)
#define SENSOR_INT_SOURCE_DATA      (0x80)

/*  Macros for Accelerometer register address */
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

/* Macros to wait for Accelerometer to activate new data is ready */
#define DATA_TIMEOUT_UNIT           (BSP_DELAY_UNITS_MILLISECONDS)
#define DATA_TIMEOUT_DIV            (10U)

/* Macros for I2C transfer */
#define I2C_ONE_BYTE                (1U)
#define I2C_TWO_BYTE                (2U)
#define I2C_DATA_BYTE               (SENSOR_DATA_SIZE)
#define I2C_TIMEOUT_UNIT            (BSP_DELAY_UNITS_MILLISECONDS)
#define I2C_TIMEOUT_DIV             (1U)

/* Macro for printing acceleration value to RTT */
#define SENSOR_STR_SIZE             (64U)

/* Public function declarations */
void sci_i2c_entry(void);

#endif /* SCI_I2C_EP_H_ */
