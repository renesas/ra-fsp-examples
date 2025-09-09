/***********************************************************************************************************************
 * File Name    : lpm_app_peripheral.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "lpm_app.h"

/*******************************************************************************************************************//**
 * @addtogroup lpm_app
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief       This function toggles user LED.
 * @param[IN]   leds
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_toggle(bsp_leds_t leds)
{
    fsp_err_t err = FSP_SUCCESS;
	/* Toggle user LED */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);
	/* Handle error */
	if (FSP_SUCCESS == err)
	{
		/* Delay 250 milliseconds */
		R_BSP_SoftwareDelay(250, BSP_DELAY_UNITS_MILLISECONDS);

		/* Toggle user LED */
		err = R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_LOW);

		/* Delay 250 milliseconds */
		R_BSP_SoftwareDelay(250, BSP_DELAY_UNITS_MILLISECONDS);
	}

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function turn on user LED.
 * @param[IN]   leds
 * @retval      fsp_err_t
 **********************************************************************************************************************/
fsp_err_t user_led_on(bsp_leds_t leds)
{
    fsp_err_t err = FSP_SUCCESS;

	/* Toggle user LED */
	err = R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);

    return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_app)
 **********************************************************************************************************************/
