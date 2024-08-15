/**********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Board startup functions.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "r_console.h"
#include "common_init.h"
#include "menu_main.h"
#include "board_cfg.h"


extern bsp_leds_t g_bsp_leds;

void R_BSP_WarmStart (bsp_warm_start_event_t event);

/**********************************************************************************************************************
 * Function Name: hal_entry
 * Description  : Blinky example application
 *                Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 * Return Value : .
 *********************************************************************************************************************/
void hal_entry(void)
{
#if BSP_TZ_SECURE_BUILD

    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* If this board has no LEDs then trap here */
    if (0 == leds.led_count)
    {
        while (1)
        {
            ;                          // There are no LEDs on this board
        }
    }

    SYSTEM_OK

    common_init();

    while (1)
    {
        main_display_menu();
    }
}
/**********************************************************************************************************************
 End of function hal_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_BSP_WarmStart
 * Description  : This function is called at various points during the startup process.
 *                This implementation uses the event that is called right before main() to set up the pins.
 * Argument     : event   Where at in the start up process the code is currently at
 * Return Value : .
 *********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take
         * more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
/**********************************************************************************************************************
 End of function R_BSP_WarmStart
 *********************************************************************************************************************/
