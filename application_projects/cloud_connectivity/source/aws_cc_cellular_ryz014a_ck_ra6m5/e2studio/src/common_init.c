/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/

#include "common_init.h"
#include "r_typedefs.h"

#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))   /*  */
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))       /*  */
#define NUM_SWITCH            (sizeof(s_irq_pins) / sizeof(s_irq_pins[0])) /*  */

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;


int32_t g_curr_led_freq = BLINK_FREQ_1HZ;

static st_irq_pins_t s_irq_pins[] =
{
     { &g_external_irq1 }
};


/*******************************************************************************************************************//**
 * @brief    LED initialization
 * @param[in]
 * @retval   FSP_SUCCESS
 ***********************************************************************************************************************/

static fsp_err_t led_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    R_BSP_PinAccessEnable();

    /* LED1,3,4 should be 'off' durning power up*/

    R_BSP_PinAccessDisable();

    return fsp_err;
}

/*******************************************************************************************************************//**
 * @brief    ICU initialization
 * @param[in]
 * @retval   FSP_SUCCESS or Error Code
 ***********************************************************************************************************************/

static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen (s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable (s_irq_pins[i].p_irq->p_ctrl);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 * @brief    Common Init containing 'led_initialize' and 'icu_initialize' func calls
 * @param[in]
 * @retval   FSP_SUCCESS or Error Code
 ***********************************************************************************************************************/

fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = led_initialize ();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = icu_initialize ();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 End of function periodic_timer_callback
 *********************************************************************************************************************/
