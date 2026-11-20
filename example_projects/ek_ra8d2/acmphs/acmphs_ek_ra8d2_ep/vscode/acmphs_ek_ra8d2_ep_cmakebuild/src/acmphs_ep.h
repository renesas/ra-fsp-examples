/***********************************************************************************************************************
 * File Name    : acmphs_ep.h
 * Description  : Contains macros, data structures, and functions used in acmphs_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ACMPHS_EP_H_
#define ACMPHS_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macro for the null char */
#define NULL_CHAR                           ('\0')

/* Macro for the buffer size */
#define BUFFER_SIZE                         (16U)

#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
#define DAC_MAX_VAL                         (4095U)     /* Max count value for 12 bit DAC */
#if (defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK))
#define LOW_VAL                             (1400U)     /* Lower value of fluctuating range */
#define HIGH_VAL                            (1600U)     /* Higher value of fluctuating range */
#define V_REF                               (3.3f)      /* Reference voltage */
#else
#define DAC_REF_VAL                         (2048U)     /* Mid count value for 12 bit DAC */
#define LOW_VAL                             (2000U)     /* Lower value of fluctuating range */
#define HIGH_VAL                            (2100U)     /* Higher value of fluctuating range */
#endif /* Specific board */
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

/* Macro for on-board LED */
#if defined (BOARD_RA6M2_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK)
#define LED_PIN                             (BSP_IO_PORT_01_PIN_06)
#elif defined (BOARD_RA6T2_MCK)
#define LED_PIN                             (BSP_IO_PORT_13_PIN_01)
#elif defined (BOARD_RA6M1_EK)
#define LED_PIN                             (BSP_IO_PORT_01_PIN_12)
#elif defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA8E1_FPB)
#define LED_PIN                             (BSP_IO_PORT_04_PIN_08)
#elif defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define LED_PIN                             (BSP_IO_PORT_01_PIN_00)
#elif defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define LED_PIN                             (BSP_IO_PORT_01_PIN_07)
#elif defined (BOARD_RA8T1_MCK)
#define LED_PIN                             (BSP_IO_PORT_10_PIN_12)
#elif defined (BOARD_RA8E2_EK)
#define LED_PIN                             (BSP_IO_PORT_04_PIN_05)
#elif defined (BOARD_RA2T1_FPB)
#define LED_PIN                             (BSP_IO_PORT_02_PIN_13)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
#define LED_PIN                             (BSP_IO_PORT_06_PIN_00)
#elif defined (BOARD_RA8T2_MCK)
#define LED_PIN                             (BSP_IO_PORT_06_PIN_14)
#endif /* Specific board */

/* Macros for DAC based on the specific DAC version */
#if (BSP_PERIPHERAL_DAC_B_PRESENT)
#define DAC0_CTRL                           (&g_dac_b0_ctrl)
#define DAC0_CFG                            (&g_dac_b0_cfg)
#define DAC1_CTRL                           (&g_dac_b1_ctrl)
#define DAC1_CFG                            (&g_dac_b1_cfg)
#define DAC_OPEN(ctrl, cfg)                 (R_DAC_B_Open((ctrl), (cfg)))
#define DAC_WRITE(ctrl, val)                (R_DAC_B_Write((ctrl), (val)))
#define DAC_START(ctrl)                     (R_DAC_B_Start((ctrl)))
#define DAC_CLOSE(ctrl)                     (R_DAC_B_Close((ctrl)))
#elif (BSP_PERIPHERAL_DAC_PRESENT)
#define DAC0_CTRL                           (&g_dac0_ctrl)
#define DAC0_CFG                            (&g_dac0_cfg)
#define DAC1_CTRL                           (&g_dac1_ctrl)
#define DAC1_CFG                            (&g_dac1_cfg)
#define DAC_OPEN(ctrl, cfg)                 (R_DAC_Open((ctrl), (cfg)))
#define DAC_WRITE(ctrl, val)                (R_DAC_Write((ctrl), (val)))
#define DAC_START(ctrl)                     (R_DAC_Start((ctrl)))
#define DAC_CLOSE(ctrl)                     (R_DAC_Close((ctrl)))
#endif /* DAC version */

/* Macros for ACMPHS based on the specific ACMPHS version */
#if (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
#define MODULE_NAME                         "r_acmphs_b"
#define MODULE_PADDING                      "          "
#define ACMPHS_OPEN(ctrl, cfg)              (R_ACMPHS_B_Open((ctrl), (cfg)))
#define ACMPHS_INFOGET(ctrl, p_info)        (R_ACMPHS_B_InfoGet((ctrl), (p_info)))
#define ACMPHS_OUTPUTENABLE(ctrl)           (R_ACMPHS_B_OutputEnable((ctrl)))
#define ACMPHS_STATUSGET(ctrl, p_status)    (R_ACMPHS_B_StatusGet((ctrl), (p_status)))
#define ACMPHS_CLOSE(ctrl)                  (R_ACMPHS_B_Close((ctrl)))
#elif (BSP_PERIPHERAL_ACMPHS_PRESENT)
#define MODULE_NAME                         "r_acmphs"
#define MODULE_PADDING                      "            "
#define ACMPHS_OPEN(ctrl, cfg)              (R_ACMPHS_Open((ctrl), (cfg)))
#define ACMPHS_INFOGET(ctrl, p_info)        (R_ACMPHS_InfoGet((ctrl), (p_info)))
#define ACMPHS_OUTPUTENABLE(ctrl)           (R_ACMPHS_OutputEnable((ctrl)))
#define ACMPHS_STATUSGET(ctrl, p_status)    (R_ACMPHS_StatusGet((ctrl), (p_status)))
#define ACMPHS_CLOSE(ctrl)                  (R_ACMPHS_Close((ctrl)))
#endif /* ACMPHS version */

/* Function declarations */
#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
fsp_err_t acmphs_ep_dac0_init(uint16_t initial_val);
fsp_err_t acmphs_ep_dac0_write(uint16_t dac_val);
#if (!(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)))
fsp_err_t acmphs_ep_dac1_init(uint16_t initial_val);
fsp_err_t acmphs_ep_dac1_write(uint16_t dac_val);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */
fsp_err_t acmphs_ep_comparator_init(comparator_info_t * p_stabilize_time);
fsp_err_t acmphs_ep_status_refresh(comparator_status_t * p_status);
fsp_err_t acmphs_ep_led_update(comparator_state_t state);
void acmphs_ep_close(void);

#endif /* ACMPHS_EP_H_ */
