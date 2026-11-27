/***********************************************************************************************************************
 * File Name    : tml_ep.h
 * Description  : Contains macros, data structures, and functions used in tml_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TML_EP_H_
#define TML_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macros for buffer */
#define BUFFER_SIZE                 (16U)

/* Enumeration for LED status */
#define LED_ON                      (BSP_IO_LEVEL_HIGH)
#define LED_OFF                     (BSP_IO_LEVEL_LOW)

/* Macros to define TML capture mode option */
#define CAPTURE        		        (1U)
#define RETURN_MAIN_MENU            (2U)

/* Macros to define TML operation selection */
#define TML_COUNTER                 (1U)
#define TML_CAPTURE                 (2U)
#define TML_COUNTER_IN_LPM          (3U)

/* Macros to define limitation range of duration time value */
#define MAX_DURATION                (60U)
#define MIN_DURATION                (0U)

/* Macro for FSP module status */
#define MODULE_CLOSED               (0U)

/* Macros for LEDs on board */
#define LED_COUNTER_MODE            (0U)
#define LED_LPM_STATE               (1U)

/* Macros for period in milliseconds of TML modes */
#define PERIOD_8BIT_MILLI           (1U)
#define PERIOD_16BIT_MILLI          (100U)
#define PERIOD_32BIT_MILLI          (1000U)

/* Macro for the conversion */
#define SECOND_TO_MILLISECOND  		(1000U)

#if (!TML_CFG_PARAM_CHECKING_ENABLE)
/* Macro used to determine whether the channel is opened */
#define TML_OPEN                    (0x00544D4CULL)
#endif /* !TML_CFG_PARAM_CHECKING_ENABLE */

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

/* External variable - Used for LED */
extern bsp_leds_t g_bsp_leds;

/* External variables - Defined in tml_ep.c */
extern volatile bool g_timer_flag[APP_TML_MAX_MODE];
extern volatile uint16_t g_number_cycles;
extern volatile uint32_t g_captured_time;

/* Function declarations */
fsp_err_t tml_init(app_tml_mode_t tml_mode);
fsp_err_t tml_start(app_tml_mode_t tml_mode);
fsp_err_t tml_stop(app_tml_mode_t tml_mode);
fsp_err_t tml_get_info(timer_ctrl_t * const p_ctrl, timer_info_t * const p_info);
fsp_err_t tml_set_period(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);
void tml_close(app_tml_mode_t tml_mode);
void tml_deinit(void);
void set_led(int led_num, bsp_io_level_t b_value);
void toggle_led(void);

#endif /* TML_EP_H_ */
