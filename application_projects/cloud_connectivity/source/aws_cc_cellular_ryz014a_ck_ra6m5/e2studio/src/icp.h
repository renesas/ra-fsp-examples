/***********************************************************************************************************************
 * File Name    : icp.h
 * Description  : Contains function prototypes for ICP sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef ICP_H_
#define ICP_H_

#include "hal_data.h"
#include "common_utils.h"

void send_icp_data_to_queue(void);

#endif /* ICP_H_ */
