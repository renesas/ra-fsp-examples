/***********************************************************************************************************************
 * File Name    : RA_HS3001.h
 * Description  : Contains data structures and function prototypes for HS3001 sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef RA_HS3001_H_
#define RA_HS3001_H_

void g_comms_i2c_bus0_quick_setup(void);
void g_hs300x_sensor0_quick_setup(void);
void hs3001_get(void);

#endif
