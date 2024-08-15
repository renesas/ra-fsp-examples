/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_setup.h"
#include "ext_irq_setup.h"

/*******************************************************************************************************************//**
 * @addtogroup r_wdt_ep
 * @{
 **********************************************************************************************************************/


/*
 * External Global Variables
 */
/* LED Structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;
extern volatile uint8_t g_timer_overflow_counter;

/*
 * function declarations
 */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*
 * private function declarations
 */
/* function used to read RTT input and proceed */
static fsp_err_t read_Input_from_RTT(void);
/* function used to enable wdt in debug mode */
static void enable_wdt_count_in_debug_mode(void);
/* function used to check if reset is caused by wdt or not */
static void check_reset_status(void);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT (BANNER_1);
    APP_PRINT (BANNER_2);
    APP_PRINT (BANNER_3, EP_VERSION);
    APP_PRINT (BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT (BANNER_5);
    APP_PRINT (BANNER_6);

    APP_PRINT ("\r\nThis example project demonstrates the typical use of the WDT HAL module APIs");
    APP_PRINT ("\r\nUser input initializes the WDT and start GPT timer");
    APP_PRINT ("\r\nWDT counter is refreshed periodically every 1 second when the GPT timer expires");

#if BSP_MCU_GROUP_RA6M3
    APP_PRINT ("\r\nOn pressing the Push button S2, WDT counter stops from refreshing");
#else
    APP_PRINT ("\r\nOn pressing the Push button, WDT counter stops from refreshing");
#endif

    APP_PRINT ("\r\nWDT resets  the MCU in 2 seconds\r\n");

    /* If this board has no LEDs then trap here */
    if (RESET_VALUE == g_bsp_leds.led_count)
    {
        APP_PRINT ("\r\nNo on-board LEDs Found");
        APP_ERR_TRAP (FSP_ERR_UNSUPPORTED);
    }

    check_reset_status();

    /* Initializing GPT in PWM Mode */
    err = init_gpt_module();
    if(FSP_SUCCESS != err)
    {
        /* Timer init failed */
        APP_ERR_PRINT ("\r\n ** GPT TIMER INIT FAILED ** \r\n");
        APP_ERR_TRAP (err);
    }

    /* Initializing External IRQ */
    err = init_icu_module();
    if(FSP_SUCCESS != err)
    {
        /* External IRQ init failed, cleanup the timer initialization */
        APP_ERR_PRINT ("\r\n ** EXTERNAL IRQ INIT FAILED ** \r\n");
        deinit_gpt_module();
        APP_ERR_TRAP (err);
    }

    /* Menu for User Selection */
    APP_PRINT ("\r\nEnter 1 to Enable WDT\r\n");
    APP_PRINT ("User Input:  ");

    while(true)
    {
        /* Process input only when User has provided one */
        if(APP_CHECK_DATA)
        {
            err = read_Input_from_RTT();
            if(FSP_SUCCESS != err)
            {
                /* Close timer and external IRQ module */
                deinit_gpt_module();
                deinit_icu_module();
                APP_ERR_TRAP (err);
            }
        }
        /* For every 3Sec. RTT prints WDT refresh message.
         * This is done to avoid the continuous print message on RTT*/
        if (WDT_REFRESH_COUNTER_VALUE == g_timer_overflow_counter)
        {
            g_timer_overflow_counter = RESET_VALUE;
            APP_PRINT("\r\nWDT counter Refreshed.");
        }
    }
}

/*******************************************************************************************************************//**
 * This function is called to read user input from RTT using RTT API.
 * @brief    Read user input from RTT. Handle the Error internally with Proper Message. Application handles the rest.
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful to perform operation
 **********************************************************************************************************************/
fsp_err_t read_Input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Variable to store user input */
    uint8_t rByte[BUFFER_SIZE] = { RESET_VALUE };
    uint8_t user_data = RESET_VALUE;

    /* Read First byte of data provided by user */
    APP_READ(rByte);

    user_data = (uint8_t) atoi((char *)&rByte);

    switch (user_data)
    {
        case ENABLE_WDT:
        {
            /* Enable WDT to count and generate NMI or Reset when the debugger(JLink) is connected. */
            enable_wdt_count_in_debug_mode();

            /* Open WDT. For every GPT timeout, wdt will get refreshed. */
            err = R_WDT_Open (&g_wdt_ctrl, &g_wdt_cfg);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** R_WDT_Open API Failed ** \r\n");
                return err;
            }

            /* Start GPT timer in Periodic mode */
            err = timer_start();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** GPT TIMER START FAILED ** \r\n");
                return err;
            }

            /* Enable External IRQ */
            err = enable_icu_module();
            if(FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** EXTERNAL IRQ ENABLE FAILED ** \r\n");
                return err;
            }

            /* Print message to indicate user about application status. */
            APP_PRINT ("\r\nWDT initialized, GPT Timer Started");
            APP_PRINT("\r\nTo stop WDT counter from refreshing, press the push button\r\n");
        }
        break;

        default:
        {
            /* Menu for User Selection */
            APP_PRINT ("\r\nInvalid Input");
            APP_PRINT ("\r\nPlease provide input from below option");
            APP_PRINT ("\r\nEnter 1 to Enable WDT\r\n");
            APP_PRINT ("User Input:  ");
        }
        break;
    }
    user_data = RESET_VALUE;
    return err;
}

/*******************************************************************************************************************//**
 * This function is called to enable WDT counter in debug mode.
 * @brief    Enables WDT counter to count in debug mode.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void enable_wdt_count_in_debug_mode(void)
{
    /* As per hardware manual's DBGREG module, section 2.6.4.2:- Clear this bit to enable WDT Reset/NMI in debug mode */
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_WDT = RESET_VALUE;
}

/*******************************************************************************************************************//**
 * This function is called to check whether reset is caused by WDT.
 * @brief    Check if reset caused by WDT or not.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void check_reset_status(void)
{
    /* Check if reset was caused by the WDT? If it was, turn on LED to indicate WDT reset triggered. */
    if (SYSTEM_RSTSR1_WDTRF_DETECT_WDT_RESET == R_SYSTEM->RSTSR1_b.WDTRF) // @suppress("Symbol is not resolved") // @suppress("Field cannot be resolved")
    {
        /* Clear the flag once read the value */
        R_SYSTEM->RSTSR1_b.WDTRF = RESET_VALUE; // @suppress("Symbol is not resolved") // @suppress("Field cannot be resolved")

        /* Turn ON LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK)
        R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], BSP_IO_LEVEL_LOW);
#else
        R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
#endif
        APP_PRINT ("\r\n************************ WDT Reset detected ************************\r\n");
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
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_wdt_ep)
 **********************************************************************************************************************/
