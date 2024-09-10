/***********************************************************************************************************************
 * File Name    : RA_ICM20948.c
 * Description  : Contains function definitions for ICM20948 sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "icm.h"
#include "ICM_20948.h"
#include "usr_data.h"

extern xyzFloat corrAccRaw; // @suppress("Global (API or Non-API) variable prefix")
extern xyzFloat gVal;
extern xyzFloat magValue;
usr_icm_data_t g_icm_data;

/*******************************************************************************************************************//**
 * @brief   Send ICM data to the queue
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void send_icm_data_to_queue(void)
{
    /* Get value of ICM sensor */
    ICM_20948_get();
    
    /* Update value for icm_data variable */
    g_icm_data.acc_data.x = corrAccRaw.x;
    g_icm_data.acc_data.y = corrAccRaw.y;
    g_icm_data.acc_data.z = corrAccRaw.z;
    g_icm_data.gyr_data.x = gVal.x;
    g_icm_data.gyr_data.y = gVal.y;
    g_icm_data.gyr_data.z = gVal.z;
    g_icm_data.mag_data.x = magValue.x;
    g_icm_data.mag_data.y = magValue.y;
    g_icm_data.mag_data.z = magValue.z;

    xQueueOverwrite(g_icm_queue, &g_icm_data);
}

