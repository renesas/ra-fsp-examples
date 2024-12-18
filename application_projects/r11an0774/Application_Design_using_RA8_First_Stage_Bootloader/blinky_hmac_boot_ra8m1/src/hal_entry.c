/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#include "hal_data.h"
#include <stdio.h>
#include <string.h>
#include "common_definitions.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);


#define MENUSTATUS1       "\nRunning the blinky application (FSBL enabled with HMAC verification). \r\n"
#define MENUSTATUS2       "The Red, Blue and Green LEDs should be blinking.\r\n"
#define MENUSTATUS3       "\nThe current setting of the FSBL registers: \r\n\r\n"
#define MENUSTATUS4       "\nThe current output of the boot measurement report:\r\n\r\n"
#define MENUSTATUS5       "\nThe digest value of the application image and the code certificate:\r\n"


extern bsp_leds_t g_bsp_leds;
extern fsp_err_t print_to_console(char * p_data);
extern void jlink_console_init (void);
char dbg_out_buffer[256];
void print_the_FSBL_registers(void);
void print_boot_measurement_report(void);
void print_digest(void);

void print_the_FSBL_registers(void)
{
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "FSBLCTRL0             : %08lx \r\n", R_OFS_DATAFLASH->FSBLCTRL0);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "FSBLCTRL1             : %08lx \r\n", R_OFS_DATAFLASH->FSBLCTRL1);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "FSBLCTRL2             : %08lx \r\n", R_OFS_DATAFLASH->FSBLCTRL2);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "SACC0                 : %08lx \r\n", R_OFS_DATAFLASH->SACC0);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "SACC1                 : %08lx \r\n", R_OFS_DATAFLASH->SACC1);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "SAMR                  : %08lx \r\n", R_OFS_DATAFLASH->SAMR);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "HOEMRTPK              : %08lx \r\n", R_OFS_DATAFLASH->HOEMRTPK);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "CFGDLOCK.CFGD0.CFGD_H : %08lx \r\n", R_OFS_DATAFLASH->CFGDLOCK.CFGD0.CFGD_H);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "CFGDLOCK.CFGD0.CFGD_H : %08lx \r\n", R_OFS_DATAFLASH->CFGDLOCK.CFGD0.CFGD_L);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "CFGDLOCK.CFGD1.CFGD_H : %08lx \r\n", R_OFS_DATAFLASH->CFGDLOCK.CFGD1.CFGD_H);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "CFGDLOCK.CFGD1.CFGD_H : %08lx \r\n", R_OFS_DATAFLASH->CFGDLOCK.CFGD1.CFGD_L);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "CFGDLOCK.CFGD2        : %08x \r\n", R_OFS_DATAFLASH->CFGDLOCK.CFGD2);
    print_to_console(dbg_out_buffer);
}

void print_boot_measurement_report(void)
{
    r_fsbl_measurement_report * const p_report = (r_fsbl_measurement_report * const)R_OFS_DATAFLASH->SAMR;
    uint32_t ram_start = 0x22000000; /*start of SRAM location */
    uint32_t ram_length = 0x000E0000; /*end of SRAM location */
    uint32_t ram_end = (uint32_t)(ram_start + ram_length);
    if (((uint32_t)p_report >= (uint32_t)ram_start) &&
            ((uint32_t)p_report < ram_end))
    {
        snprintf(dbg_out_buffer, sizeof(dbg_out_buffer),
         "p_report->sha256_oem_bl_fsblctrl1[0:15]:\r\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\r\n",
         p_report->sha256_oem_bl_fsblctrl1[0],
         p_report->sha256_oem_bl_fsblctrl1[1],
         p_report->sha256_oem_bl_fsblctrl1[2],
         p_report->sha256_oem_bl_fsblctrl1[3],
         p_report->sha256_oem_bl_fsblctrl1[4],
         p_report->sha256_oem_bl_fsblctrl1[5],
         p_report->sha256_oem_bl_fsblctrl1[6],
         p_report->sha256_oem_bl_fsblctrl1[7],
         p_report->sha256_oem_bl_fsblctrl1[8],
         p_report->sha256_oem_bl_fsblctrl1[9],
         p_report->sha256_oem_bl_fsblctrl1[10],
         p_report->sha256_oem_bl_fsblctrl1[11],
         p_report->sha256_oem_bl_fsblctrl1[12],
         p_report->sha256_oem_bl_fsblctrl1[13],
         p_report->sha256_oem_bl_fsblctrl1[14],
         p_report->sha256_oem_bl_fsblctrl1[15]);
        print_to_console(dbg_out_buffer);

        snprintf(dbg_out_buffer, sizeof(dbg_out_buffer),
        "p_report->sha256_oem_bl_fsblctrl1[16:31]:\r\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\r\n",
        p_report->sha256_oem_bl_fsblctrl1[  16  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  17  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  18  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  19  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  20  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  21  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  22  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  23  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  24  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  25  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  26  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  27  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  28  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  29  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  30  ]   ,
        p_report->sha256_oem_bl_fsblctrl1[  31  ]  );
        print_to_console(dbg_out_buffer);

        snprintf(dbg_out_buffer, sizeof(dbg_out_buffer),
        "p_report->signer_id[0:15]:\r\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\r\n",
        p_report->signer_id[0],
        p_report->signer_id[1],
        p_report->signer_id[2],
        p_report->signer_id[3],
        p_report->signer_id[4],
        p_report->signer_id[5],
        p_report->signer_id[6],
        p_report->signer_id[7],
        p_report->signer_id[8],
        p_report->signer_id[9],
        p_report->signer_id[10],
        p_report->signer_id[11],
        p_report->signer_id[12],
        p_report->signer_id[13],
        p_report->signer_id[14],
        p_report->signer_id[15]);
        print_to_console(dbg_out_buffer);

        snprintf(dbg_out_buffer, sizeof(dbg_out_buffer),
        "p_report->signer_id[16:31]:\r\n%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,\r\n",
        p_report->signer_id[  16  ]   ,
        p_report->signer_id[  17  ]   ,
        p_report->signer_id[  18  ]   ,
        p_report->signer_id[  19  ]   ,
        p_report->signer_id[  20  ]   ,
        p_report->signer_id[  21  ]   ,
        p_report->signer_id[  22  ]   ,
        p_report->signer_id[  23  ]   ,
        p_report->signer_id[  24  ]   ,
        p_report->signer_id[  25  ]   ,
        p_report->signer_id[  26  ]   ,
        p_report->signer_id[  27  ]   ,
        p_report->signer_id[  28  ]   ,
        p_report->signer_id[  29  ]   ,
        p_report->signer_id[  30  ]   ,
        p_report->signer_id[  31  ]  );
        print_to_console(dbg_out_buffer);

        snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "p_report->version_num_oem_bl:\r\n%04x \r\n", p_report->version_num_oem_bl);
       print_to_console(dbg_out_buffer);
    }
    else
    {

        snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%s", "FSBL Measurement Report not valid\r\n");
        print_to_console(dbg_out_buffer);
    }
}

void print_digest(void)
{
    uint32_t code_cert_length = sizeof(r_code_cert);
    r_oem_app_bl_digest * const p_digest = (r_oem_app_bl_digest * const)(R_OFS_DATAFLASH->SACC0 + code_cert_length);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
            p_digest->value[0],
            p_digest->value[1],
            p_digest->value[2],
            p_digest->value[3],
            p_digest->value[4],
            p_digest->value[5],
            p_digest->value[6],
            p_digest->value[7],
            p_digest->value[8],
            p_digest->value[9],
            p_digest->value[10],
            p_digest->value[11],
            p_digest->value[12],
            p_digest->value[13],
            p_digest->value[14],
            p_digest->value[15],
            p_digest->value[16],
            p_digest->value[17],
            p_digest->value[18],
            p_digest->value[19],
            p_digest->value[20],
            p_digest->value[21],
            p_digest->value[22],
            p_digest->value[23],
            p_digest->value[24],
            p_digest->value[25],
            p_digest->value[26],
            p_digest->value[27],
            p_digest->value[28],
            p_digest->value[29],
            p_digest->value[30],
            p_digest->value[31]
        );

    print_to_console(dbg_out_buffer);

}
/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry (void)
{
#if BSP_TZ_SECURE_BUILD

    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif

    /* Define the units to be used with the software delay function */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

    /* Set the blink frequency (must be <= bsp_delay_units */
    const uint32_t freq_in_hz = 2;

    /* Calculate the delay in terms of bsp_delay_units */
    const uint32_t delay = bsp_delay_units / freq_in_hz;

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

    jlink_console_init ();
    /* Holds level to set for pins */
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%s\r\n", MENUSTATUS1);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%s\r\n", MENUSTATUS2);
    print_to_console(dbg_out_buffer);
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%s\r\n", MENUSTATUS3);
    print_to_console(dbg_out_buffer);
    print_the_FSBL_registers();
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%s\r\n", MENUSTATUS4);
    print_to_console(dbg_out_buffer);
    print_boot_measurement_report();
    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%s\r\n", MENUSTATUS5);
    print_to_console(dbg_out_buffer);
    print_digest();

    while (true)
    {
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

        /* Delay */
        R_BSP_SoftwareDelay(delay, bsp_delay_units);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        IOPORT_CFG_OPEN(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}
