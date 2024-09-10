/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * File Name    : dwt.h
 * Description  : Contains exported functions defined in dwt.c.
 **********************************************************************************************************************/
#ifndef DWT_H_
#define DWT_H_
#include "hal_data.h"


/* enable DWT */
void InitCycleCounter(void);

/* reset the DWT cycle counter */
void ResetCycleCounter(void);

/* enable cycle counter */
void EnableCycleCounter(void);

/* retrieve the DWT cycle count */
uint32_t GetCycleCounter(void);

/* disable the cycle counter */
void DisableCycleCounter(void);

#endif /* DWT_H_ */
