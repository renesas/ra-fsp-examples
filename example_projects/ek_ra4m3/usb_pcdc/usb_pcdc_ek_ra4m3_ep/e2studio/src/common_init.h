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

#define LED_INTENSITY_10      (0x3E7)         // 10 percent
#define LED_INTENSITY_50      (0x1388)        // 50 percent
#define LED_INTENSITY_90      (0x2328)        // 90 percent

#define BLINK_FREQ_1HZ        (60000000)
#define BLINK_FREQ_5HZ        (12000000)
#define BLINK_FREQ_10HZ       (6000000)

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
