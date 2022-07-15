/***********************************************************************************************************************
 * File Name    : RA_ZMOD4XXX_OAQ1stGen.c
 * Description  : Contains data structures and function definitions for ZMOD4510 sensor data read
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "hal_data.h"
#include "RA_ZMOD4XXX_Common.h"
#include "sensor_thread.h"
#include "common_utils.h"

static demo_sequence_t sequence = DEMO_SEQUENCE_1;
volatile float gs_demo_air_quality;
rm_zmod4xxx_raw_data_t raw_data1;

/*******************************************************************************************************************//**
 * @brief       Start reading ZMOD4510 data
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void start_oaq_1st_gen(void)
{
    fsp_err_t err;
    rm_zmod4xxx_oaq_1st_data_t zmod4xxx_data;
    char flt_oaq_str1[20] =
    { RESET_VALUE };

    switch (sequence)
    {
        case DEMO_SEQUENCE_1:
        {
            /* Clear status */
            g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_WAIT;
#if DEMO_IRQ_ENABLE
            g_irq_callback_status1 = DEMO_CALLBACK_STATUS_WAIT;
#endif

            /* Start measurement */
            err = g_zmod4xxx_sensor1.p_api->measurementStart (g_zmod4xxx_sensor1.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_2;
            }
            else
            {
                APP_DBG_PRINT("\r\nTask zmod4510 task measurement failed in seq 1:%d\r\n", err);
            }
        }
        break;

        case DEMO_SEQUENCE_2:
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status1)
            {
                case DEMO_CALLBACK_STATUS_WAIT:
                break;
                case DEMO_CALLBACK_STATUS_SUCCESS:
                    sequence = DEMO_SEQUENCE_3;
                break;
                case DEMO_CALLBACK_STATUS_REPEAT:
                    sequence = DEMO_SEQUENCE_1;
                break;
                default:
                    APP_DBG_PRINT("\r\nTask zmod4510 task delete in seq 2:%d\r\n", err)
                    ;
                break;
            }

            /* Wait 10ms */
            vTaskDelay (5);
        }
        break;

#if DEMO_IRQ_ENABLE
        case DEMO_SEQUENCE_3:
        {
            /* Check IRQ callback status */
            switch (g_irq_callback_status1)
            {
                case DEMO_CALLBACK_STATUS_WAIT:
                break;
                case DEMO_CALLBACK_STATUS_SUCCESS:
                    sequence = DEMO_SEQUENCE_5;
                break;
                default:
                break;
            }
        }
        break;
#else
        case DEMO_SEQUENCE_3 :
        {
            /* Clear status */
            g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_WAIT;

            /* Get status */
            err = g_zmod4xxx_sensor1.p_api->statusCheck(g_zmod4xxx_sensor1.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_4;
            }
            else
            {
              //  demo_err();
            }
        }
        break;

        case DEMO_SEQUENCE_4 :
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status1)
            {
                case DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = DEMO_SEQUENCE_5;
                    break;
                case DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = DEMO_SEQUENCE_3;
                    break;
                default :
                    break;
            }

            /* Wait 50ms */
            vTaskDelay(50);
        }
        break;
#endif
        case DEMO_SEQUENCE_5:
        {
            /* Clear status */
            g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_WAIT;

            /* Read data */
            err = g_zmod4xxx_sensor1.p_api->read (g_zmod4xxx_sensor1.p_ctrl, &raw_data1);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_6;
            }
            else
            {
                sequence = DEMO_SEQUENCE_3;
                APP_DBG_PRINT("\r\nTask zmod4510 task read failed in seq 5:%d\r\n", err);
            }
        }
        break;

        case DEMO_SEQUENCE_6:
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status1)
            {
                case DEMO_CALLBACK_STATUS_WAIT:
                break;
                case DEMO_CALLBACK_STATUS_SUCCESS:
                    sequence = DEMO_SEQUENCE_7;
                break;
                case DEMO_CALLBACK_STATUS_REPEAT:
                    sequence = DEMO_SEQUENCE_5;
                break;
                default:
                    APP_DBG_PRINT("\r\nTask zmod4510 task delete in seq 6:%d\r\n", err)
                    ;
                break;
            }

            /* Wait 10ms */
            vTaskDelay (5);
        }
        break;

        case DEMO_SEQUENCE_7:
        {
            /* Calculate read data */
            err = g_zmod4xxx_sensor1.p_api->oaq1stGenDataCalculate (g_zmod4xxx_sensor1.p_ctrl, &raw_data1,
                                                                    &zmod4xxx_data);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_1;

                /* Set data */
                gs_demo_air_quality = zmod4xxx_data.aiq;
                print_float (flt_oaq_str1, sizeof(flt_oaq_str1), gs_demo_air_quality);
                APP_DBG_PRINT("\n ZMOD4510 OAQ DATA \r\n OAQ = %s \r\n", flt_oaq_str1);
            }
            else if (FSP_ERR_SENSOR_IN_STABILIZATION == err)
            {
                sequence = DEMO_SEQUENCE_1;
            }
            else
            {
                APP_DBG_PRINT("\r\nTask zmod4510 task delete in seq 7:%d\r\n", err);
            }

            /* Delay required time */
            vTaskDelay (RM_ZMOD4XXX_ALG_REQ_DELAY_IN_MS);
        }
        break;

        default:
        {
            APP_DBG_PRINT("\r\nTask zmod4510 task delete in default case:%d\r\n", err);
        }
        break;
    }
}

