#include <touch_ft5x16/touch_ft5x16.h>
#include <touch_thread.h>



extern bsp_leds_t g_bsp_leds;
//static int which_time = 0;
static bool which_time_isr = false;

/*******************************************************************************************************************//**
 * Touch IRQ callback function
 *
 * NOTE: This function will return to the highest priority available task.
 *
 * @param      p_args          IRQ callback data
 **********************************************************************************************************************/
void touch_irq_cb(external_irq_callback_args_t * p_args)
{
    if(19 == p_args->channel)
     {
      //  which_time_isr++;
       if(which_time_isr == false)
       {
           which_time_isr= true;
       }
       else
       {
           R_BSP_IpcNmiRequestSet();
           R_AGT_Start(&g_timer_periodic_ctrl);

       }
     }
}

void my_ipc_nmi_callback(void)
{
    ;
}
/*******************************************************************************************************************//**
 * @brief This function is callback for periodic timer and blinks LED on every 1 Second.
 *
 * @param[in] (timer_callback_args_t *)
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{

    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {

        // send message to CM85. CM85 enters deep sleep after receiving this message.
        R_IPC_MessageSend(&g_ipc0_ctrl, 1);
        R_AGT_Stop(&g_timer_periodic_ctrl);

    }
    return;
}


fsp_err_t agt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open Timer1 in Periodic mode */
    err = R_AGT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);


       /* Start the timer in periodic mode only if the timer is in stooped state */
       err = R_AGT_Start(&g_timer_periodic_ctrl);
       /* Handle error */


    return err;
}
void blink_led(uint32_t i)
{
    /* LED type structure */
      bsp_leds_t leds = g_bsp_leds;

      /* If this board has no LEDs then trap here */
      if (0 == leds.led_count)
      {
          while (1)
          {
              ;                          // There are no LEDs on this board
          }
      }

      /* Holds level to set for pins */
       bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

       for (volatile int j=0; j<5; j++)
         {
             /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
                 * handled. This code uses BSP IO functions to show how it is used.
                 */
                R_BSP_PinAccessEnable();


                {
                    /* Get pin to toggle */
                    uint32_t pin = leds.p_leds[i];

                    /* Write to this pin */
                    R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);
                }

                /* Protect PFS registers */
                R_BSP_PinAccessDisable();

                /* Toggle level for next write */
                if (BSP_IO_LEVEL_LOW == pin_level)
                {
                    pin_level = BSP_IO_LEVEL_HIGH;
                }
                else
                {
                    pin_level = BSP_IO_LEVEL_LOW;
                }

                vTaskDelay(configTICK_RATE_HZ*4);
         }
}
/*Initialize LCD Touch, IPC and AGT*/
void touch_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Initialize FT5X06 driver */
   ft5x06_init(&g_i2c_master0, &g_i2c_semaphore, BSP_IO_PORT_06_PIN_06);

    /* Enable touch IRQ */
    R_ICU_ExternalIrqOpen(g_touch_irq.p_ctrl, g_touch_irq.p_cfg);
    R_ICU_ExternalIrqEnable(g_touch_irq.p_ctrl);

    R_IPC_Open(g_ipc0.p_ctrl, g_ipc0.p_cfg);

    R_BSP_IpcNmiEnable(my_ipc_nmi_callback);
    agt_init();


   // static bool second_event= false;

    /*send event to tell cm85 to start camera capture*/
    /* TODO: add your own code here */
    while (1)
    {
        vTaskDelay (1);
        __WFI();  // Wait For Interrupt
    }
}
