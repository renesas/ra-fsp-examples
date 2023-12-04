/***********************************************************************************************************************
 * File Name    : lpm_ep.c
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
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
 * Copyright (C) 2020-2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "source_cancel_lpm.h"
#include "lpm_ep.h"

/**********************************************************************************************************************
* External variables
**********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
/* LPM instances */
static lpm_instance_t const * g_lpm_instance [APP_LPM_MAX_STATE] =
{
     [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
     [APP_LPM_DEEP_SLEEP_STATE] = &g_lpm_deep_sleep,
#endif
     [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
     [APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE] = &g_lpm_sw_standby_with_snooze,
#endif
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
     [APP_LPM_DEEP_SW_STANDBY_STATE] = &g_lpm_deep_sw_standby,
#endif
};

/* LPM Mode name */
static const uint8_t * g_lpm_mode_name [APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE]                   = (uint8_t *)NAME_LPM_SLEEP_MODE,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    [APP_LPM_DEEP_SLEEP_STATE]              = (uint8_t *)NAME_LPM_DEEP_SLEEP_MODE,
#endif
    [APP_LPM_SW_STANDBY_STATE]              = (uint8_t *)NAME_LPM_SW_STANDBY_MODE,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
     [APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE] = (uint8_t *)NAME_LPM_SW_STANDBY_MODE_WITH_SNOOZE,
#endif
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    [APP_LPM_DEEP_SW_STANDBY_STATE]         = (uint8_t *)NAME_LPM_DEEP_SW_STANDBY_MODE,
#endif
};

/**********************************************************************************************************************
* Private function declarations
**********************************************************************************************************************/
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_exit_procedure(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_wait_start(void);
static fsp_err_t lpm_operation(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_ep_startup(void);
static fsp_err_t lpm_ep_main_process(void);
static void led_lpm_state_set(led_power_t state);
static void led_error_state_set(led_power_t state);
#if defined(SSRAM_START) && defined(SSRAM_LENGTH)
static bool standby_sram_write(uint32_t offset_addr, uint8_t * p_write_buf, uint32_t len);
static bool standby_sram_verify(uint32_t offset_addr, uint8_t * p_verify, uint32_t len);
#endif
static void handle_error(fsp_err_t err_value, uint8_t * err_func, uint8_t * err_str);

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/
void lpm_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Perform EP startup procedure */
    err = lpm_ep_startup();
    TERMINAL_ERR_HANDLE(err, "lpm_ep_startup Failed\r\n");

    /* Perform EP main procedure */
    while (true)
    {
        err = lpm_ep_main_process();
        TERMINAL_ERR_HANDLE(err, "lpm_ep_main_process Failed\r\n");
    }
}

static fsp_err_t lpm_ep_startup(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Turn off LEDs state at start up */
    led_lpm_state_set(LED_POWER_OFF);
    led_error_state_set(LED_POWER_OFF);

    /* Initialize SCI UART module first to print log to terminal */
    err = uart_init();
    TERMINAL_ERR_RETURN(err, "uart_init Failed\r\n");

    /* In case of internal reset */
    if (true == g_source_flag)
    {
        /* Print internal reset banner */
        TERMINAL_PRINT(BANNER_RESET);

        /* Detect source generates internal reset */
        source_cancel_lpm_detect();

        /* Clear all source detection flags */
        source_cancel_lpm_flag_clear();

        /* Verify data stored in Standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_verify(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            TERMINAL_ERR_RETURN(FSP_ERR_INVALID_DATA, "standby_sram_verify failed\r\n");
        }

        TERMINAL_PRINT("\r\nData stored in standby SRAM matches fixed data\r\n");
#endif
        /* Prints a new cycle start message */
        TERMINAL_PRINT("\r\nStart a new LPM operating cycle\r\n");
    }

    /* In case of normal reset */
    else
    {
        /* Print EP information */
        uart_ep_info_print();

        /* Store fixed data into standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_write(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            TERMINAL_ERR_RETURN(FSP_ERR_INVALID_DATA,"standby_sram_write failed\r\n");
        }

        /* Print stored successful message */
        TERMINAL_PRINT("\r\nStored fixed data into standby SRAM\r\n");
#endif
    }

    /* Initialize all modules used to cancel LPM */
    err = source_cancel_lpm_init();
    TERMINAL_ERR_HANDLE(err, "source_cancel_lpm_init Failed\r\n");

    /* Print instructions to start the LPM operating sequence */
    TERMINAL_PRINT("\r\nPress S1 button or wait 10 seconds to start LPM operating sequence\r\n");

    /* Wait until start-enable event occurs */
    err = lpm_wait_start();
    TERMINAL_ERR_RETURN(err, "lpm_wait_start Failed\r\n");

    /* Turn on the LED1 to indicate a successful startup */
    led_lpm_state_set(LED_POWER_ON);

    return err;
}

static fsp_err_t lpm_ep_main_process(void)
{
    fsp_err_t err = FSP_SUCCESS;
    app_lpm_states_t lpm_state = APP_LPM_SLEEP_STATE;

    /* Prints LPM operation start message */
    TERMINAL_PRINT("\r\nLPM operating is started\r\n");

    /* Example project loop*/
    while (true)
    {
        /* Software delay to next LPM mode */
        R_BSP_SoftwareDelay(LPM_TIME_TRANSITION_VALUE, LPM_TIME_TRANSITION_UNIT);

        /* Perform a LPM operating sequence */
        err = lpm_operation(lpm_state);
        TERMINAL_ERR_RETURN(err, "lpm_operation Failed\r\n");

        /* Go to next LPM mode */
        lpm_state ++;

        /* Check end of LPM mode list */
        if (APP_LPM_MAX_STATE <= lpm_state)
        {
            lpm_state = APP_LPM_SLEEP_STATE;
            TERMINAL_PRINT("\r\nStart a new LPM operating sequence cycle\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function will handle error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   err        error value
 *  @param[in]   err_func   function occurred error
 *  @param[in]   err_str    error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err_value, uint8_t * err_func, uint8_t * err_str)
{
    /* Turn on LED2 to indicate an error has occurred */
    led_error_state_set(LED_POWER_ON);

    /* Print error information and error code */
    uart_error_info_print(err_func, err_str);
    uart_error_code_print(err_value);

    /* Close all opened modules */
    source_cancel_lpm_deinit();
    uart_deinit();

    /* Trap error */
    APP_ERR_TRAP(err_value);
}

/**********************************************************************************************************************
* Private Function implementations
**********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure before entering LPM mode
 *  @param[in]  lpm_mode    LPM mode prepares to enter
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Print LPM mode name */
    TERMINAL_PRINT(g_lpm_mode_name[lpm_mode]);

    /* Turn off LED1 in case of LPM mode */
    led_lpm_state_set(LED_POWER_OFF);

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Start/enable hardware modules to cancel LPM mode */
    err = source_cancel_lpm_enable();
    TERMINAL_ERR_RETURN(err, "source_cancel_lpm_enable Failed\r\n");

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* Start/enable hardware modules to cancel Snooze mode */
    if (APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE == lpm_mode)
    {
        err = source_cancel_snooze_enable();
        TERMINAL_ERR_RETURN(err, "source_cancel_snooze_enable Failed\r\n");
    }
#endif

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK)
    /* SCI UART module Stop with Undefined in SW Standby and Deep SW Standby mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        uart_deinit();
    }
#endif

    /* Disable unused IO ports to reduce power */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg);
    TERMINAL_ERR_RETURN(err, "R_IOPORT_PinsCfg API Failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure after exiting LPM mode
 *  @param[in]  lpm_mode    LPM mode just exited
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t lpm_exit_procedure(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Put IO port configuration back to user's selections */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
    TERMINAL_ERR_RETURN(err, "R_IOPORT_PinsCfg API Failed\r\n");

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    /* Re-initialize SCI UART after SW Standby Mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        uart_init();
    }
#endif

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* Stop/disable hardware modules used to cancel snooze mode */
    if (APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE == lpm_mode)
    {
        err = source_cancel_snooze_disable();
        TERMINAL_ERR_RETURN(err, "source_cancel_snooze_disable Failed\r\n");
    }
#else
    (void) lpm_mode;
#endif

    /* Stop/disable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_disable();
    TERMINAL_ERR_RETURN(err, "source_cancel_lpm_disable Failed\r\n");

    /* Detect and print LPM cancellation source */
    source_cancel_lpm_detect();

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Turn on LED1 in case of normal mode */
    led_lpm_state_set(LED_POWER_ON);

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform a sample LPM operation
 *  @param[in]  lpm_mode    LPM mode to operate
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t lpm_operation(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check valid LPM mode */
    if (APP_LPM_MAX_STATE <= lpm_mode)
    {
        TERMINAL_ERR_RETURN(FSP_ERR_INVALID_MODE, "This MCU does not support this LPM mode\r\n");
    }

    /* Open this LPM mode */
    err = R_LPM_Open(g_lpm_instance[lpm_mode]->p_ctrl, g_lpm_instance[lpm_mode]->p_cfg);
    TERMINAL_ERR_RETURN(err, "R_LPM_Open API Failed\r\n");

    /* Procedure before enter to LPM  */
    err = lpm_enter_procedure(lpm_mode);
    TERMINAL_ERR_RETURN(err, "lpm_enter_procedure Failed\r\n");

    /* Enter to LPM */
    err = R_LPM_LowPowerModeEnter(g_lpm_instance[lpm_mode]->p_ctrl);
    TERMINAL_ERR_RETURN(err, "R_LPM_LowPowerModeEnter API Failed\r\n");

    /* Procedure after exit LPM */
    err = lpm_exit_procedure(lpm_mode);
    TERMINAL_ERR_RETURN(err, "lpm_exit_procedure Failed\r\n");

    /* Close LPM */
    err = R_LPM_Close(g_lpm_instance[lpm_mode]->p_ctrl);
    TERMINAL_ERR_RETURN(err, "R_LPM_Close API Failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will wait for trigger to start LPM operation sequence
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t lpm_wait_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable sources used to cancel LPM */
    err = source_cancel_lpm_disable();
    TERMINAL_ERR_RETURN(err, "source_cancel_lpm_disable Failed\r\n");

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Start/enable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_enable();
    TERMINAL_ERR_RETURN(err, "source_cancel_lpm_enable Failed\r\n");

    /* Wait for an event */
    while (false == g_source_flag);

    /* Stop/disable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_disable();
    TERMINAL_ERR_RETURN(err, "source_cancel_lpm_disable Failed\r\n");

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will set the state of the LED1
 *  @param[in]  state   LED state want to set
 *  @retval     None
 **********************************************************************************************************************/
static void led_lpm_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_LPM_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function will set the state of the LED2
 *  @param[in]  state   LED state want to set
 *  @retval     None
 **********************************************************************************************************************/
static void led_error_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ERROR_STATE], (bsp_io_level_t)state);
    }
}

#if defined(SSRAM_START) && defined(SSRAM_LENGTH)
/*******************************************************************************************************************//**
 *  @brief      This function will perform write data to Standby SRAM
 *  @param[in]  offset_addr   offset address at Standby SRAM sections
 *  @param[in]  p_souce       pointer to source data to be written
 *  @param[in]  len           write data length
 *  @retval     true          successful operation
 **********************************************************************************************************************/
static bool standby_sram_write(uint32_t offset_addr, uint8_t * p_source, uint32_t len)
{
    uint8_t * p_ssram = (uint8_t *)SSRAM_START + offset_addr;

    /* Over Standby SRAM address range */
    if (SSRAM_LENGTH <= offset_addr + len)
    {
        return false;
    }

    /* Write to Standby SRAM */
    memcpy(p_ssram, p_source, len);
    return true;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform verify data between input buffer and standby SRAM
 *  @param[in]  offset_addr   offset address of the start address of data at standby SRAM sections
 *  @param[out] p_verify      pointer to source data for verification
 *  @param[in]  len           verify data length
 *  @retval     true          successful operation
 **********************************************************************************************************************/
static bool standby_sram_verify(uint32_t offset_addr, uint8_t * p_source, uint32_t len)
{
    uint8_t * p_ssram = (uint8_t *)SSRAM_START + offset_addr;

    /* Over Standby SRAM address range */
    if (SSRAM_LENGTH <= offset_addr + len)
    {
        return false;
    }

    if (memcmp(p_ssram, p_source, len))
    {
        return false;
    }
    return true;
}
#endif

