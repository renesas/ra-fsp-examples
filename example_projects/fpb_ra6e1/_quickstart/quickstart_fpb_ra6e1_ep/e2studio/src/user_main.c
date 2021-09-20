/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : user_main.c
 * Version      : .
 * OS           : .
 * H/W Platform : FPB-RA6E1
 * Description  : This file includes the main tutorial code.
 * Operation    : See readme.txt
 * Limitations  : .
 *********************************************************************************************************************/



#include "hal_data.h"
#include "user_main.h"

extern bsp_leds_t g_bsp_leds;

/* Define the units to be used with the software delay function */
const bsp_delay_units_t g_bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;
uint32_t g_delay = g_bsp_delay_units / 2;
const uint8_t g_led_flash_speed1=2;
const uint8_t g_led_flash_speed2=20;

/* Set the blink frequency*/
const uint32_t g_freq_count1 = 1;
const uint32_t g_freq_count2 = 2;

/**********************************************************************************************************************
 * Function Name: st_irq_pins_t;
 * Description  : stores location of the pins used for irqs
 * Argument     : s_irq_pins
 * Return Value : .
 *********************************************************************************************************************/
static st_irq_pins_t s_irq_pins[] =
{
    { &g_external_irq1ds }
};
/**********************************************************************************************************************
 End of function st_irq_pins_t
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : initialises irq pins to allow for button presses
 * Return Value : fsp_err
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    uint8_t g_irq_switch = ((sizeof(s_irq_pins)) / (sizeof(s_irq_pins[0]))); /*  */

    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < g_irq_switch; i++ )
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
/**********************************************************************************************************************
 End of function icu_initialize
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: user_main
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void user_main(void)
{
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

    icu_initialize();

    /* Holds level to set for pins */
    bsp_io_level_t pin_level1 = BSP_IO_LEVEL_HIGH;
    bsp_io_level_t pin_level2 = BSP_IO_LEVEL_HIGH;

    uint32_t count=0;

    while (1)
    {
        count++;

        /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
         * handled. This code uses BSP IO functions to show how it is used.
         */
        R_BSP_PinAccessEnable();

            /* Write to this pin */
            if ((count % g_freq_count1) == 0)
            {
                /*cast bsp_io_port_pin_t*/
                R_BSP_PinWrite((bsp_io_port_pin_t)leds.p_leds[0], pin_level1);
                pin_level1 = !pin_level1;
            }
            if ((count % g_freq_count2) == 0)
            {
                /*cast bsp_io_port_pin_t*/
                R_BSP_PinWrite((bsp_io_port_pin_t)leds.p_leds[1], pin_level2);
                pin_level2 = !pin_level2;
            }


        /* Protect PFS registers */
        R_BSP_PinAccessDisable();

        /* Delay */
        R_BSP_SoftwareDelay(g_delay, g_bsp_delay_units);
    }
}
/**********************************************************************************************************************
 End of function user_main
 *********************************************************************************************************************/

/** External IRQ for SW 1 on ICU Instance. */
/**********************************************************************************************************************
 * Function Name: callback_irq1ds_button
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void callback_irq1ds_button(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    switch (g_delay)
    {
        case g_bsp_delay_units/g_led_flash_speed1:
            g_delay = g_bsp_delay_units / g_led_flash_speed2;
            break;
        case g_bsp_delay_units / g_led_flash_speed2:
            g_delay = g_bsp_delay_units / g_led_flash_speed1;
            break;
        default:
            g_delay = g_bsp_delay_units / g_led_flash_speed1;
            break;
    }
}
/**********************************************************************************************************************
 End of function callback_irq1ds_button
 *********************************************************************************************************************/
