/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : time_counter.h
 * Description  : Contains exported functions defined in time_counter.c.
 **********************************************************************************************************************/
#ifndef TIME_COUNTER_H_
#define TIME_COUNTER_H_
#include "hal_data.h"

FSP_CPP_HEADER


void     TimeCounter_Init(void);
void     TimeCounter_Disable(void);
void     TimeCounter_CountReset(void);
uint32_t TimeCounter_CurrentCountGet(void);
uint32_t TimeCounter_CountValueConvertToMs(uint32_t t1, uint32_t t2);
uint32_t TimeCounter_ConvertFromMsToFps(uint32_t ms);
FSP_CPP_FOOTER

#endif /* TIME_COUNTER_H_ */
