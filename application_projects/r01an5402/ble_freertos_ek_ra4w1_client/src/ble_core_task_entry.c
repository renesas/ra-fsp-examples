#include "ble_core_task.h"

extern void     app_main(void);

/* BLE_CORE_TASK entry function */
/* pvParameters contains TaskHandle_t */
void ble_core_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    app_main();

    while (1)
    {
        vTaskDelay (1000 / portTICK_PERIOD_MS);
    }
}
