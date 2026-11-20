/***********************************************************************************************************************
 * File Name    : sdram_ep.h
 * Description  : Contains macros, variables, and functions used in sdram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SDRAM_EP_H_
#define SDRAM_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macros for text color */
#define CTRL_RESET                  "\x1B[0m"
#define CTRL_TEXT_BRIGHT_GREEN      "\x1B[1;32m"

/* Macros for LED */
#define LED_LPM_STATE               (0U)
#define LED_ERROR_STATE             (2U)
#define LED_LPM_ON_TIME             (1U)

/* Macros for SDRAM transfer size */
#define BLOCK_DATA_SIZE             (4096U)
#define DATA_SIZE                   (4U)
#define NUM_ADDR_FOR_BLOCK          (BLOCK_DATA_SIZE * DATA_SIZE)

/* Macros for SDRAM memory */
#define SDRAM_START_ADDR            (0x68000000UL)
#define SDRAM_SIZE                  (0x04000000UL)

/* Enumeration for LED state use in application */
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
} led_power_t;

/* Enumeration for SDRAM transmission direction */
typedef enum e_sdram_trans_dir
{
    SDRAM_TRANS_DIR_WRITE = 0,
    SDRAM_TRANS_DIR_READ,
} sdram_trans_dir_t;

/* External variable - Used for LED */
extern bsp_leds_t g_bsp_leds;

/* External variable - Defined in sdram_ep.c */
extern volatile bool g_sdram_transmission_complete;

/* Function declarations */
fsp_err_t lpm_enter(void);
void lpm_deinit(void);
fsp_err_t timer_init(void);
fsp_err_t timer_start(void);
fsp_err_t timer_stop(void);
void timer_deinit(void);
fsp_err_t dmac_init(void);
fsp_err_t dmac_reset(transfer_ctrl_t * const p_api_ctrl, void const * volatile p_src, void * volatile p_dest,
                     uint16_t const num_transfers);
fsp_err_t dmac_software_start(void);
void dmac_deinit(void);
void led_error_state_set(led_power_t state);
void led_lpm_state_set(led_power_t state);

#endif /* SDRAM_EP_H_ */
