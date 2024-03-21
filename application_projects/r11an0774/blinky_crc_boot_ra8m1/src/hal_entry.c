/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include "hal_data.h"
#include <stdio.h>
#include <string.h>
#include "common_definitions.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);


#define MENUSTATUS1       "\nRunning the blinky application (FSBL enabled with CRC verification). \r\n"
#define MENUSTATUS2       "The Red, Blue and Green LEDs should be blinking.\r\n"
#define MENUSTATUS3       "\nThe current setting of the FSBL registers: \r\n\r\n"
#define MENUSTATUS4       "\nThe current output of the boot measurement report:\r\n\r\n"
#define MENUSTATUS5       "\nThe CRC value of the application image:\r\n"

#define BYTE_LOCATION_CRC_VALUE     (U108)


extern bsp_leds_t g_bsp_leds;
extern fsp_err_t print_to_console(char * p_data);
extern void jlink_console_init (void);
char dbg_out_buffer[256];
void print_the_FSBL_registers(void);
void print_boot_measurement_report(void);
void print_crc(void);

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
    uint32_t ram_start = 0x22000000;
    uint32_t ram_length = 0x000E0000;
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

void print_crc(void)
{
    r_code_cert * const p_code_cert = (r_code_cert * const)R_OFS_DATAFLASH->SACC0;

    snprintf(dbg_out_buffer, sizeof(dbg_out_buffer), "%02x%02x%02x%02x\r\n",
                p_code_cert->tlv_crc_signer_id[4],
                p_code_cert->tlv_crc_signer_id[5],
                p_code_cert->tlv_crc_signer_id[6],
                p_code_cert->tlv_crc_signer_id[7]);

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
    print_crc();

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
