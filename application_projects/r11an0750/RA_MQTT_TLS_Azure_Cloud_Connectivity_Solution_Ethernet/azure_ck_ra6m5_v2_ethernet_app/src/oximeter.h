/***********************************************************************************************************************
 * File Name    : oximeter.h
 * Description  : Contains user defined data types used in oximeter
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OXIMETER_H_
#define OXIMETER_H_

#include "hal_data.h"
#include "common_utils.h"
#include "ob1203_bio/ob1203_bio.h"
#include "OB_1203_Thread.h"


void RA_ob1203_init(void);
void ob1203_spo2_main(void);

#endif /* OXIMETER_H_ */
