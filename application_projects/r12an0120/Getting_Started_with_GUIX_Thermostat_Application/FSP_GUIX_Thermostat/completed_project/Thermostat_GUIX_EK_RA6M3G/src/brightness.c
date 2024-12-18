/***********************************************************************************************************************
 * File Name    : brightness.c
 * Description  : Contains functions to control LCD backlight
 ***********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "brightness.h"
#include "system_cfg.h"
#include "system_thread.h"

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
Public Functions
***********************************************************************************************************************/
void brightness_down(uint8_t * p_brightness)
{
    fsp_err_t err                 = FSP_SUCCESS;
    timer_info_t info             = {0};
    uint32_t duty_cycle_count     = 0;
	int8_t brightness             = (int8_t)*p_brightness;

	brightness = (int8_t)(brightness - BRIGHTNESS_INC);
    if (brightness < BRIGHTNESS_MIN)
    {
    	brightness = BRIGHTNESS_MIN;
    } else
    if (brightness > (int8_t)*p_brightness)
    {
    	/* Underflow occurred. */
    	brightness = BRIGHTNESS_MIN;
    }

    /* Get the current period setting. */
    R_GPT_InfoGet(&g_timer_PWM_ctrl, &info);

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. */
    duty_cycle_count = (uint32_t) ((info.period_counts * brightness)/GPT_PWM_MAX_PERCENT);
    err = R_GPT_DutyCycleSet(&g_timer_PWM_ctrl, duty_cycle_count, GPT_IO_PIN_GTIOCA);
    if (FSP_SUCCESS == err)
    {
    	*p_brightness = (uint8_t)brightness;
    }
}

void brightness_up(uint8_t * p_brightness)
{
    fsp_err_t err                 = FSP_SUCCESS;
    timer_info_t info             = {0};
    uint32_t duty_cycle_count     = 0;
	int8_t brightness             = (int8_t)*p_brightness;

    brightness = (int8_t)(brightness + BRIGHTNESS_INC);
    if ((uint8_t)brightness > BRIGHTNESS_MAX)
    {
    	brightness = BRIGHTNESS_MAX;
    } else
    if (brightness < *p_brightness) {
        /* Overflow occurred. */
        brightness = BRIGHTNESS_MAX;
    }
    /* Get the current period setting. */
    R_GPT_InfoGet(&g_timer_PWM_ctrl, &info);

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. */
    duty_cycle_count = (uint32_t) ((info.period_counts * brightness)/GPT_PWM_MAX_PERCENT);

    err = R_GPT_DutyCycleSet(&g_timer_PWM_ctrl, duty_cycle_count, GPT_IO_PIN_GTIOCA);

    if (FSP_SUCCESS == err)
    {
    	*p_brightness = (uint8_t)brightness;
    }
}

/*******************************************************************************************************************//**
 * @}
***********************************************************************************************************************/
