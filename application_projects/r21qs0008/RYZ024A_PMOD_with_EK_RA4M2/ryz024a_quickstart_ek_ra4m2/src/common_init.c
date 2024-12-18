/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/
#define COMMON_INIT_C

#include "common_init.h"

#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))   /*  */
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))       /*  */
/* Enable SW toggling of the LED. ie. PWM via interrupt handler */
#define USE_SW_TIMER          (1)

const char_t * const gp_cursor_store = "\x1b[s";
const char_t * const gp_cursor_restore = "\x1b[u";
const char_t * const gp_cursor_temp = "\x1b[8;41H\x1b[K";

const char_t * const gp_green_fg = "\x1b[32m";
const char_t * const gp_white_fg = "\x1b[37m";

const char_t * const gp_clear_screen = "\x1b[2J";
const char_t * const gp_cursor_home = "\x1b[H";

adc_info_t g_adc_info_rtn;


/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t adc_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanStart (&g_adc_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    fsp_err = R_ADC_InfoGet (&g_adc_ctrl, &g_adc_info_rtn);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function adc_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = adc_initialize ();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }



    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/
