/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : led_screen_demo.c
 * Version      : .
 * Description  : The led demo screen display.
 *********************************************************************************************************************/

#include <math.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "menu_camview.h"
#include "jlink_console.h"

#include "r_ioport.h"
#include "r_mipi_dsi_api.h"

#include "hal_data.h"
#include "dsi_layer.h"

#include "camera_layer.h"
#include "graphics\graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "touch_gt911.h"
#include "gimp.h"

#include "font_ai_face_digit.h"
#include "bg_font_18_full.h"

extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t   in_transition(void);

extern void   show_menu_icon(void);
extern bool_t process_main_menu(void);

void do_kis_screen(void);

static volatile d2_point reset_vpos = 143;
static volatile d2_point reset_hpos = 380;

#define USE_ALIGNMENT

#ifdef USE_ALIGNMENT

/*COMMON all text aligned */
 #define ALIGNED_Y_OFFSET          (380)
#endif

/* PART_NUMBER */
#define PART_NUMBER_X_OFFSET       (230)

#ifdef USE_ALIGNMENT
 #define PART_NUMBER_Y_OFFSET      (ALIGNED_Y_OFFSET)
#else
 #define PART_NUMBER_Y_OFFSET      (265)
#endif

/* DEVICE_NUMBER */
#define DEVICE_NUMBER_X_OFFSET     (202)
#ifdef USE_ALIGNMENT
 #define DEVICE_NUMBER_Y_OFFSET    (ALIGNED_Y_OFFSET)
#else
 #define DEVICE_NUMBER_Y_OFFSET    (240)
#endif

/* UID */
#define UID_X_OFFSET               (173)
#ifdef USE_ALIGNMENT
 #define UID_Y_OFFSET              (ALIGNED_Y_OFFSET)
#else
 #define UID_Y_OFFSET              (330)
#endif

/* Temperature */
#define TEMPERATURE_X_OFFSET       (145)
#ifdef USE_ALIGNMENT
 #define TEMPERATURE_Y_OFFSET      (ALIGNED_Y_OFFSET)
#else
 #define TEMPERATURE_Y_OFFSET      (380)
#endif

/* URL */
#define URL_X_OFFSET               (117)
#ifdef USE_ALIGNMENT
 #define URL_Y_OFFSET              (ALIGNED_Y_OFFSET)
#else
 #define URL_Y_OFFSET              (330)
#endif

static char_t s_temp_buffer[32] = "";
static char_t s_url_buffer[]    = "renesas.com/ra/ek-ra8d1";

// static char_t s_url_buffer[]    = "abcdefghijklmnopqrstuvwxyz0123456789";

/**********************************************************************************************************************
 * Function Name: check_for_temp
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void check_for_temp (void)
{
    uint16_t wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
    uint16_t fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
    uint16_t wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
    uint16_t fr_mcu_temp_c = g_board_status.temperature_c.mantissa;
    char_t   fs[16]        = "";
    char_t   cs[16]        = "";

    /* Temprature as degrees F */
    sprintf(fs, "%d.%02d", wn_mcu_temp_f, fr_mcu_temp_f);

    /* Temprature as degrees C */
    sprintf(cs, "%d.%02d", wn_mcu_temp_c, fr_mcu_temp_c);

    /* Fix output as two decimal places */
    size_t len_f = (strcspn(fs, ".") + 3);
    size_t len_c = (strcspn(cs, ".") + 3);

    /* Update temperature to display */
    memset(s_temp_buffer, " ", 30);
    s_temp_buffer[30] = '\0';
    s_temp_buffer[31] = '\0';

    sprintf(s_temp_buffer, "%.*s'F / %.*s'C       ", len_f, fs, len_c, cs);
}

/**********************************************************************************************************************
 * End of function check_for_temp
 *********************************************************************************************************************/

#define CURRENCY_HB_HEIGHT_SIZE    (26)
#define CURRENCY_HB_WIDTH_SIZE     (70)

#define BLANK_POS_X                (144)
#define BLANK_POS_Y                (300)

/**********************************************************************************************************************
 * Function Name: do_kis_screen
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void do_kis_screen (void)
{
    st_gimp_image_t * img          = NULL;
    static char_t     uid_str[256] = "";

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    check_for_temp();

    if (in_transition())
    {
        img = (st_gimp_image_t *) get_image_data(GI_KIS_SCREEN);

// img = (st_gimp_image_t *)get_image_data(NOT_IN_USE_8);

        d2_setblitsrc(d2_handle, img->pixel_data, 480, 480, LCD_VPIX, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                    480,
                    LCD_VPIX,                     // Source width/height
                    (d2_blitpos) 0,
                    0,                            // Source position
                    (d2_width) ((480) << 4),
                    (d2_width) ((LCD_VPIX) << 4), // Destination width/height
                    0,
                    0,                            // Destination position
                    d2_tm_filter);

        bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet();

        sprintf(uid_str,
                "%08x-%08x-%08x-%08x",
                (size_t) p_uid->unique_id_words[0],
                (size_t) p_uid->unique_id_words[1],
                (size_t) p_uid->unique_id_words[2],
                (size_t) p_uid->unique_id_words[3]);

        print_bg_font_18(d2_handle, PART_NUMBER_X_OFFSET, PART_NUMBER_Y_OFFSET, (char *) PART_NUMBER);
        print_bg_font_18(d2_handle, DEVICE_NUMBER_X_OFFSET, DEVICE_NUMBER_Y_OFFSET, (char *) DEVICE_NUMBER);
        print_bg_font_18(d2_handle, UID_X_OFFSET, UID_Y_OFFSET, (char *) uid_str);
        print_bg_font_18(d2_handle, TEMPERATURE_X_OFFSET, TEMPERATURE_Y_OFFSET, (char *) s_temp_buffer);
        print_bg_font_18(d2_handle, URL_X_OFFSET, URL_Y_OFFSET, (char *) s_url_buffer);

        /* foreground */
        show_menu_icon();
    }
    else
    {
        process_main_menu();

        d2_point vpos = reset_vpos;
        d2_point hpos = reset_hpos;

        /* do font */
        print_bg_font_18(d2_handle, PART_NUMBER_X_OFFSET, PART_NUMBER_Y_OFFSET, (char *) PART_NUMBER);
        print_bg_font_18(d2_handle, DEVICE_NUMBER_X_OFFSET, DEVICE_NUMBER_Y_OFFSET, (char *) DEVICE_NUMBER);
        print_bg_font_18(d2_handle, UID_X_OFFSET, UID_Y_OFFSET, (char *) uid_str);
        print_bg_font_18(d2_handle, TEMPERATURE_X_OFFSET, TEMPERATURE_Y_OFFSET, (char *) s_temp_buffer);
        print_bg_font_18(d2_handle, URL_X_OFFSET, URL_Y_OFFSET, (char *) s_url_buffer);
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

/**********************************************************************************************************************
 * End of function do_kis_screen
 *********************************************************************************************************************/
