/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macros for user buffer */
#define NUM_SAMPLES_PER_CHANNEL     (256U)
#define NUM_ADC_CHANNELS            (2U)
#define NUM_SAMPLE_BUFFER           (2U)

typedef enum
{
    ADC_UNIT_0 = 0,
    ADC_UNIT_1,
    ADC_UNIT_NUM
} adc_unit_num_t;

typedef enum
{
    ADC_GROUP_SCAN = 0,
    ADC_GROUP_SCAN_NUM
} adc_scan_group_t;

/* External variables - defined in adc_ep.c */
extern uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS]
                            [NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
extern volatile bool g_adc0_group_flag;
extern volatile bool g_err_flag_adc0;
extern transfer_info_t g_transfer_adc_group[];

/* Function declarations */
fsp_err_t adc_init(void);
fsp_err_t adc_channel_config(void);
fsp_err_t adc_scan_start(void);
void adc_deinit(void);

#endif /* ADC_EP_H_ */
