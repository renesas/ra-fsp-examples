/***********************************************************************************************************************
 * File Name    : adc_app.h
 * Description  : Contains macros, data structures and functions used in the ADC configuration.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_APP_H_
#define ADC_APP_H_

#include "board_cfg.h"

/* Function declarations */
fsp_err_t hal_adc_init(void);
float adc_data_read(void);
void hal_adc_deinit(void);

/* Macro for ADC type */
#if (BSP_PERIPHERAL_ADC_PRESENT)
#define ADC_TYPE    "ADC"
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
#define ADC_TYPE    "ADC_B"
#endif

#endif /* ADC_APP_H_ */
