/***********************************************************************************************************************
 * File Name    : application_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#include "application_thread.h"
#include "common_utils.h"
#include <usr_app.h>
#include <usr_hal.h>
#include <usr_network.h>
#include "user_choice.h"
#include "ICM42605.h"

extern TX_THREAD c2d_thread;

extern UINT sample_environmental_sensor_telemetry_messages_send (void);
extern UINT sample_barometric_sensor_telemetry_messages_send(void);
extern UINT sample_gas_telemetry_messages_send(void);
extern UINT sample_inertial_sensor_telemetry_messages_send(void);
extern UINT sample_gas_oaq_sensor_telemetry_messages_send(void);
extern UINT sample_biometric_telemetry_messages_send(void);

void temp_cold(void);
void temp_warm(void);
void temp_hot(void);
void TricolorLEDOFF(void);
void TC_RED_ON (void);
void TC_RED_OFF (void);
void TC_BLUE_ON (void);
void TC_BLUE_OFF (void);
void TC_GREEN_ON (void);
void TC_GREEN_OFF (void);
extern  unsigned int DecodeBASE64(char *incertbuff,unsigned char *outcerthexbuff, const int length_out);
extern char g_certificate[];
extern char g_private_key[];

UCHAR sample_device_cert_ptr[1024];
unsigned int sample_device_cert_len=0;
UCHAR sample_device_private_key_ptr[2048];
unsigned int sample_device_private_key_len=0;

usr_ob1203_data_t g_ob1203_data_r;
usr_hs3001_data_t g_hs3001_data_r;
usr_icp_data_t g_icp_data_r;
usr_icm_acc_gyr_data_t g_icm_acc_gyr_data_r;
usr_iaq_data_t g_iaq_data_r;
usr_oaq_data_t g_oaq_data_r;

char tempstaring[20]="\0";

/* application_thread_entry function */
void application_thread_entry(void)
{
    UINT status = EXIT_SUCCESS;
    mqtt_rx_payload_t mq_data =  { RESET_VALUE };
    fsp_pack_version_t version = {RESET_VALUE};
    TricolorLEDOFF();
    //version get API for FLEX pack information
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, AP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(AP_INFO);

    IotLog("HAL Initialization  \r\n");
    status = usr_hal_init ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErrTrap(status);
    }

    status = _nx_crypto_initialize ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErrTrap(status);
    }

    status = usr_packet_pool_init ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErrTrap(status);
    }

    status = usr_netx_init ();
    if (EXIT_SUCCESS != status)
    {
        IotLogErrTrap(status);
    }
    sample_device_cert_len = DecodeBASE64(&g_certificate[0], &sample_device_cert_ptr[0], sizeof(sample_device_cert_ptr));
    sample_device_private_key_len = DecodeBASE64(&g_private_key[0], &sample_device_private_key_ptr[0], sizeof(sample_device_private_key_ptr));

    sample_entry (&g_ip0, &g_packet_pool0, &g_dns0, unix_time_get);

    /* Resume the thread represented by "c2d_thread" */
    status = tx_thread_resume (&c2d_thread);

    do
    {
        status = tx_queue_receive (&g_topic_queue, (VOID*) &mq_data, TX_WAIT_FOREVER);

        if (TX_SUCCESS == status)
        {
            switch (mq_data.id)
            {
                case ID_HS3001:
                {
#if _HS3001_SENSOR_ENABLE_
                    IotLog("\n HS3001 DATA SEND \r\n");
                    if (EXIT_SUCCESS != sample_environmental_sensor_telemetry_messages_send ())
                    {
                        IotLogErr("** HS3001 Sensor Message Publish failed **\r\n");
                        FAILURE_INDICATION;
                    }
                    else
                    {
                        ACTIVITY_INDICATION;
                    }
#endif
                }
                break;

                case ID_ZMOD4410:
                {
#if _ZMOD4410_SENSOR_ENABLE_
                    IotLog("\n ZMOD4410 DATA SEND  \r\n");
                    if (EXIT_SUCCESS != sample_gas_telemetry_messages_send ())
                    {
                        IotLogErr("** ZMOD4410 Sensor Message Publish failed **\r\n");
                        FAILURE_INDICATION;
                    }
                    else
                    {
                        ACTIVITY_INDICATION;
                    }
#endif
                }
                break;

                case ID_ICP20100:
                {
#if _ICP20100_SENSOR_ENABLE_
                    IotLog("\n ICP DATA SEND  \r\n");

                    if (EXIT_SUCCESS != sample_barometric_sensor_telemetry_messages_send ())
                    {
                        IotLogErr("** ICP20100 Sensor Message Publish failed **\r\n");
                        FAILURE_INDICATION;
                    }
                    else
                    {
                        ACTIVITY_INDICATION;
                    }
#endif
                }
                break;

                case ID_ICM42605:
                {
#if _ICM42605_SENSOR_ENABLE_
                    IotLog("\n ICM DATA SEND  \r\n");

                    if (EXIT_SUCCESS != sample_inertial_sensor_telemetry_messages_send ())
                    {
                        IotLogErr("** ICM42605 Sensor Message Publish failed **\r\n");
                        FAILURE_INDICATION;
                    }
                    else
                    {
                        ACTIVITY_INDICATION;
                    }
#endif
                }
                break;

                case ID_ZMOD4510:
                {
#if _ZMOD4510_SENSOR_ENABLE_
                    IotLog("\n ZMOD4510 DATA SEND  \r\n");

                    if (EXIT_SUCCESS != sample_gas_oaq_sensor_telemetry_messages_send ())
                    {
                        IotLogErr("** ZMOD4510 Sensor Message Publish failed **\r\n");
                        FAILURE_INDICATION;
                    }
                    else
                    {
                        ACTIVITY_INDICATION;
                    }
#endif
                }
                break;

                case ID_OB1203:
                    {
#if _OB1203_SENSOR_ENABLE_
                        IotLog("\n OB1203 DATA SEND  \r\n");

                        if (EXIT_SUCCESS != sample_biometric_telemetry_messages_send ())
                        {
                            IotLogErr("** OB1203 Sensor Message Publish failed **\r\n");
                            FAILURE_INDICATION;
                        }
                        else
                        {
                            ACTIVITY_INDICATION;
                        }
#endif
                    }
                    break;

                case ID_LED_ACT:
                {
                    IotLog("Topic Received from Cloud %s \r\n",mq_data.value.led_data.led_act_topic_msg);
                    if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_BLUE_ON"))
                    {
                        TC_BLUE_ON();
                        IotLog("\r\n 3CBLUE LED ON\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_BLUE_OFF"))
                    {
                        TC_BLUE_OFF();
                        IotLog("\r\n 3CBLUE LED OFF\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_GREEN_ON"))
                    {
                        TC_GREEN_ON();
                        IotLog("\r\n 3CGREEN LED ON\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_GREEN_OFF"))
                    {
                        TC_GREEN_OFF();
                        IotLog("\r\n 3CGREEN LED OFF\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_RED_ON"))
                    {
                        TC_RED_ON();
                         IotLog("\r\n 3CRED LED ON\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_RED_OFF"))
                    {
                        TC_RED_OFF();
                         IotLog("\r\n 3CRED LED OFF\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "TC_NONE"))
                    {
                        TricolorLEDOFF();
                         IotLog("\r\n TC ALL LED OFF\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "BLUE_ON"))
                    {
                        led_on_off(LED_BLUE,LED_ON);
                         IotLog("\r\n BLUE LED ON\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "BLUE_OFF"))
                    {
                        led_on_off(LED_BLUE,LED_OFF);
                         IotLog("\r\n BLUE LED OFF\r\n");
                    }
                    else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "RED_ON"))
					 {
						 led_on_off(LED_RED,LED_ON);
						  IotLog("\r\n RED LED ON\r\n");
					 }
					 else if (RESET_VALUE == strcmp ((char*) mq_data.value.led_data.led_act_topic_msg, "RED_OFF"))
					 {
						 led_on_off(LED_RED,LED_OFF);
						  IotLog("\r\n RED LED OFF\r\n");
					 }
                    else
                    {
                        IotLog("\r\nMsg Topic didn't match !!!\r\n");
                    }
                    ACTIVITY_INDICATION;
                }
                break;

                case ID_DISPLAY_HS3001:
				   {
#if _HS3001_SENSOR_ENABLE_
                        status = tx_queue_receive (&g_hs3001_queue, &g_hs3001_data_r, TX_NO_WAIT);
                        if(TX_SUCCESS != status)
                        {
                            g_hs3001_data_r.gs_humidity = 0;
                            g_hs3001_data_r.gs_temperature = 0;
                            APP_DBG_PRINT("Can not get value of HS3001 sensor \r\n");
                        }
                        else
                        {
                            /* Do nothing */
                        }
                        APP_PRINT("\n HS3001 \r\n TEMP = %f \r\n", g_hs3001_data_r.gs_temperature);
                        APP_PRINT(" HUMIDITY = %f \r\n", g_hs3001_data_r.gs_humidity);
#endif
					}
					break;

				case ID_DISPLAY_ICM42605:
				   {
#if _ICM42605_SENSOR_ENABLE_
                        status = tx_queue_receive (&g_icm_queue, &g_icm_acc_gyr_data_r, TX_NO_WAIT);
                        if(TX_SUCCESS != status)
                        {
                           g_icm_acc_gyr_data_r.acc_data.x = 0;
                           g_icm_acc_gyr_data_r.acc_data.y = 0;
                           g_icm_acc_gyr_data_r.acc_data.z = 0;
                           g_icm_acc_gyr_data_r.gyr_data.x = 0;
                           g_icm_acc_gyr_data_r.gyr_data.y = 0;
                           g_icm_acc_gyr_data_r.gyr_data.z = 0;
                           APP_DBG_PRINT("Can not get the value of the Accelerometer and Gyrometer - ICM sensor \r\n");
                        }
                        else
                        {
                            /* Do nothing */
                        }

                        APP_PRINT("\n ICM42605 \r\n accelerometerX = %f \r\n", g_icm_acc_gyr_data_r.acc_data.x);
                        APP_PRINT(" accelerometerY = %f \r\n", g_icm_acc_gyr_data_r.acc_data.y);
                        APP_PRINT(" accelerometerZ = %f \r\n", g_icm_acc_gyr_data_r.acc_data.z);

						APP_PRINT(" gyroscopeX = %f \r\n", g_icm_acc_gyr_data_r.gyr_data.x);
						APP_PRINT(" gyroscopeY = %f \r\n", g_icm_acc_gyr_data_r.gyr_data.y);
						APP_PRINT(" gyroscopeZ = %f \r\n", g_icm_acc_gyr_data_r.gyr_data.z);

                        /* No support for ICM Magnotometer Sensor Data */
						APP_PRINT(" magX = %f \r\n", 0);
						APP_PRINT(" magY = %f \r\n", 0);
						APP_PRINT(" magZ = %f \r\n", 0);
#endif
					}
					break;

				case ID_DISPLAY_ICP20100:
				   {
#if _ICP20100_SENSOR_ENABLE_
                        status = tx_queue_receive (&g_icp_queue, &g_icp_data_r, TX_NO_WAIT);
                        if(TX_SUCCESS != status)
                        {
                            g_icp_data_r.pressure_Pa = 0;
                            g_icp_data_r.temperature_C = 0;
                            APP_DBG_PRINT("Can not get value of ICP20100 sensor \r\n");
                        }
                        else
                        {
                            /* Do nothing */
                        }
                        APP_PRINT("\n ICP20100 \r\n TEMP = %f \r\n", g_icp_data_r.temperature_C);
                        APP_PRINT(" ChPressure = %f \r\n", g_icp_data_r.pressure_Pa);
#endif
				   }
					break;

				case ID_DISPLAY_OB1203:
				   {
#if _OB1203_SENSOR_ENABLE_
                        status = tx_queue_receive (&g_ob1203_queue, &g_ob1203_data_r, TX_NO_WAIT);
                        if(TX_SUCCESS != status)
                        {
                            g_ob1203_data_r.spo2_val = 0;
                            g_ob1203_data_r.heart_rate_Val = 0;
                            g_ob1203_data_r.breathing_rate = 0;
                            g_ob1203_data_r.r_p2p = 0;
                            APP_DBG_PRINT("Can not get value of OB1203 sensor \r\n");
                        }
                        else
                        {
                            /* Do nothing */
                        }
                        APP_PRINT("\n OB1203 \r\n spo2_val = %d \r\n", g_ob1203_data_r.spo2_val);
                        APP_PRINT(" heart_rate_Val = %d \r\n", g_ob1203_data_r.heart_rate_Val);
                        APP_PRINT(" breathing_rate = %d \r\n", g_ob1203_data_r.breathing_rate);
                        APP_PRINT(" r_p2p = %f \r\n", g_ob1203_data_r.r_p2p);
#endif
				   }
					break;

				case ID_DISPLAY_ZMOD4410:
				   {
#if _ZMOD4410_SENSOR_ENABLE_
                        status = tx_queue_receive (&g_iaq_queue, &g_iaq_data_r, TX_NO_WAIT);
                        if(TX_SUCCESS != status)
                        {
                           g_iaq_data_r.gs_eco2 = 0;
                           g_iaq_data_r.gs_etoh = 0;
                           g_iaq_data_r.gs_tvoc = 0;
                           APP_DBG_PRINT("Can not get value of ZMOD4410 sensor \r\n");
                        }
                        else
                        {
                           /* Do nothing */
                        }
                        APP_PRINT("\n ZMOD4410 \r\n ECO2 = %f \r\n", g_iaq_data_r.gs_eco2);
                        APP_PRINT(" ETOH = %f \r\n", g_iaq_data_r.gs_etoh);
                        APP_PRINT(" TVOC = %f \r\n", g_iaq_data_r.gs_tvoc);
#endif
				   }
				   break;

				case ID_DISPLAY_ZMOD4510:
				   {
#if _ZMOD4510_SENSOR_ENABLE_
                        status = tx_queue_receive (&g_oaq_queue, &g_oaq_data_r, TX_NO_WAIT);
                        if(TX_SUCCESS != status)
                        {
                            g_oaq_data_r.gs_air_quality = 0;
                            APP_DBG_PRINT("Can not get value of ZMOD4510 sensor \r\n");
                        }
                        else
                        {
                            /* Do nothing */
                        }
                        APP_PRINT("\n ZMOD4510 \r\n OAQ = %f \r\n", g_oaq_data_r.gs_air_quality);
#endif
				    }
					break;

                default:
                {
                    IotLogErr("** Error in MQTT Messages.. MQTT Client Cleanup started  **\r\n");
                    FAILURE_INDICATION;
                }
                break;
            }
            memset (&mq_data.value.led_data.led_act_topic_msg, '\0', sizeof(mq_data.value.led_data.led_act_topic_msg));
        }

    }    while (1);

}

/*******************************************************************************************************************//**
 * @brief       BLUE LED 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void temp_cold(void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_HIGH); // red
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03, BSP_IO_LEVEL_HIGH); // green
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_LOW); // BLUE
    tx_thread_sleep (10);
    R_BSP_PinAccessDisable ();

}

/*******************************************************************************************************************//**
 * @brief       GREEN LED 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void temp_warm(void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_HIGH); // red
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03, BSP_IO_LEVEL_LOW); // green
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_HIGH); // BLUE
    tx_thread_sleep (10);
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       RED LED 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void temp_hot(void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_LOW); // red
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03, BSP_IO_LEVEL_HIGH); // green
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_HIGH); // BLUE
    tx_thread_sleep (10);
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       TricolorLED OFF 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TricolorLEDOFF(void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_HIGH); // red
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03, BSP_IO_LEVEL_HIGH); // green
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_HIGH); // BLUE
    tx_thread_sleep (10);
    R_BSP_PinAccessDisable ();

}

/*******************************************************************************************************************//**
 * @brief       TricolorLED_RED ON 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TC_RED_ON (void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_LOW); // red
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       TricolorLED_RED OFF 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TC_RED_OFF (void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_HIGH); // red
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       TricolorLED_BLUE ON 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TC_BLUE_ON (void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_LOW); // blue
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       TricolorLED_BLUE OFF 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TC_BLUE_OFF (void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_HIGH); // blue
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       TricolorLED_GREEN ON 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TC_GREEN_ON (void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03, BSP_IO_LEVEL_LOW); // green
    R_BSP_PinAccessDisable ();
}

/*******************************************************************************************************************//**
 * @brief       TricolorLED_GREEN OFF 
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void TC_GREEN_OFF (void)
{
    R_BSP_PinAccessEnable ();
    tx_thread_sleep (10);
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_06_PIN_03, BSP_IO_LEVEL_HIGH); // green
    R_BSP_PinAccessDisable ();
}
