/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"

#include "jlink_console.h"

#include "app_common_util.h"
#include "app_dwt.h"
#include "app_runtime_encryption.h"
#include "app_ospi_operations.h"

#define LED1_ON                             (R_PORT6->PCNTR3_b.POSR = (1 << 0))
#define LED1_OFF                            (R_PORT6->PCNTR3_b.PORR = (1 << 0))

#define NUM_ITERACTIONS_TO_EVALUATE         (6U)
#define CPU_CLOCK_IN_MHZ                    (R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)/1000000)

static uint8_t BSP_ALIGN_VARIABLE(8) ospi_read_data[ENCRYPTED_DATA_SIZE] = {RESET_VALUE};

void app_main(void);

void app_main(void)
{
    fsp_err_t err = FSP_SUCCESS;
    psa_status_t psa_status = PSA_SUCCESS;

    volatile uint32_t cycle_count = 0;

    /* Enable the DWT */
    InitCycleCounter();
    EnableCycleCounter();

    /* Initialize the jlink virtual console */
    err = jlink_console_init();
    if (FSP_SUCCESS != err)
    {
        app_handle_error(err, APP_JLINK_CONSOLE_INIT);
    }

    /* Initialize mbedtls crypto and generate wrapped AES key */
    psa_status = init_mbedtls_crypto();
    if (PSA_SUCCESS != psa_status)
    {
        app_handle_error((uint32_t)psa_status, APP_WRAPPED_KEY_GEN);
    }

    /* Generate Runtime Encrypted OSPI Data */
    psa_status = AES_CTR_encryption();
    if (PSA_SUCCESS != psa_status)
    {
        app_handle_error((uint32_t)psa_status, APP_RUNTIME_ENCRYPTION);
    }

    /* Initialize the OSPI with DOTF enabled */
    err = ospi_b_init();
    if (FSP_SUCCESS != err)
    {
        app_handle_error(err, APP_OSPI_INIT);
    }

    /* Write the Encrypted Data to OSPI area */
    err = write_encrypted_data_to_ospi();
    if (FSP_SUCCESS != err)
    {
        app_handle_error(err, APP_WRITE_ENCRYPTED_DATA);
    }

    for (uint32_t i = 0; i < NUM_ITERACTIONS_TO_EVALUATE; i++)
    {
        /* Reset the buffer to read from the OSPI area */
        memset((void *)&ospi_read_data[0], 0xFF, PLAINTEXT_DATA_SIZE);

        /* Access the plaintext data from OSPI and record the timing */
        ResetCycleCounter();
        memcpy((void *) &ospi_read_data[0], (void *) (PLAINTEXT_START_ADDRESS), PLAINTEXT_DATA_SIZE);

        cycle_count = GetCycleCounter();
        cycle_count = cycle_count / CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

#if (BSP_CFG_DCACHE_ENABLED)
        SCB_CleanDCache();
#endif
        sprintf(sprintf_buffer, "\r\nCompatibility Mode: Time used to access the plaintext OSPI data without using DOTF: %u microseconds; \r\n", (unsigned int)cycle_count);
        jlink_print_to_console(sprintf_buffer);

        /* Reset the buffer to read from the OSPI area */
        memset((void *)&ospi_read_data[0], 0xFF, ENCRYPTED_DATA_SIZE);

        /* Access the encrypted data from OSPI and record the timing */
        ResetCycleCounter();
        memcpy((void *)&ospi_read_data[0], (void *)(ENCRYPTED_START_ADDRESS), ENCRYPTED_DATA_SIZE);

        cycle_count = GetCycleCounter();
        cycle_count = cycle_count / CPU_CLOCK_IN_MHZ;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        /* Read and verify the decrypted data */
        volatile uint8_t *p_encrypted_data = (uint8_t*)(ENCRYPTED_START_ADDRESS);
        volatile uint8_t *p_plaintext_data = (uint8_t*)(PLAINTEXT_START_ADDRESS);

        for (uint32_t index = 0; index < ENCRYPTED_DATA_SIZE; index++)
        {
            if (p_encrypted_data[index] != p_plaintext_data[index])
            {
                app_handle_error(APP_ERR_COMMON, APP_OSPI_DECRYPTED_DATA_NOT_MATCH);
            }
        }
#if (BSP_CFG_DCACHE_ENABLED)
        SCB_CleanDCache();
#endif
        sprintf(sprintf_buffer, "\r\nCompatibility Mode: Time used to access the encrypted OSPI data using DOTF: %u microseconds; \r\n", (unsigned int)cycle_count);
        jlink_print_to_console(sprintf_buffer);
    }

    sprintf (sprintf_buffer, "\r\nDOTF RSIP Compatibility Mode evaluation finished successfully. \r\n");
    jlink_print_to_console(sprintf_buffer);

    while (1)
    {
        LED1_ON;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
        LED1_OFF;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

