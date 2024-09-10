/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "ble_core_task.h"

extern void app_main(void);
/* BLE_CORE_TASK entry function */
void ble_core_task_entry(void)
{
    /* TODO: add your own code here */
    app_main();
    while (1)
    {
        tx_thread_sleep (1);
    }
}
