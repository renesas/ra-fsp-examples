/***********************************************************************************************************************
 * File Name    : adc_app.h
 * Description  : Contains macros, data structures and functions used in the adc_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_APP_H_
#define ADC_APP_H_

#include "board_cfg.h"

/* Macro definitions */
#if defined (BOARD_RA8P1_EK)
#define ADCTEMP_AS_C(a)             ((((float)(a)) * (0.298147f)) - 287.388f)
#define ADCTEMP_AS_F(a)             ((((float)(a)) * (0.536652f)) - 485.298f)
#else
#define ADCTEMP_AS_C(a)             ((((float)(a)) * (0.196551f)) - 277.439f)
#define ADCTEMP_AS_F(a)             ((((float)(a)) * (0.353793f)) - 467.39f)
#endif

/* Macro for ADC version */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
#define ADC_TYPE                    "ADC_B"
#else
#define ADC_TYPE                    "ADC"
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

/* User function declarations */
fsp_err_t hal_adc_init(void);
float adc_data_read(void);
void hal_adc_deinit(void);

#endif /* ADC_APP_H_ */
