/***********************************************************************************************************************
 * File Name    : mipi_dsi_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in mipi_dsi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MIPI_DSI_EP_H_
#define MIPI_DSI_EP_H_

#include "hal_data.h"
#include "gt911.h"

/* Macros for the LCD initialization table */
#define MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG     ((mipi_cmd_id_t) 0xFE)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE   ((mipi_cmd_id_t) 0xFD)

/* Macros for the frame buffer */
#define BYTES_PER_PIXEL                             (4)
#define COLOR_BAND_COUNT                            (8)
#define BLUE                                        (0x000000FF)
#define LIME                                        (0xFF00FF00)
#define RED                                         (0x00FF0000)
#define BLACK                                       (0x00000000)
#define WHITE                                       (0xFFFFFFFF)
#define YELLOW                                      (0xFFFFFF00)
#define AQUA                                        (0xFF00FFFF)
#define MAGENTA                                     (0x00FF00FF)

/* Macros for the ULPS entry time selection */
#define SELECT_5S                                   ('1')
#define SELECT_15S                                  ('2')
#define SELECT_30S                                  ('3')
#define SELECT_DISABLE_ULPS                         ('4')

/* Macros for GPT timer period */
#define GPT_UNITS_SECONDS                           (1)
#define GPT_DESIRED_PERIOD_5SEC                     (5)
#define GPT_DESIRED_PERIOD_15SEC                    (15)
#define GPT_DESIRED_PERIOD_30SEC                    (30)
#define ULPS_EXIT_PERIOD_30SEC                      (30)

/* Macros for the display pin */
#if defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK)
#define PIN_DISPLAY_INT                             (BSP_IO_PORT_01_PIN_11)
#define PIN_DISPLAY_RST                             (BSP_IO_PORT_06_PIN_06)
#define PIN_DISPLAY_BACKLIGHT                       (BSP_IO_PORT_05_PIN_14)
#elif defined (BOARD_RA8D1_EK)
#define PIN_DISPLAY_INT                             (BSP_IO_PORT_05_PIN_10)
#define PIN_DISPLAY_RST                             (BSP_IO_PORT_10_PIN_01)
#define PIN_DISPLAY_BACKLIGHT                       (BSP_IO_PORT_04_PIN_04)
#elif defined (BOARD_RA8P1_EK_V2)
#define PIN_DISPLAY_INT                             (BSP_IO_PORT_03_PIN_03)
#define PIN_DISPLAY_RST                             (BSP_IO_PORT_06_PIN_06)
#define PIN_DISPLAY_BACKLIGHT                       (BSP_IO_PORT_01_PIN_09)
#endif /* Specific board */

/* Macro for the ULPS enter timeout */
#define ENTER_ULPS_DELAY                            (8000000U)

/* LCD controller initialization table entry */
typedef struct
{
    unsigned char size;
    unsigned char buffer[10];
    mipi_cmd_id_t cmd_id;
    mipi_dsi_cmd_flag_t flags;
} lcd_table_setting_t;

/* External variables - Defined in mipi_dsi_ep.c */
extern const lcd_table_setting_t g_lcd_init_focuslcd[];
extern volatile bool g_vsync_flag;
extern volatile bool g_irq_state;
extern volatile bool g_timer_overflow;

/* Function declarations */
fsp_err_t mipi_dsi_push_table(const lcd_table_setting_t * table);
fsp_err_t mipi_dsi_ulps_enter_start(void);
fsp_err_t mipi_dsi_ulps_exit_start(void);
fsp_err_t mipi_dsi_display_open(void);
fsp_err_t mipi_dsi_display_start(void);
fsp_err_t mipi_dsi_display_buffer_change(uint8_t * p_buffer);
fsp_err_t mipi_dsi_gpt_open(void);
fsp_err_t mipi_dsi_timer_info_get(void);
fsp_err_t mipi_dsi_gpt_stop(void);
fsp_err_t mipi_dsi_gpt_reset(void);
fsp_err_t mipi_dsi_touch_i2c_open(void);
fsp_err_t mipi_dsi_touch_irq_open(void);
fsp_err_t mipi_dsi_touch_irq_enable(void);
fsp_err_t wait_for_mipi_dsi_event(mipi_dsi_phy_status_t event);
fsp_err_t timer_period_set(uint32_t period_time);
void touch_screen_reset(void);
void mipi_dsi_deinit(void);

#endif /* MIPI_DSI_EP_H_ */
