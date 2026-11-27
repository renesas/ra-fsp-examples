/***********************************************************************************************************************
 * File Name    : lvd_ep.h
 * Description  : Contains macros, variables, and functions used in lvd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LVD_EP_H_
#define LVD_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for LVD0 detect reset value */
#define SYSTEM_RSTSR0_DETECT_LVD0_RESET         (1U)

/* Macro for the reset value */
#define RESET_VALUE                             (0x00)

/* Macro for the LED level that indicates an LVD0 reset was detected */
#if defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
#define LED_ON                                  (BSP_IO_LEVEL_LOW)
#else
#define LED_ON                                  (BSP_IO_LEVEL_HIGH)
#endif /* Specific board */

/* Extern variables */
extern bsp_leds_t g_bsp_leds;
extern volatile bool g_lvd1_status;

/* Function declarations */
bool lvd0_reset(void);
fsp_err_t lvd_init(void);
fsp_err_t lvd_status_clear(void);
void lvd_deinit(void);

#endif /* LVD_EP_H_ */
