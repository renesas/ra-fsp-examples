/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : ai_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include <stdarg.h>
#include "ai_thread.h"
#include "common_data.h"
#include "common_init.h"
#include "common_utils.h"
#include "menu_camview.h"
#include "timer.h"
#include "camera_layer.h"

// Use following define to enable output to serial console
// #define ENABLE_DEBUG_OUT (1)

volatile bool_t is_ai_running = false;
extern int two_apps(bool_t, bool_t);

st_ai_detection_point_t      g_ai_detection[MAX_DETECTION_NUMS]      = {};
st_ai_classification_point_t g_ai_classification[MAX_DETECTION_NUMS] = {};

bool_t ia_processing(void);
bool_t can_swap_demos(void);
int    e_printf(const char * format, ...);
void   update_detection_result(signed short index, signed short x, signed short y, signed short w, signed short h);
void   update_classification_result(unsigned index, unsigned short category, float probability);

/**********************************************************************************************************************
 * Function Name: ia_processing
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t ia_processing (void)
{
    if (false == is_ai_running)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**********************************************************************************************************************
 * End of function ia_processing
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_detection_result
 * Description  : .
 * Arguments    : index
 *              : x
 *              : y
 *              : w
 *              : h
 * Return Value : .
 *********************************************************************************************************************/
void update_detection_result (signed short index, signed short x, signed short y, signed short w, signed short h)
{
    if (index < MAX_DETECTION_NUMS)
    {
        g_ai_detection[index].m_x = x;
        g_ai_detection[index].m_y = y;
        g_ai_detection[index].m_w = w;
        g_ai_detection[index].m_h = h;
    }
}

/**********************************************************************************************************************
 * End of function update_detection_result
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_classification_result
 * Description  : .
 * Arguments    : index
 *              : category
 *              : probability
 * Return Value : .
 *********************************************************************************************************************/
void update_classification_result (unsigned index, unsigned short category, float probability)
{
    if (index < MAX_DETECTION_NUMS)
    {
        g_ai_classification[index].prob     = probability;
        g_ai_classification[index].category = category;
    }
}

/**********************************************************************************************************************
 * End of function update_classification_result
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: e_printf
 * Description  : .
 * Arguments    : format
 *              :
 * Return Value : .
 *********************************************************************************************************************/
int e_printf (const char * format, ...)
{
    UNUSED_VARIABLE(format);

#if ENABLE_DEBUG_OUT
    va_list args;
    va_start(args, format);
    int result = vsprintf(s_print_buffer, format, args);
    va_end(args);
    sprintf(s_print_buffer, "%s\r\n", s_print_buffer);
    print_to_console((void *) s_print_buffer);

    return result;
#endif

    return 0;
}

/**********************************************************************************************************************
 * End of function e_printf
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: can_swap_demos
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t can_swap_demos (void)
{
    bool_t ret = false;

    vTaskSuspendAll();
    if (is_ai_running == false)
    {
        ret = true;
    }

    xTaskResumeAll();

    return ret;
}

/**********************************************************************************************************************
 * End of function can_swap_demos
 *********************************************************************************************************************/
uint32_t       t_rgb888 = 0;
extern uint8_t s_ceu_buffer;
extern void bsp_camera_rgb565_to_rgb888(const void * inbuf, void * outbuf, uint16_t width, uint16_t height);

extern uint8_t bsp_camera_out_buffer[2][BSP_CAM_WIDTH * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL];
extern uint8_t bsp_camera_out_buffer888[BSP_CAM_WIDTH * BSP_CAM_HEIGHT * 3];

/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : .
 * Return Value : The main menu controller.
 *********************************************************************************************************************/

/* AI Thread entry function */
/* pvParameters contains TaskHandle_t */

/**********************************************************************************************************************
 * Function Name: ai_thread_entry
 * Description  : .
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void ai_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    timer_init();
    vTaskDelay(3000);

    while (1)
    {
        bsp_camera_rgb565_to_rgb888(bsp_camera_out_buffer[s_ceu_buffer],
                                    bsp_camera_out_buffer888,
                                    BSP_CAM_WIDTH,
                                    BSP_CAM_HEIGHT);
        vTaskDelay(10);

        if ((do_detection == true) || (do_classification == true))
        {
            is_ai_running = true;

            two_apps(do_detection, do_classification);

            is_ai_running = false;
        }

        vTaskDelay(400);
    }
}

/**********************************************************************************************************************
 * End of function ai_thread_entry
 *********************************************************************************************************************/
