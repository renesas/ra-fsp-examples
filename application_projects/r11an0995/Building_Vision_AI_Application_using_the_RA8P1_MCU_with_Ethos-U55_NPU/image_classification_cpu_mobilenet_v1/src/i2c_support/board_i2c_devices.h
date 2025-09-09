/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BOARD_I2C_DEVICES__H_
#define BOARD_I2C_DEVICES__H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"


uint8_t wrSWReg16_8(int regID, int regDat);
uint8_t rdSWReg16_8(uint16_t regID, uint8_t* regDat);

#endif /*  */
