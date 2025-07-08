/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : user_main.c
 * Version      : .
 * OS           : .
 * H/W Platform : FPB-RA2T1
 * Description  : This file includes the main tutorial code.
 * Operation    : See readme.txt
 * Limitations  : .
 *********************************************************************************************************************/



#include "hal_data.h"
#include "user_main.h"




extern bsp_leds_t g_bsp_leds;

/* Define the units to be used with the software delay function */
const bsp_delay_units_t g_bsp_delay_units   = BSP_DELAY_UNITS_MILLISECONDS;
uint32_t                g_delay             = g_bsp_delay_units / 2;
const uint8_t           g_led_flash_speed1  = 2;
const uint8_t           g_led_flash_speed2  = 20;

/* Set the blink frequency*/
const uint32_t g_freq_count1 = 1;
const uint32_t g_freq_count2 = 2;

static void user_def_nmi (bsp_grp_irq_t irq);


/**********************************************************************************************************************
 * Function Name: user_main
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void user_main(void)
{

    bsp_leds_t  leds        = g_bsp_leds;   /* LED type structure */

    /* If this board has no LEDs then trap here */
    if (0 == leds.led_count)
    {
        while (1)
        {
            ;                          /* There are no LEDs on this board */
        }
    }

    R_BSP_GroupIrqWrite (BSP_GRP_IRQ_NMI_PIN, user_def_nmi);

    R_ICU->NMICR_b.NFLTEN = 0;
    R_ICU->NMICR = 0xB1;
    R_ICU->NMICLR_b.NMICLR = 1;
    R_ICU->NMIER_b.NMIEN = 1;       /* Enable the NMI */

    bsp_io_level_t pin_level1 = BSP_IO_LEVEL_HIGH;  /* Holds level to set for pins */
    bsp_io_level_t pin_level2 = BSP_IO_LEVEL_HIGH;  /* Holds level to set for pins */
    uint32_t       count      = 0;

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
        R_BSP_PinAccessDisable();
        R_BSP_SoftwareDelay(g_delay, g_bsp_delay_units); /* Delay */
    }
}
/**********************************************************************************************************************
 End of function user_main
 *********************************************************************************************************************/

/** Define the NMI callback function */
/**********************************************************************************************************************
 * Function Name: user_def_nmi
 * Description  : .
 * Argument     : irq
 * Return Value : .
 *********************************************************************************************************************/
static void user_def_nmi(bsp_grp_irq_t irq)
{
	FSP_PARAMETER_NOT_USED(irq);
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
 End of function user_def_nmi
 *********************************************************************************************************************/

