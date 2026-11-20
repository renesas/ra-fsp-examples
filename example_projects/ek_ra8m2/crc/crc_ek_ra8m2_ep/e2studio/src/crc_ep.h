/***********************************************************************************************************************
 * File Name    : crc_ep.h
 * Description  : Contains macro definitions and function declarations used in crc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CRC_EP_H_
#define CRC_EP_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro length of input buffer to calculate CRC in normal mode */
#define NUM_BYTES               (4U)

/* Macro for size of input buffer */
#define BUF_LEN                 (8U)

/* Macros for 8 and 16 bit seed value and data length */
#define SEED_VALUE              (0x00000000)
#define EIGHT_BIT_DATA_LEN      (5U)
#define SIXTEEN_BIT_DATA_LEN    (6U)

/* Macros for LED status */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
	|| defined (BOARD_RA4T1_MCK) ||defined (BOARD_RA8T1_MCK) ||defined (BOARD_RA8T2_MCK)
#define LED_ON                  (BSP_IO_LEVEL_LOW)
#define LED_OFF                 (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                 (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Macro for LED toggle delay */
#define TOGGLE_DELAY            (0x15E)

/* Macro for time out */
#define TIME_OUT                (10000000U)

/* Function declarations */
fsp_err_t crc_init(void);
fsp_err_t crc_calculate(crc_input_t * const p_crc_input, uint32_t * crc_value);
#if (BSP_FEATURE_CRC_HAS_SNOOP)
fsp_err_t crc_snoop_enable(void);
fsp_err_t crc_calculated_value(uint32_t * crc_value);
fsp_err_t crc_snoop_disable(void);
#endif /* BSP_FEATURE_CRC_HAS_SNOOP */
void crc_deinit(void);

#endif /* CRC_EP_H_ */
