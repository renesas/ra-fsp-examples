/***********************************************************************************************************************
 * File Name    : lpm_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef LPM_EP_H_
#define LPM_EP_H_

#include "common_utils.h"
#include "uart_terminal.h"

/**********************************************************************************************************************
* Macro definitions
**********************************************************************************************************************/
/* Macro for Standby SRAM */
#if defined(BOARD_RA4E1_EK) || defined(BOARD_RA4E2_EK) || defined(BOARD_RA4M2_EK) || defined(BOARD_RA4M3_EK) || \
    defined(BOARD_RA6E1_EK) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA6M4_EK) || defined(BOARD_RA6M5_EK) || \
    defined(BOARD_RA6T2_MCK)
#define SSRAM_START             (0x28000000)
#define SSRAM_LENGTH            (0x400)

#elif defined(BOARD_RA6M1_EK) || defined(BOARD_RA6M2_EK) || defined(BOARD_RA6M3_EK)
#define SSRAM_START             (0x200FE000)
#define SSRAM_LENGTH            (0x10000)

#elif defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK)
#define SSRAM_START             (0x26000000)
#define SSRAM_LENGTH            (0x400)
#endif

#if defined(SSRAM_START)
#define APP_SSRAM_DATA          (MODULE_NAME)
#define APP_SSRAM_LEN           (sizeof(APP_SSRAM_DATA))
#define APP_SSRAM_OFFSET        (0U)
#endif

/* Macro for define LED on board */
#define LED_LPM_STATE                           (0U)
#define LED_ERROR_STATE                         (1U)

/* Macro for delay time in application */
#define LPM_TIME_TRANSITION_VALUE               (500U)
#define LPM_TIME_TRANSITION_UNIT                (BSP_DELAY_UNITS_MILLISECONDS)

/* Macro for LPM cancel source name */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    #define NAME_LPM_CANCEL_SOURCE_TIMER            "\r\nLPM is canceled by ULPT0 module\r\n"
#else
    #define NAME_LPM_CANCEL_SOURCE_TIMER            "\r\nLPM is canceled by AGT1 module\r\n"
#endif

#define NAME_LPM_CANCEL_SOURCE_IRQ              "\r\nLPM is canceled by ICU External IRQ module\r\n"
#define NAME_LPM_CANCEL_SOURCE_DTC              "\r\nLPM is canceled by DTC module\r\n"
#define NAME_LPM_CANCEL_SOURCE_NOT_DETECTED     "\r\nLPM is canceled, but source has not been detected\r\n"

/* Macro for LPM mode name */
#define NAME_LPM_SLEEP_MODE                     "\r\nMCU enters Sleep mode\r\n"
#define NAME_LPM_DEEP_SLEEP_MODE                "\r\nMCU enters Deep Sleep mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE                "\r\nMCU enters SW Standby mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE_WITH_SNOOZE    "\r\nMCU enters SW Standby mode with Snooze enable\r\n"
#define NAME_LPM_DEEP_SW_STANDBY_MODE           "\r\nMCU enters Deep SW Standby mode\r\n"

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumeration for low power mode use in application */
typedef enum e_app_lpm_state
{
    APP_LPM_SLEEP_STATE = 0,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    APP_LPM_DEEP_SLEEP_STATE,
#endif
    APP_LPM_SW_STANDBY_STATE,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE,
#endif
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    APP_LPM_DEEP_SW_STANDBY_STATE,
#endif
    APP_LPM_MAX_STATE
} app_lpm_states_t;

/* Enumeration for led state use in application */
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
}led_power_t;

/**********************************************************************************************************************
* Public functions declarations
**********************************************************************************************************************/
void lpm_ep_entry(void);

#endif /* LPM_EP_H_ */
