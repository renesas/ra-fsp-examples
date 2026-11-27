/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macros for user buffer */
#define NUM_SAMPLES_PER_CHANNEL             (256U)
#define NUM_ADC_CHANNELS                    (2U)
#define NUM_SAMPLE_BUFFER                   (2U)

/* Macros based on specific ADC version */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
#define ADC0_REG                            (R_ADC_B)
#define ADC1_REG                            (R_ADC_B)
#define ADC_CHANNEL_CONFIG                  (g_adc0_scan_cfg)
#define ADC_OPEN(ctrl, cfg)                 (R_ADC_B_Open((ctrl), (cfg)))
#define ADC_SCANCFG(ctrl, ch_cfg)           (R_ADC_B_ScanCfg((ctrl), (ch_cfg)))
#define ADC_SCANSTART(ctrl)                 (R_ADC_B_ScanStart((ctrl)))
#define ADC_CLOSE(ctrl)                     (R_ADC_B_Close((ctrl)))
#define ADC_CALIBRATE(ctrl)                 (R_ADC_B_Calibrate((ctrl), NULL))
#define ADC_STATUSGET(ctrl, p_status)       (R_ADC_B_StatusGet((ctrl), (p_status)))
#elif (BSP_PERIPHERAL_ADC_PRESENT)
#define ADC0_REG                            (R_ADC0)
#define ADC1_REG                            (R_ADC1)
#define ADC_CHANNEL_CONFIG                  (g_adc0_channel_cfg)
#define ADC_OPEN(ctrl, cfg)                 (R_ADC_Open((ctrl), (cfg)))
#define ADC_SCANCFG(ctrl, ch_cfg)           (R_ADC_ScanCfg((ctrl), (ch_cfg)))
#define ADC_SCANSTART(ctrl)                 (R_ADC_ScanStart((ctrl)))
#define ADC_CLOSE(ctrl)                     (R_ADC_Close((ctrl)))
#define ADC_CALIBRATE(ctrl)                 (R_ADC_Calibrate((ctrl), NULL))
#define ADC_STATUSGET(ctrl, p_status)       (R_ADC_StatusGet((ctrl), (p_status)))
#endif /* ADC version */

typedef enum
{
    ADC_UNIT_0 = 0,
    ADC_UNIT_1,
    ADC_UNIT_NUM
} adc_unit_num_t;

typedef enum
{
    ADC_GROUP_SCAN_A = 0,
    ADC_GROUP_SCAN_B,
    ADC_GROUP_SCAN_NUM
} adc_scan_group_t;

/* Extern variables - defined in adc_ep.c */
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
extern uint16_t g_buffer_adc[ADC_UNIT_NUM][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS]
                            [NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];                            
#else
extern uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS]
                            [NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

extern transfer_info_t g_transfer_adc_group_a[];
extern transfer_info_t g_transfer_adc_group_b[];
extern volatile bool g_adc0_group_a_flag;
extern volatile bool g_adc0_group_b_flag;

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
extern volatile bool g_adc1_group_a_flag;
extern volatile bool g_adc1_group_b_flag;
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
extern volatile bool g_err_flag_adc_b;
#elif (BSP_PERIPHERAL_ADC_PRESENT)
extern volatile bool g_err_flag_adc0;
#if (BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK)
extern volatile bool g_err_flag_adc1;
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT || BSP_PERIPHERAL_ADC_PRESENT */

/* Function declarations */
fsp_err_t adc_init(void);
fsp_err_t adc_channel_config(void);
fsp_err_t adc_scan_start(void);
void adc_deinit(void);

#endif /* ADC_EP_H_ */
