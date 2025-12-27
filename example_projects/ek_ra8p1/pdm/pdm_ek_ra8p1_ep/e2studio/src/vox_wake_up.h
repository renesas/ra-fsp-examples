/***********************************************************************************************************************
 * File Name    : vox_wake_up.h
 * Description  : Contains data structures and functions used in vox_wake_up.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef VOX_WAKE_UP_H_
#define VOX_WAKE_UP_H_

#include "common_utils.h"
#include "pdm_ep.h"

#if (ENABLE_VOX_WAKEUP == 1U)

/* Macros for text color */
#define CTRL_RESET                "\x1B[0m"
#define CTRL_TEXT_BRIGHT_GREEN    "\x1B[1;32m"

/* Macros for User LED */
#define LED_ERROR_STATE           (2U)
#define LED_LPM_STATE             (0U)
#define LED_LPM_LIGHT_TIME        (1)

#define PRINT_DELAY               (100)

/* Enumeration for led state use in application */
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
} led_power_t;

/* Public function declaration */
fsp_err_t vox_wake_up_operation(void);

#endif /* ENABLE_VOX_WAKEUP */
#endif /* VOX_WAKE_UP_H_ */
