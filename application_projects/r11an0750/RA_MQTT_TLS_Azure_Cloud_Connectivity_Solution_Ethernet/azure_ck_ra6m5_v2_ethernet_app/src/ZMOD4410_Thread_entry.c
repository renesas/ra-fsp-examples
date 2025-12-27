/***********************************************************************************************************************
 * File Name    : ZMOD4410_Thread_entry.c
 * Description  : Contains data structures and functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "ZMOD4410_Thread.h"
#include "RA_ZMOD4XXX_Common.h"
#include "common_utils.h"
#include "user_choice.h"
#include "usr_app.h"
/* ZMOD4410_Thread entry function */
void ZMOD4410_Thread_entry(void)
{
    /* TODO: add your own code here */
    while (1)
    {
#if _ZMOD4410_SENSOR_ENABLE_
        start_iaq_1st_gen();
#endif
        tx_thread_sleep (pdMS_TO_TICKS(60));
    }
}
