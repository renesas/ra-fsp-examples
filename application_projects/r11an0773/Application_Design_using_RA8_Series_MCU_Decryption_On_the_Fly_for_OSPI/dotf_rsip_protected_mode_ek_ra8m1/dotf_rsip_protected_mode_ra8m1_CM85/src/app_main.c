/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"

#include "jlink_console.h"

#include "app_common_util.h"
#include "app_dwt.h"
#include "app_ospi_operations.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define LED1_ON                                 (R_PORT6->PCNTR3_b.POSR = (1 << 0))
#define LED1_OFF                                (R_PORT6->PCNTR3_b.PORR = (1 << 0))

#define CPU_CLOCK_IN_MHZ                        (R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)/1000000)
#define NUM_ITERACTIONS_TO_EVALUATE             (6U)

#define ENCRYPTION_KEY_LENGTH                   (0x100)

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static volatile uint32_t cycle_count = 0;
static volatile uint32_t exec_time_ns = 0;

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

const uint8_t encryption_key[0x100] BSP_PLACE_IN_SECTION(".data_flash") __attribute__((aligned(4)));
const uint32_t encryption_iv[] BSP_ALIGN_VARIABLE(4) = {0x00000000, 0x00000000, 0x00000000, 0x09000000};

void app_main(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable the DWT */
    InitCycleCounter();
    EnableCycleCounter();

    /* Initialize the jlink virtual console */
    err = jlink_console_init();
    if (FSP_SUCCESS != err)
    {
        app_handle_error(err, APP_JLINK_CONSOLE_INIT);
    }

    /* Initialize the OSPI with DOTF enabled */
    err = ospi_b_init();
    if (FSP_SUCCESS != err)
    {
        app_handle_error(err, APP_OSPI_INIT);
    }

    for (uint32_t i = 0; i < NUM_ITERACTIONS_TO_EVALUATE; i++)
    {
        /* Execution using plaintext OSPI code */
        ResetCycleCounter();
        execute_plaintext_fun_ospi();
        cycle_count = GetCycleCounter();
        exec_time_ns = MULTIPICATION_FACTOR_TO_NANOSECOND * cycle_count  / CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

#if (BSP_CFG_DCACHE_ENABLED)
        SCB_CleanDCache();
#endif
        sprintf (sprintf_buffer, "\r\nProtected Mode: Time used in executing the plaintext Fibonacci calculation function from OSPI without DOTF: %u nanoseconds; \r\n", (unsigned int)exec_time_ns);
        jlink_print_to_console(sprintf_buffer);

        /* Execution using encrypted OSPI code */
        ResetCycleCounter();
        execute_encrypted_func_ospi();
        cycle_count = GetCycleCounter();
        exec_time_ns = MULTIPICATION_FACTOR_TO_NANOSECOND * cycle_count  / CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

#if (BSP_CFG_DCACHE_ENABLED)
        SCB_CleanDCache();
#endif
        sprintf (sprintf_buffer, "\r\nProtected Mode: Time used in executing the encrypted Fibonacci calculation function from OSPI using DOTF: %u nanoseconds; \r\n", (unsigned int)exec_time_ns);
        jlink_print_to_console(sprintf_buffer);
    }

    sprintf(sprintf_buffer, "\r\nDOTF RSIP protected mode evaluation is successful. \r\n");
    jlink_print_to_console(sprintf_buffer);

    while (1)
    {
        LED1_ON;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
        LED1_OFF;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    }


}
