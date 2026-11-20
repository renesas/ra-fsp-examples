/***********************************************************************************************************************
 * File Name    : switch_init.h
 * Description  : Contains macros and functions used in switch_init.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SWITCH_INIT_H_
#define SWITCH_INIT_H_

#include "hal_data.h"

/* Macros for GPIO output states */
#define LOW_STATE                   (0U)
#define HIGH_STATE                  (1U)

/* Macros for switch control registers */
#define SWITCH_ADDR                 (0x43)
#define SOFT_RESET_REG              (0x01)
#define PIN_DIR_REG                 (0x03)
#define OUTPUT_STATE_REG            (0x05)
#define OUTPUT_ENABLE_REG           (0x07)

/* Macros for switch output configuration values */
#define OUTPUT_ENABLE               (0U)
#define OUTPUT_DIR                  (1U)

/* Function declaration */
fsp_err_t set_switch_state(uint8_t sw_num, uint8_t target_state);

#endif /* SWITCH_INIT_H_ */
