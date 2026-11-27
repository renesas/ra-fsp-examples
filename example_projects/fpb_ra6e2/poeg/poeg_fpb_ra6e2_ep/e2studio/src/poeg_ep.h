/***********************************************************************************************************************
 * File Name    : poeg_ep.h
 * Description  : Contains data structures and functions used in poeg_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef POEG_EP_H_
#define POEG_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                                     (0x00)

/* Macro for the buffer size */
#define BUFFER_SIZE                                     (16U)

/* Macro for the null char */
#define NULL_CHAR                                       ('\0')

/* Macros for input command to be received through the terminal */
#define POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL          (1U)
#define POEG_OUTPUT_DISABLE_USING_SOFTWARE_API          (2U)
#define POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL       (3U)
#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
#define POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS            (4U)
#define POEG_RESET                                      (5U)
#else
#define POEG_RESET                                      (4U)
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    /* Macros for ACMPHS */
    #if (BSP_PERIPHERAL_DAC_PRESENT) || (BSP_PERIPHERAL_DAC_B_PRESENT)
    #define DAC_MAX_VAL                                 (4095U)        /* Max count value for 12 bit DAC */
        #if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
        #define LOW_VAL                                 (1400U)        /* Lower value of fluctuating range */
        #define HIGH_VAL                                (1600U)        /* Higher value of fluctuating range */
        #define V_REF                                   (3.3f)         /* Reference Voltage */
        #define ADC_REF_VAL                             "1.18V"
        #else
        #define DAC_REF_VAL                             (2048U)        /* Reference value for 12 bit DAC */
        #define ADC_REF_VAL                             "1.65V"
        #define LOW_VAL                                 (2000U)        /* Lower value of fluctuating range */
        #define HIGH_VAL                                (2100U)        /* Higher value of fluctuating range */
        #endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
    #endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

    /* Macros for used functions based on the specific ACMPHS version */
    #if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
    #define ACMPHS_OPEN(ctrl, cfg)                      (R_ACMPHS_B_Open((ctrl), (cfg)))
    #define ACMPHS_INFOGET(ctrl, info)                  (R_ACMPHS_B_InfoGet((ctrl), (info)))
    #define ACMPHS_OUTPUT_ENABLE(ctrl)                  (R_ACMPHS_B_OutputEnable(ctrl))
    #define ACMPHS_CLOSE(ctrl)                          (R_ACMPHS_B_Close(ctrl))
    #elif (BSP_PERIPHERAL_ACMPHS_PRESENT)
    #define ACMPHS_OPEN(ctrl, cfg)                      (R_ACMPHS_Open((ctrl), (cfg)))
    #define ACMPHS_INFOGET(ctrl, info)                  (R_ACMPHS_InfoGet((ctrl), (info)))
    #define ACMPHS_OUTPUT_ENABLE(ctrl)                  (R_ACMPHS_OutputEnable(ctrl))
    #define ACMPHS_CLOSE(ctrl)                          (R_ACMPHS_Close(ctrl))
    #endif /* ACMPHS version */

    /* Macros for used functions based on the specific DAC version */
    #if (BSP_PERIPHERAL_DAC_B_PRESENT)
    typedef dac_b_instance_ctrl_t dac_type_ctrl_t;
    #define DAC_OPEN(ctrl, cfg)                         (R_DAC_B_Open((ctrl), (cfg)))
    #define DAC_WRITE(ctrl, val)                        (R_DAC_B_Write((ctrl), (val)))
    #define DAC_START(ctrl)                             (R_DAC_B_Start(ctrl))
    #define DAC_CLOSE(ctrl)                             (R_DAC_B_Close(ctrl))
    #elif (BSP_PERIPHERAL_DAC_PRESENT)
    typedef dac_instance_ctrl_t dac_type_ctrl_t;
    #define DAC_OPEN(ctrl, cfg)                         (R_DAC_Open((ctrl), (cfg)))
    #define DAC_WRITE(ctrl, val)                        (R_DAC_Write((ctrl), (val)))
    #define DAC_START(ctrl)                             (R_DAC_Start(ctrl))
    #define DAC_CLOSE(ctrl)                             (R_DAC_Close(ctrl))
    #endif /* DAC version */
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/* Macros for POEG channel */
#define POEG_CHANNEL_0                                  (1U)
#define POEG_CHANNEL_1                                  (2U)

/* Macro for GTETRG pin */
#if defined(BOARD_RA6M1_EK) || defined(BOARD_RA6M2_EK)
#define GTETRG_PIN                                      "P104 (J3:39)"
#elif defined(BOARD_RA6M3_EK)
#define GTETRG_PIN                                      "P104 (J1:3)"
#elif defined(BOARD_RA6T1_RSSK)
#define GTETRG_PIN                                      "P104 (CNC:16)"
#elif defined(BOARD_RA4T1_MCK)
#define GTETRG_PIN                                      "P104 (CN3:12)"
#elif defined(BOARD_RA6T2_MCK)
#define GTETRG_PIN                                      "PA12 (CN6:12)"
#elif defined(BOARD_RA6T3_MCK)
#define GTETRG_PIN                                      "P104 (J4:12)"
#elif defined(BOARD_RA8D1_EK)
#define GTETRG_PIN                                      "P401 (J51:7)"
#elif defined(BOARD_RA8M1_EK)
#define GTETRG_PIN                                      "P614 (J54:27)"
#elif defined(BOARD_RA8T1_MCK)
#define GTETRG_PIN                                      "P104 (CN4:8)"
#elif defined(BOARD_RA8E1_FPB)
#define GTETRG_PIN                                      "P104 (J1:45)"
#elif defined(BOARD_RA4L1_EK)
#define GTETRG_PIN                                      "P104 (J3:9)"
#elif defined(BOARD_RA8E2_EK)
#define GTETRG_PIN                                      "P614 (J37:9)"
#elif defined(BOARD_RA2L2_EK)
#define GTETRG_PIN                                      "P104 (J2:35)"
#elif defined(BOARD_RA2T1_FPB)
#define GTETRG_PIN                                      "P104 (J4:8)"
#elif defined(BOARD_RA8P1_EK) || defined(BOARD_RA8D2_EK)
#define GTETRG_PIN                                      "P614 (J14:20)"
#elif defined(BOARD_RA4C1_EK)
#define GTETRG_PIN                                      "P104 (J2:20)"
#elif defined(BOARD_RA8T2_MCK)
#define GTETRG_PIN                                      "PA07 (CN2:17)"
#elif defined(BOARD_RA8M2_EK)
#define GTETRG_PIN                                      "P113 (J1:15)"
#elif defined(BOARD_RA6E2_FPB)
#define GTETRG_PIN                                      "P104 (J4:12)"
#endif /* Specific board */

/* Extern variable */
extern bsp_leds_t g_bsp_leds;

/* Macros for LED */
#define STATE_LED                                       (0U)    /* On-board LED1 */
#if defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T1_RSSK) || defined(BOARD_RA6T2_MCK) ||\
    defined(BOARD_RA6T3_MCK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8T2_MCK)
#define LED_ON                                          (BSP_IO_LEVEL_LOW)
#define LED_OFF                                         (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                                          (BSP_IO_LEVEL_HIGH)
#define LED_OFF                                         (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Extern variables - defined in poeg_ep.c */
extern volatile bool g_interrupt_pin_trigger_flag;
extern volatile bool g_interrupt_output_level_flag;
extern volatile bool g_interrupt_acmphs_flag;
extern volatile uint8_t g_mode_flag;

/* Function declarations */
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t init_poeg(poeg_ctrl_t * const p_ctrl, poeg_cfg_t const * const p_cfg, IRQn_Type IRQn);
fsp_err_t start_gpt(timer_ctrl_t * const p_ctrl);
void deinit_gpt_timer(timer_ctrl_t * const p_ctrl);
void deinit_poeg(poeg_ctrl_t * const p_ctrl);

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
fsp_err_t enable_acmphs_mode_modules(void);
void deinit_acmphs(void);
void deinit_dac(dac_type_ctrl_t * p_ctrl);
extern comparator_info_t g_stabilize_time;
#if defined (BOARD_RA6T2_MCK)
void deinit_adc(adc_ctrl_t * p_ctrl);
#endif /* BOARD_RA6T2_MCK */
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

#endif /* POEG_EP_H_ */
