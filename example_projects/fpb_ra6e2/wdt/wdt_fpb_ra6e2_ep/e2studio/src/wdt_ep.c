/***********************************************************************************************************************
 * File Name    : wdt_ep.c
 * Description  : Contains data structures and functions used in wdt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "wdt_ep.h"
#include "timer_setup.h"

/* External global variable */
extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * @brief     This function initializes the WDT.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_wdt_module(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open WDT. For every GPT timeout, WDT will get refreshed. */
    err = R_WDT_Open(&g_wdt_ctrl, &g_wdt_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function checks whether reset is caused by WDT.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void check_reset_status(void)
{
    /* Clear the flag once read the value */
    R_SYSTEM->RSTSR1_b.WDTRF = RESET_VALUE;

    /* Turn ON LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], LED_ON);
}

/*******************************************************************************************************************//**
 * @brief       This function enables WDT counter in debug mode.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void enable_wdt_count_in_debug_mode(void)
{
    /* As per hardware manual's DBGREG module,
     * section 2.9.5.3:- Clear this bit to enable WDT Reset/NMI in debug mode */
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_WDT = RESET_VALUE;
}
