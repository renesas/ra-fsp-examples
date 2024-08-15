/***********************************************************************************************************************
 * File Name    : adc_app.h
 * Description  : Contains macros, data structures and functions used in the adc configuration
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_APP_H_
#define ADC_APP_H_

#include "board_cfg.h"

/* User function declarations */
fsp_err_t hal_adc_init(void);
float adc_data_read(void);
void hal_adc_deinit(void);

#endif /* ADC_APP_H_ */
