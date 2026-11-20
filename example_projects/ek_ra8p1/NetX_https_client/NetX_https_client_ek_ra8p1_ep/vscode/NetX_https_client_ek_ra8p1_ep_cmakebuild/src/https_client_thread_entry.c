/***********************************************************************************************************************
 * File Name    : https_client_thread_entry.c
 * Description  : Contains the entry function of the HTTPS Client Thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "https_client_thread.h"
#include "NetX_https_client_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the HTTPS Client thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void https_client_thread_entry(void)
{
    https_client_thread_entry_run();
}
