/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#include "display_thread.h"
#include "common_util.h"
#include "r_display_api.h"
#include "display_layer.h"
#include <stdio.h>
#include "time_counter.h"
#include "console_output.h"
#include "camera_control.h"



#define LCD_RESYNC_DELAY 10000  // Delay loop count for LCD sync (tunable)

void glcdc_vsync_isr(display_callback_args_t *p_args);
volatile uint32_t ipc_msg = 0;

extern void  do_image_classification_screen(bool ai_result_new);


/* 
Receiving the IPC message from Cortex-M33
Cortex-M85 goes to Deep Sleep Mode afterwards
*/
void ipc_channel0_cb(ipc_callback_args_t *p_args)
{

     if(IPC_EVENT_MESSAGE_RECEIVED == (p_args->event))
     {
         ipc_msg = p_args->message;
     }
}

/*
 * Disable the LCD Display and Ethos-U before entering Deep Sleep Mode.
 * Additional peripherals can be disabled here if needed.
 */
void disable_before_deepsleep(void)
{
    /* Disable Ethos-U (set module-stop bit MSTPA16) */
    R_MSTP->MSTPCRA_b.MSTPA16 = 1;

    /* Disable GLCDC background output */
    R_GLCDC->BG.EN_b.EN = 0;

    /* Disable MIPI-CSI controlled by MSTPC17 */
    R_MSTP->MSTPCRC_b.MSTPC17 = 1;

    /* Wait until output is fully disabled (optional) */
    __DSB();  // Ensure memory operations complete
    __ISB();  // Synchronize pipeline

    /* Disable the backlight */
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_BLEN, BSP_IO_LEVEL_LOW);
}

/*
 * Re-enable the LCD Display and Ethos-U after exiting Deep Sleep Mode.
 */
void enable_after_deepsleep(void)
{
    /* Clear module-stop bit MSTPA16 to enable Ethos-U */
    R_MSTP->MSTPCRA_b.MSTPA16 = 0;

    /* Enable MIPI-CSI controlled by MSTPC17 */
    R_MSTP->MSTPCRC_b.MSTPC17 = 0;

    /* Optional: ensure Ethos-U is stable before proceeding */
    __DSB();
    __ISB();

    /* Enable GLCDC background output */
    R_GLCDC->BG.EN_b.EN  = 1;
    R_GLCDC->BG.EN_b.VEN = 1;  // Enable vertical sync if required

    /* Short delay to ensure proper sync with the LCD panel */
    for (volatile int i = 0; i < LCD_RESYNC_DELAY; i++)
    {
        __NOP();
    }

    /* Enable the backlight */
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_BLEN, BSP_IO_LEVEL_HIGH);
}
void console_output_processing_time(void)
{
    sprintf (sprintf_buffer, "\r\nProcessing time:\r\n");
    print_to_console(sprintf_buffer);

    sprintf (sprintf_buffer, "  Camera image capture vsync period : %4d ms, %4d fps\r\n",
            application_processing_time.camera_image_capture_time_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.camera_image_capture_time_ms));
    print_to_console(sprintf_buffer);

    sprintf (sprintf_buffer, "  AI inference pre processing time  : %4d ms, %4d fps\r\n",
            application_processing_time.ai_inference_pre_processing_time_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.ai_inference_pre_processing_time_ms));
    print_to_console(sprintf_buffer);
    sprintf (sprintf_buffer, "  AI inference time                 : %4d ms, %4d fps\r\n",
            application_processing_time.ai_inference_time_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.ai_inference_time_ms));
    print_to_console(sprintf_buffer);
    sprintf (sprintf_buffer, "  LCD display vsync period          : %4d ms, %4d fps\r\n",
            application_processing_time.lcd_display_update_refresh_ms, TimeCounter_ConvertFromMsToFps(application_processing_time.lcd_display_update_refresh_ms));
    print_to_console(sprintf_buffer);
}
/* Display Thread entry function */
/* pvParameters contains TaskHandle_t */
void display_thread_entry(void *pvParameters)
{

    FSP_PARAMETER_NOT_USED (pvParameters);

    console_output_init();
    bool ai_result_updated = false;
    drw_init();

    memset(vin_image_buffer_1, 0x55, sizeof(vin_image_buffer_1));
    memset(vin_image_buffer_2, 0x55, sizeof(vin_image_buffer_2));
    memset(vin_image_buffer_3, 0x55, sizeof(vin_image_buffer_3));

    initialise_display();


  
    // Set display initialization complete flag
    xEventGroupSetBits(g_ai_app_event, HARDWARE_DISPLAY_INIT_DONE);
    while(true)
    {
        if ( 1== ipc_msg)
        {
		
            ipc_msg = 0;

            disable_before_deepsleep();
			/*Enter Deep Sleep Mode after reception of the IPC Message from Cortex-M33 */
            R_LPM_LowPowerModeEnter(&g_lpm_sleep_ctrl);
            enable_after_deepsleep();

        }

         /* Wait for vertical blanking period */
        xEventGroupWaitBits(g_ai_app_event, GLCDC_VSYNC, pdFALSE, pdTRUE, 1);
        {
            static bool wait_for_first_time = true;
           
            if (ai_result_updated)
            {
                wait_for_first_time = false;
            }
            if(wait_for_first_time == false)
            {
                
                R_GLCDC_BufferChange(&g_plcd_display_ctrl, (uint8_t * const) gp_next_buffer, DISPLAY_FRAME_LAYER_1);
                do_image_classification_screen(ai_result_updated);
                console_output_processing_time();
            }
            ai_result_updated = ((xEventGroupGetBits(g_ai_app_event) & AI_INFERENCE_RESULT_UPDATED) != 0) ? true : false;
            xEventGroupClearBits(g_ai_app_event, AI_INFERENCE_RESULT_UPDATED);
        }
        vTaskDelay(150);
    }


}




/**********************************************************************************************************************
 End of function glcdc_vsync_isr
 *********************************************************************************************************************/
