/***********************************************************************************************************************
 * File Name    : led_blinky_entry.c
 * Description  : Contains macros and functions used in led_blinky_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "led_blinky.h"
#include "usbx_otg_cdc.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_otg_cdc_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 * Macro definitions
 *****************************************************************************/
#define LED_1  BSP_IO_PORT_01_PIN_06

/******************************************************************************
 * Private global variables

 ******************************************************************************/
volatile UCHAR led_status = VALUE_0;

/******************************************************************************
 * Function Name   : led_blinky_entry
 * Description     : Function for LED blinking
 * Arguments       : none
 * Return value    : none
 ******************************************************************************/
void led_blinky_entry(void)
{

    uint32_t pin = LED_1;
    /* Holds level to set for pins */
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;
    R_BSP_PinAccessEnable();
    while (true)
    {
        /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
         * handled. This code uses BSP IO functions to show how it is used.
         */

        if(led_status == VALUE_1)
        {
            /* Write to this pin */
            R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);

            /* Toggle level for next write */
            if (BSP_IO_LEVEL_LOW == pin_level)
            {
                pin_level = BSP_IO_LEVEL_HIGH;
            }
            else
            {
                pin_level = BSP_IO_LEVEL_LOW;
            }

            tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
        }
        else if(led_status == VALUE_0)
        {
            /* Update all board LEDs */
            pin_level = BSP_IO_LEVEL_HIGH;

            /* Write to this pin */
            R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);
            tx_thread_sleep (VALUE_2);
        }
        else
        {
            tx_thread_sleep (VALUE_1);
        }
    }
}
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_otg_cdc_ep)
 **********************************************************************************************************************/
