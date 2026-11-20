/***********************************************************************************************************************
 * File Name    : drw_example_app.c
 * Description  : Example application layer for the DRW EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: drw_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drw_ep.h"
#include "drw_example_app.h"
#include "console/uart_console.h"

/* Variables to store resolution information */
static uint16_t s_hz_size = RESET_VALUE;
static uint16_t s_vr_size = RESET_VALUE;

/* Variables used for buffer usage */
static uint32_t s_buffer_size = RESET_VALUE;
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
static uint32_t * s_p_single_buffer, * s_p_double_buffer;
#else
static uint8_t * s_p_single_buffer, * s_p_double_buffer;
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

/* Private functions */
static void handle_error(fsp_err_t err, const char * err_str);
static void drw_handle_error(d2_s32 err, d2_device * p_dave);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DRW example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void drw_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    d2_s32 drw_err = D2_OK;
    fsp_pack_version_t version = {RESET_VALUE};

#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
    uint32_t * p_framebuffer = NULL;
#else
    uint8_t * p_framebuffer = NULL;
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Get LCDC configuration */
    s_hz_size = (g_display_cfg.input[ARRAY_INDEX].hsize);
    s_vr_size = (g_display_cfg.input[ARRAY_INDEX].vsize);

    /* Initialize buffer pointers */
    s_buffer_size = (uint32_t) (s_hz_size * s_vr_size * BYTES_PER_PIXEL);

#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
    s_p_single_buffer = (uint32_t *) g_display_cfg.input[ARRAY_INDEX].p_base;
#else
    s_p_single_buffer = (uint8_t *) g_display_cfg.input[ARRAY_INDEX].p_base;
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

    /* Set the address of the second frame buffer */
    s_p_double_buffer = (void *) ((uint8_t *) s_p_single_buffer + s_buffer_size);

    /* Initialize GLCDC module */
    err = glcdc_init();
    handle_error(err, "glcdc_init failed\r\n");
    printf("\r\nGLCDC module initialized successfully\r\n");

    /* Wait for a Vsync event */
    while (!g_vsync_flag)
    {
        /* Do nothing */
    }

    /* Reset g_vsync_flag */
    g_vsync_flag = false;

    /* Initialize DRW module */
    drw_err = drw_init();
    drw_handle_error(drw_err, gp_davey);
    printf("\r\nDRW module initialized successfully\r\n");

    printf("\r\nPress any key except Enter to stop rendering and disable the GLCDC module:\r\n");

    while (true)
    {
        /* Swap the active frame buffer */
        p_framebuffer = (p_framebuffer == s_p_single_buffer) ? s_p_double_buffer : s_p_single_buffer;

        do
        {
            /* Now that the frame buffer is ready, update the GLCDC buffer pointer on the next Vsync */
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
            err = glcdc_buffer_change(&g_display_ctrl, (uint8_t*) p_framebuffer, DISPLAY_FRAME_LAYER_1);
#else
            err = glcdc_buffer_change(&g_display_ctrl, p_framebuffer, DISPLAY_FRAME_LAYER_1);
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */
            if (FSP_SUCCESS != err)
            {
                g_vsync_flag = false;

                /* Wait for a Vsync event */
                while (!g_vsync_flag)
                {
                    /* Do nothing */
                }
            }
        }
        while (FSP_ERR_INVALID_UPDATE_TIMING == err);

        g_vsync_flag = false;

        /* Wait for a Vsync event */
        while (!g_vsync_flag)
        {
            /* Do nothing */
        }

        /* Draw the new frame buffer now */
        drw_err = display_draw(p_framebuffer);
        drw_handle_error(drw_err, gp_davey);

        /* Check input data */
        if (uart_console_has_char())
        {
            /* Clean the user input to avoid an infinite loop */
            getchar();

            printf("\r\nTerminating render....\r\n");
            break;
        }
    }

    /* Wait for rendering operations to finish */
    drw_err = d2_flushframe(gp_davey);
    drw_handle_error(drw_err, gp_davey);

    /* De-initialize hardware and close the handle */
    drw_err = d2_deinithw(gp_davey);
    drw_handle_error(drw_err, gp_davey);

    drw_err = d2_closedevice(gp_davey);
    drw_handle_error(drw_err, gp_davey);

    /* Stop GLCDC module */
    err = glcdc_stop();
    handle_error(err, "glcdc_stop failed\r\n");
    printf("\r\nGLCDC module stopped\r\n");

    /* Close GLCDC module */
    glcdc_deinit();
    printf("\r\nGLCDC module closed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        glcdc_deinit();
        drw_deinit();
        __BKPT(0);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error for DRW module by printing error message and code, de-initializing the
 *            related modules, and entering a trap state.
 * @param[in] err                       Error code returned from DRW APIs.
 * @param[in] p_dave                    Pointer to device structure.
 * @retval    None.
 **********************************************************************************************************************/
static void drw_handle_error(d2_s32 err, d2_device * p_dave)
{
    if (D2_OK != err)
    {
        /* Print the error message */
        printf("\r\n[ERR] %s", d2_geterrorstring(p_dave));
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        glcdc_deinit();
        drw_deinit();
        __BKPT(0);
    }
}
