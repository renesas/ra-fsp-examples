/***********************************************************************************************************************
 * File Name    : user_app_thread_entry.c
 * Description  : Contains data structures and functions used in user_app_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "aws_https_client_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This is the User Thread for the EP.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void user_app_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    aws_https_client_example_app_run();
}
