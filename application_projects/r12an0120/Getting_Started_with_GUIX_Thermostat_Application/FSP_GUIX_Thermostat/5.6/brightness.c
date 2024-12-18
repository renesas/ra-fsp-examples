/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the �contents�) are proprietary and confidential to Renesas Electronics Corporation 
 * and/or its licensors (�Renesas�) and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �AS IS� WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : brightness.c
* Description  : Brightness settings.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           TODO       1.00    Initial Release.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup  Brightness
 *
 * @{
 **********************************************************************************************************************/

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
