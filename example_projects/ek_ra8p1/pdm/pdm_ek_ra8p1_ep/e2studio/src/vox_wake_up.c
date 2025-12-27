/***********************************************************************************************************************
 * File Name    : vox_wake_up.c
 * Description  : Contains data structures and functions use to demonstrate VOX wake up operation.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "vox_wake_up.h"

#if (ENABLE_VOX_WAKEUP == 1U)

/* Private functions declarations */
static void led_lpm_state_set(led_power_t state);
static fsp_err_t enter_lpm(void);

/* External variable */
extern const bsp_leds_t g_bsp_leds;

/***********************************************************************************************************************
 *  Function Name: sdram_self_refresh
 *  Description  : This function is used to perform PDM vox wake up operation by sound detection
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t vox_wake_up_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Make sure that MOCO is enabled during SW standby mode */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_CGC);
    R_SYSTEM->MOCOSCR = 1;
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_CGC);

    /* Enter LPM mode */
    err = enter_lpm();
    APP_ERR_RET(FSP_SUCCESS != err, err, "enter_lpm failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function vox_wake_up_operation.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: enter_lpm
 *  Description  : This function is used to perform a procedure when MCU enters LPM SW standby mode.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t enter_lpm(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM SW Standby mode */
    err = R_LPM_Open(&g_sw_standby_ctrl, &g_sw_standby_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_LPM_Open API failed**\r\n");

    /* Turn ON LPM state LED in one second */
    led_lpm_state_set(LED_POWER_ON);
    R_BSP_SoftwareDelay(LED_LPM_LIGHT_TIME, BSP_DELAY_UNITS_SECONDS);

    /* Turn OFF LED before enter LPM mode */
    led_lpm_state_set(LED_POWER_OFF);

    /* Delay to ensure message was printed before enter LPM mode */
    R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    /* Enter LPM SW standby mode */
    err = R_LPM_LowPowerModeEnter(&g_sw_standby_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_LPM_LowPowerModeEnter API failed**\r\n");

    /* Turn ON LPM LED when MCU is returned to the normal mode */
    led_lpm_state_set(LED_POWER_ON);

    /* De-initialize LPM SW standby mode */
    err = R_LPM_Close(&g_sw_standby_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_LPM_Close API failed**\r\n");

    APP_PRINT(CTRL_TEXT_BRIGHT_GREEN "\r\nMCU returned to normal mode after sound detection.\r\n" CTRL_RESET);
    return err;
}
/***********************************************************************************************************************
* End of function enter_lpm.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: led_lpm_state_set
 *  Description  : This function is used to set the state of the LPM state LED.
 *  Arguments    : state    LED state want to set.
 *  Return Value : None.
 **********************************************************************************************************************/
static void led_lpm_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}
/***********************************************************************************************************************
* End of function led_lpm_state_set.
***********************************************************************************************************************/
#endif /* ENABLE_VOX_WAKEUP */
