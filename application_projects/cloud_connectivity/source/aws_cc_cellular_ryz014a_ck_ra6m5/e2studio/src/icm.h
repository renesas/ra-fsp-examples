/***********************************************************************************************************************
 * File Name    : icm.h
 * Description  : Contains function prototypes for ICM20948 sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef ICM_H_
#define ICM_H_

#include "hal_data.h"
#include "common_utils.h"

void send_icm_data_to_queue(void);

#endif /* ICM_H_ */
