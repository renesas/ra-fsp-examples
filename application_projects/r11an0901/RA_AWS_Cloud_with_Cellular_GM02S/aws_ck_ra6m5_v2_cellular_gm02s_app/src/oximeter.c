/***********************************************************************************************************************
 * File Name    : oximeter.c
 * Description  : Contains data structure and function definitions of OB1203 sensor data calculation
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

#include "oximeter.h"
#include "user_choice.h"
#include "usr_data.h"
#include "OB1203_Config.h"

typedef enum e_demo_sequence
{
    DEMO_SEQUENCE_1 = (1),
    DEMO_SEQUENCE_2,
    DEMO_SEQUENCE_3,
    DEMO_SEQUENCE_4,
    DEMO_SEQUENCE_5,
    DEMO_SEQUENCE_6,
    DEMO_SEQUENCE_7,
    DEMO_SEQUENCE_8,
    DEMO_SEQUENCE_9,
    DEMO_SEQUENCE_10,
    DEMO_SEQUENCE_11,
    DEMO_SEQUENCE_12,
} demo_sequence_t;

/* Static variables */
static spo2_t gs_spo2;
static volatile ob1203_bio_data_t gs_ob1203_bio_data;

/* value output to display */
uint16_t spo2_val;
uint16_t heart_Rate_Val;
uint16_t breathing_rate;
float r_p2p;
usr_ob1203_data_t ob1203_data_s;

bool result;
rm_ob1203_raw_data_t g_raw_data;
rm_ob1203_ppg_data_t ppg_data;
ob1203_bio_t ob1203_bio;
bool change = false;
bool valid = false;
bool update = false;
bool ready = false;
ob1203_bio_gain_currents_t gain_currents;
demo_sequence_t sequence = DEMO_SEQUENCE_1;

static void print_ob_data();

/***********************************************************************************************************************
 * @brief       Initialization of ob1203 sensor
 * @param       None
 * @retval      None        
 **********************************************************************************************************************/
void RA_ob1203_init(void)
{
    /* Set default gain and currents */
    gain_currents.gain.ppg_prox = g_ob1203_sensor0_extended_cfg.ppg_prox_gain;
    gain_currents.currents.ir_led = g_ob1203_sensor0_extended_cfg.ppg_ir_led_current;
    gain_currents.currents.red_led = g_ob1203_sensor0_extended_cfg.ppg_red_led_current;

    /* Open OB1203 Bio extension */
    result = ob1203_bio_open(&ob1203_bio,
                             (rm_ob1203_instance_t*)&g_ob1203_sensor1, // Proximity mode
                             (rm_ob1203_instance_t*)&g_ob1203_sensor0,  // PPG mode
                             &gs_spo2);
    if (false == result)
    {
    	APP_PRINT ("\r\nOB1203 sensor setup fail \r\n");
    }
    else
    {
    	APP_PRINT ("\r\nOB1203 sensor setup success \r\n");
    }

}

/***********************************************************************************************************************
 * @brief       Main activities of ob1203 sensor    
 * @param       None
 * @retval      None        
 **********************************************************************************************************************/
void ob1203_spo2_main(void)
{
    {
        switch (sequence)
        {
            case DEMO_SEQUENCE_1 :
            {
                /* Initialize an operation mode */
                result = ob1203_bio_operation_mode_init(&ob1203_bio);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_operation_mode_init failed\r\n");
                }

                sequence = DEMO_SEQUENCE_2;
            }
            break;

            case DEMO_SEQUENCE_2 :
            {
                /* Start a measurement */
                result = ob1203_bio_measurement_start(&ob1203_bio);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_measurement_start failed\r\n");
                }

                sequence = DEMO_SEQUENCE_3;
            }
            break;

            case DEMO_SEQUENCE_3:
            {
                /* Wait measurement period */
                result = ob1203_bio_measurement_period_wait(&ob1203_bio);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_measurement_period_wait failed\r\n");
                }

                sequence = DEMO_SEQUENCE_4;
            }
            break;

            case DEMO_SEQUENCE_4 :
            {
                /* Check if an operation mode needs to be changed */
                result = ob1203_bio_mode_change_check(&ob1203_bio, &change);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_mode_change_check failed\r\n");
                }

                if (false != change)
                {
                    /* Stop the measurement */
                    result = ob1203_bio_measurement_stop(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_measurement_stop failed\r\n");
                    }

                    /* Change to another mode */
                    sequence = DEMO_SEQUENCE_1;
                }
                else
                {
                    /* No change */
                    sequence = DEMO_SEQUENCE_5;
                }
            }
            break;

            case DEMO_SEQUENCE_5:
            {
                /* Read raw data */
                result = ob1203_bio_ppg_raw_data_read(&ob1203_bio, &g_raw_data);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_ppg_raw_data_read failed\r\n");
                }

                sequence = DEMO_SEQUENCE_6;
            }
            break;

            case DEMO_SEQUENCE_6:
            {
                /* Calculate PPG data from raw data */
                result = ob1203_bio_ppg_data_calculate(&ob1203_bio, &g_raw_data, &ppg_data, &valid);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_ppg_data_calculate failed\r\n");
                }

                if (false != valid)
                {
                    /* Valid data */
                    sequence = DEMO_SEQUENCE_7;
                }
                else
                {
                    /* Check if an operation mode needs to be changed */
                    result = ob1203_bio_mode_change_check(&ob1203_bio, &change);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_mode_change_check failed\r\n");
                    }


                    if (false != change)
                    {
                        /* Stop the measurement */
                        result = ob1203_bio_measurement_stop(&ob1203_bio);
                        if (false == result)
                        {
                            APP_PRINT ("\r\nOB1203_bio_measurement_stop failed\r\n");
                        }

                        spo2_val = 0;
                        heart_Rate_Val = 0;
                        breathing_rate = 0;
                        r_p2p = 0;
                        /* Change to another mode */
                        sequence = DEMO_SEQUENCE_1;
                    }
                    else
                    {
                        /* Invalid data */
                        sequence = DEMO_SEQUENCE_3;
                    }
                }
            }
            break;

            case DEMO_SEQUENCE_7:
            {
                /* Auto gain and currents control */
                result = ob1203_bio_auto_gain_currents_control(&ob1203_bio,
                                                               &ppg_data,
                                                               &gain_currents,
                                                               &update);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_auto_gain_currents_control failed\r\n");
                }

                if (false != update)
                {
                    /* Stop the measurement */
                    result = ob1203_bio_measurement_stop(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_measurement_stop failed\r\n");
                    }

                    /* Reconfigure gain and currents */
                    result = ob1203_bio_gain_currents_reconfigure(&ob1203_bio, &gain_currents);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_gain_currents_reconfigure failed\r\n");
                    }

                    sequence = DEMO_SEQUENCE_2;
                }
                else
                {
                    sequence = DEMO_SEQUENCE_8;
                }
            }
            break;

            case DEMO_SEQUENCE_8:
            {
                /* Check if the preparation for the algorithm is complete */
                result = ob1203_bio_algorithm_preparation_check(&ob1203_bio, &ready);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_algorithm_preparation_check failed\r\n");
                }

                if (false == ready)
                {
                    /* Stop the measurement */
                    result = ob1203_bio_measurement_stop(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_measurement_stop failed\r\n");
                    }

                    /* Reset the algorithm */
                    result = ob1203_bio_algorithm_reset(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_algorithm_reset failed\r\n");
                    }

                    /* Clear PPG samples */
                    result = ob1203_bio_samples_clear(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_samples_clear failed\r\n");
                    }

                    sequence = DEMO_SEQUENCE_2;
                }
                else
                {
                    sequence = DEMO_SEQUENCE_9;
                }
            }
            break;

            case DEMO_SEQUENCE_9:
            {
                /* Add PPG samples */
                result = ob1203_bio_samples_add(&ob1203_bio, &ppg_data);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_samples_add failed\r\n");
                }

                sequence = DEMO_SEQUENCE_10;
            }
            break;

            case DEMO_SEQUENCE_10:
            {
                /* Calculate heart rate and SpO2 values */
                result = ob1203_bio_hr_spo2_calculate(&ob1203_bio, (ob1203_bio_data_t *)&gs_ob1203_bio_data);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_hr_spo2_calculate failed\r\n");
                }
                else
                {
                    if ((gs_ob1203_bio_data.heart_rate != 0) || (gs_ob1203_bio_data.spo2 != 0))
                    {
                    	spo2_val = gs_ob1203_bio_data.spo2;
                    	heart_Rate_Val = gs_ob1203_bio_data.heart_rate;
                    	breathing_rate = gs_ob1203_bio_data.respiration_rate;
                    	r_p2p = gs_ob1203_bio_data.perfusion_index;
                    	print_ob_data();
                    }
                }

                sequence = DEMO_SEQUENCE_11;
            }
            break;

            case DEMO_SEQUENCE_11:
            {
                /* Calculate a respiration rate value */
                result = ob1203_bio_rr_calculate(&ob1203_bio,
                                                 (ob1203_bio_data_t *)&gs_ob1203_bio_data);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_rr_calculate failed\r\n");
                }
                sequence = DEMO_SEQUENCE_12;
            }
            break;

            case DEMO_SEQUENCE_12:
            {
                /* Check perfusion index (PI) */
                result = ob1203_bio_perfusion_index_check(&ob1203_bio,
                                                          &valid);
                if (false == result)
                {
                    APP_PRINT ("\r\nOB1203_bio_perfusion_index_check failed \r\n");
                }

                if (false != valid)
                {
                    sequence = DEMO_SEQUENCE_3;
                }
                else
                {
                    /* Stop the measurement */
                    APP_PRINT ("\r\n **Bio Perfusion Index : False \n");
                    result = ob1203_bio_measurement_stop(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_measurement_stop failed\r\n");
                    }

                    /* Reset the algorithm */
                    result = ob1203_bio_algorithm_reset(&ob1203_bio);
                    if (false == result)
                    {
                        APP_PRINT ("\r\nOB1203_bio_algorithm_reset failed\r\n");
                    }

                    sequence = DEMO_SEQUENCE_2;
                }
            }
            break;

            default:
            {
                APP_PRINT ("\r\nOB1203_bio DEFAULT\r\n");
            }
            break;
        }
    }

    /* Update value for ob1203_data variable */
    ob1203_data_s.r_p2p = r_p2p;
    ob1203_data_s.spo2_val = spo2_val;
    ob1203_data_s.heart_rate_Val = heart_Rate_Val;
    ob1203_data_s.breathing_rate = breathing_rate;

    xQueueOverwrite(g_ob1203_queue, &ob1203_data_s);
}

/***********************************************************************************************************************
 * @brief       Print ob1203's data.
 * @param       None
 * @retval      None        
 **********************************************************************************************************************/
static void print_ob_data(void)
{
#if _OB1203_SENSOR_ENABLE_
	APP_DBG_PRINT("\r\n SPO2 : %d  ", spo2_val);
	APP_DBG_PRINT(" Heart rate : %d  ", heart_Rate_Val);
	APP_DBG_PRINT(" breath rate : %d  ", breathing_rate);
	APP_DBG_PRINT("perfusion_index : %d \r\n ",r_p2p);
#endif
}
