/***********************************************************************************************************************
 * File Name    : sdram_ep.c
 * Description  : Contains data structures and functions used in sdram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sdram_ep.h"

/* Global variable */
volatile bool g_sdram_transmission_complete = false;

/***********************************************************************************************************************
 * Function Name: lpm_enter
 * Description  : This function performs a procedure when MCU enters LPM mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t lpm_enter(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM SW Standby mode */
    err = R_LPM_Open(&g_sw_standby_ctrl, &g_sw_standby_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Turn ON the LPM state LED for one second */
    led_lpm_state_set(LED_POWER_ON);
    R_BSP_SoftwareDelay(LED_LPM_ON_TIME, BSP_DELAY_UNITS_SECONDS);

    /* Turn OFF the LPM state LED before entering LPM mode */
    led_lpm_state_set(LED_POWER_OFF);

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK)
    /* UART module stops with undefined in SW Standby mode */
    uart_console_deinit();
#endif /* BOARD_RA8D1_EK, BOARD_RA8E2_EK, BOARD_RA8P1_EK */

    /* Enter LPM SW Standby mode */
    err = R_LPM_LowPowerModeEnter(&g_sw_standby_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK)
    /* Re-initialize UART module after exiting SW Standby mode */
    uart_console_init();
#endif

    /* Turn ON the LPM state LED when MCU is returned to the normal mode */
    led_lpm_state_set(LED_POWER_ON);

    /* De-initialize LPM SW Standby mode */
    err = R_LPM_Close(&g_sw_standby_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function lpm_enter
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lpm_deinit
 * Description  : This function closes the opened LPM module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void lpm_deinit(void)
{
    /* Close the opened LPM module */
    if (g_sw_standby_ctrl.lpm_open)
    {
        R_LPM_Close(&g_sw_standby_ctrl);
    }
}
/***********************************************************************************************************************
* End of function lpm_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_init
 * Description  : This function opens the ULPT timer.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Opened successfully.
 *                Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ULPT_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function timer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_start
 * Description  : This function starts the ULPT timer.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Started successfully.
 *                Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ULPT_Start(&g_timer_cancel_lpm_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function timer_start
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_stop
 * Description  : This function stops the ULPT timer.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Stopped successfully.
 *                Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t timer_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ULPT_Stop(&g_timer_cancel_lpm_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function timer_stop
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_deinit
 * Description  : This function closes the opened ULPT timer.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void timer_deinit(void)
{
    /* Close the opened ULPT timer */
    if (g_timer_cancel_lpm_ctrl.open)
    {
        R_ULPT_Close(&g_timer_cancel_lpm_ctrl);
    }
}
/***********************************************************************************************************************
* End of function timer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dmac_init
 * Description  : This function opens the DMAC module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Opened successfully.
 *                Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t dmac_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_DMAC_Open(&g_sdram_transmission_ctrl, &g_sdram_transmission_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function dmac_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dmac_reset
 * Description  : This function resets transfer source, destination, and number of transfers.
 * Arguments    : p_api_ctrl                Pointer to the transfer control block.
 *                p_src                     Pointer to the transfer source.
 *                p_dest                    Pointer to the transfer destination.
 *                num_transfers             Number of transfers.
 * Return Value : FSP_SUCCESS               Reset successfully.
 *                Any other error codes     Failed to reset.
 **********************************************************************************************************************/
fsp_err_t dmac_reset(transfer_ctrl_t * const p_api_ctrl, void const * volatile p_src, void * volatile p_dest,
                     uint16_t const num_transfers)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_DMAC_Reset(p_api_ctrl, p_src, p_dest, num_transfers);

    return err;
}
/***********************************************************************************************************************
* End of function dmac_reset
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dmac_software_start
 * Description  : This function starts the DMAC transfer in repeat mode using software trigger.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Started successfully.
 *                Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t dmac_software_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_DMAC_SoftwareStart(&g_sdram_transmission_ctrl, TRANSFER_START_MODE_REPEAT);

    return err;
}
/***********************************************************************************************************************
* End of function dmac_software_start
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: dmac_deinit
 *  Description  : This function closes the opened DMAC module.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void dmac_deinit(void)
{
    /* Close the opened DMAC module */
    if (g_sdram_transmission_ctrl.open)
    {
        R_DMAC_Close(&g_sdram_transmission_ctrl);
    }
}
/***********************************************************************************************************************
* End of function dmac_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: led_lpm_state_set
 * Description  : This function sets the state of the LPM state LED.
 * Arguments    : state                     LED state to be set.
 * Return Value : None.
 **********************************************************************************************************************/
void led_lpm_state_set(led_power_t state)
{
    /* Check if the board supports this LED */
    if (LED_LPM_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}
/***********************************************************************************************************************
* End of function led_lpm_state_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: led_error_state_set
 * Description  : This function sets the state of the error LED.
 * Arguments    : state                     LED state to be set.
 * Return Value : None.
 **********************************************************************************************************************/
void led_error_state_set(led_power_t state)
{
    /* Check if the board supports this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ERROR_STATE], (bsp_io_level_t)state);
    }
}
/***********************************************************************************************************************
* End of function led_error_state_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: transmission_sdram_callback
 * Description  : This function is a callback for SDRAM transmission.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void transmission_sdram_callback(dmac_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        /* Set the transmission complete flag */
        g_sdram_transmission_complete = true;
    }
}
/***********************************************************************************************************************
* End of function transmission_sdram_callback
***********************************************************************************************************************/
