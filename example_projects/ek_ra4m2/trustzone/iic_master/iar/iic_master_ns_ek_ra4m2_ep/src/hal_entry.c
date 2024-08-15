/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 * **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "i2c_sensor.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);

void hal_entry(void) {
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t xyz_axis[6]      = {RESET_VALUE};
    char flt_str[64]        = {RESET_VALUE};
    float x_axis            = 0.0f;
    float y_axis            = 0.0f;
    float z_axis            = 0.0f;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3,EP_VERSION);
    APP_PRINT(BANNER_4,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT("\nThis EP utilizes PMOD ACL sensor as iic slave device\n")
    APP_PRINT("Upon successful initialization, MCU displays sensor axis data\n\n\n")

    /* Opening IIC master module and Initializing PMOD ACL sensor */
    err = init_sensor();

    if(FSP_SUCCESS != err)
    {
        /* Sensor init failed, so cleanup the sensor specific initialization */
        APP_ERR_PRINT("** SENSOR INIT FAILED ** \r\n");
        deinit_sensor();
        APP_ERR_TRAP(err);
    }

    /* Stay in forever loop and read the sensor data every 3 Seconds
     * If the Sensor data reading returns error, cleanup and break with TRAP.
     * Note: For Demonstration Purpose the failure ends up with TRAP.
     * Note: This can be handled in many ways as per the Application needs
     */
    while (true)
    {
        /* Read PMOD ACL sensor data */
        err =  read_sensor_data(xyz_axis);

        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n ** SENSOR READ DATA FAILED ** \r\n");
            deinit_sensor();
            APP_ERR_TRAP(err);
        }
        else
        {
             /*
              * X,Y,Z - axis data has to be evaluated so accessing particular array value,
              * shifting it by 8 bits obtains data value to float variable
              */

            x_axis = (float) (xyz_axis[0] | (xyz_axis[1] << BIT_SHIFT_8));   // X-axis value
            y_axis = (float) (xyz_axis[2] | (xyz_axis[3] << BIT_SHIFT_8));   // Y-axis value
            z_axis = (float) (xyz_axis[4] | (xyz_axis[5] << BIT_SHIFT_8));   // Z-axis value

            snprintf(flt_str,SIZE_64,"X-axis = %.02f, Y-axis = %.02f, Z-axis = %.02f",x_axis,y_axis,z_axis);

            APP_PRINT("%s \r\n",flt_str);

            /* 3 Seconds Wait time between successive readings */
            R_BSP_SoftwareDelay(SENSOR_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
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
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
	if (BSP_WARM_START_RESET == event) {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
	}

	if (BSP_WARM_START_POST_C == event) {
		/* C runtime environment and system clocks are setup. */

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
