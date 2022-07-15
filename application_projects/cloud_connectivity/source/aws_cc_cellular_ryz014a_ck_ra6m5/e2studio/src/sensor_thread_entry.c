/***********************************************************************************************************************
 * File Name    : sensor_thread_entry.c
 * Description  : Contains data structures and functions
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
#include "sensor_thread.h"
#include "RA_HS3001.h"
#include "RA_ZMOD4XXX_Common.h"
#include "common_utils.h"
#include "user_choice.h"
#include "ICP_10101.h"
#include "RmcI2C.h"
#include "ICM_20948.h"
#include "usr_data.h"

void RA_Init(void);
void updateData(void);

#define UNUSED(x)  (void)(x)
#define INT_CHANNEL (1)

extern volatile iaq_demo_data_t g_iaq_data;
extern volatile float gs_demo_air_quality;
extern volatile sensor_demo_data_t g_demo_data;
extern uint16_t spo2_val; //value output to display
extern uint16_t heart_Rate_Val; //value output to display
extern uint16_t breathing_rate;
extern float r_p2p;
extern xyzFloat corrAccRaw, gVal, magValue;

/* For testing purpose later needs to be moved to the right location */
extern usr_iaq_data_t app_iaq_data;
extern usr_oaq_data_t app_oaq_data;
extern usr_hs3001_data_t app_hs3001_data;
extern usr_icp_data_t app_icp_data;
extern usr_ob1203_data_t app_ob1203_data;
extern float Temperature, Pressure;
extern usr_icm_data_t app_icm_data;

extern uint32_t  console_status;
extern TaskHandle_t sensor_thread;
extern TaskHandle_t oximeter_thread;
extern TaskHandle_t zmod_thread;

uint32_t  sensor_status = RESET_VALUE;


void RA_Init(void);
void updateData(void)
{

        mqtt_rx_payload_t payload_data = {'\0'};
        static  uint8_t  msgId = ID_IAQ_DATA_PUSH;
        BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;

        app_iaq_data.gs_tvoc = g_iaq_data.gs_demo_tvoc;
        app_iaq_data.gs_eco2 = g_iaq_data.gs_demo_eco2;
        app_iaq_data.gs_etoh = g_iaq_data.gs_demo_etoh;

        app_oaq_data.gs_air_quality = gs_demo_air_quality;

        app_hs3001_data.gs_humidity = g_demo_data.gs_demo_humidity;
        app_hs3001_data.gs_temperature = (g_demo_data.gs_demo_temperature * (float)1.8)+(float)32.0;

        app_icm_data.acc_data.x = corrAccRaw.x;
        app_icm_data.acc_data.y = corrAccRaw.y;
        app_icm_data.acc_data.z = corrAccRaw.z;
        app_icm_data.mag_data.x = magValue.x;
        app_icm_data.mag_data.y = magValue.y;
        app_icm_data.mag_data.z = magValue.z;
        app_icm_data.gyr_data.x = gVal.x;
        app_icm_data.gyr_data.y = gVal.y;
        app_icm_data.gyr_data.z = gVal.z;

        app_icp_data.pressure_Pa = Pressure;
        app_icp_data.temperature_C = (Temperature * (float)1.8)+(float)32.0;

        app_ob1203_data.breathing_rate = breathing_rate;
        app_ob1203_data.heart_rate_Val = heart_Rate_Val;
        app_ob1203_data.r_p2p = r_p2p;
        app_ob1203_data.spo2_val = spo2_val;


        if(ID_IAQ_DATA_PUSH == msgId)
        {
            payload_data.id = ID_IAQ_DATA_PUSH;
            xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);
            msgId = ID_OAQ_DATA_PUSH;
        }
        else if (ID_OAQ_DATA_PUSH == msgId)
        {
            payload_data.id = ID_OAQ_DATA_PUSH;
            xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);
            msgId = ID_HS_DATA_PUSH;
        }
        else if (ID_HS_DATA_PUSH == msgId)
        {
            payload_data.id = ID_HS_DATA_PUSH;
            xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);
            msgId = ID_ICM_DATA_PUSH;
        }
        else if (ID_ICM_DATA_PUSH == msgId)
        {
            payload_data.id = ID_ICM_DATA_PUSH;
            xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);
            msgId = ID_ICP_DATA_PUSH;
        }
        else if (ID_ICP_DATA_PUSH == msgId)
        {
            payload_data.id = ID_ICP_DATA_PUSH;
            xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);
            msgId = ID_OB1203_DATA_PUSH;
        }
        else if (ID_OB1203_DATA_PUSH == msgId)
        {
            payload_data.id = ID_OB1203_DATA_PUSH;
            xQueueGenericSendFromISR (g_topic_queue, &payload_data, &xHigherPriorityTaskWokenByPost, queueSEND_TO_FRONT);
            msgId = ID_IAQ_DATA_PUSH;
        }
}


/*******************************************************************************************************************//**
 * @brief       Quick setup for g_comms_i2c_bus0
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_comms_i2c_bus0_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t *p_driver_instance = (i2c_master_instance_t*) g_comms_i2c_bus0_extended_cfg.p_driver_instance;
    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open (p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nI2C bus setup unsuccess\r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nI2C bus setup success\r\n");
    }

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
    {
        *(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle) = xSemaphoreCreateCountingStatic (
                (UBaseType_t) 1, (UBaseType_t) 0,
                g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
    {
        *(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle) = xSemaphoreCreateRecursiveMutexStatic (
                g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
    }
}

/*******************************************************************************************************************//**
 * @brief       Initialization of UART and external interrupt
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void RA_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version =
    { RESET_VALUE };
    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* opening ExternalIRQ for IRQ14 P403 for OB1203 */
    err = R_ICU_ExternalIrqOpen (&g_sensorIRQ_ctrl, &g_sensorIRQ_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nOB1203 Sensor External Irq Open failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nOB1203 Sensor External Irq Open success \r\n");
    }
}

static void reset_zmod_sensor(void)
{
    R_BSP_PinAccessEnable ();

    /* ZMOD Reset for CK-RA5M5 */
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_HIGH); // 4510
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH); // 4410
    vTaskDelay (10);

    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_LOW); // 4510
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_LOW); // 4410
    vTaskDelay (10);

    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_HIGH); // 4510
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH); // 4410
    vTaskDelay (10);

    R_BSP_PinAccessDisable ();
}

/* Sensor_Thread entry function */
/* pvParameters contains TaskHandle_t */
void sensor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* wait for application thread to finish MQTT connection */
    xTaskNotifyWait(pdFALSE, pdFALSE, (uint32_t* )&sensor_status, portMAX_DELAY);
    RA_Init ();
    g_comms_i2c_bus0_quick_setup ();
    reset_zmod_sensor ();

    /* Start Oximeter thread execution */
    xTaskNotifyFromISR(oximeter_thread, 1, 1, NULL);

#if  _HS3001_SENSOR_ENABLE_
    /* Open HS3001 */
    g_hs300x_sensor0_quick_setup ();
#endif

#if  _ZMOD4410_SENSOR_ENABLE_
    /* Open ZMOD4410 */
    g_zmod4xxx_sensor0_quick_setup ();
#endif

    /* Start sensing ZMOD 4410 sensor datat */
    xTaskNotifyFromISR(zmod_thread, 1, 1, NULL);

#if _ZMOD4510_SENSOR_ENABLE_
    /* Open ZMOD4510 */
    g_zmod4xxx_sensor1_quick_setup ();
#endif

#if	_ICP10101_SENSOR_ENABLE_
    /* Initialize ICP10101 sensor */
    RmComDevice_init ();
    begin ();
    measureStart (VERY_ACCURATE);
#endif

#if _ICM20948_SENSOR_ENABLE_
    /* Open ICM20948 */
    RmComDevice_init_Icm ();
    ICM20948_Sensor_init ();
#endif

    /* start user timer */
    user_timer_start ();
    while (1)
    {

#if _HS3001_SENSOR_ENABLE_
        /* Read HS3001 sensor data */
        hs3001_get ();
        vTaskDelay (5);
#endif

#if  _ICP10101_SENSOR_ENABLE_
        /* Read ICP10101 sensor data */
        ICP_10101_get ();
        vTaskDelay (5);
#endif

#if _ZMOD4510_SENSOR_ENABLE_
        /* Read ZMOD4510 sensor data */
        start_oaq_1st_gen ();
        vTaskDelay (5);
#endif

#if _ICM20948_SENSOR_ENABLE_
        /* Read ICM20948 sensor data */
        ICM_20948_get ();
        vTaskDelay (5);
#endif

    }
}
