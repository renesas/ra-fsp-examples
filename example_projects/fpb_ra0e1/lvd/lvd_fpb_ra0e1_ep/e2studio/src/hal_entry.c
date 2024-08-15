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

/*******************************************************************************************************************//**
 * @addtogroup r_lvd_ep
 * @{
 **********************************************************************************************************************/

/* Macro definition */

/* LVD0 detect reset value */
#define SYSTEM_RSTSR0_DETECT_LVD0_RESET        (1u)
#define SET_FLAG                               (1u)

/* LVD Example project details */
#define LVD_INFO     "\r\nThe example project demonstrates the typical use of the LVD HAL module APIs."\
                     "\r\nThe example project enables LVD0 circuit to reset MCU and LVD1 to generate maskable interrupt.\r\n"\
                     "\r\nLVD0 resets MCU only when input voltage drops below LVD0 threshold value."\
                     "\r\nTo detect this, user has to increase back the voltage value above LVD0 threshold value."\
                     "\r\nBy doing this, LED will turn ON indicating LVD0 reset detection."\
                     "\r\nEx: 2.8V is LVD0 threshold value, when input Voltage drops from 2.9V to 2.7V, LVD0 resets MCU."\
                     "\r\nUser has to rise input voltage above 2.8V again to detect LVD0 reset.\r\n"\
                     "\r\nLVD1 generates a maskable interrupt only when input voltage rises above LVD1 threshold value."\
                     "\r\nEx: 2.9V is LVD1 threshold value, when input voltage is raised from 2.8V to 3.0V,"\
                     "\r\nLVD1 generates the maskable interrupt.\r\n"

/* Macro to display instructions on how to run LVD EP */
#define LVD_NOTE   "\r\nNote:\
                   \r\n1. To run LVD Example project, use external debugger for flashing the code, disconnect\
                   \r\ndebugging mode and run the project.\
                   \r\n2. To detect LVD0 Reset, input voltage has to be raised back above threshold value(Vdet0) once it is\
                   \r\ndropped below threshold value(Vdet0). This will turn ON LED and prints LVD0 reset detection message\
                   \r\non JlinkRTTviewer. Vdet0 values are documented in readme.txt for specific board.\r\n"

/* External Global Variables */
/* LED Structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;
volatile uint8_t g_lvd1_status = RESET_VALUE;

/* function declarations */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Private function declarations */
static fsp_err_t check_reset_status(void);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;                    // Error status
    fsp_err_t lvd_err = FSP_SUCCESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    APP_PRINT(LVD_INFO);

    APP_PRINT(LVD_NOTE);

    /* If this board has no LEDs then trap here */
    if (RESET_VALUE == g_bsp_leds.led_count)
    {
        APP_PRINT ("\r\nNo on-board LEDs Found");
        APP_ERR_TRAP (FSP_ERR_UNSUPPORTED);
    }

    /* Check if reset is caused by LVD0 monitor */
    /* LVD0 is enabled in OFS1 register for this example project with specific detection voltage for each MCU.
     * User can disable it by going to configuration.xml -> BSP -> properties -> OFS1 register settings.
     * Here user will get both the options to enable Voltage detection 0 circuit and the voltage level. */

    err = check_reset_status();
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** Unable to turn LED on ** \r\n");
        APP_ERR_TRAP (err);
    }

    /* Open LVD1 instance to detect Voltage rise above threshold value by generating an maskable interrupt */
    err = R_LVD_Open(&g_lvd_ctrl, &g_lvd_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_LVD_Open API failed ** \r\n");
        APP_ERR_TRAP (err);
    }

    APP_PRINT ("\r\nLVD1 Enabled to generate a maskable interrupt when Voltage raises above the threshold value\r\n");

    while(true)
    {
        /* Check If Voltage increased than threshold value. If yes, notify user */
        if(SET_FLAG == g_lvd1_status)
        {
            APP_PRINT ("\r\nVoltage Raised above threshold value.");

            /* Clear LVD1 Status */
            err = R_LVD_StatusClear(&g_lvd_ctrl);
            if (FSP_SUCCESS != err)
            {
                /* Print Error on RTT console */
                APP_ERR_PRINT ("\r\n ** R_LVD_StatusClear API failed ** \r\n");

                /* Close LVD1 driver */
                lvd_err = R_LVD_Close(&g_lvd_ctrl);
                if (FSP_SUCCESS != lvd_err)
                {
                    APP_ERR_PRINT ("\r\n ** R_LVD_Close API failed ** \r\n");
                }
                APP_ERR_TRAP (err);
            }

            APP_PRINT ("\r\nLVD1 Status flag is Cleared.\r\n");
            g_lvd1_status = RESET_VALUE;
        }
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
 * @brief    Check if reset caused by LVD0 or not. If yes, Turn LED ON.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static fsp_err_t check_reset_status(void)
{
    fsp_err_t io_err = FSP_SUCCESS;
    /* Check if reset was caused by the LVD0? If it was, turn on LED to indicate LVD0 reset triggered. */
#if defined (BOARD_RA0E1_FPB)
    if (SYSTEM_RSTSR0_DETECT_LVD0_RESET == R_SYSTEM->RESF_b.LVIRF)
#else
    if (SYSTEM_RSTSR0_DETECT_LVD0_RESET == R_SYSTEM->RSTSR0_b.LVD0RF)
#endif
    {
        /* Clear the flag once read the value */
#if defined (BOARD_RA0E1_FPB)
        // Do nothing
#else
        R_SYSTEM->RSTSR0_b.LVD0RF = RESET_VALUE;
#endif


        /* Notify user that reset is caused by LVD0 monitor */
        APP_PRINT ("\r\n************************ LVD0 Reset detected ************************\r\n");

        /* Turn ON LED */
        io_err = R_IOPORT_PinWrite (&g_ioport_ctrl, g_bsp_leds.p_leds[0], BSP_IO_LEVEL_HIGH);
        if (FSP_SUCCESS != io_err)
        {
            /* Print Error on RTT console */
            APP_ERR_PRINT ("\r\n ** R_IOPORT_PinWrite API failed ** \r\n");
        }
    }
    return io_err;
}

/*******************************************************************************************************************//**
 * @brief This function is callback for r_lvd1 instance and notifies user about rise in Voltage level.
 * @param[in]   p_args   LVD callback parameters
 * @retval      None
 **********************************************************************************************************************/
void lvd1_callback(lvd_callback_args_t *p_args)
{
    /* Voltage Raising detected */
    if(LVD_CURRENT_STATE_ABOVE_THRESHOLD == p_args->current_state)
    {
        /* Set the lvd1 status flag to notify the user about rise in voltage */
        g_lvd1_status = SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_lvd_ep)
 **********************************************************************************************************************/
