/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "flash_hp_ep.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
void readfromRTT(void);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version;
    uint32_t read_data = RESET_VALUE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT("\r\n\nThis example project demonstrates basic functionalities of Flash HP");
    APP_PRINT("\r\ndriver. An RTT input command triggers the Initialization, Erase, Blank Check,");
    APP_PRINT("\r\nFlash Write, Flash Read operations in normal operating mode for Code/Data flash");
    APP_PRINT("\r\non predefined block and outputs the status/data to RTT Viewer.");
    /* Setup MCU port settings after C runtime environment and system clocks are setup */
    R_BSP_WarmStart(BSP_WARM_START_POST_C);

    /* Open Flash_HP */
    err = R_FLASH_HP_Open(&g_flash_ctrl, &g_flash_cfg);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("\r\nR_FLASH_HP_Open API failed");
        APP_ERR_TRAP(err);
    }

    /* Setup Default  Block 0 as Startup Setup Block */
    err = R_FLASH_HP_StartUpAreaSelect(&g_flash_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
    if (err != FSP_SUCCESS)
    {
        APP_PRINT("\r\nR_FLASH_HP_StartUpAreaSelect API failed");
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\n\nChoose an option from the below menu");
    APP_PRINT("\r\n Press 1 for Code Flash Operations \r\n Press 2 for Data Flash Operations \r\n Press 3 for EXIT");

    unsigned char rByte[BUFF_SIZE] = { RESET_VALUE };
    while (EXIT != read_data)
    {

        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            read_data = (uint32_t)(atoi((char *)(rByte)));
            switch (read_data)
            {
                case CODE_FLASH:
                    APP_PRINT("\n\rEntering to code flash operations");
                    err = flash_hp_code_flash_operations();
                    if( FSP_SUCCESS != err)
                    {
                        flash_hp_deinit();
                        APP_ERR_TRAP(err);
                    }
                    break;

                case DATA_FLASH:
                    APP_PRINT("\n\rEntering to data flash operations");
                    err = flash_hp_data_flash_operations();
                    if (FSP_SUCCESS != err)
                    {
                        flash_hp_deinit();
                        APP_ERR_TRAP(err);
                    }
                    break;
                case EXIT:
                    APP_PRINT("\n\rExiting the flash_hp, the user has to restart the application");
                    flash_hp_deinit();
                    break;
                default:
                    APP_PRINT("\r\nChoose an option from the below menu");
                    APP_PRINT("\r\n Press 1 for Code Flash Operations "\
                              "\r\n Press 2 for Data Flash Operations "\
                              "\r\n Press 3 for EXIT");
                    break;
            }
            if ((CODE_FLASH <= read_data) && (EXIT > read_data))
            {
                APP_PRINT("\r\n Enter any key (other than 1,2,3) to go back to the main menu\r\n");
            }
            read_data = RESET_VALUE;
            memset(rByte,RESET_VALUE,BUFF_SIZE);
        }
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
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

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);

#if BSP_CFG_SDRAM_ENABLED

        /* Setup SDRAM and initialize it. Must configure pins first. */
        R_BSP_SdramInit(true);
#endif
    }
}

#if BSP_TZ_SECURE_BUILD

FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif
