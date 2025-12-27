/***********************************************************************************************************************
 * File Name    : sensor_thread_entry.c
 * Description  : Contains data structures and functions
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include <sensors_thread.h>
#include "RA_HS3001.h"
#include "RA_ZMOD4XXX_Common.h"
#include "common_utils.h"
#include "user_choice.h"
#include "icm.h"
#include "ICM42605.h"
#include "usr_data.h"
#include "icp.h"
#include "ICP_20100.h"
#include "RmcI2C.h"


#define UNUSED(x)  ((void)(x))
#define INT_CHANNEL (1)

void RA_Init(void);
void updateData(void);
void g_comms_i2c_bus1_quick_setup(void);
void g_comms_i2c_bus2_quick_setup(void);



extern TaskHandle_t sensors_thread;
extern TaskHandle_t oximeter_thread;
extern TaskHandle_t zmod_thread;
extern TaskHandle_t console_thread;

/*******************************************************************************************************************//**
 * @brief       Send ID to queue 
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void updateData(void)
{

    mqtt_rx_payload_t payload_data = {'\0'};
    static  uint8_t  msgId = ID_IAQ_DATA_PUSH;
    BaseType_t xHigherPriorityTaskWokenByPost = pdFALSE;

    if (ID_IAQ_DATA_PUSH == msgId)
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
    else
    {
        /* Do nothing */
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

    /* Recover if I2C0 bus is busy */
    bsp_recover_iic(I2C_SCL_0, I2C_SDA_0);

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
 * @brief       Quick setup for g_comms_i2c_bus1
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_comms_i2c_bus1_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t *p_driver_instance = (i2c_master_instance_t*) g_comms_i2c_bus1_extended_cfg.p_driver_instance;
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

    /* Recover if I2C1 bus is busy */
    bsp_recover_iic(I2C_SCL_1, I2C_SDA_1);

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore)
    {
        *(g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_handle) = xSemaphoreCreateCountingStatic (
                (UBaseType_t) 1, (UBaseType_t) 0,
                g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex)
    {
        *(g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_handle) = xSemaphoreCreateRecursiveMutexStatic (
                g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
    }
}

/*******************************************************************************************************************//**
 * @brief       Quick setup for g_comms_i2c_bus1
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_comms_i2c_bus2_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t *p_driver_instance = (i2c_master_instance_t*) g_comms_i2c_bus2_extended_cfg.p_driver_instance;
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

    /* Recover if I2C2 bus is busy */
    bsp_recover_iic(I2C_SCL_2, I2C_SDA_2);

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore)
    {
        *(g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_handle) = xSemaphoreCreateCountingStatic (
                (UBaseType_t) 1, (UBaseType_t) 0,
                g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex)
    {
        *(g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_handle) = xSemaphoreCreateRecursiveMutexStatic (
                g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
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

#if _ICP20100_SENSOR_ENABLE_
    /* opening ExternalIRQ for IRQ6 P301 for ICP-20100 */
    err = R_ICU_ExternalIrqOpen(&g_external_irq6_ctrl, &g_external_irq6_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP-20100 Sensor External Irq Open failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nICP-20100 Sensor External Irq Open success \r\n");
    }
    err = R_ICU_ExternalIrqEnable(&g_external_irq6_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICP-20100 Sensor External Irq Enable failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nICP-20100 Sensor External Irq Enable success \r\n");
    }
#endif

#if _ICM42605_SENSOR_ENABLE_
    err = R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq12 Open failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq12 Open success \r\n");
    }

    err = R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq12 Enable failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq12 Enable success \r\n");
    }

    err = R_ICU_ExternalIrqOpen(&g_external_irq3_ctrl, &g_external_irq3_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq3 Open failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq3 Open success \r\n");
    }

    err = R_ICU_ExternalIrqEnable(&g_external_irq3_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq3 Enable failed \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor External Irq3 Enable success \r\n");
    }
#endif
}

/*******************************************************************************************************************//**
 * @brief       Reset zmod sensor
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
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

/*******************************************************************************************************************//**
 * @brief       sensor_thread_entry function
 * @param[in]   pvParameters
 * @retval      None
 ***********************************************************************************************************************/
void sensors_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    RA_Init ();
    g_comms_i2c_bus0_quick_setup ();
    g_comms_i2c_bus1_quick_setup ();
    g_comms_i2c_bus2_quick_setup ();
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

#if _ZMOD4510_SENSOR_ENABLE_
    /* Open ZMOD4510 */
    g_zmod4xxx_sensor1_quick_setup ();
#endif

    /* Start sensing ZMOD 4410 and 4510 sensor data */
    xTaskNotifyFromISR(zmod_thread, 1, 1, NULL);


#if _ICP20100_SENSOR_ENABLE_
    RmComDevice_init();
    icp_setup();
#endif

#if _ICM42605_SENSOR_ENABLE_
    RmComDevice_init_Icm ();
    icm_setup();
#endif

    /* Sensor initializations are done. Now notify the Console thread, so that User Menu can be displayed on the Console. */
	xTaskNotifyFromISR(console_thread, 1, 1, NULL);

    /* wait for application thread to finish MQTT connection */
    xTaskNotifyWait(pdFALSE, pdFALSE, (uint32_t* )&sensors_thread, portMAX_DELAY);

    /* start user timer */
    user_timer_start ();
    while (1)
    {

#if _HS3001_SENSOR_ENABLE_
        /* Read HS3001 sensor data */
        hs3001_get ();
#endif
#if _ICP20100_SENSOR_ENABLE_
        /* Read ICP sensor data */
        icp_get ();
#endif
#if _ICM42605_SENSOR_ENABLE_
        /* Read ICM sensor data */
        icm42605_loop();
#endif
        vTaskDelay (5);
    }
}
