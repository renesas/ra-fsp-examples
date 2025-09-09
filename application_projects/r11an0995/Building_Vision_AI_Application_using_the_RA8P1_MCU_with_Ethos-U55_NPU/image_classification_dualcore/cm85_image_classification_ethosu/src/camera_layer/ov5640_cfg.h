/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef OV5640_CFG_H_
#define OV5640_CFG_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"

#define OV5640_I2C_SLAVE_ADDR 0x78>>1
#define OV5640_CAM_PWR_ON                   (BSP_IO_PORT_07_PIN_04)
#define OV5640_CAM_RESET                    (BSP_IO_PORT_07_PIN_09)

void ov5640_write_reg(uint16_t reg, uint8_t dat);
uint8_t ov5640_read_reg(uint16_t reg);

void ov5640_hw_init(void);
void ov5640_exit_power_down(void);
void ov5640_hw_reset(void);
void ov5640_sw_reset(void);

int OV5640_get_sysclk(void);

#endif
