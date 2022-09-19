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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "common_utils.h"
#include "adc_ep.h"


/*******************************************************************************************************************//**
 * @addtogroup r_adc_ep
 * @{
 **********************************************************************************************************************/
extern volatile bool b_ready_to_read;
void R_BSP_WarmStart(bsp_warm_start_event_t event);


/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 * This is the main loop of the Application.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version =   { RESET_VALUE };

    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(BANNER_7);

    /* Menu for User Selection */
    APP_PRINT(MENUOPTION1);
    APP_PRINT(MENUOPTION2);
    APP_PRINT(MENUOPTION3);
    APP_PRINT(MENUOPTION4);

    while (true)
    {
        if (APP_CHECK_DATA)
        {
            err = read_process_input_from_RTT ();

            /* handle error */
            if (FSP_SUCCESS != err)
            { /* adc scan start or stop failed, cleanup the adc initialization */
                deinit_adc_module ();
                APP_ERR_TRAP(err);
            }
        }

        /* read the adc output data and status */
        if (true == b_ready_to_read)
        {
            err = adc_read_data();
        }

        /* handle error */
        if (FSP_SUCCESS != err)
        { /* adc reading failed, cleanup the adc initialization */
            deinit_adc_module ();
            APP_ERR_TRAP(err);
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
 * @} (end addtogroup r_adc_ep)
 **********************************************************************************************************************/
