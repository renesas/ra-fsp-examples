/***********************************************************************************************************************
 * File Name    : RmcI2C.h
 * Description  : Contains data structures and function declarations
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#ifndef RMCI2C_H_
#define RMCI2C_H_

#include "hal_data.h"
#include "common_utils.h"

/* Define macros for I2C pins */
/* I2C channel 0 */
#define I2C_SDA_0 (BSP_IO_PORT_04_PIN_01)
#define I2C_SCL_0 (BSP_IO_PORT_04_PIN_00)

/* Function declarations */
void RmComDevice_init(void);
void ICM20948_Sensor_init(void);
void RmComDevice_init_Icm(void);
void _delay(uint32_t ms);
void bsp_recover_iic(const bsp_io_port_pin_t SCL, const bsp_io_port_pin_t SDA);

fsp_err_t RmCom_I2C_r(uint8_t reg, uint8_t *val, uint8_t num);
fsp_err_t RmCom_I2C_w(uint8_t reg, uint8_t *val, uint8_t num);
fsp_err_t ICP_RmCom_I2C_r(uint8_t *val, uint32_t num);
fsp_err_t ICP_RmCom_I2C_w(uint8_t *val, uint32_t num);

#endif /* RMCI2C_H_ */
