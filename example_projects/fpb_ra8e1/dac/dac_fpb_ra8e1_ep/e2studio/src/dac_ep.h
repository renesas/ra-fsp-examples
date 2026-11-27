/***********************************************************************************************************************
 * File Name    : dac_ep.h
 * Description  : Contains declarations of data structures and functions used in dac_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DAC_EP_H_
#define DAC_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macros for DAC input value range */
#define DAC_MIN_VAL                             (1)
#define DAC_MAX_VAL                             (4095)

/* Macro for DAC output stabilization delay in milliseconds */
#define STABILIZATION_DELAY                     (1)

/* Macro for for the user input buffer size */
#define BUFFER_SIZE                             (16U)

/* Macro for the reset value */
#define RESET_VALUE                             (0x00)

/* Macro for ADC module based on specific version */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
#define R_ADC_OPEN(ctrl, cfg)                   (R_ADC_B_Open((ctrl), (cfg)))
#define R_ADC_SCAN_CFG(ctrl, scan_cfg)          (R_ADC_B_ScanCfg((ctrl), (scan_cfg)))
#define R_ADC_CALIBRATE(ctrl, NULL)             (R_ADC_B_Calibrate((ctrl), (NULL)))
#define R_ADC_SCAN_GROUP_START(ctrl, group)     (R_ADC_B_ScanGroupStart((ctrl), (group)))
#define R_ADC_READ(ctrl, channel, data)         (R_ADC_B_Read((ctrl), (channel), (data)))
#define R_ADC_CLOSE(ctrl)                       (R_ADC_B_Close(ctrl))
#else
#define R_ADC_OPEN(ctrl, cfg)                   (R_ADC_Open((ctrl), (cfg)))
#define R_ADC_SCAN_CFG(ctrl, channel_cfg)       (R_ADC_ScanCfg((ctrl), (channel_cfg)))
#define R_ADC_CALIBRATE(ctrl, NULL)             (R_ADC_Calibrate((ctrl), (NULL)))
#define R_ADC_SCAN_START(ctrl)                  (R_ADC_ScanStart(ctrl))
#define R_ADC_READ(ctrl, channel, data)         (R_ADC_Read((ctrl), (channel), (data)))
#define R_ADC_CLOSE(ctrl)                       (R_ADC_Close(ctrl))
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

/* Macro for DAC module based on specific version */
#if (BSP_PERIPHERAL_DAC_B_PRESENT)
#define MODULE_NAME                             "r_dac_b"
#define MODULE_PADDING                          "            "
#define R_DAC_OPEN(ctrl, cfg)                   (R_DAC_B_Open((ctrl), (cfg)))
#define R_DAC_WRITE(ctrl, val)                  (R_DAC_B_Write((ctrl), (val)))
#define R_DAC_START(ctrl)                       (R_DAC_B_Start(ctrl))
#define R_DAC_STOP(ctrl)                        (R_DAC_B_Stop(ctrl))
#define R_DAC_CLOSE(ctrl)                       (R_DAC_B_Close(ctrl))
#else
#define MODULE_NAME                             "r_dac"
#define MODULE_PADDING                          "              "
#define R_DAC_OPEN(ctrl, cfg)                   (R_DAC_Open((ctrl), (cfg)))
#define R_DAC_WRITE(ctrl, val)                  (R_DAC_Write((ctrl), (val)))
#define R_DAC_START(ctrl)                       (R_DAC_Start(ctrl))
#define R_DAC_STOP(ctrl)                        (R_DAC_Stop(ctrl))
#define R_DAC_CLOSE(ctrl)                       (R_DAC_Close(ctrl))
#endif /* BSP_PERIPHERAL_DAC_B_PRESENT */

/* Macro ADC channel selection for specific board */
#if defined(BOARD_RA4W1_EK)
#define ADC_CHANNEL                             (ADC_CHANNEL_4)
#elif defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8T2_MCK)
#define ADC_CHANNEL                             (ADC_CHANNEL_2)
#elif defined(BOARD_RA2A1_EK) || defined(BOARD_RA4L1_EK) || defined(BOARD_RA8P1_EK) ||\
      defined(BOARD_RA8D2_EK) || defined(BOARD_RA8M2_EK)
#define ADC_CHANNEL                             (ADC_CHANNEL_1)
#else
#define ADC_CHANNEL                             (ADC_CHANNEL_0)
#endif

/* Board-specific macros for EK-RA2A1 and MCK-RA6T2 */
#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
#define VREFADCG_VALUE                          (0x06)
#define VREFADCG_ENABLE                         (0x18)
#endif /* BOARD_RA2A1_EK || BOARD_RA6T2_MCK */

/* Macro for DAC output pin for EK-RA2A1 */
#if defined(BOARD_RA2A1_EK)
#define DAC12_OUTPUT_PIN                        (BSP_IO_PORT_05_PIN_00)
#endif /* BOARD_RA2A1_EK */

/* Extern variables */
extern volatile bool g_is_read_complete;
extern volatile bool g_error_flag;
extern uint16_t g_adc_data;

/* Function declarations */
fsp_err_t dac_open(void);
void dac_deinit(void);
fsp_err_t adc_open(void);
fsp_err_t adc_scan_cfg(void);
fsp_err_t adc_read(adc_ctrl_t * p_ctrl, adc_channel_t const reg_id, uint16_t * const p_data);
void adc_deinit(void);
#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
fsp_err_t adc_calibrate(void);
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */
fsp_err_t dac_adc_operations(int32_t *input);

#endif /* DAC_EP_H_ */
