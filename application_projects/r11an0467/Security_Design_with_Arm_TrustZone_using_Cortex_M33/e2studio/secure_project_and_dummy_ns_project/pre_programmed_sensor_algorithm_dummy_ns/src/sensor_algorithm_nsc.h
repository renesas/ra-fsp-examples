/***********************************************************************************************************************
 * File Name    : sensor_algorithm_nsc.h
 * Description  : contains non-secure callable API interfaces
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#ifndef SENSOR_ALGORITHM_NSC_H_
#define SENSOR_ALGORITHM_NSC_H_

#include <hal_data.h>

BSP_CMSE_NONSECURE_ENTRY void sensor_algorithm_start_guard(void);
BSP_CMSE_NONSECURE_ENTRY void sensor_algorithm_stop_guard(void);

#endif /* SENSOR_ALGORITHM_NSC_H_ */
