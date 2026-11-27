/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains class, data structures and functions used in adc_ep.cpp.
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

/* Macros for commands to be received through terminal input */
#define SCAN_START                          ('1')
#define SCAN_STOP                           ('2')

/* Macro for checking the deviation in ADC values */
#define TEMPERATURE_DEVIATION_LIMIT         (0x04)

/* Macro to provide delay in read ADC data */
#define ADC_READ_DELAY                      (0x01)

/* Macro to set value to output voltage control */
#define VREFADCG_VALUE                      (0x03)

/* Macro to set value to enable VREFADC output */
#define VREFADCG_ENABLE                     (0x03)

/* Macro to shift the VREFADC enable bit into position */
#define SHIFT_BY_ONE                        (0x01)

/* Macro to shift the VREFADC value bits into position */
#define SHIFT_BY_THREE                      (0x03)

/* Macro for reset value */
#define RESET_VALUE                         (0x00)

/* Macros for ADC based on the specific version */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    extern const adc_d_extended_cfg_t g_adc_cfg_extend;
    #define MODE_1                          "One-shot mode"
    #define MODE_2                          "Sequential mode"
    #define MODULE_NAME                     "r_adc_d"
    #define MODULE_PADDING                  "          "
    #define ADC_CHANNEL_CONFIG              (g_adc_channel_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_D_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_D_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_D_ScanStart((ctrl)))
    #define ADC_SCANSTOP(ctrl)              (R_ADC_D_ScanStop((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_D_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_D_Close((ctrl)))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_D_StatusGet((ctrl), (p_status)))
    #define ADC_IS_SINGLE_SCAN()            (ADC_D_CONVERSION_MODE_ONESHOT == g_adc_cfg_extend.conversion_operation)

#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    extern const adc_b_extended_cfg_t g_adc_cfg_extend;
    #define MODE_1                          "Single Scan mode"
    #define MODE_2                          "Continuous Scan mode"
    #define MODULE_NAME                     "r_adc_b"
    #define MODULE_PADDING                  "          "
    #define ADC_CHANNEL_CONFIG              (g_adc_scan_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_B_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_B_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_B_ScanStart((ctrl)))
    #define ADC_SCANSTOP(ctrl)              (R_ADC_B_ScanStop((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_B_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_B_Close((ctrl)))
    #define ADC_CALIBRATE(ctrl)             (R_ADC_B_Calibrate((ctrl), NULL))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_B_StatusGet((ctrl), (p_status)))
    #define ADC_IS_SINGLE_SCAN()            (ADC_MODE_SINGLE_SCAN == g_adc_cfg_extend.adc_b_mode)

#else
    #define MODE_1                          "Single Scan mode"
    #define MODE_2                          "Continuous Scan mode"
    #define MODULE_NAME                     "r_adc"
    #define MODULE_PADDING                  "           "
    #define ADC_CHANNEL_CONFIG              (g_adc_channel_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_ScanStart((ctrl)))
    #define ADC_SCANSTOP(ctrl)              (R_ADC_ScanStop((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_Close((ctrl)))
    #define ADC_CALIBRATE(ctrl)             (R_ADC_Calibrate((ctrl), NULL))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_StatusGet((ctrl), (p_status)))
    #define ADC_IS_SINGLE_SCAN()            (ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode)
#endif /* ADC version */

/* Extern variable - defined in adc_ep.cpp */
extern volatile bool g_ready_to_read;

class adc_ep
{
public:
    fsp_err_t init_adc_module(void);
    static void deinit_adc_module(void);
    fsp_err_t adc_scan_start(void);
    fsp_err_t adc_scan_stop(void);
    fsp_err_t adc_read_data(uint16_t *p_data);
#ifdef BOARD_RA2A1_EK
    bool recalibration_was_triggered(void);
#endif /* BOARD_RA2A1_EK */

private:
    uint16_t g_adc_data = RESET_VALUE;
#ifdef BOARD_RA2A1_EK
    uint16_t g_prev_adc_data = RESET_VALUE;
    bool g_recalibration_triggered = false;
    fsp_err_t adc_deviation_in_output(void);
#endif /* BOARD_RA2A1_EK */
#if (defined BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
    fsp_err_t adc_start_calibration(void);
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */
};

#endif /* ADC_EP_H_ */
