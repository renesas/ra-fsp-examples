/***********************************************************************************************************************
 * File Name    : http_client_thread_entry.c
 * Description  : Contains data structures and functions used in http_client_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "http_client_thread.h"
#include "http_client_example.h"

void http_client_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    http_client_example_run();
}
