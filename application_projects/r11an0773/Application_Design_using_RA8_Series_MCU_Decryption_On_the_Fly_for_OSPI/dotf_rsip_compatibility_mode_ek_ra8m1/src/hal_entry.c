
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "hal_data.h"
#include "dwt.h"
#include "stdio.h"
#include "common_util.h"
#include "ospi_operations.h"
#include "runtime_encryption.h"
#include "jlink_console.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define LED1_ON     R_PORT6->PCNTR3_b.POSR = (1 << 0)
#define LED1_OFF    R_PORT6->PCNTR3_b.PORR = (1 << 0)
#define OSPI_B_TIME_ERASE_4K                (100000U)        //  Type 4KB sector is 95 KBps -> Type 0.042s
#define OSPI_B_TIME_WRITE                   (10000U)
#define OSPI_B_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_PULSE             (1000U)          //  Type 500us
#define OSPI_B_TIME_RESET_SETUP             (2U)             //  Type 50ns
#define NUM_ITERACTIONS_TO_EVALUATE         (6U)
#define CPU_CLOCK_IN_MHZ                        (R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)/1000000)

/***********************************************************************************************************************
 * Private global variables and functions
 ************************************************************************************************************************/
static uint8_t __attribute__((aligned(8))) ospi_read_data[ENCRYPTED_DATA_SIZE] = {0};

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = 0;
    volatile uint32_t cycle_count = 0;
    bool mbedtls_init = false;

    /* enable the DWT */
    InitCycleCounter();
    EnableCycleCounter();

    /*initialize the jlink virtual console */
    err = jlink_console_init();
    handle_error(err, APP_JLINK_CONSOLE_INIT);

    /* Initialize mbedtls crypto and generate wrapped AES key */
    mbedtls_init = init_mbedtls_crypto();
    if(false == mbedtls_init)
    {
        handle_error(APP_ERR, APP_WRAPPED_KEY_GEN);
    }

    /* Generate Runtime Encrypted OSPI Data */
    AES_CTR_encryption();
    /*Initialize the OSPI with DOTF enabled */
    ospi_init();

    /*Write the Encrypted OSPI Data*/
    write_encrypted_data_to_ospi();

    for (uint32_t i=0; i<NUM_ITERACTIONS_TO_EVALUATE; i++)
    {

        /* reset the buffer to read from the OSPI area */
        memset((void *)&ospi_read_data[0], 0xFF, PLAINTEXT_DATA_SIZE );
        /* access the plaintext data from OSPI and record the timing */
        ResetCycleCounter();
        memcpy((void *) &ospi_read_data[0], (void *) &(__ospi_device_1_plaintext_start__), PLAINTEXT_DATA_SIZE);
        cycle_count = GetCycleCounter();
        cycle_count = cycle_count/CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        SCB_CleanDCache();
        sprintf (sprintf_buffer, "\r\nCompatibility Mode: Time used to access the plaintext OSPI data without using DOTF: %u microseconds; \r\n", (unsigned int)cycle_count);
        print_to_console(sprintf_buffer);

        /* reset the buffer to read from the OSPI area */
        memset((void *)&ospi_read_data[0], 0xFF, ENCRYPTED_DATA_SIZE );
        /* access the encrypted data from OSPI and record the timing */
        ResetCycleCounter();
        memcpy((void *) &ospi_read_data[0], (void *) &(__ospi_device_1_start__), ENCRYPTED_DATA_SIZE);
        cycle_count = GetCycleCounter();
        cycle_count = cycle_count/CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        /* Read and verify the decrypted data */
        for(uint32_t index=0; index<ENCRYPTED_DATA_SIZE; index++)
        {
          if(*((uint8_t*) &(__ospi_device_1_start__) + index) != plaintext_data[index])
          {
              handle_error(APP_ERR, APP_OSPI_DECRYPTED_DATA_NOT_MATCH);
          }
        }

        SCB_CleanDCache();
        sprintf (sprintf_buffer, "\r\nCompatibility Mode: Time used to access the encrypted OSPI data using DOTF: %u microseconds; \r\n", (unsigned int)cycle_count);
        print_to_console(sprintf_buffer);

    }

    sprintf (sprintf_buffer, "\r\nDOTF RSIP Compatibility Mode evaluation finished successfully. \r\n");
    print_to_console(sprintf_buffer);

    while(1)
    {
        LED1_ON;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
        LED1_OFF;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    }


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