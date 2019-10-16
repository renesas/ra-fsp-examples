/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"

#include "agt_ep.h"
#include "icu_ep.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Internal function to read input from RTT */
static void read_Input_from_RTT(void);

volatile uint8_t g_icu_flag = RESET_FLAG;               //flag to check ICU is enable or not
volatile uint8_t g_one_shot_timer_flag = RESET_FLAG;    //flag to check timer is enable or not
volatile uint8_t g_periodic_timer_flag = RESET_FLAG;    //flag to check timer1 is enable or not
volatile uint32_t g_error_flag = RESET_FLAG;            //flag to capture error in ISR's

/*******************************************************************************************************************//**
 * @brief     The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 *            called by main() when no RTOS is used.
 * @param[in] None
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3,EP_VERSION);
    APP_PRINT(BANNER_4,version.major, version.minor, version.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(BANNER_7);

    /* Initialize AGT driver */
    err = agt_init();
    if (FSP_SUCCESS != err)
    {   /* AGT module init failed */
        APP_ERR_PRINT("\r\n ** AGT INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize ICU driver */
    err = ext_irq_init();
    if (FSP_SUCCESS != err)
    {   /* ICU module init failed */
        agt_deinit();
        APP_ERR_PRINT("\r\n ** ICU INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Menu for User Selection */
    APP_PRINT("\r\nMenu to Select\r\n");
    APP_PRINT("Press 1 to enable Push Button\r\n");
    APP_PRINT("Press 2 to stop the timer and disable push button\r\n");
    APP_PRINT("User Input:  ");

    while(true)
    {
        /* Process input only when User has provided one */
        if (APP_CHECK_DATA)
        {
            read_Input_from_RTT();
        }

        /* Check if AGT0 is enabled in OneShot mode */
        if (SET_FLAG == g_one_shot_timer_flag)
        {
            g_one_shot_timer_flag = RESET_FLAG;
            APP_PRINT("\r\nAGT0 is Enabled in OneShot mode");
        }

        /* Check if AGT1 is enabled in Periodic mode */
        if (SET_FLAG == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = RESET_FLAG;
            APP_PRINT ("\r\n\r\nOne-shot mode AGT timer elapsed\n");
            APP_PRINT ("\r\n\r\nAGT1 is Enabled in Periodic mode");
            APP_PRINT ("\r\nLED will toggle after every 1 second\r\n");
            APP_PRINT ("\r\nEnter any other key (except '1' and '2') to go back to the main menu\r\nUser Input:  ");
        }

        /* Check if AGT1 is already running in Periodic mode */
        if (ALREADY_RUNNING == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = RESET_FLAG;
            APP_PRINT ("\r\n\r\nOne-shot mode AGT timer elapsed\n");
            APP_PRINT ("\r\n\r\nAGT1 is already running in Periodic mode");
            APP_PRINT ("\r\nEnter any other key (except '1' and '2') to go back to the main menu\r\nUser Input:  ");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function reads the command(input) for RTT and processes the command.
 *
 * @param[in] None
 **********************************************************************************************************************/
static void read_Input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Variable to store user input */
    unsigned char rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint8_t converted_rtt_input = RESET_VALUE;

    APP_READ(rByte);
    converted_rtt_input = (uint8_t)atoi((char *)rByte);

    switch (converted_rtt_input)
    {
        case ENABLE_EXT_ICU:
        {
            /* Enable Push button */
            err = ext_irq_enable();
            if (FSP_SUCCESS != err)
            {
                ext_irq_deinit();
                agt_deinit();
                APP_ERR_PRINT("\r\nEnabling external IRQ failed.Closing all drivers. Restart the Application\r\n");
                APP_ERR_TRAP(err);
            }
            else
            {
                /* Do Nothing */
            }
        }
        break;

        case STOP_TIMER_DISABLE_EXT_ICU:
        {
            /* Disable Push button and stop timers */
            err = stop_timer_disble_icu();
            if (FSP_SUCCESS != err)
            {
                ext_irq_deinit();
                agt_deinit();
                APP_ERR_PRINT("\r\nDisabling the push button and stopping the timer failed.\r\nClosing all drivers. Restart the Application\r\n");
                APP_ERR_TRAP(err);
            }
            else
            {
                APP_PRINT("\r\nAGT timer stopped and Push Button disabled");
                APP_PRINT("\r\nEnter any other key (except '1' and '2') to go back to the main menu\r\nUser Input:  ");
            }
        }
        break;
        default:
        {
            /* Menu for User Selection */
            APP_PRINT("\r\nPlease select the operation from below Menu\r\n");
            APP_PRINT("Press 1 to enable Push Button\r\n");
            APP_PRINT("Press 2 to stop the timer and disable push button\r\n");
            APP_PRINT("User Input:  ");
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
