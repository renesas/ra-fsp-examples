/***********************************************************************************************************************
 * File Name    : iic_sensor.h
 * Description  : Contains macros and functions used in iic_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IIC_SENSOR_H_
#define IIC_SENSOR_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macros for transfer byte counts */
#define ONE_BYTE                (0x01)
#define TWO_BYTE                (0x02)

/* Macros for sensor configuration */
#define MEASURE_PAYLOAD_SIZE    (0x03)
#define SENSOR_READ_DELAY       (0x03)
#define ENABLE_BIT              (0x08)
#define DATA_REGISTERS          (0x06)

/* Macros for internal accelerometer registers */
#define DEVICE_ID_REG           (0x00)
#define DEVICE_SIGNATURE        (0xE5)
#define POWER_CTL_REG           (0x2D)
#define AXIS_DATA               (0x32)

/* Macros for sensor data */
#define BIT_SHIFT_8             (8U)
#define SENSOR_DATA_SIZE        (0x06)
#define ADXL_DATA_SIZE          (0x07)

/* Function declarations */
fsp_err_t init_iic_master(void);
fsp_err_t init_sensor(void);
fsp_err_t read_sensor_data(uint8_t * xyz_data);
void deinit_sensor(void);
void i2c_master_callback(i2c_master_callback_args_t * p_args);

#endif /* IIC_SENSOR_H_ */
