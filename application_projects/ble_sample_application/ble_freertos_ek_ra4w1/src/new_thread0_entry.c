#include "new_thread0.h"
/* New Thread entry function */
extern void app_main(void);
/* pvParameters contains TaskHandle_t */
void new_thread0_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    app_main();
    while (1)
    {
        vTaskDelay (1);
    }
}
