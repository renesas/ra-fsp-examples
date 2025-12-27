/***********************************************************************************************************************
 * File Name    : OB_1203_Thread_entry.c
 * Description  : Contains entry function of OB sensor Thread .
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "OB_1203_Thread.h"
#include "oximeter.h"
#include "user_choice.h"
#include "usr_app.h"

/* OB_1203_Thread entry function */
void OB_1203_Thread_entry(void)
{
    /* TODO: add your own code here */
    /* Open OB1203 */
#if _OB1203_SENSOR_ENABLE_
    RA_ob1203_init();
#endif

    while (1)
    {
#if _OB1203_SENSOR_ENABLE_
        /* Start reading OB1203 sensor data */
        ob1203_spo2_main();
#endif
        tx_thread_sleep (pdMS_TO_TICKS(10));
    }
}
