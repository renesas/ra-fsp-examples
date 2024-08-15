/***********************************************************************************************************************
 * File Name    : i2c_sensor.h
 * Description  : Contains data structures and functions used in i2c_sensor.h/c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#ifndef I2C_SENSOR_H_
#define I2C_SENSOR_H_

#define ONE_BYTE                0x01
#define TWO_BYTE                0x02

#define MEASURE_PAYLOAD_SIZE    0x03        //measurement enable data length
#define ACCELERO_DELAY          0xC8
#define SENSOR_READ_DELAY       0x03
#define ENABLE_BIT              0x08
#define DATA_REGISTERS          0x06

/* Accelerometer internal register whichever consumed here */
#define DEVICE_ID_REG           0x00
#define DEVICE_SIGNATURE        0xE5
#define POWER_CTL_REG           0x2D
#define AXIS_DATA               0x32

#define SENSOR_DATA_SIZE        0x06

/*
 * function declarations
 */
fsp_err_t init_sensor(void);
void deinit_sensor(void);
fsp_err_t read_sensor_data(uint8_t *xyz_data);

#endif /* I2C_SENSOR_H_ */
