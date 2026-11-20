/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains macros and functions used in the adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

#include "hal_data.h"

/* Macro to convert the ADC temperature reading to degrees Celsius */
#if defined(BOARD_RA8P1_EK)
#define ADCTEMP_AS_C(a)             ((((float)(a)) * 0.298147f) - 287.388f)
#else
#define ADCTEMP_AS_C(a)             ((((float)(a)) * (0.196551f)) - 277.439f)
#endif /* Specific board */

/* Macros for ADC based on specific version */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    #define ADC_CHANNEL_CONFIG              (g_adc0_scan_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_B_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_B_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_B_ScanStart((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_B_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_B_Close((ctrl)))
#else
    #define ADC_CHANNEL_CONFIG              (g_adc0_channel_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_ScanStart((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_Close((ctrl)))
#endif /* ADC version */

/* User function declarations */
fsp_err_t adc_init(void);
fsp_err_t adc_data_read(float * const p_temp_c);
void adc_deinit(void);

#endif /* ADC_EP_H_ */
