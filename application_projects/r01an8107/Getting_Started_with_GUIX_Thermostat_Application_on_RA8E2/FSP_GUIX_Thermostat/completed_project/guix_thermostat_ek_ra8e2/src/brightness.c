/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * File Name    : brightness.c
 * Description  : Contains functions to control LCD backlight
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
    if (brightness > (int8_t) *p_brightness)
    {
        brightness = (10);
    }
    else
    {
    }

    /* Get the current period setting. */
    R_GPT_InfoGet(&g_timer_PWM_ctrl, &info);

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. */
    duty_cycle_count = (uint32_t) ((info.period_counts * brightness)/GPT_PWM_MAX_PERCENT);
    err = R_GPT_DutyCycleSet(&g_timer_PWM_ctrl, duty_cycle_count, GPT_IO_PIN_GTIOCB);
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
    if (brightness < *p_brightness)
    {
        brightness = (100);
    }
    else
    {
    }
    /* Get the current period setting. */
    R_GPT_InfoGet(&g_timer_PWM_ctrl, &info);

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. */
    duty_cycle_count = (uint32_t) ((info.period_counts * brightness)/GPT_PWM_MAX_PERCENT);

    err = R_GPT_DutyCycleSet(&g_timer_PWM_ctrl, duty_cycle_count, GPT_IO_PIN_GTIOCB);

    if (FSP_SUCCESS == err)
    {
    	*p_brightness = (uint8_t)brightness;
    }
}

/*******************************************************************************************************************//**
 * @}
***********************************************************************************************************************/
