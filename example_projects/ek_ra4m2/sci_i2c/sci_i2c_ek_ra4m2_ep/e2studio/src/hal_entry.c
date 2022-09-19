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
#include "sci_i2c_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup sci_i2c_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);


/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t          err        = FSP_SUCCESS;
    fsp_pack_version_t version    = {RESET_VALUE};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
    APP_PRINT(EP_INFO);

    /*  SCI_I2C master and IIC slave driver initialization */
    err = init_i2c_driver();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* SCI_I2C Master and IIC Slave initialization failed  */
        APP_ERR_PRINT("\r\n ** IIC driver initialization FAILED ** \r\n");

        /* Turn ON LED */
        set_led(LED_ON);
        APP_ERR_TRAP(err);
    }

    while(true)
    {

        /* Performs SCI_I2C write/read operation */
        err = process_master_WriteRead();

        /*
         * Perform clean_up - On SCI_I2C Master write/read operation failure
         * Note: For Demonstration Purpose the failure ends up with TRAP.
         * Note: This can be handled in many ways as per the Application needs
         */

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Turn on LED */
            set_led(LED_ON);

            /* print RTT message */
            APP_ERR_PRINT ("\r\n **SCI_I2C Master transaction Failed, closing open modules ** \r\n");

            /* de-initialize opened SCI I2C master and IIC slave modules */
            deinit_i2c_driver();

            APP_ERR_TRAP(err);
        }
		
		/* 1 Seconds Wait time to recognize LED toggling between successive Write and Read operations. */
        R_BSP_SoftwareDelay(DELAY_OPERATION, BSP_DELAY_UNITS_SECONDS);

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
 * @} (end addtogroup sci_i2c_ep)
 **********************************************************************************************************************/
