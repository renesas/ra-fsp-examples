/***********************************************************************************************************************
 * File Name    : RA_ICP10101.c
 * Description  : Contains function definitions for ICP10101 sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "icp.h"
#include "ICP_10101.h"
#include "usr_data.h"

extern float Temperature; // @suppress("Global (API or Non-API) variable prefix")
extern float Pressure;
usr_icp_data_t g_icp_data;

/*******************************************************************************************************************//**
 * @brief   Send ICP data to the queue
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void send_icp_data_to_queue(void)
{
    /* Get value of ICP sensor */
    ICP_10101_get();
    
    /* Update value for icp_data variable */
    g_icp_data.temperature_C = Temperature;
    g_icp_data.pressure_Pa = Pressure;

    xQueueOverwrite(g_icp_queue, &g_icp_data);
}

