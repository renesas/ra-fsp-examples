/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Entry function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_pwm.h"
#include "uart_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup r_sci_uart_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);

    APP_PRINT("\r\n\r\nThe project initializes the UART with baud rate of 115200 bps.");
    APP_PRINT("\r\nOpen Serial Terminal with this baud rate value and");
    APP_PRINT("\r\nProvide Input ranging from 1 - 100 to set LED Intensity\r\n");

    /* Initializing GPT in PWM mode */
    err = gpt_initialize();
    if (FSP_SUCCESS != err)
    {
        APP_PRINT ("\r\n ** GPT TIMER INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Starting GPT */
    err = gpt_start();
    if (FSP_SUCCESS != err)
    {
        APP_PRINT ("\r\n ** GPT START FAILED ** \r\n");
        timer_gpt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Initializing UART */
    err = uart_initialize();
    if (FSP_SUCCESS != err)
    {
        APP_PRINT ("\r\n ** UART INIT FAILED ** \r\n");
        timer_gpt_deinit();
        APP_ERR_TRAP(err);
    }

    /* The user defined function to demonstrate UART functionality */
    err = uart_ep_demo();
    if (FSP_SUCCESS != err)
    {
        APP_PRINT ("\r\n ** UART EP Demo FAILED ** \r\n");
        timer_gpt_deinit();
        deinit_uart();
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_sci_uart_ep)
 **********************************************************************************************************************/
