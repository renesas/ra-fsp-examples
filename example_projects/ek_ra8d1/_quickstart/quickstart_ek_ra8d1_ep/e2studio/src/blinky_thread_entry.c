/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : blinky_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "blinky_thread.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "menu_main.h"
#include "common_utils.h"
#include "common_init.h"
#include "gimp.h"

#include "r_typedefs.h"
#include "portable.h"

#include "touch_gt911.h"

static char_t s_buffer[128] = {};

#define SPLASH_SCREEN           (0)
#define MAIN_SCREEN             (1)
#define MENU_SCREEN             (2)
#define HELP_SCREEN             (3)
#define KIS_SCREEN              (4)
#define CAM_SCREEN              (5)
#define LED_SCREEN              (6)
#define WEATHER_SCREEN          (7)
#define AI_FACE_RECONITION      (8)
#define AI_OBJECT_RECONITION    (9)
#define CURRENCY_SCREEN         (10)
#define TIMEZONE_SCREEN         (11)

// #define SHOW_MODE (1)

extern void reset_transition(void);

extern volatile touch_data_t g_touch_data;

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

#ifdef SHOW_MODE
static uint32_t g_demo[] =
{
    MAIN_SCREEN,
    MENU_SCREEN,
    KIS_SCREEN,
    HELP_SCREEN,
    LED_SCREEN,
    CURRENCY_SCREEN,
    TIMEZONE_SCREEN,
    WEATHER_SCREEN,
    AI_FACE_RECONITION,
    AI_OBJECT_RECONITION
};
#endif

/**********************************************************************************************************************
 * Function Name: blinky_thread_entry
 * Description  : .
 * Argument     : pvParameters (contains TaskHandle_t)
 * Return Value : .
 *********************************************************************************************************************/
void blinky_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    // dumb way to let system startup
    vTaskDelay(1000);

    while (1)
    {
#define TP_MONITOR_MODE    (1)
        if (g_touch_data.num_points > 0)
        {
            if (g_touch_data.num_points > 0)
            {
                for (uint8_t np = 0; np < g_touch_data.num_points; np++)
                {
                    if (np > 0)
                    {
// print_to_console(": ");
                    }

                    sprintf(s_print_buffer,
                            "tp [%d] [%03d,%03d]",
                            np,
                            g_touch_data.point[np].x,
                            g_touch_data.point[np].y);

// print_to_console(s_print_buffer);
                }

// print_to_console("\r\n");
            }
        }
#ifdef TP_MONITOR_MODE
#endif

#ifdef SHOW_MODE
        loop++;
        for (current_demo = 0; current_demo < demo_len; current_demo++)
        {
            uxCurrentSize = xPortGetFreeHeapSize();
            uxMinLastSize = xPortGetMinimumEverFreeHeapSize();

            glcd_layer_change.layer.coordinate.x = 270;
            glcd_layer_change.layer.coordinate.y = 950; // need to center horizontical;

            glcd_layer_change.input = g_display0.p_cfg->input[1];
            (void) R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_2);

            display_input_cfg_t const * p_input = &g_display0.p_cfg->input[1]; // Layer 2
            uint8_t * buf_ptr = (uint8_t *) fb_foreground;
            memset(buf_ptr, 0, p_input->hstride * p_input->vsize * 4);

            sprintf(s_buffer,
                    "[%05d] : Selected screen [%d:%d]  Heap: highest %u  current %u lowest %u\r",
                    (size_t) loop,
                    (size_t) current_demo,
                    (size_t) g_demo[current_demo],
                    uxMaxSize,
                    uxCurrentSize,
                    uxMinLastSize);
            print_to_console((void *) s_buffer);

            demo_screen = g_demo[current_demo];
            reset_transition();
            vTaskDelay(8000);
        }
#endif

        vTaskDelay(100);
    }
}

/**********************************************************************************************************************
 * End of function blinky_thread_entry
 *********************************************************************************************************************/
void debug_memory_free(uint8_t id, void * ptr);

/**********************************************************************************************************************
 * Function Name: debug_memory_free
 * Description  : .
 * Arguments    : id
 *              : ptr
 * Return Value : .
 *********************************************************************************************************************/
void debug_memory_free (uint8_t id, void * ptr)
{
    char ref[32] = "UKN";

    switch (id)
    {
        case 0:
        {
            sprintf(ref, "wi_alloc");
            break;
        }

        case 1:
        {
            sprintf(ref, "wi_txalloc");
            break;
        }

        default:
            sprintf(ref, "UKN");
    }

    sprintf(s_buffer, "%s Heap: current %u after malloc %p \r\n", ref, xPortGetFreeHeapSize(), ptr);

    // print_to_console((void*)s_buffer);
}

/**********************************************************************************************************************
 * End of function debug_memory_free
 *********************************************************************************************************************/
