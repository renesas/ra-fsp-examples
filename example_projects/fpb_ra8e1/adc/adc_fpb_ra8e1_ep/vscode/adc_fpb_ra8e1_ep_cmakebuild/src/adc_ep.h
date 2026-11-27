/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains data structures and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

#include "hal_data.h"

/* Macros for user commands */
#define SCAN_START                          ('1')
#define SCAN_STOP                           ('2')
#define BUFF_SIZE                           (0x0F)
#define ADC_READ_DELAY                      (0x01)

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macros for board-specific ADC configuration (RA2A1 only) */
#ifdef BOARD_RA2A1_EK
#define TEMPERATURE_DEVIATION_LIMIT         (0x04)
#define VREFADCG_VALUE                      (0x03)
#define VREFADCG_ENABLE                     (0x03)
#define SHIFT_BY_ONE                        (0x01)
#define SHIFT_BY_THREE                      (0x03)
#endif /* BOARD_RA2A1_EK */

/* Macro for ADC channel */
#if defined(BOARD_RA4W1_EK)
#define ADC_CHANNEL                         (ADC_CHANNEL_4)
#elif defined(BOARD_RA2E2_EK)
#define ADC_CHANNEL                         (ADC_CHANNEL_9)
#elif defined(BOARD_RA6T1_RSSK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA4L1_EK)\
   || defined(BOARD_RA4C1_EK)   || defined(BOARD_RA0L1_FPB) || defined(BOARD_RA8T2_MCK)\
   || defined(BOARD_RA8M2_EK)
#define ADC_CHANNEL                         (ADC_CHANNEL_2)
#elif defined(BOARD_RA6T2_MCK)
#define ADC_CHANNEL                         (ADC_CHANNEL_TEMPERATURE)
#elif defined(BOARD_RA2A1_EK) || defined(BOARD_RA8P1_EK) || defined(BOARD_RA0E2_FPB)
#define ADC_CHANNEL                         (ADC_CHANNEL_1)
#else
#define ADC_CHANNEL                         (ADC_CHANNEL_0)
#endif /* ADC channel */

/* Macros for ADC based on specific version */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    extern const adc_d_extended_cfg_t g_adc_cfg_extend;
    #define MODE_1                          "One-shot mode"
    #define MODE_2                          "Sequential mode"
    #define MODULE_NAME                     "r_adc_d"
    #define MODULE_PADDING                  ""
    #define ADC_CHANNEL_CONFIG              (g_adc_channel_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_D_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_D_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_D_ScanStart((ctrl)))
    #define ADC_SCANSTOP(ctrl)              (R_ADC_D_ScanStop((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_D_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_D_Close((ctrl)))
    #define ADC_CALIBRATE(ctrl)             (FSP_SUCCESS)
    #define ADC_STATUSGET(ctrl, p_status)   (FSP_SUCCESS)
    #define ADC_IS_SINGLE_SCAN()            (ADC_D_CONVERSION_MODE_ONESHOT == g_adc_cfg_extend.conversion_operation)

#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    extern const adc_b_extended_cfg_t g_adc_b_cfg_extend;
    #define MODE_1                          "Single Scan mode"
    #define MODE_2                          "Continuous Scan mode"
    #define MODULE_NAME                     "r_adc_b"
    #define MODULE_PADDING                  ""
    #define ADC_CHANNEL_CONFIG              (g_adc_b_scan_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_B_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_B_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_B_ScanStart((ctrl)))
    #define ADC_SCANSTOP(ctrl)              (R_ADC_B_ScanStop((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_B_Read((ctrl), (ch), (p_data)))
    #define ADC_CLOSE(ctrl)                 (R_ADC_B_Close((ctrl)))
    #define ADC_CALIBRATE(ctrl)             (R_ADC_B_Calibrate((ctrl), NULL))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_B_StatusGet((ctrl), (p_status)))
    #define ADC_IS_SINGLE_SCAN()            (ADC_MODE_SINGLE_SCAN == g_adc_b_cfg_extend.adc_b_mode)

#else
    #define MODE_1                          "Single Scan mode"
    #define MODE_2                          "Continuous Scan mode"
    #define MODULE_NAME                     "r_adc"
    #define MODULE_PADDING                  "  "
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

/* Macros for voltage conversion */
#ifndef BOARD_RA6T2_MCK
#define V_REF                               (3.3F)
    #ifdef BOARD_RA2A1_EK
        #define ADC_16_BIT                  (32768U)
        #define ADC_H_LMT                   (24824U)
        #define ADC_L_LMT                   (9929U)
        #define ADC_COUNTS_FULL_SCALE       (ADC_16_BIT)   /* EK-RA2A1: 16-bit */
    #else
        #define ADC_12_BIT                  (4096U)
        #define ADC_H_LMT                   (3103U)
        #define ADC_L_LMT                   (1241U)
        #define ADC_COUNTS_FULL_SCALE       (ADC_12_BIT)   /* Default: 12-bit */
    #endif /* BOARD_RA2A1_EK */
#endif /* BOARD_RA6T2_MCK */

/* Extern variables */
extern volatile bool g_ready_to_read;
extern uint16_t g_adc_data;
extern float g_adc_volt;
extern uint8_t g_volt_str[5];

#if (BSP_PERIPHERAL_ADC_PRESENT)
extern bool g_window_comp_event;
#endif /* BSP_PERIPHERAL_ADC_PRESENT */

/* Function declarations */
fsp_err_t adc_scan_start(void);
fsp_err_t adc_scan_stop(void);
fsp_err_t adc_read_data(void);
void deinit_adc_module(void);

#endif /* ADC_EP_H_ */
