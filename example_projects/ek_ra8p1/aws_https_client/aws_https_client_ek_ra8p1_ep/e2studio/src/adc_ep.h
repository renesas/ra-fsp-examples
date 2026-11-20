/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains macros and functions used in the adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

#include "board_cfg.h"

/* Macros for ADC based on specific version */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_CALIBRATE(ctrl, p_cfg)      (R_ADC_Calibrate((ctrl), (p_cfg)))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_StatusGet((ctrl), (p_status)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_ScanStart((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_Close((ctrl)))
    #define ADC_IRQ_DISABLE()               (R_BSP_IrqDisable((IRQn_Type) VECTOR_NUMBER_ADC0_SCAN_END))
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_B_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_B_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_CALIBRATE(ctrl, p_cfg)      (R_ADC_B_Calibrate((ctrl), (p_cfg)))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_B_StatusGet((ctrl), (p_status)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_B_ScanStart((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_B_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_B_Close((ctrl)))
    #define ADC_IRQ_DISABLE()               (R_BSP_IrqDisable((IRQn_Type) VECTOR_NUMBER_ADC_ADI0))
#endif /* ADC version */

/* Function declarations */
fsp_err_t adc_init(void);
fsp_err_t adc_data_read(float * const p_temp_c);
void adc_deinit(void);

#endif /* ADC_EP_H_ */
