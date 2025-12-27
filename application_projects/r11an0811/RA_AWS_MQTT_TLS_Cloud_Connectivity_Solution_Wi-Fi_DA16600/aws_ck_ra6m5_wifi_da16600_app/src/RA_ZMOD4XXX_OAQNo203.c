/***********************************************************************************************************************
 * File Name    : RA_ZMOD4XXX_OAQNo203.c
 * Description  : Contains data structures and function definitions for ZMOD4510 sensor data read
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <sensors_thread.h>
#include "hal_data.h"
#include "RA_ZMOD4XXX_Common.h"
#include "common_utils.h"

static demo_sequence_t sequence = DEMO_SEQUENCE_1;
float gs_demo_air_quality;
rm_zmod4xxx_raw_data_t raw_data1;

/*******************************************************************************************************************//**
 * @brief       Start reading ZMOD4510 data
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void start_oaq_no2_o3(void)
{
    fsp_err_t err;
    rm_zmod4xxx_no2_o3_data_t zmod4xxx_data;
	

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
            err = g_zmod4xxx_sensor1.p_api->no2O3DataCalculate (g_zmod4xxx_sensor1.p_ctrl, &raw_data1,
                                                                    &zmod4xxx_data);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_1;

                /* Set data */
                gs_demo_air_quality = zmod4xxx_data.epa_aqi;
                APP_DBG_PRINT("\n ZMOD4510 OAQ DATA \r\n OAQ = %f \r\n", gs_demo_air_quality);
            }
            else if (FSP_ERR_SENSOR_IN_STABILIZATION == err)
            {
                sequence = DEMO_SEQUENCE_1;
            }
            else
            {
                APP_DBG_PRINT("\r\nTask zmod4510 task delete in seq 7:%d\r\n", err);
            }

        }
        break;

        default:
        {
            APP_DBG_PRINT("\r\nTask zmod4510 task delete in default case:%d\r\n", err);
        }
        break;
    }
    xQueueOverwrite(g_oaq_queue, &gs_demo_air_quality);
}

