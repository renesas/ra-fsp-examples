/***********************************************************************************************************************
 * File Name    : phid_handler_thread_entry.c
 * Description  : Contains function call for USB PHID + PHID Composite demonstration.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "phid_handler_thread.h"
#include "usb_phid_phid_example_app.h"

/*******************************************************************************************************************//**
 * @brief       PHID handler thread entry.
 * @param[in]   pvParameters.
 * @retval      None.
 **********************************************************************************************************************/
void phid_handler_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    phid_handler_thread_entry_run();
}
