/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : lp_mode.h
 * Version      : 1.0
 * Description  : Information for configuration of the Low Power mode part of the demonstration.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "board_hw_cfg.h"
 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef LP_MODE_H_
#define LP_MODE_H_

#define LPM_BANNER      "4. LOW POWER MODES DEMONSTRATION"                                                          \
                        "\r\n"                                                                                      \
                        "\r\n> Use SW1 to toggle between low power modes or"                                        \
                        "\r\n> Select from the options in the menu below:"                                          \
                        "\r\n"                                                                                      \
                        "\r\n1. Transition to Sleep Mode"                                                           \
                        "\r\n2. Transition to Software Standby Mode"                                                \
                        "\r\n"                                                                                      \
                        "\r\nThe LEDs will indicate the mode that the device is using"                              \
                        "\r\nBLUE_LED  Device is in full power mode "                                               \
                        "\r\nGREEN_LED Device is in sleep mode"                                                     \
                        "\r\nNO LED    Device is in standby mode"                                                   \
                        "\r\n"                                                                                      \
                        "\r\nCaution: When starting a debug connection,"                                            \
                        "\r\n         the MCU must be in Normal or Sleep mode. "                                    \
                        "\r\n         If the MCU is in Software Standby mode,"                                      \
                        "\r\n         the debugger can cause the MCU to hang."                                      \
                        "\r\n"                                                                                      \
                        "\r\nPlease check Hardware Manual for more information"                                     \
                        "\r\n"                                                                                      \
                        "\r\n   Note: The ADC temperature sensor will be "                                          \
                        "\r\n         turned off to save power."                                                    \
                        "\r\n"                                                                                      \
                        "\r\n> Press space bar to exit demo and return to MENU"


#define LPM_SLEEP_MODE      "\r\n1. Transition to Sleep Mode\r\n"                                  \
                            "\r\nSleep mode can be cancelled by pressing SW2 on the board.\r\n"    \
                            "\r\n(SW2 : Canceled by IRQ)\r\n"

#define LPM_SOFTWARE_STANDBY_MODE   "\r\n2. Transition to Software Standby Mode\r\n"                                   \
                                    "\r\nSoftware standby mode can be cancelled by pressing SW2 on the board.\r\n"     \
                                    "\r\n(SW2 : Canceled by IRQ)\r\n"

/* Macro for define LED on board */
#define LED_LPM_STATE                           (0U)
#define LED_SLEEP_STATE                         (1U)
#define LED_SW_STANDBY_STATE                    (2U)

/* Macro for delay time in application */
#define LPM_TIME_TRANSITION_VALUE               (500U)
#define LPM_TIME_TRANSITION_UNIT                (BSP_DELAY_UNITS_MILLISECONDS)

#define NAME_LPM_CANCEL_SOURCE_IRQ              "\r\nLPM is canceled by ICU External IRQ module\r\n"
#define NAME_LPM_CANCEL_SOURCE_NOT_DETECTED     "\r\nLPM is canceled, but source has not been detected\r\n"

/* Macro for LPM mode name */
#define NAME_LPM_SLEEP_MODE                     "\r\nMCU enters Sleep mode\r\n"
#define NAME_LPM_DEEP_SLEEP_MODE                "\r\nMCU enters Deep Sleep mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE                "\r\nMCU enters SW Standby mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE_WITH_SNOOZE    "\r\nMCU enters SW Standby mode with Snooze enable\r\n"
#define NAME_LPM_DEEP_SW_STANDBY_MODE           "\r\nMCU enters Deep SW Standby mode\r\n"

#define LPM_SWITCH_MODE     (1 << 0)    /* Mask for Low Power Mode switch EVENT */
#define LPM_LEAVE_MODE      (1 << 1)    /* Mask for Low Power leave EVENT */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/* Enumeration for low power mode use in application */
typedef enum e_app_lpm_state
{
    APP_LPM_SLEEP_STATE = 0,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    APP_LPM_DEEP_SLEEP_STATE,
#endif
    APP_LPM_SW_STANDBY_STATE,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    APP_LPM_MODE_STANDBY_SNOOZE,
#endif
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    APP_LPM_DEEP_SW_STANDBY_STATE,
#endif
    APP_LPM_MAX_STATE
} e_app_lpm_states_t;

/* Enumeration for led state use in application */
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
}e_led_power_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern volatile EventBits_t g_s_lp_bits;

extern rtc_alarm_time_t g_mins_wake;

extern uint32_t         g_alarm_cal;
extern bool_t           g_s_alarm_reached;
extern bool_t           g_alarm_ring;
extern rtc_alarm_time_t g_rtc_alarm;
extern const bsp_leds_t g_bsp_leds;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern test_fn lpm_main_display_menu (void);

#endif /* LP_MODE_H_ */
