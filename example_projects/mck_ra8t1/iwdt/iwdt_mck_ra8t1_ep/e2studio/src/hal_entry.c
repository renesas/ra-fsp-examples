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

/*******************************************************************************************************************//**
 * @addtogroup r_iwdt_ep
 * @{
 **********************************************************************************************************************/

/* External Global Variables */
extern volatile uint8_t g_timer_overflow_counter;

/* Function declarations */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* private function declarations */
static fsp_err_t read_Input_from_RTT(void);
static void enable_iwdt_count_in_debug_mode(void);
static void check_reset_status(void);

#define     EP_INFO     "\r\nThis example project demonstrates the typical use of the IWDT HAL module."\
		                "\r\nUser input initializes the IWDT and start GPT timer. IWDT counter is refreshed"\
						"\r\nperiodically every 1 second when the GPT timer expires."\
						"\r\nOn entering 2 from RTTViewer, IWDT counter stops from refreshing and resets"\
						"\r\nthe MCU. \r\n"
#define     MENU        "\r\nEnter 1 to Enable IWDT\r\n"\
		                "User Input:  "

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
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    APP_PRINT (EP_INFO);

    /* Check whether reset is caused by IWDT */
    check_reset_status();

    APP_PRINT("\r\nClock source for IWDT is IWDTCLK\r\n");

    /* Initializing GPT in Periodic Mode */
    err = init_gpt_module();
    if (FSP_SUCCESS != err)
    {
        /* timer init failed */
        APP_ERR_PRINT ("\r\n ** GPT TIMER INIT FAILED ** \r\n");
        APP_ERR_TRAP (err);
    }

    /* Menu for User Selection */
    APP_PRINT (MENU);

    while(true)
    {
        /* Process input only when User has provided one */
        if (APP_CHECK_DATA)
        {
            err = read_Input_from_RTT();
            if (FSP_SUCCESS != err)
            {
                /* Close timer module */
                deinit_gpt_module();
                APP_ERR_TRAP (err);
            }
        }

        /* For every 2 Second. RTT prints IWDT refresh message.
         * This is done to avoid the continuous print message on RTT */
        if (IWDT_REFRESH_COUNTER_VALUE == g_timer_overflow_counter)
        {
            g_timer_overflow_counter = RESET_VALUE;
            APP_PRINT("\r\nIWDT counter Refreshed.");
        }
    }
}

/*******************************************************************************************************************//**
 * This function is called to read user input from RTT using RTT API.
 * @brief    Read user input from RTT. Handle the Error internally with Proper Message. Application handles the rest.
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS, Unsuccessful to perform operation
 **********************************************************************************************************************/
static fsp_err_t read_Input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to store user input */
    uint8_t rByte[BUFFER_SIZE] = { RESET_VALUE };
    uint8_t user_data = RESET_VALUE;
    static bool b_is_iwdt_enable = false;

    /* Read First byte of data provided by user */
    APP_READ(rByte);

    user_data = (uint8_t) atoi((char *)&rByte);

    /* perform operation on user input */
    switch (user_data)
    {
        case ENABLE_IWDT:
        {
            /* setting the variable */
            b_is_iwdt_enable = true;
            /* Enable IWDT to count and generate NMI or Reset when the debugger(JLink) is connected. */
            enable_iwdt_count_in_debug_mode();

            /* Open IWDT. For every GPT timeout, iwdt will get refreshed. */
            err = R_IWDT_Open (&g_iwdt_ctrl, &g_iwdt_cfg);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** R_IWDT_Open API Failed ** \r\n");
                return err;
            }

            /* Start GPT timer in Periodic mode */
            err = timer_start();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** GPT TIMER START FAILED ** \r\n");
                return err;
            }
            /* Print message to indicate user about application status. */
            APP_PRINT ("\r\nIWDT initialized, GPT Timer Started");
            APP_PRINT("\r\nTo stop IWDT counter from refreshing, Enter '2' from RTTViewer\r\n");
        }
        break;

        case STOP_IWDT_REFRESH:
        {
            if (true == b_is_iwdt_enable)
            {
                /* resetting the variable */
                b_is_iwdt_enable = false;
                /* gpt timer will stops running */
                err = R_GPT_Stop(&g_timer_ctrl);
                if (FSP_SUCCESS != err)
                {
                    /* Print Error on RTT console */
                    APP_ERR_PRINT("\r\n ** R_GPT_Stop API failed ** \r\n");
                    return err;
                }
                APP_PRINT("\r\n\r\nGPT timer stopped.");
                APP_PRINT("\r\n*Independent Watchdog Timer will overflow in 17 seconds.*");
            }
            else
            {
                APP_PRINT (MENU);
            }
        }
        break;

        default:
        {
            /* Menu for User Selection */
            APP_PRINT ("\r\nInvalid Input");
            APP_PRINT ("\r\nPlease provide input from below option");
            APP_PRINT (MENU);
        }
        break;
    }
    user_data = RESET_VALUE;
    return err;
}

/*******************************************************************************************************************//**
 * This function is called to enable IWDT counter in debug mode.
 * @brief       Enables IWDT counter to count in debug mode.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void enable_iwdt_count_in_debug_mode(void)
{
    /* As per hardware manual's DBGREG module, section 2.6.4.2:- Clear this bit to enable IWDT Reset/NMI in debug mode */
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_IWDT = RESET_VALUE;
}

/*******************************************************************************************************************//**
 * This function is called to check whether reset is caused by IWDT/NMI.
 * @brief       Check if reset caused by IWDT or NMI.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void check_reset_status(void)
{
    /* Check if reset was caused by the IWDT/NMI  */
    if (SYSTEM_RSTSR1_IWDTRF_DETECT_RESET == R_SYSTEM->RSTSR1_b.IWDTRF)
    {
        /* Clear the flag once read the value */
        R_SYSTEM->RSTSR1_b.IWDTRF = RESET_VALUE;
        APP_PRINT ("\r\n************************ IWDT Reset detected ************************\r\n");
    }
    else if (SYSTEM_RSTSR1_SWRF_DETECT_RESET == R_SYSTEM->RSTSR1_b.SWRF)
    {
        /* Clear the flag once read the value */
        R_SYSTEM->RSTSR1_b.SWRF = RESET_VALUE;
        APP_PRINT ("\r\n******************* NMI Software Reset detected *********************\r\n");
    }
    else
    {
        /* None */
    }
}

/*******************************************************************************************************************//**
 * @brief       NMI_callback.
 * @param[IN]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void NMI_callback(wdt_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Issue a software reset to reset the MCU. */
    __NVIC_SystemReset();
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
 * @} (end addtogroup r_iwdt_ep)
 **********************************************************************************************************************/
