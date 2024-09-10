/***********************************************************************************************************************
 * File Name    : i2c.h
 * Description  : File contains i2c function prototypes and macros
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#include "hal_data.h"
#ifndef I2C_H
#define I2C_H

void comms_i2c_callback(rm_comms_callback_args_t * p_args);
#define DEL (NOP(); NOP(); NOP(); NOP();) //NOP(); NOP();
/* Start user code for pragma. Do not edit comment generated here */
#define I2C_TRANSMISSION_IN_PROGRESS        (0)
#define I2C_TRANSMISSION_COMPLETE           (1)
#define I2C_TRANSMISSION_ABORTED            (2)

int8_t I2C_w(uint8_t addr, uint8_t reg, uint8_t *val, char num);
int8_t I2C_r(uint8_t addr, uint8_t reg, uint8_t *val, char num);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
