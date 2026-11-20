/***********************************************************************************************************************
 * File Name    : glcdc_ep.c
 * Description  : Contains data structures and functions used in glcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "glcdc_ep.h"

/* Global variables */
uint16_t g_hz_size = RESET_VALUE;
uint16_t g_vr_size = RESET_VALUE;
uint8_t * g_p_single_buffer = NULL;
uint8_t * g_p_double_buffer = NULL;

/* Private function */
#ifdef DISPLAY_IN_FORMAT_32BITS_RGB888_0
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
#else
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

/*******************************************************************************************************************//**
 * @brief     This function initializes and start the GLCDC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized and started successfully.
 * @retval    Any other error codes     Failed to initialize or start.
 **********************************************************************************************************************/
fsp_err_t glcdc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t buffer_size = RESET_VALUE;

    /* Get LCDC configuration */
    g_hz_size = (g_display_cfg.input[ARRAY_INDEX].hsize);
    g_vr_size = (g_display_cfg.input[ARRAY_INDEX].vsize);

    /* Initialize buffer pointers */
    buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL);
    g_p_single_buffer = (uint8_t *) g_display_cfg.input[ARRAY_INDEX].p_base;

    /* Set the address of the double buffer, used for drawing color bands with good quality */
    g_p_double_buffer = g_p_single_buffer + buffer_size;

    /* Initialize GLCDC module */
    err = R_GLCDC_Open(&g_display_ctrl, &g_display_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function displays color bands on the screen using appropriate color codes and coordinate values.
 * @param[in] x1, y1                    Start point coordinates.
 * @param[in] x2, y2                    End point coordinates.
 * @param[in] color                     Color to display.
 * @retval    None.
 **********************************************************************************************************************/
#ifdef DISPLAY_IN_FORMAT_32BITS_RGB888_0
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
#else
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */
{
    /* Declare local variables */
    uint16_t start_x, start_y, display_length, display_height;
    uint32_t start_addr;

    /* Assign coordinate values and calculate start address */
    start_x = x1;
    start_y = y1;
    start_addr = (uint32_t) ((start_x * BYTES_PER_PIXEL) + (start_y * g_hz_size * BYTES_PER_PIXEL));

    /* Calculate display box length and height */
    display_length = (uint16_t) ((x2 - x1) * BYTES_PER_PIXEL);
    display_height = (y2 - y1);

    /* Display required color band */
    for (uint16_t ver_value = Y1_CO_ORDINATE; ver_value < (display_height - INC_DEC_VALUE); ver_value++)
    {
        for (uint32_t hor_value = start_addr; hor_value < (start_addr + display_length); hor_value += BYTES_PER_PIXEL)
        {
#ifdef DISPLAY_IN_FORMAT_32BITS_RGB888_0
            *(uint32_t *) (g_p_single_buffer + hor_value) = color;
            *(uint32_t *) (g_p_double_buffer + hor_value) = color;
#else
            *(uint16_t *) (g_p_single_buffer + hor_value) = color;
            *(uint16_t *) (g_p_double_buffer + hor_value) = color;
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */
        }
        start_addr = (uint32_t) (start_addr + (g_hz_size * BYTES_PER_PIXEL));
    }
}

/*******************************************************************************************************************//**
 * @brief     This function clears the screen and displays eight horizontal color bands on the graphical LCD.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void glcdc_draw_color_bands(void)
{
#ifdef DISPLAY_IN_FORMAT_32BITS_RGB888_0
    uint32_t color[COLOR_BAND_COUNT] = {RED, GREEN, BLUE, BLACK, WHITE, YELLOW, MAGENTA, CYAN};
#else
    uint16_t color[COLOR_BAND_COUNT] = {RED, GREEN, BLUE, BLACK, WHITE, YELLOW, MAGENTA, CYAN};
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */
    uint16_t width = g_vr_size / COLOR_BAND_COUNT;

    /* Clear LCD screen using appropriate coordinates */
    screen_display((uint16_t) X1_CO_ORDINATE, (uint16_t) Y1_CO_ORDINATE, g_hz_size, g_vr_size, BLACK);

    /* Display color bands on LCD screen */
    for (uint8_t display_count = RESET_VALUE; display_count < COLOR_BAND_COUNT; display_count++)
    {
        screen_display((uint16_t) X1_CO_ORDINATE, (display_count * width), g_hz_size,
                       (uint16_t) (((display_count * width) + width) + INC_DEC_VALUE), color[display_count]);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened GLCDC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void glcdc_deinit(void)
{
    /* De-initialize the opened GLCDC module */
    if (g_display_ctrl.state)
    {
        R_GLCDC_Close(&g_display_ctrl);
    }
}
