#include "touch_thread.h"
#include "touch_ft5x06.h"
#include "GUI.h"

/******************************************************************************
 * Static global variables
 *****************************************************************************/
static touch_data_t touch_data;

/******************************************************************************
 * Function definitions
 *****************************************************************************/

/*******************************************************************************************************************//**
 * Touch IRQ callback function
 *
 * NOTE: This function will return to the highest priority available task.
 *
 * @param      p_args          IRQ callback data
 **********************************************************************************************************************/
void touch_irq_cb(external_irq_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    BaseType_t context_switch;

    /* Set touch IRQ semaphore */
    xSemaphoreGiveFromISR(g_touch_semaphore, &context_switch);

    /* Return to the highest priority available task */
    portYIELD_FROM_ISR(context_switch);
}

/*******************************************************************************************************************//**
 * Wait for the next touch event
 **********************************************************************************************************************/
static void touch_wait()
{
    xSemaphoreTake(g_touch_semaphore, portMAX_DELAY);
}

/*******************************************************************************************************************//**
 * Touch Thread entry function
 * @param      pvParameters    Task parameters (contains TaskHandle_t)
 **********************************************************************************************************************/
void touch_thread_entry(void *pvParameters)
{
    static GUI_PID_STATE State;
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Initialize FT5X06 driver */
    ft5x06_init(&g_i2c_touch, &g_i2c_semaphore, BSP_IO_PORT_03_PIN_04);

    /* Enable touch IRQ */
    R_ICU_ExternalIrqOpen(g_touch_irq.p_ctrl, g_touch_irq.p_cfg);
    R_ICU_ExternalIrqEnable(g_touch_irq.p_ctrl);

    while (1)
    {
        /* Wait for IRQ from touch controller. */
        touch_wait();

        /* Get touch data from the FT5X06 */
        ft5x06_payload_get(&touch_data);

        /* Store touch data in emWin */
        if (touch_data.num_points)
        {
          State.x       = touch_data.point[0].x;
          State.y       = touch_data.point[0].y;
          State.Pressed = 1;
          GUI_PID_StoreState(&State);
        }
        else if (State.Pressed == 1)
        {
          State.Pressed = 0;
          GUI_PID_StoreState(&State);
        }
    }
}
