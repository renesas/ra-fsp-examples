/***********************************************************************************************************************
 * File Name    : adc_hal.h
 * Description  : Contains data structures and functions used in adc_hal.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef ADC_HAL_H_
#define ADC_HAL_H_

#define NUM_SAMPLES_PER_CHANNEL (256)
#define NUM_ADC_CHANNELS        (2)
#define NUM_SAMPLE_BUFFER       (2)
#define ZERO                    (0U)
#define ONE                     (1U)
#define TWO                     (2U)
#define THREE                   (3U)
#define FOUR                    (4U)   //ADC channel 4
#define FIVE                    (5U)   //ADC channel 5
#define SIX                     (6U)   //ADC channel 6
#define SEVEN                   (7U)   //ADC channel 7
#define EIGHT                   (8U)   //ADC channel 8
#define NINE                    (9U)   //ADC channel 9
#define TEN                     (10U)  //ADC channel 10
#define SIXETEEN                (16U)  //ADC channel 16
#define SEVENETEEN              (17U)  //ADC channel 17
#define EIGHTTEEN               (18U)  //ADC channel 18
#define NINETEEN                (19U)  //ADC channel 19
#define TWENTY                  (20U)  //ADC channel 20
#define TWENTY_ONE              (21U)  //ADC channel 21
#define TWENTY_TWO              (22U)  //ADC Channel 22

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

/* functions declaration */
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc);
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc);
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc);
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc);
#ifdef BOARD_RA2A1_EK
fsp_err_t adc_start_calibration(void);
#endif

#endif /* ADC_HAL_H_ */
