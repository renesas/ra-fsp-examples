/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "glcdc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup glcdc_ep
 * @{
 **********************************************************************************************************************/

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/* Variables to store resolution information */
uint16_t g_hz_size, g_vr_size;
/* Variables used for buffer usage */
uint32_t g_buffer_size;
uint8_t * g_p_single_buffer, * g_p_double_buffer;

/* User defined functions */
#ifndef BOARD_RA8D1_EK
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
#else
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
#endif
static void color_band_display(void);

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t          err        = FSP_SUCCESS;
    fsp_pack_version_t version    = {RESET_VALUE};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize sdram */
#ifdef BOARD_RA8D1_EK
    R_BSP_SdramInit(true);
#endif

    /* Get LCDC configuration */
    g_hz_size = (g_display_cfg.input[0].hsize);
    g_vr_size = (g_display_cfg.input[0].vsize);
    /* Initialize buffer pointers */
    g_buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL);
    g_p_single_buffer = (uint8_t *) g_display_cfg.input[0].p_base;
    /* Double buffer for drawing color bands with good quality */
    g_p_double_buffer = g_p_single_buffer + g_buffer_size;

    /* Initialize GLCDC driver */
    err = R_GLCDC_Open(&g_display_ctrl, &g_display_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
        APP_ERR_PRINT("\r\n ** GLCDC driver initialization FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* GLCDC initialization failed  */
        APP_ERR_PRINT("\r\n ** GLCDC driver start FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Clear LCD screen using appropriate co-ordinates */
    screen_display((uint16_t)X1_CO_ORDINATE, (uint16_t)Y1_CO_ORDINATE, g_hz_size, g_vr_size, BLACK);

    /* Display color bands on LCD screen */
    color_band_display();

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function displays color bands on the screen using appropriate color codes and co-ordinate values. *
 * @param[IN]  x1,y1  start point coordinates
 *             x2,y2  end point coordinates
 *             color color to display
 * @retval None
 **********************************************************************************************************************/
#ifndef BOARD_RA8D1_EK
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    /*Declare local variables */
    uint16_t start_x, start_y, display_length, display_height;
    uint32_t start_addr;

    /* Assign co-ordinate values and calculate start address */
    start_x = x1;
    start_y = y1;
    start_addr = (uint32_t)((start_x * BYTES_PER_PIXEL) + (start_y * g_hz_size* BYTES_PER_PIXEL));

    /* Calculate display box length and height */
    display_length = (uint16_t)((x2 - x1) * BYTES_PER_PIXEL);
    display_height = (y2 - y1);

    /* Display required color band */
    for(uint16_t ver_value = Y1_CO_ORDINATE; ver_value < (display_height - INC_DEC_VALUE); ver_value++)
    {
        for(uint32_t hor_value = start_addr; hor_value < (start_addr + display_length); hor_value += BYTES_PER_PIXEL)
        {
            *(uint16_t *) (g_p_single_buffer + hor_value) = color;
            *(uint16_t *) (g_p_double_buffer + hor_value) = color;
        }
        start_addr = (uint32_t)(start_addr + (g_hz_size * BYTES_PER_PIXEL));
    }
}
#else
static void screen_display(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    /*Declare local variables */
    uint16_t start_x, start_y, display_length, display_height;
    uint32_t start_addr;

    /* Assign co-ordinate values and calculate start address */
    start_x = x1;
    start_y = y1;
    start_addr = (uint32_t)((start_x * BYTES_PER_PIXEL) + (start_y * g_hz_size* BYTES_PER_PIXEL));

    /* Calculate display box length and height */
    display_length = (uint16_t)((x2 - x1) * BYTES_PER_PIXEL);
    display_height = (y2 - y1);

    /* Display required color band */
    for(uint16_t ver_value = Y1_CO_ORDINATE; ver_value < (display_height - INC_DEC_VALUE); ver_value++)
    {
        for(uint32_t hor_value = start_addr; hor_value < (start_addr + display_length); hor_value += BYTES_PER_PIXEL)
        {
            *(uint32_t *) (g_p_single_buffer + hor_value) = color;
            *(uint32_t *) (g_p_double_buffer + hor_value) = color;
        }
        start_addr = (uint32_t)(start_addr + (g_hz_size * BYTES_PER_PIXEL));
    }
}
#endif

/*******************************************************************************************************************//**
 * @brief     This function display eight color band horizontally on Graphical LCD .
 * @param[IN]  None
 * @retval     None
 ***********************************************************************************************************************/
static void color_band_display(void)
{
#ifndef BOARD_RA8D1_EK
    uint16_t color[COLOR_BAND_COUNT]= {RED, GREEN, BLUE, BLACK, WHITE, YELLOW, MAGENTA, CYAN};
#else
    uint32_t color[COLOR_BAND_COUNT]= {RED, GREEN, BLUE, BLACK, WHITE, YELLOW, MAGENTA, CYAN};
#endif
    uint16_t width = g_vr_size/COLOR_BAND_COUNT;

    for (uint8_t display_count = RESET_VALUE; display_count < COLOR_BAND_COUNT; display_count++)
    {
        screen_display((uint16_t)X1_CO_ORDINATE, (display_count * width), g_hz_size, (uint16_t)(((display_count * width) + width) + INC_DEC_VALUE), color[display_count]);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
