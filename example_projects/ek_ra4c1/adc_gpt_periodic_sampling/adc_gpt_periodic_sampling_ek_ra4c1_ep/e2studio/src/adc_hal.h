/***********************************************************************************************************************
 * File Name    : adc_hal.h
 * Description  : Contains data structures and functions used in adc_hal.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_HAL_H_
#define ADC_HAL_H_

#define NUM_SAMPLES_PER_CHANNEL (256)
#define NUM_ADC_CHANNELS        (2)
#define NUM_SAMPLE_BUFFER       (2)
#define ZERO                    (0U)   /* ADC channel 0 */
#define ONE                     (1U)   /* ADC channel 1 */
#define TWO                     (2U)   /* ADC channel 2 */
#define THREE                   (3U)   /* ADC channel 3 */
#define FOUR                    (4U)   /* ADC channel 4 */
#define FIVE                    (5U)   /* ADC channel 5 */
#define SIX                     (6U)   /* ADC channel 6 */
#define SEVEN                   (7U)   /* ADC channel 7 */
#define EIGHT                   (8U)   /* ADC channel 8 */
#define NINE                    (9U)   /* ADC channel 9 */
#define TEN                     (10U)  /* ADC channel 10 */
#define SIXTEEN                (16U)  /* ADC channel 16 */
#define SEVENTEEN              (17U)  /* ADC channel 17 */
#define EIGHTEEN               (18U)  /* ADC channel 18 */
#define NINETEEN                (19U)  /* ADC channel 19 */
#define TWENTY                  (20U)  /* ADC channel 20 */
#define TWENTY_ONE              (21U)  /* ADC channel 21 */
#define TWENTY_TWO              (22U)  /* ADC channel 22 */
#define TWENTY_THREE            (23U)  /* ADC channel 23 */

typedef enum
{
    ADC_UNIT_0 = 0,
    ADC_UNIT_1,
    ADC_UNIT_NUM
}adc_unit_num_t;

typedef enum
{
    ADC_GROUP_SCAN_A = 0,
    ADC_GROUP_SCAN_B,
    ADC_GROUP_SCAN_NUM
}adc_scan_group_t;

/* Functions declarations */
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc);
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc);
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc);
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc);
#ifdef BOARD_RA2A1_EK
fsp_err_t adc_start_calibration(void);
#endif

#endif /* ADC_HAL_H_ */
