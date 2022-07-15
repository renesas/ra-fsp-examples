#include "oximeter_thread.h"
#include "console_menu/console.h"
#include "common_utils.h"
#include "oximeter.h"
#include "user_choice.h"
#include "ob1203_bio/OB1203/OB1203.h"
#include "ob1203_bio/SPO2/SPO2.h"
#include "oximstruct.h"

#define UNUSED(x)  (void)(x)

extern TaskHandle_t oximeter_thread;
extern void (*p_IntB_Event)(void);
void g_ob1203_sensor0_quick_setup();

uint32_t  oximeter_status = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       OB1203 sensor IRQ callback
 * @param[in]   p_args      pointer to call back parameter
 * @retval
 ***********************************************************************************************************************/
void sensorOBIRQCallback(external_irq_callback_args_t *p_args)
{
    UNUSED(p_args);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (p_IntB_Event != NULL)
    {
        p_IntB_Event ();
        /* Give semaphore */
        xSemaphoreGiveFromISR(g_ob1203_semaphore, &xHigherPriorityTaskWoken);
    }
}

/*******************************************************************************************************************//**
 * @brief       OB1203 sensor IRQ callback
 * @param[in]
 * @retval
 ***********************************************************************************************************************/
void g_ob1203_sensor0_quick_setup()
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_comms_i2c_device3.p_api->open (g_comms_i2c_device3.p_ctrl, g_comms_i2c_device3.p_cfg);
    if (FSP_SUCCESS == err)
    {
        APP_PRINT("\r\nOB1203 sensor setup success \r\n");
    }
    else
    {
        APP_DBG_PRINT("\r\nOB1203 Sensor open fail\r\n");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief       Oximeter Thread entry function
 * @param[in]
 * @retval
 ***********************************************************************************************************************/
void oximeter_thread_entry(void *pvParameters){
    FSP_PARAMETER_NOT_USED (pvParameters);

    xTaskNotifyWait(pdFALSE, pdFALSE, (uint32_t *)&oximeter_status, portMAX_DELAY);

#if _OB1203_SENSOR_ENABLE_
    /* Open OB1203 */
    g_ob1203_sensor0_quick_setup();
    /* OB1203 configuration */
    /* do initial configuration of OB1203, Kalman filters, algorithm, etc. */
    main_init(&ox,&sp,&ob);
#endif

    while (1){
#if _OB1203_SENSOR_ENABLE_
        /* Start reading OB1203 sensor data */
        ob1203_spo2_main(&ox, &sp, &ob);
        vTaskDelay(5);
#else
        break;
#endif
    }
}
