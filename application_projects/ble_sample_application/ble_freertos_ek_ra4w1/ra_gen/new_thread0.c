/* generated thread source file - do not edit */
#include "new_thread0.h"

#if 1
static StaticTask_t new_thread0_memory;
static uint8_t new_thread0_stack[0x2000] BSP_PLACE_IN_SECTION(".stack.new_thread0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
#endif
TaskHandle_t new_thread0;
void new_thread0_create(void);
static void new_thread0_func(void *pvParameters);
void rtos_startup_err_callback(void *p_instance, void *p_data);
void rtos_startup_common_init(void);
extern uint32_t g_fsp_common_thread_count;

const rm_freertos_port_parameters_t new_thread0_parameters =
{ .p_context = (void *) NULL, };

void new_thread0_create(void)
{
    /* Increment count so we will know the number of threads created in the RA Configuration editor. */
    g_fsp_common_thread_count++;

    /* Initialize each kernel object. */

#if 1
    new_thread0 = xTaskCreateStatic (
#else
                                     BaseType_t new_thread0_create_err = xTaskCreate(
#endif
                                     new_thread0_func,
                                     (const char *) "New Thread", 0x2000 / 4, // In words, not bytes
                                     (void *) &new_thread0_parameters, //pvParameters
                                     1,
#if 1
                                     (StackType_t *) &new_thread0_stack,
                                     (StaticTask_t *) &new_thread0_memory
#else
                                     & new_thread0
#endif
                                     );

#if 1
    if (NULL == new_thread0)
    {
        rtos_startup_err_callback (new_thread0, 0);
    }
#else
    if (pdPASS != new_thread0_create_err)
    {
        rtos_startup_err_callback(new_thread0, 0);
    }
#endif
}
static void new_thread0_func(void *pvParameters)
{
    /* Initialize common components */
    rtos_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. Pass task handle. */
    new_thread0_entry (pvParameters);
}
