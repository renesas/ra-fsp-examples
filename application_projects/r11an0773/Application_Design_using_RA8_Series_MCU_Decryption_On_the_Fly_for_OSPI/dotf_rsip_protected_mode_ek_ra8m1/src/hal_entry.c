
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "ospi_operations.h"
#include "hal_data.h"
#include "dwt.h"
#include "common_util.h"
#include "jlink_console.h"
#include "ospi_operations.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define CPU_CLOCK_IN_MHZ                        (R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)/1000000)
#define LED1_ON     R_PORT6->PCNTR3_b.POSR = (1 << 0)
#define LED1_OFF    R_PORT6->PCNTR3_b.PORR = (1 << 0)
#define NUM_ITERACTIONS_TO_EVALUATE              (6U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
const uint8_t encryption_key[0x100] BSP_PLACE_IN_SECTION(".data_flash") __attribute__((aligned(4)));
const uint32_t encryption_iv[] __attribute__((aligned(4))) = {0x00000000, 0x00000000, 0x00000000, 0x09000000};


/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static volatile uint32_t cycle_count = 0;
static volatile uint32_t cycle_count2 = 0;


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = 0;

    err = jlink_console_init();
    handle_error(err, APP_JLINK_CONSOLE_INIT);

    /*Initialize the OSPI with DOTF enabled */
    ospi_init();

    InitCycleCounter();
    EnableCycleCounter();


    for (uint32_t i=0; i<NUM_ITERACTIONS_TO_EVALUATE; i++)
    {

        /* execution using plaintext OSPI code */
        ResetCycleCounter();
        execute_plaintext_fun_ospi();
        cycle_count = GetCycleCounter();

        cycle_count2 = MULTIPICATION_FACTOR_TO_NANOSECOND*cycle_count/CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        SCB_CleanDCache();
        sprintf (sprintf_buffer, "\r\nProtected Mode: Time used in executing the plaintext Fibonacci calculation function from OSPI without DOTF: %u nanoseconds; \r\n", (unsigned int)cycle_count2);
                                         print_to_console(sprintf_buffer);

        /* execution using encrypted OSPI code */
        ResetCycleCounter();
        execute_encrypted_func_ospi();
        cycle_count = GetCycleCounter();

        cycle_count2 = MULTIPICATION_FACTOR_TO_NANOSECOND*cycle_count/CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        SCB_CleanDCache();
        sprintf (sprintf_buffer, "\r\nProtected Mode: Time used in executing the encrypted Fibonacci calculation function from OSPI using DOTF: %u nanoseconds; \r\n", (unsigned int)cycle_count2);
        print_to_console(sprintf_buffer);
    }


    sprintf(sprintf_buffer, "\r\nDOTF RSIP protected mode evaluation is successful. \r\n");
    print_to_console(sprintf_buffer);

    while(true)
    {
        LED1_ON;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
        LED1_OFF;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

    };


}


/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
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

        /// ==== Reset external OSPI memory ====
        /// To ensure that the external memory starts from initial state,
        /// apply a pin reset before setting the reset port as a peripheral function pin.
        /// This must be done if the OSPI was left in 8D-8D-8D state from previous operations.
        /* Setup pin config for memory reset pin*/
        R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06,
                        ((uint32_t) IOPORT_CFG_NMOS_ENABLE |
                         (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                         (uint32_t) IOPORT_CFG_PORT_OUTPUT_HIGH));

        /* Assert reset the external OSPI memory */
        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_LOW);

        /* Delay 1ms */
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

        /* Dessert reset the external OSPI memory */
        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_HIGH);
        /// ==== END of external memory reset ====

        /* Configure pins. */
        IOPORT_CFG_OPEN (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}