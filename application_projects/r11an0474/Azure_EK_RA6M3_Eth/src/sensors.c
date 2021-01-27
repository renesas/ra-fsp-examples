/***********************************************************************************************************************
 * File Name    : sensors.c
 * Description  : sensor source file
 **********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include <main_thread.h>

static float temp __attribute__((__used__));

fsp_err_t sensor_init(void);

void adc_callback(adc_callback_args_t *p_args)
{
    BaseType_t ctx = false;
    uint16_t adc_val = 0;

    if((NULL != p_args) && (ADC_EVENT_SCAN_COMPLETE == p_args->event))
    {
        R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_val);

        temp = ((((float) adc_val) * 0.353793f) - 467.39f);

        xQueueGenericSendFromISR(g_sensor_queue, &temp, &ctx, queueSEND_TO_BACK);
        portYIELD_FROM_ISR(ctx);
    }
}


fsp_err_t sensor_init(void)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    if(FSP_SUCCESS == status)
    {
        status = R_ELC_Enable(&g_elc_ctrl);
        if(FSP_SUCCESS == status)
        {
            status = R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
            if(FSP_SUCCESS == status)
            {
                status = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
                if(FSP_SUCCESS == status)
                {
                    status = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
                    if(FSP_SUCCESS == status)
                    {
                        status = R_ADC_ScanStart(&g_adc0_ctrl);
                        if(FSP_SUCCESS == status)
                        {
                            status = R_GPT_Start(&g_timer0_ctrl);
                            if(FSP_SUCCESS == status)
                            {
                                /* Initialize USB Module */
                                status = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
                            }
                        }
                    }
                }
            }
        }
    }

    return status;
}

