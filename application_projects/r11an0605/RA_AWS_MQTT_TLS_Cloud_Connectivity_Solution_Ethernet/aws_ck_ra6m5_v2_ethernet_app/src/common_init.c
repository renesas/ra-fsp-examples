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

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;


int32_t g_curr_led_freq = BLINK_FREQ_1HZ;


/*******************************************************************************************************************//**
 * @brief    LED initialization
 * @param[in]
 * @retval   FSP_SUCCESS
 ***********************************************************************************************************************/

static fsp_err_t led_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    R_BSP_PinAccessEnable();

    /* LED1,3,4 should be 'off' during power up*/

    R_BSP_PinAccessDisable();

    return fsp_err;
}

/*******************************************************************************************************************//**
 * @brief    Common Init containing 'led_initialize'
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
