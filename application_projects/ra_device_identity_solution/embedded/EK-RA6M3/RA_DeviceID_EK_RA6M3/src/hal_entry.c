/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : warm start up and copy the secure SRAM program to destination.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "hal_data.h"
#include "common_util.h"

#if defined(__GNUC__)
/*LDRA_INSPECTED 219 S Linker sections start with underscore. */
extern uint32_t __SECURE_ROM_End;
/*LDRA_INSPECTED 219 S Linker sections start with underscore. */
extern uint32_t __secure_ram_end__;
/*LDRA_INSPECTED 219 S Linker sections start with underscore. */
extern uint32_t __secure_ram_start__;
#endif
/* Function declaration */
void R_BSP_WarmStart (bsp_warm_start_event_t event) __attribute__((section(".secure_code")));

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {
	/* TODO: add your own code here */
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_RESET == event) {
        #if BSP_FEATURE_FLASH_LP_VERSION != 0

                /* Enable reading from data flash. */
                R_FACI_LP->DFLCTL = 1U;

                /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
                 * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
        #endif
	}

    if (BSP_WARM_START_POST_CLOCK == event)
    {
#if 0
        uint32_t prc0;
        /* C runtime environment has not been setup so you cannot use globals. System clocks and pins are not setup. */
        prc0 = R_SYSTEM->PRCR;
        /* Enable Register access */
        R_SYSTEM->PRCR = (uint16_t) ((R_SYSTEM->PRCR | PRCR_KEY) | PRC0_MASK);
        /* Enable the trace clock, using 1/4 divisor - gives TRCLK 1/2 ICLK */
        /* As the PLL is configured to 240Mhz for S5D9 device by SSP (to be able to generate the USB clock) */
        /* but TRCLK max freq = 60Mhz. Changing at runtime works for ITM printf, not ITM trace */
        TRCKCR = (TRCK_4 | TRCKEN);

        /* Restore previous PRC0 setting */
        R_SYSTEM->PRCR = (uint16_t) (((R_SYSTEM->PRCR | PRCR_KEY) & ~PRC0_MASK) | prc0);
#endif

        #if defined(__GNUC__)
              /* Copy the flash driver .code_in_ram region */
                uint32_t index, bytes = ((uint32_t)&__secure_ram_end__ - (uint32_t)&__secure_ram_start__);
                uint8_t *pdest = (uint8_t *)&__secure_ram_start__;
                uint8_t *psource = (uint8_t *)&__SECURE_ROM_End;

                for (index = 0U; index < bytes; index++,pdest++,psource++)
                {
                    *pdest = *psource;
                }
        #endif
    }

	if (BSP_WARM_START_POST_C == event) {

        /* C runtime environment and system clocks are setup. */

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}
