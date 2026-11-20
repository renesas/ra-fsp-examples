/***********************************************************************************************************************
 * File Name    : sdram_example_app.c
 * Description  : Example application layer for the SDRAM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sdram_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdram_ep.h"
#include "sdram_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static uint32_t s_buf_write[BLOCK_DATA_SIZE] BSP_ALIGN_VARIABLE(32) = {RESET_VALUE};
static uint32_t s_buf_read[BLOCK_DATA_SIZE] BSP_ALIGN_VARIABLE(32) = {RESET_VALUE};
static uint32_t s_buf_sdram[SDRAM_SIZE / 4] BSP_ALIGN_VARIABLE(32) BSP_PLACE_IN_SECTION(".sdram") = {RESET_VALUE};

/* Private functions */
static fsp_err_t sdram_transmission(uint32_t start_addr, sdram_trans_dir_t dir);
static fsp_err_t sdram_verify_data(void);
static fsp_err_t sdram_self_refresh(void);
static void make_buffer(uint32_t start_addr);
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: sdram_example_app_run
 * Description  : Single entry-point for the SDRAM example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void sdram_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Turn OFF the LEDs at startup */
    led_error_state_set(LED_POWER_OFF);
    led_lpm_state_set(LED_POWER_OFF);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        /* Turn ON the error LED to indicate that an error has occurred */
        led_error_state_set(LED_POWER_ON);

        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize ULPT timer to wake up MCU to normal mode */
    err = timer_init();
    handle_error(err, "timer_init failed\r\n");

    /* Initialize the DMAC module for SDRAM transmission */
    err = dmac_init();
    handle_error(err, "dmac_init failed\r\n");

    /* Write to entire SDRAM */
    printf("Writing to the entire SDRAM...\r\n");

    for (uint32_t i = 0; i < (SDRAM_SIZE / NUM_ADDR_FOR_BLOCK); ++i)
    {
        /* Set start address for the transfer block */
        uint32_t block_start_addr = SDRAM_START_ADDR + (NUM_ADDR_FOR_BLOCK * i);

        /* Perform SDRAM write operation */
        err = sdram_transmission(block_start_addr, SDRAM_TRANS_DIR_WRITE);
        handle_error(err, "sdram_transmission failed\r\n");
    }

    /* Verify written data */
    err = sdram_verify_data();
    handle_error(err, "sdram_verify_data failed\r\n");

    printf(CTRL_TEXT_BRIGHT_GREEN "SDRAM transfer completed successfully\r\n\r\n" CTRL_RESET);

    /* Perform SDRAM self-refresh operation */
    err = sdram_self_refresh();
    handle_error(err, "sdram_self_refresh failed\r\n");

    printf(CTRL_TEXT_BRIGHT_GREEN "SDRAM data is fully retained\r\n" CTRL_RESET);

    printf("\r\nPlease reset the MCU to restart the example project\r\n");

    /* De-initialize the opened modules */
    dmac_deinit();
    timer_deinit();
}
/***********************************************************************************************************************
* End of function sdram_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sdram_transmission
 * Description  : This function performs an SDRAM data transfer.
 * Arguments    : start_addr                Start address of the transfer block.
 *                dir                       Transfer direction.
 * Return Value : FSP_SUCCESS               Transferred successfully.
 *                Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static fsp_err_t sdram_transmission(uint32_t start_addr, sdram_trans_dir_t dir)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t start_value = ((start_addr - SDRAM_START_ADDR) / DATA_SIZE);
    uint32_t timeout = BLOCK_DATA_SIZE;

    if (SDRAM_TRANS_DIR_WRITE == dir)
    {
        /* Fill the write buffer based on the start address */
        make_buffer(start_addr);

#if (BSP_CFG_DCACHE_ENABLED)
        /* Clean cache before executing any transmission to ensure that DMAC can see the data we initialized */
        SCB_CleanDCache_by_Addr(s_buf_write, sizeof(s_buf_write));
#endif /* BSP_CFG_DCACHE_ENABLED */

        /* Set the information of DMAC module for SDRAM write operation */
        err = dmac_reset(&g_sdram_transmission_ctrl, (void*) s_buf_write, (void*) &s_buf_sdram[start_value],
                         BLOCK_DATA_SIZE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else if (SDRAM_TRANS_DIR_READ == dir)
    {
#if (BSP_CFG_DCACHE_ENABLED)
        /* Invalidate the destination buffer before starting DMAC to prevent
         * dirty or stale cache lines from overwriting data written by DMAC */
        SCB_InvalidateDCache_by_Addr(s_buf_read, sizeof(s_buf_read));
#endif /* BSP_CFG_DCACHE_ENABLED */

        /* Set the information of DMAC module for SDRAM read operation */
        err = dmac_reset(&g_sdram_transmission_ctrl, (void*) &s_buf_sdram[start_value], (void*) s_buf_read,
                         BLOCK_DATA_SIZE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Clear the transfer complete flag */
    g_sdram_transmission_complete = false;

    /* Start transfer operation */
    err = dmac_software_start();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for transfer complete */
    while (!g_sdram_transmission_complete && --timeout)
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }
    if (RESET_VALUE == timeout)
    {
        return FSP_ERR_TIMEOUT;
    }

#if (BSP_CFG_DCACHE_ENABLED)
    if (SDRAM_TRANS_DIR_READ == dir)
    {
        /* Invalidate the destination buffer after DMAC completes so that
         * subsequent CPU reads access the data updated by DMAC */
        SCB_InvalidateDCache_by_Addr(s_buf_read, sizeof(s_buf_read));
    }
#endif /* BSP_CFG_DCACHE_ENABLED */

    return err;
}
/***********************************************************************************************************************
* End of function sdram_transmission
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sdram_verify_data
 * Description  : This function verifies the expected data in SDRAM.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Verified successfully.
 *                Any other error codes     Failed to verify.
 **********************************************************************************************************************/
static fsp_err_t sdram_verify_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    printf("Verifying SDRAM data...\r\n");

    for (uint32_t i = 0; i < SDRAM_SIZE / NUM_ADDR_FOR_BLOCK; ++i)
    {
        uint32_t block_start_addr = SDRAM_START_ADDR + (NUM_ADDR_FOR_BLOCK * i);

        /* Fill the write buffer with data based on the block start address */
        make_buffer(block_start_addr);

        /* Read data from SDRAM into the read buffer */
        err = sdram_transmission(block_start_addr, SDRAM_TRANS_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

#if (BSP_CFG_DCACHE_ENABLED)
        /* Invalidate cache to reflect data from memory to cache before checking the data */
        SCB_InvalidateDCache_by_Addr(s_buf_read, sizeof(s_buf_read));
#endif /* BSP_CFG_DCACHE_ENABLED */

        /* Compare the write and read buffers */
        int ret = memcmp(s_buf_write, s_buf_read, BLOCK_DATA_SIZE);
        if (RESET_VALUE != ret)
        {
            return FSP_ERR_INVALID_DATA;
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function sdram_verify_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sdram_self_refresh
 * Description  : This function performs the SDRAM self-refresh operation.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t sdram_self_refresh(void)
{
    fsp_err_t err = FSP_SUCCESS;

    printf("Enabling SDRAM self-refresh before entering LPM SW Standby mode\r\n\r\n");
    printf("Entering LPM SW Standby mode...\r\n");

    /* Enable SDRAM self-refresh */
    R_BSP_SdramSelfRefreshEnable();

    /* Start ULPT timer */
    err = timer_start();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enter LPM mode */
    printf("Please wait 10 seconds to return to the normal mode\r\n");
    err = lpm_enter();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    printf(CTRL_TEXT_BRIGHT_GREEN "MCU returned to the normal mode\r\n\r\n" CTRL_RESET);

    /* Stop timer */
    err = timer_stop();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Disable SDRAM self-refresh */
    R_BSP_SdramSelfRefreshDisable();

    /* Verify written data */
    err = sdram_verify_data();

    return err;
}
/***********************************************************************************************************************
* End of function sdram_self_refresh
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: make_buffer
 * Description  : This function fills the write buffer with data based on the specified start address.
 * Arguments    : start_addr                Start address of the transfer block.
 * Return Value : None.
 **********************************************************************************************************************/
static void make_buffer(uint32_t start_addr)
{
    /* Calculate the initial data value for the transfer block */
    uint32_t start_value = ((start_addr - SDRAM_START_ADDR) / DATA_SIZE);
    for (uint32_t i = 0; i < BLOCK_DATA_SIZE; ++i)
    {
        s_buf_write[i] = start_value + i;
    }
}
/***********************************************************************************************************************
* End of function make_buffer
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        dmac_deinit();
        lpm_deinit();
        timer_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
