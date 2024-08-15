/***********************************************************************************************************************
 * File Name    : tml_ep.h
 * Description  : Define macros and functions used in tml_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

#ifndef TML_EP_H_
#define TML_EP_H_

#define EP_INFO             "\r\nThe example project showcases how the TML driver operates on Renesas RA MCUs."\
							"\r\nThe demonstration includes showcasing the TML timer with counter operation"\
							"\r\nin normal mode, capture operation, and counter operation in low power mode."\
							"\r\nIn TML counter operation (normal mode), three TML instances (which are configured"\
							"\r\nas 8-bit, 16-bit, and 32-bit counter modes) operate for the inputted duration by"\
							"\r\nthe user. In TML 16-bit capture operation, user can trigger an ELC software event"\
							"\r\nto get the raw counts value of 16-bit timer. In TML counter operation (low power"\
							"\r\nmodes), the 32-bit timer is configured as counter mode with period 10 seconds."\
							"\r\nOnce the 32-bit timer has expired, the RA board is woken up from LPM to normal mode."\
							"\r\nThe EP information and error messages will be printed to the Host PC.\r\n\r\n"

#define PRINT_MENU         "\r\n Main Menu:"\
						   "\r\n 1. TML counter mode"\
						   "\r\n 2. TML capture mode "\
						   "\r\n 3. TML counter mode operates in LPM mode "\
						   "\r\n User Input: "

/* Macros to define TML capture mode option */
#define CAPTURE        		(1U)
#define RETURN_MAIN_MENU    (2U)

/* Macros to define TML operation selection */
#define TML_COUNTER        (1U)
#define TML_CAPTURE        (2U)
#define TML_COUNTER_IN_LPM (3U)

/* Macros for delay time in application */
#define LPM_TIME_TRANSITION_VALUE     (1000U)
#define LPM_TIME_TRANSITION_UNIT      (BSP_DELAY_UNITS_MILLISECONDS)
#define LPM_NAME_PRINT_TIME        	  (10U)

/* Macros to define limitation range of duration time value */
#define MAX_DURATION        (60U)
#define MIN_DURATION        (0U)

/* Macro for FSP Module status */
#define MODULE_CLOSED      (0U)

/* Macros for LEDs on board */
#define LED_COUNTER_MODE   (0U)
#define LED_LPM_STATE	   (1U)

/* Macros for period in milliseconds of TML modes */
#define PERIOD_8BIT_MILLI       (1)
#define PERIOD_16BIT_MILLI      (100)
#define PERIOD_32BIT_MILLI      (1000)

/* Macros for name of TML modules in each mode */
#define NAME_8BIT_MODE       			"8-bit counter timer"
#define NAME_16BIT_MODE      			"16-bit counter timer"
#define NAME_32BIT_MODE      			"32-bit counter timer"
#define NAME_16BIT_CAPTURE_MODE 		"16-bit capture timer"

/* Macros for LPM modes name */
#define NAME_LPM_SLEEP_MODE                     "MCU enters Sleep mode"
#define NAME_LPM_SW_STANDBY_MODE                "MCU enters SW Standby mode"

/* Macro for the conversion */
#define SECOND_TO_MILLISECOND  (1000)

/* Enumeration for TML modes use in example project */
typedef enum e_app_tml_mode
{
	APP_TML_8_BIT = 0,
	APP_TML_16_BIT,
	APP_TML_32_BIT,
	APP_TML_16_BIT_CAPTURE,
	APP_TML_32_BIT_COUNTER_LPM,
	APP_TML_MAX_MODE
} app_tml_mode_t;

/* Enumeration for low power modes use in example project */
typedef enum e_app_lpm_state
{
	APP_LPM_SLEEP_STATE = 0,
	APP_LPM_SW_STANDBY_STATE,
	APP_LPM_MAX_STATE
} app_lpm_states_t;

/* Enumeration for LED status */
typedef enum e_led_power
{
	LED_ON = BSP_IO_LEVEL_HIGH,
	LED_OFF = BSP_IO_LEVEL_LOW
} led_power_t;

/* function declarations */
void tml_entry(void);

#endif /* TML_EP_H_ */



