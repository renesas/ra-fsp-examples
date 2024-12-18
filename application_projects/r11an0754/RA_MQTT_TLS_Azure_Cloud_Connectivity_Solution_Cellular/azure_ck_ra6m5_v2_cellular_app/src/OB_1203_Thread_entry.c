/***********************************************************************************************************************
 * File Name    : OB_1203_Thread_entry.c
 * Description  : Contains entry function of OB sensor Thread .
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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
