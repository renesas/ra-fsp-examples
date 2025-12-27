/***********************************************************************************************************************
 * File Name    : ZMOD4510_Thread_entry.c
 * Description  : Contains data structures and functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "ZMOD4510_Thread.h"
#include "RA_ZMOD4XXX_Common.h"
#include "user_choice.h"
#include "usr_app.h"
/* ZMOD4510_Thread entry function */
void ZMOD4510_Thread_entry(void)
{
    /* TODO: add your own code here */
    g_zmod4xxx_sensor1_quick_setup();
    while (1)
    {
#if _ZMOD4510_SENSOR_ENABLE_
        start_oaq_1st_gen();
#endif
        tx_thread_sleep (pdMS_TO_TICKS(30));
    }
}
