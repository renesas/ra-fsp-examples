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
#include "i2c_slave.h"

/*******************************************************************************************************************//**
 * @addtogroup r_iic_slave_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t          err                  =  FSP_SUCCESS;
    fsp_pack_version_t version              = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(EP_INFO);

    /* I2C master and slave driver initialization */
    err = init_i2C_driver();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* I2C master and slave initialization failed */
        APP_ERR_PRINT("\r\n ** IIC driver initialization FAILED ** \r\n");

        /* Turn ON LED */
        set_led(LED_ON);

        APP_ERR_TRAP(err);
    }

    while(true)
    {
        /* Performs on I2C slave read write operation */
        err = process_slave_WriteRead();

        /*
         * Perform clean_up - On slave i2c read write operation failure
         * Note: For Demonstration Purpose the failure ends up with TRAP.
         * Note: This can be handled in many ways as per the Application needs
         */
        if (FSP_SUCCESS != err)
        {
            /* Print RTT message */
            APP_ERR_PRINT ("\r\n **I2C slave transaction Failed, closing open modules ** \r\n");

            /* de-initialize opened I2C module */
            deinit_i2c_driver();
            APP_ERR_TRAP(err);
        }

        /* 1 seconds wait time to recognize LED toggling between successive read and write operations. */
        R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
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
 * @} (end addtogroup r_iic_slave_ep)
 **********************************************************************************************************************/
