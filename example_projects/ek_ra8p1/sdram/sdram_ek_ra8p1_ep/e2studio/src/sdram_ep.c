/***********************************************************************************************************************
 * File Name    : sdram_ep.c
 * Description  : Contains data structures and functions
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#include "sdram_ep.h"

/* External variables */
extern bsp_leds_t g_bsp_leds;

/* Private function declarations */
static void led_error_state_set(led_power_t state);
static void led_lpm_state_set(led_power_t state);
static fsp_err_t sdram_verify_data(void);
static fsp_err_t enter_lpm(void);
static fsp_err_t sdram_self_refresh(void);
static void dmac_deinit(void);
static void lpm_deinit(void);
static void timer_deinit(void);
static void make_buffer(uint32_t start_addr);
static fsp_err_t sdram_transmission(uint32_t start_addr, sdram_trans_dir_t dir);

/* Public function declaration */
void handle_error(fsp_err_t err, uint8_t * err_str);

/* Private Variables */
static uint32_t p_buf_write[BLOCK_DATA_SIZE] BSP_ALIGN_VARIABLE(32) = {RESET_VALUE};
static uint32_t p_buf_read[BLOCK_DATA_SIZE] BSP_ALIGN_VARIABLE(32) = {RESET_VALUE};
static volatile _Bool g_sdram_transmission_complete = false;

static uint32_t g_buf_sdram[SDRAM_SIZE/4] BSP_ALIGN_VARIABLE(32) BSP_PLACE_IN_SECTION(".sdram") = {RESET_VALUE};

/***********************************************************************************************************************
 *  Function Name: sdram_entry
 *  Description  : This function is used to start the SDRAM example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void sdram_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version          = { RESET_VALUE };

    /* Turn off LEDs state at start up */
    led_error_state_set(LED_POWER_OFF);
    led_lpm_state_set(LED_POWER_OFF);

    /* Initialize UART module first to print log to serial terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn on error LED to indicate an error has occurred */
        led_error_state_set(LED_POWER_ON);

        /* Error trap */
        ERROR_TRAP;
    }

    /* Version get API for FSP information */
    R_FSP_VersionGet (&version);

    /* Print the EP banner on the terminal */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Print the EP information on the terminal */
    APP_PRINT(EP_INFO);

    /* Initialize ULPT timer to wake up MCU to normal mode */
    err = R_ULPT_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
    APP_ERR_HANDLE(err, "**R_ULPT_Open API failed**\r\n");

    /* Initialize the DMAC module for SDRAM transmission */
    err = R_DMAC_Open(&g_sdram_transmission_ctrl, &g_sdram_transmission_cfg);
    APP_ERR_HANDLE(err,"**R_DMAC_Open API failed**\r\n");

    /* Write to entire SDRAM */
    APP_PRINT("Writing to the entire SDRAM...\r\n");

    for (uint32_t i = 0; i < (SDRAM_SIZE / NUM_ADDR_FOR_BLOCK); ++i)
    {
        /* Set start address for the transfer block */
        uint32_t block_start_addr = SDRAM_START_ADDR + (NUM_ADDR_FOR_BLOCK * i);

        /* Perform SDRAM write operation */
        err = sdram_transmission(block_start_addr, WRITE);
        APP_ERR_HANDLE(err, "sdram_transmission failed\r\n");
    }

    /* Verify written data */
    err = sdram_verify_data();
    APP_ERR_HANDLE(err, "sdram_verify_data failed\r\n");

    APP_PRINT(CTRL_TEXT_BRIGHT_GREEN "SDRAM transfer operation successful\r\n\r\n" CTRL_RESET);

    /* Perform SDRAM self-refresh operation */
    err =  sdram_self_refresh();
    APP_ERR_HANDLE(err, "sdram_self_refresh failed\r\n");

    APP_PRINT(CTRL_TEXT_BRIGHT_GREEN "SDRAM data is fully retained!\r\n" CTRL_RESET);

    APP_PRINT("\r\nPlease reset the MCU to restart the Example Project\r\n");

    /* De-Initialize the DMAC module */
    err = R_DMAC_Close (&g_sdram_transmission_ctrl);
    APP_ERR_HANDLE(err,"\r\n**R_DMAC_Close API failed**\r\n");
}
/***********************************************************************************************************************
* End of function sdram_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: make_buffer
 *  Description  : This function makes a buffer with data depending on the corresponding to the addresses.
 *  Arguments    : start_addr      Start address of each transfer data block
 *  Return Value : None
 **********************************************************************************************************************/
static void make_buffer(uint32_t start_addr)
{
    /* Convert start address to start value of each block transfer */
    uint32_t start_value = ((start_addr - SDRAM_START_ADDR)/ DATA_SIZE);
    for (uint32_t i = 0 ; i < BLOCK_DATA_SIZE; ++i)
    {
        p_buf_write[i] = start_value + i;
    }
}
/***********************************************************************************************************************
* End of function make_buffer
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sdram_transmission
 *  Description  : This function performs SDRAM transmission.
 *  Arguments    : start_addr      Start address of each transfer data block
 *                 dir             Direction of transmission
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t sdram_transmission(uint32_t start_addr, sdram_trans_dir_t dir)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t start_value = ((start_addr - SDRAM_START_ADDR)/ DATA_SIZE);
    uint32_t timeout = BLOCK_DATA_SIZE;

    if (WRITE == dir)
    {
        /* Create the buffer corresponding to the start address */
        make_buffer(start_addr);

#ifdef BSP_CFG_DCACHE_ENABLED
        /* Clean cache before executing any transmission to ensure that DMAC can see the data we initialized. */
        SCB_CleanDCache_by_Addr(p_buf_write, BLOCK_DATA_SIZE);
#endif /* BSP_CFG_DCACHE_ENABLED */

        /* Set the information of DMAC module for SDRAM write operation */
        err = R_DMAC_Reset(&g_sdram_transmission_ctrl, (void*) p_buf_write, (void*) &g_buf_sdram[start_value], BLOCK_DATA_SIZE);
        APP_ERR_RET(err != FSP_SUCCESS,err, "**R_DMAC_Reset API failed**\r\n");
    }

    else /* READ == dir */
    {
        /* Clean buffer p_buf_read before read data from SDRAM */
        memset(p_buf_read, RESET_VALUE, sizeof(p_buf_read));

        /* Set the information of DMAC module for SDRAM read operation */
        err = R_DMAC_Reset(&g_sdram_transmission_ctrl, (void*) &g_buf_sdram[start_value], (void*) p_buf_read , BLOCK_DATA_SIZE);
        APP_ERR_RET(err != FSP_SUCCESS,err, "**R_DMAC_Reset API failed**\r\n");
    }

    /* Clear the transfer complete flag */
    g_sdram_transmission_complete = false;

    /* Start transfer operation */
    err = R_DMAC_SoftwareStart (&g_sdram_transmission_ctrl, TRANSFER_START_MODE_REPEAT);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_DMAC_SoftwareStart API failed**\r\n");

    /* Wait for transfer complete */
    while (!g_sdram_transmission_complete && --timeout)
    {
        R_BSP_SoftwareDelay (1, BSP_DELAY_UNITS_MICROSECONDS);
    }
    if (RESET_VALUE == timeout)
    {
        APP_ERR_RET(FSP_ERR_TIMEOUT != FSP_SUCCESS,FSP_ERR_TIMEOUT, "\r\nWrite operation timeout\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function sdram_transmission
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sdram_verify_data
 *  Description  : This function is used to verify the expected data in SDRAM.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t sdram_verify_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    APP_PRINT("Verifying SDRAM data...\r\n");

    for (uint32_t i = 0; i < SDRAM_SIZE / NUM_ADDR_FOR_BLOCK; ++i)
    {
        uint32_t block_start_addr = SDRAM_START_ADDR + (NUM_ADDR_FOR_BLOCK * i);

        /* Create the buffer corresponding to the addresses and store in p_buf_write */
        make_buffer(block_start_addr);

        /* Read data from SDRAM and store in p_buf_read */
        err = sdram_transmission(block_start_addr, READ);
        APP_ERR_RET(err != FSP_SUCCESS,err, "sdram_transmission failed\r\n");

#ifdef BSP_CFG_DCACHE_ENABLED
        /* Invalidate cache to reflect data from memory to cache before checking the data. */
        SCB_InvalidateDCache_by_Addr(&p_buf_read, BLOCK_DATA_SIZE);
#endif /* BSP_CFG_DCACHE_ENABLED */

        /* Compare data of p_buf_write with p_buf_read */
        int ret = memcmp (&p_buf_write, &p_buf_read, BLOCK_DATA_SIZE);
        if(RESET_VALUE != ret)
        {
            APP_ERR_RET(FSP_ERR_INVALID_DATA != FSP_SUCCESS,FSP_ERR_INVALID_DATA,"Data is mismatch\r\n");
        }
    }
    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function sdram_verify_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: led_lpm_state_set
 *  Description  : This function sets the state of the LPM state LED.
 *  Arguments    : state           LED state want to set
 *  Return Value : None
 **********************************************************************************************************************/
static void led_lpm_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}
/***********************************************************************************************************************
* End of function led_lpm_state_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: led_error_state_set
 *  Description  : This function sets the state of the Error LED.
 *  Arguments    : state           LED state want to set
 *  Return Value : None
 **********************************************************************************************************************/
static void led_error_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ERROR_STATE], (bsp_io_level_t)state);
    }
}
/***********************************************************************************************************************
* End of function led_error_state_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sdram_self_refresh
 *  Description  : This function performs SDRAM self-refresh operation.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t sdram_self_refresh(void)
{
    fsp_err_t err = FSP_SUCCESS;

    APP_PRINT("Enable SDRAM self-refresh before entering LPM SW standby mode\r\n\r\n");

    APP_PRINT("Entering LPM SW standby mode...\r\n");

    /* Enable SDRAM self-refresh */
    R_BSP_SdramSelfRefreshEnable();

    /* Start ULPT timer */
    err = R_ULPT_Start(&g_timer_cancel_lpm_ctrl);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_ULPT_Start API failed**\r\n");

    /* Enter LPM mode */
    err = enter_lpm();
    APP_ERR_RET(err != FSP_SUCCESS,err, "erter_lpm failed\r\n");

    /* Stop timer */
    err = R_ULPT_Stop(&g_timer_cancel_lpm_ctrl);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_ULPT_Stop API failed**\r\n");

    /* Disable SDRAM self-refresh */
    R_BSP_SdramSelfRefreshDisable();

    /* Verify written data */
    err = sdram_verify_data();
    APP_ERR_RET(err != FSP_SUCCESS,err, "sdram_verify_data failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function sdram_self_refresh
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: enter_lpm
 *  Description  : This function performs a procedure when MCU enters LPM mode.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t enter_lpm(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM sw standby mode */
    err = R_LPM_Open(&g_sw_standby_ctrl, &g_sw_standby_cfg);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_LPM_Open failed**\r\n");

    /* Turn ON LPM state LED in one second */
    led_lpm_state_set(LED_POWER_ON);
    R_BSP_SoftwareDelay(LED_LPM_LIGHT_TIME, BSP_DELAY_UNITS_SECONDS);

    /* Turn OFF led before enter LPM mode */
    led_lpm_state_set(LED_POWER_OFF);

    APP_PRINT("Please wait 10 seconds to return to the normal mode\r\n");

    /* Delay to ensure message was printed before enter LPM mode */
    R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

#if defined(BOARD_RA8D1_EK)  || defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK)
    /* SCI UART module Stop with Undefined in SW Standby mode */
    TERM_DEINIT();
#endif

    /* Enter LPM SW standby mode */
    err = R_LPM_LowPowerModeEnter(&g_sw_standby_ctrl);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_LPM_LowPowerModeEnter failed**\r\n");

#if defined(BOARD_RA8D1_EK)  || defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK)
    /* Re-initialize SCI UART after SW Standby Mode */
    TERM_INIT();
#endif

    /* Turn ON LPM led when MCU is returned to the normal mode */
    led_lpm_state_set(LED_POWER_ON);

    /* De-initialize LPM SW standby mode */
    err = R_LPM_Close(&g_sw_standby_ctrl);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_LPM_Close failed**\r\n");

    APP_PRINT("MCU returned to the normal mode\r\n\r\n");
//    APP_PRINT(CTRL_TEXT_BRIGHT_GREEN "MCU returned to the normal mode\r\n\r\n" CTRL_RESET);
    return err;
}
/***********************************************************************************************************************
* End of function enter_lpm
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: transmission_sdram_callback
 *  Description  : This function is a callback function of SDRAM transmission.
 *  Arguments    : p_args
 *  Return Value : None
 **********************************************************************************************************************/
void transmission_sdram_callback(dmac_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Set the transmission complete flag */
        g_sdram_transmission_complete = true;
    }

}
/***********************************************************************************************************************
* End of function transmission_sdram_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: dmac_deinit
 *  Description  : This function closes opened DMAC module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void dmac_deinit(void)
{
    /* De-initialize DMAC module if it opened */
    if (MODULE_CLOSE != g_sdram_transmission_ctrl.open)
    {
        if(FSP_SUCCESS != R_DMAC_Close(&g_sdram_transmission_ctrl))
        {
            APP_ERR_PRINT("\r\n**R_DMAC_Close API failed**\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function dmac_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: lpm_deinit
 *  Description  : This function closes opened LPM module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void lpm_deinit(void)
{
    /* De-initialize LPM module if it opened */
    if (MODULE_CLOSE != g_sw_standby_ctrl.lpm_open)
    {
        if(FSP_SUCCESS != R_LPM_Close(&g_sw_standby_ctrl))
        {
            APP_ERR_PRINT("\r\n**R_LPM_Close API failed**\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function lpm_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: timer_deinit
 *  Description  : This function closes opened timer module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void timer_deinit(void)
{
    /* De-initialize ULPT module if it opened */
    if (MODULE_CLOSE != g_timer_cancel_lpm_ctrl.open)
    {
        if(FSP_SUCCESS != R_ULPT_Close(&g_timer_cancel_lpm_ctrl))
        {
            APP_ERR_PRINT("\r\n**R_ULPT_Close API failed**\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function timer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  Arguments    : err             error status
 *                 err_str         error string
 *  Return Value : None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, uint8_t * err_str)
{
    /* Print error information and error code */
    APP_ERR_PRINT(err_str);

    /* De-initialize opened DMAC module */
    dmac_deinit();

    /* De-initialize opened LPM module */
    lpm_deinit();

    /* De-initialize opened timer module */
    timer_deinit();

    /* Turn on error LED to indicate an error has occurred */
    led_error_state_set(LED_POWER_ON);

    /* Trap the error */
    APP_ERR_TRAP(err);
}


/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
