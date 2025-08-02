/***********************************************************************************************************************
 * File Name    : adc_hal.h
 * Description  : Contains data structures and functions used in adc_hal.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_HAL_H_
#define ADC_HAL_H_

#define NUM_SAMPLES (2048)
#define ZERO        (0U)

/* functions declaration */
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc);
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc);
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc);
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc);

#endif /* ADC_HAL_H_ */
