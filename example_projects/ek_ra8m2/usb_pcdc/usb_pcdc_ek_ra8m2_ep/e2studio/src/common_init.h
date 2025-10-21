/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common initialization functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdbool.h>
#include "hal_data.h"

#define LED_INTENSITY_10        (0x3E7)         /* 10 percent */
#define LED_INTENSITY_50        (0x1388)        /* 50 percent */
#define LED_INTENSITY_90        (0x2328)        /* 90 percent */

#define BLINK_FREQ_1HZ          (150000000)
#define BLINK_FREQ_5HZ          (30000000)
#define BLINK_FREQ_10HZ         (15000000)

#define KIT_INFO                ('1')
#define NEXT_STEPS              ('2')
#define CARRIAGE_RETURN         ('\r')

#define NUM_STRING_DESCRIPTOR   (7U)

extern bsp_leds_t g_bsp_leds;

/* Macro definitions */
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define APP_ERR_TRAP(a)             ({if(a) {__asm("BKPT #0\n");}}) /* Trap the error location */

fsp_err_t common_init(void);
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
fsp_err_t adc_start_calibration(void);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
