/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "blinky_thread.h"
#include "common_utils.h"
#include "app_definitions.h"
#include "flash_access_ns.h"

extern bsp_leds_t g_bsp_leds;
static uint8_t writeBuffer[FLASH_WRITE_LENGTH];    /* buffer for flash writing */
static void setup_the_writeBuffer()
{
    volatile int i=0;

    for (i=0; i<FLASH_WRITE_LENGTH; i++)
            writeBuffer[i]=FLASH_WRITE_TEST_DATA;
}

/* Blinky Thread entry function */
void blinky_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    bool status_flash = false;

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

    /* Holds level to set for pins */
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

    while (1)
    {
        APP_PRINT(MENUSTATUS1);
        setup_the_writeBuffer();

        /* Below flash write will succeed as the block to be written is not locked for programming and erasing.
         * In addition, reading Non-secure flash to confirm the write is successful is also granted
         */
        status_flash = flash_write_ns(FLASH_WRITE_TEST_BLOCK, (unsigned char *)writeBuffer);
        if(true == status_flash)
        {
            APP_PRINT(ACCESS_SUCCESSFUL);
        }
        else
            APP_PRINT(ACCESS_UN_SUCCESSFUL);
        /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
         * handled. This code uses BSP IO functions to show how it is used.
         */
        R_BSP_PinAccessEnable();

        /* Update all board LEDs */
        for (uint32_t i = 0; i < leds.led_count; i++)
        {
            /* Get pin to toggle */
            uint32_t pin = leds.p_leds[i];

            /* Write to this pin */
            R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);
        }

        /* Protect PFS registers */
        R_BSP_PinAccessDisable();

        /* Toggle level for next write */
        if (BSP_IO_LEVEL_LOW == pin_level)
        {
            pin_level = BSP_IO_LEVEL_HIGH;
        }
        else
        {
            pin_level = BSP_IO_LEVEL_LOW;
        }

        APP_PRINT(MENUSTATUS2);
        vTaskDelay(configTICK_RATE_HZ);
    }
}
