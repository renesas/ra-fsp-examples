/***********************************************************************************************************************
 * File Name    : lpm_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in lpm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LPM_EP_H_
#define LPM_EP_H_

#include <stdbool.h>
#include "hal_data.h"
#include "console/uart_console.h"

/* Macro for the reset value */
#define RESET_VALUE                             (0x00)

/* Macros for Standby SRAM */
#if defined(BOARD_RA4E1_FPB) || defined(BOARD_RA4E2_EK) || defined(BOARD_RA4M2_EK) || defined(BOARD_RA4M3_EK) ||\
    defined(BOARD_RA6E1_FPB) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA6M4_EK) || defined(BOARD_RA6M5_EK) ||\
    defined(BOARD_RA6T2_MCK)
#define SSRAM_START                             (0x28000000)
#define SSRAM_LENGTH                            (0x400)
#elif defined(BOARD_RA6M1_EK) || defined(BOARD_RA6M2_EK) || defined(BOARD_RA6M3_EK) || defined(BOARD_RA6M3G_EK)
#define SSRAM_START                             (0x200FE000)
#define SSRAM_LENGTH                            (0x10000)
#elif defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8E1_FPB) ||\
      defined(BOARD_RA8E2_EK)
#define SSRAM_START                             (0x26000000)
#define SSRAM_LENGTH                            (0x400)
#endif /* Specific board */

#if defined(SSRAM_START)
#define APP_SSRAM_DATA                          (MODULE_NAME)
#define APP_SSRAM_LEN                           (sizeof(APP_SSRAM_DATA))
#define APP_SSRAM_OFFSET                        (0U)
#endif /* SSRAM_START */

/* Macros for the on-board LED */
#define LED_LPM_STATE                           (0U)
#define LED_ERROR_STATE                         (1U)

/* Macros for delay time */
#define LPM_TIME_TRANSITION_VALUE               (500U)
#define LPM_TIME_TRANSITION_UNIT                (BSP_DELAY_UNITS_MILLISECONDS)

/* Macros for LPM cancel source name */
#if (BSP_PERIPHERAL_ULPT_PRESENT)
#define NAME_LPM_CANCEL_SOURCE_TIMER            "\r\nLPM is canceled by ULPT0 module\r\n"
#elif (BSP_PERIPHERAL_TML_PRESENT)
#define NAME_LPM_CANCEL_SOURCE_TIMER            "\r\nLPM is canceled by TML0 module\r\n"
#else
#define NAME_LPM_CANCEL_SOURCE_TIMER            "\r\nLPM is canceled by AGT1 module\r\n"
#endif /* Specific timer */
#define NAME_LPM_CANCEL_SOURCE_IRQ              "\r\nLPM is canceled by ICU External IRQ module\r\n"
#define NAME_LPM_CANCEL_SOURCE_DTC              "\r\nLPM is canceled by DTC module\r\n"
#define NAME_LPM_CANCEL_SOURCE_NOT_DETECTED     "\r\nLPM is canceled, but source has not been detected\r\n"

/* Macros for LPM mode name */
#define NAME_LPM_SLEEP_MODE                     "\r\nMCU enters Sleep mode\r\n"
#define NAME_LPM_DEEP_SLEEP_MODE                "\r\nMCU enters Deep Sleep mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE                "\r\nMCU enters SW Standby mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE_WITH_SNOOZE    "\r\nMCU enters SW Standby mode with Snooze enable\r\n"
#define NAME_LPM_DEEP_SW_STANDBY_MODE           "\r\nMCU enters Deep SW Standby mode\r\n"

/* Enumeration for LPM state */
typedef enum e_app_lpm_state
{
    APP_LPM_SLEEP_STATE = 0,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    APP_LPM_DEEP_SLEEP_STATE,
#endif /* BSP_FEATURE_LPM_HAS_DEEP_SLEEP */
    APP_LPM_SW_STANDBY_STATE,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE,
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    APP_LPM_DEEP_SW_STANDBY_STATE,
#endif /* BSP_FEATURE_LPM_HAS_DEEP_STANDBY */
    APP_LPM_MAX_STATE
} app_lpm_states_t;

/* Enumeration for LED state */
#if defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T2_MCK) || defined(BOARD_RA6T3_MCK) || defined(BOARD_RA6T1_RSSK) || \
    defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8T2_MCK)
typedef enum e_led_power
{
    LED_ON = BSP_IO_LEVEL_LOW,
    LED_OFF = BSP_IO_LEVEL_HIGH,
} led_power_t;
#else
typedef enum e_led_power
{
    LED_ON = BSP_IO_LEVEL_HIGH,
    LED_OFF = BSP_IO_LEVEL_LOW,
} led_power_t;
#endif /* Specific board */

/* Extern variable - Used for the on-board LED */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t lpm_init(lpm_ctrl_t * const p_api_ctrl, lpm_cfg_t const * const p_cfg);
fsp_err_t lpm_mode_enter(lpm_ctrl_t * const p_api_ctrl);
fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode);
fsp_err_t lpm_exit_procedure(app_lpm_states_t lpm_mode);
fsp_err_t lpm_wait_start(void);
void lpm_deinit(lpm_ctrl_t * const p_api_ctrl);
void led_lpm_state_set(led_power_t state);
void led_error_state_set(led_power_t state);

#if defined(SSRAM_START) && defined(SSRAM_LENGTH)
bool standby_sram_write(uint32_t offset_addr, uint8_t * p_write_buf, uint32_t len);
bool standby_sram_verify(uint32_t offset_addr, uint8_t * p_verify, uint32_t len);
#endif /* SSRAM_START, SSRAM_LENGTH */

#endif /* LPM_EP_H_ */
