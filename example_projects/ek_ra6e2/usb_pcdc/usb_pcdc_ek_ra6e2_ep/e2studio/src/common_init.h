/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"

#define LED_INTENSITY_10      (1000)         // 10 percent
#define LED_INTENSITY_50      (5000)        // 50 percent
#define LED_INTENSITY_90      (9000)        // 90 percent

#define BLINK_FREQ_1HZ        (29296)
#define BLINK_FREQ_5HZ        (5859)
#define BLINK_FREQ_10HZ       (2929)

#define KIT_INFO              ('1')
#define NEXT_STEPS            ('2')
#define CARRIAGE_RETURN       ('\r')

#define NUM_STRING_DESCRIPTOR               (7U)

extern bsp_leds_t g_bsp_leds;


/* Macro definitions */
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define APP_ERR_TRAP(a)             if(a) {__asm("BKPT #0\n");} /* trap the error location */


fsp_err_t common_init(void);

