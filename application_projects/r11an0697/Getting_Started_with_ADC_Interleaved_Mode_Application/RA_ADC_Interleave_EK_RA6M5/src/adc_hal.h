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

#define NUM_SAMPLES (2048)
#define ZERO        (0U)

/* functions declaration */
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc);
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc);
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc);
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc);

#endif /* ADC_HAL_H_ */
