/***********************************************************************************************************************
 * File Name    : sdhi_thread_entry.c
 * Description  : Contains data structures and functions used in sdhi_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sdhi_thread.h"
#include "sdhi_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function starts the SDHI example application.
 * @param[in] pvParameters              SDHI thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void sdhi_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    sdhi_example_app_run();
}
