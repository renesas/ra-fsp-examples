/***********************************************************************************************************************
 * File Name    : Sensor_Thread_entry.c
 * Description  : Contains data structures and functions
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "RA_HS3001.h"
#include "RA_ZMOD4XXX_Common.h"
#include "ICP_20100.h"
#include "RmcI2C.h"
#include "usr_app.h"
#include "user_choice.h"
#include "icm.h"
#include "icp.h"
#include "ICM42605.h"

extern TX_THREAD ZMOD4410_Thread;
extern TX_THREAD OB_1203_Thread;
extern TX_THREAD ZMOD4510_Thread;


UINT status1 = EXIT_SUCCESS;

void RA_Init(void);
void g_comms_i2c_bus0_quick_setup(void);
void g_comms_i2c_bus1_quick_setup(void);
void g_comms_i2c_bus2_quick_setup(void);
static void reset_zmod_sensor(void);
void start_periodic_msg_sending(void);

/* Sensor_Thread_entry function */
void Sensor_Thread_entry(void)
{
    /* TODO: add your own code here */
    RA_Init ();
    reset_zmod_sensor();
    g_comms_i2c_bus0_quick_setup();
    g_comms_i2c_bus1_quick_setup ();
    g_comms_i2c_bus2_quick_setup ();
    tx_thread_sleep (pdMS_TO_TICKS(100));

#if _HS3001_SENSOR_ENABLE_
    g_hs300x_sensor0_quick_setup();
    tx_thread_sleep (pdMS_TO_TICKS(100));
#endif

#if _ZMOD4410_SENSOR_ENABLE_
    g_zmod4xxx_sensor0_quick_setup();
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
    status1 = tx_thread_resume (&ZMOD4410_Thread);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
#endif

#if _ICP20100_SENSOR_ENABLE_
    RmComDevice_init();
    icp_setup();
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
#endif

#if _ICM42605_SENSOR_ENABLE_
    RmComDevice_init_Icm ();
    icm_setup();
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
#endif

#if _OB1203_SENSOR_ENABLE_
    status1 = tx_thread_resume (&OB_1203_Thread);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
#endif

#if _ZMOD4510_SENSOR_ENABLE_
    status1 = tx_thread_resume (&ZMOD4510_Thread);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
#endif
    start_periodic_msg_sending();

    while (1)
    {
#if _HS3001_SENSOR_ENABLE_
        hs3001_get();
#endif
#if _ICP20100_SENSOR_ENABLE_
        /* Read ICP sensor data */
        icp_get ();
#endif
#if _ICM42605_SENSOR_ENABLE_
        /* Read ICM sensor data */
        icm42605_loop();
#endif
        tx_thread_sleep (pdMS_TO_TICKS(10));
    }
}

/**************************************************************************************
 * @brief       Start_periodic_msg_sending function
 * @param       None
 * @retval
 **************************************************************************************/
void start_periodic_msg_sending(void)
{
	fsp_err_t status = FSP_SUCCESS;
    R_GPT_Start (&gpt_ctrl);

    if (FSP_SUCCESS == status)
    {
        IotLog("\n** Periodic MSG Sending TIMER Start  %u **\r\n",status);

    }
    else
    {
    	IotLog("\n** Periodic MSG Sending TIMER Failed  %u **\r\n",status);
    }
}

/*******************************************************************************************************************//**
 * @brief       Quick setup for g_comms_i2c_bus0
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_comms_i2c_bus0_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance = (i2c_master_instance_t *) g_comms_i2c_bus0_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    assert(FSP_SUCCESS == err);

    /* Recover if I2C0 bus is busy */
    bsp_recover_iic(I2C_SCL_0, I2C_SDA_0);

#if BSP_CFG_RTOS
    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_semaphore_create(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
                            g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                            (ULONG) 0);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
            = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0, g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
#endif
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_mutex_create(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
                        g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                        TX_INHERIT);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
            = xSemaphoreCreateRecursiveMutexStatic(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
#endif
    }
#endif
}

/*******************************************************************************************************************//**
 * @brief       Quick setup for g_comms_i2c_bus1
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_comms_i2c_bus1_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance = (i2c_master_instance_t *) g_comms_i2c_bus1_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    assert(FSP_SUCCESS == err);

    /* Recover if I2C1 bus is busy */
    bsp_recover_iic(I2C_SCL_1, I2C_SDA_1);

#if BSP_CFG_RTOS
    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_semaphore_create(g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
                            g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                            (ULONG) 0);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
            = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0, g_comms_i2c_bus1_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
#endif
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_mutex_create(g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
                        g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                        TX_INHERIT);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
            = xSemaphoreCreateRecursiveMutexStatic(g_comms_i2c_bus1_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
#endif
    }
#endif
}

/*******************************************************************************************************************//**
 * @brief       Quick setup for g_comms_i2c_bus2
 * @param[in]   None
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_comms_i2c_bus2_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance = (i2c_master_instance_t *) g_comms_i2c_bus2_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    assert(FSP_SUCCESS == err);

    /* Recover if I2C2 bus is busy */
    bsp_recover_iic(I2C_SCL_2, I2C_SDA_2);

#if BSP_CFG_RTOS
    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_semaphore_create(g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
                            g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                            (ULONG) 0);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
            = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0, g_comms_i2c_bus2_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
#endif
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_mutex_create(g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
                        g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                        TX_INHERIT);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
            = xSemaphoreCreateRecursiveMutexStatic(g_comms_i2c_bus2_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
#endif
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
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_LOW); // 4510
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_LOW); // 4410
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_06, BSP_IO_LEVEL_HIGH); // 4510
    R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_03_PIN_07, BSP_IO_LEVEL_HIGH); // 4410
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_PinAccessDisable ();
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
