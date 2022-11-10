/***********************************************************************************************************************
 * File Name    : led_blinky_entry.c
 * Description  : Contains macros and functions used in led_blinky_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
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
