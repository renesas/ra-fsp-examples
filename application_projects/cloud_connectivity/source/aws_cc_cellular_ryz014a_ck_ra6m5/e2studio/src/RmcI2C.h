/***********************************************************************************************************************
 * File Name    : RmcI2C.h
 * Description  : Contains data structures and function declarations
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef RMCI2C_H_
#define RMCI2C_H_

#include "hal_data.h"
#include "common_utils.h"

/* Function declarations */
void RmComDevice_init(void);
void ICM20948_Sensor_init();
void RmComDevice_init_Icm(void);
void delay_Microseconds(uint32_t us);
void _delay(uint32_t ms);

fsp_err_t RmCom_I2C_r(uint8_t reg, uint8_t *val, uint8_t num);
fsp_err_t RmCom_I2C_w(uint8_t reg, uint8_t *val, uint8_t num);
fsp_err_t ICP_RmCom_I2C_r(uint8_t *val, uint32_t num);
fsp_err_t ICP_RmCom_I2C_w(uint8_t *val, uint32_t num);

#endif /* RMCI2C_H_ */
