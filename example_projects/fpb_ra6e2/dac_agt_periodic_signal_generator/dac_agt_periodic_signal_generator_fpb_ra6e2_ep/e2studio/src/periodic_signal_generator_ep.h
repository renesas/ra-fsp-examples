/***********************************************************************************************************************
 * File Name    : periodic_signal_generator_ep.h
 * Description  : Contains declarations of data structures and functions used in periodic_signal_generator_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PERIODIC_SIGNAL_GENERATOR_EP_H_
#define PERIODIC_SIGNAL_GENERATOR_EP_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the input buffer size */
#define BUFFER_SIZE                 (16U)

/* Minimum number of samples per period */
#define SPP_MIN_VAL                 (2U)

/* Maximum number of samples per period */
#define SPP_MAX_VAL                 (256U)

/* Mid-scale DAC value for centering the sine wave */
#define DAC_MID_VAL                 (2047.5)

#ifndef M_PI
#define M_PI                        (3.14159265358979323846)
#endif /* M_PI */

/* Board-specific DAC address register and maximum conversion period (in microseconds) */
#if defined (BOARD_RA2L1_EK)
#define DADR0                       (0x4005E000)
#define DAC_MAX_CONVERSION_PERIOD   (30U)
#elif defined (BOARD_RA2A1_EK)
#define DADR0                       (0x4005E000)
#define DAC_MAX_CONVERSION_PERIOD   (1U)
#elif defined (BOARD_RA4E2_EK) || defined (BOARD_RA4M2_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6M4_EK)\
      || defined (BOARD_RA6E2_EK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA4L1_EK) || defined (BOARD_RA6E2_FPB)
#define DADR0                       (0x40171000)
#define DAC_MAX_CONVERSION_PERIOD   (4U)
#elif defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define DADR0                       (0x40333000)
#define DAC_MAX_CONVERSION_PERIOD   (3U)
#elif defined (BOARD_RA8T1_MCK)
#define DADR1                       (0x40333002)
#define DAC_MAX_CONVERSION_PERIOD   (3U)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) || defined (BOARD_RA8T2_MCK)
#define DADR0                       (0x40233000)
#define DAC_MAX_CONVERSION_PERIOD   (3.5)
#endif /* Specific board */

/* Macros for DAC based on peripheral variant */
#if (BSP_PERIPHERAL_DAC_B_PRESENT)
    #define DAC_OPEN(ctrl, cfg)     (R_DAC_B_Open((ctrl), (cfg)))
    #define DAC_START(ctrl)         (R_DAC_B_Start((ctrl)))
    #define DAC_CLOSE(ctrl)         (R_DAC_B_Close((ctrl)))
#elif (BSP_PERIPHERAL_DAC_PRESENT)
    #define DAC_OPEN(ctrl, cfg)     (R_DAC_Open((ctrl), (cfg)))
    #define DAC_START(ctrl)         (R_DAC_Start((ctrl)))
    #define DAC_CLOSE(ctrl)         (R_DAC_Close((ctrl)))
#endif /* DAC version */

/* Function declarations */
fsp_err_t dac_init(void);
fsp_err_t dtc_init(void);
fsp_err_t agt_init(void);
fsp_err_t dac_output_set(uint16_t input_spp, uint32_t input_freq);
void dac_freq_limits_get(uint16_t input_spp, uint32_t *p_min_hz, uint32_t *p_max_hz);
void dac_deinit(void);
void agt_deinit(void);
void dtc_deinit(void);

#endif /* PERIODIC_SIGNAL_GENERATOR_EP_H_ */
