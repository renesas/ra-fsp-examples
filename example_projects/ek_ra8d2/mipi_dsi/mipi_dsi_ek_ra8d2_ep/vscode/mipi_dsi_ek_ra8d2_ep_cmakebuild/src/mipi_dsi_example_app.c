/***********************************************************************************************************************
 * File Name    : mipi_dsi_example_app.c
 * Description  : Example application layer for the MIPI DSI EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: mipi_dsi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "mipi_dsi_ep.h"
#include "mipi_dsi_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static volatile bool s_ulps_flag = false;
static bool s_guide_displayed = false;
static uint16_t period_sec = RESET_VALUE;
static uint16_t s_hz_size, s_vr_size;
static uint32_t s_buffer_size, s_hstride;
static uint32_t * sp_single_buffer = NULL;
static uint32_t * sp_double_buffer = NULL;
static uint32_t * sp_frame_buffer = NULL;
static coord_t touch_coordinates[5];

/* Private functions */
static void mipi_dsi_start_display(void);
static void mipi_dsi_ulps_enter(void);
static void mipi_dsi_ulps_exit(void);
static fsp_err_t mipi_dsi_set_display_time(void);
static void display_draw(uint32_t * framebuffer);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the MIPI DSI example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void mipi_dsi_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);
    printf(EP_MENU);

    /* Initialize GLCDC module */
    err = mipi_dsi_display_open();
    handle_error(err, "mipi_dsi_display_open failed\r\n");

    /* Initialize GPT module */
    err = mipi_dsi_gpt_open();
    handle_error(err, "mipi_dsi_gpt_open failed\r\n");

    /* LCD reset */
    touch_screen_reset();

    /* Initialize I2C Master module */
    err = mipi_dsi_touch_i2c_open();
    handle_error(err, "mipi_dsi_touch_i2c_open failed\r\n");

    /* Initialize LCD */
    err = mipi_dsi_push_table(g_lcd_init_focuslcd);
    handle_error(err, "mipi_dsi_push_table failed\r\n");

    /* Initialize ICU module */
    err = mipi_dsi_touch_irq_open();
    handle_error(err, "mipi_dsi_touch_irq_open failed\r\n");

    /* Start display 8-color bars */
    mipi_dsi_start_display();
}

/*******************************************************************************************************************//**
 * @brief     This function starts video mode and draws color bands on the LCD screen.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void mipi_dsi_start_display(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get LCDC configuration */
    s_hz_size = (g_display_cfg.input[0].hsize);
    s_vr_size = (g_display_cfg.input[0].vsize);
    s_hstride = (g_display_cfg.input[0].hstride);

    /* Initialize buffer pointers */
    s_buffer_size = (uint32_t) (s_hz_size * s_vr_size * BYTES_PER_PIXEL);
    sp_single_buffer = (uint32_t *) g_display_cfg.input[0].p_base;

    /* Double buffer for drawing color bands with good quality */
    sp_double_buffer = sp_single_buffer + s_buffer_size;

    /* Get timer information */
    err = mipi_dsi_timer_info_get();
    handle_error(err, "mipi_dsi_timer_info_get failed\r\n");

    /* Start video mode */
    err = mipi_dsi_display_start();
    handle_error(err, "mipi_dsi_display_start failed\r\n");

    /* Enable external interrupt */
    err = mipi_dsi_touch_irq_enable();
    handle_error(err, "mipi_dsi_touch_irq_enable failed\r\n");

    while (true)
    {
        uint8_t status_register = RESET_VALUE;
        bool touch_flag = false;

        /* The user selects time to enter ULPS */
        err = mipi_dsi_set_display_time();
        handle_error(err, "mipi_dsi_set_display_time failed\r\n");

        if (g_irq_state)
        {
            g_irq_state = false;

            /* Get buffer status from gt911 device */
            memset(touch_coordinates, 0, sizeof(touch_coordinates));
            err = gt911_get_status(&status_register, &touch_coordinates[0],
                                   sizeof(touch_coordinates) / sizeof(touch_coordinates[0]));
            handle_error(err, "gt911_get_status failed\r\n");

            if (status_register & GT911_BUFFER_STATUS_READY)
            {
                touch_flag = true;

                /* Reset display time when touch is detected */
                err = mipi_dsi_gpt_reset();
                g_timer_overflow = false;
                handle_error(err, "mipi_dsi_gpt_reset failed\r\n");
            }
        }

        if (s_ulps_flag)
        {
            if (touch_flag || g_timer_overflow)
            {
                /* Exit Ultra-low Power State (ULPS) and turn on the backlight */
                mipi_dsi_ulps_exit();

                /* Reset g_timer_overflow flag */
                g_timer_overflow = false;

                /* Set display time */
                err = timer_period_set(period_sec);
                handle_error(err, "timer_period_set failed\r\n");
            }
        }
        else
        {
            if (!g_timer_overflow)
            {
                /* Swap the active frame buffer */
                sp_frame_buffer = (sp_frame_buffer == sp_single_buffer) ? sp_double_buffer : sp_single_buffer;

                /* Display color bands on LCD screen */
                display_draw(sp_frame_buffer);

                /* Now that the frame buffer is ready, update the GLCDC buffer pointer on the next Vsync */
                err = mipi_dsi_display_buffer_change((uint8_t *) sp_frame_buffer);
                handle_error(err, "mipi_dsi_display_buffer_change failed\r\n");

                /* Wait for a Vsync event */
                g_vsync_flag = false;
                while (false == g_vsync_flag);
            }
            else
            {
                g_timer_overflow = false;

                /* Enter Ultra-low Power State (ULPS) and turn off the backlight */
                mipi_dsi_ulps_enter();

                /* Set timer to exit Ultra-low Power State (ULPS) */
                err = timer_period_set(ULPS_EXIT_PERIOD_30SEC);
                handle_error(err, "timer_period_set failed\r\n");
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function enters Ultra-low Power State (ULPS) and turns off the backlight.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void mipi_dsi_ulps_enter(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timedelay_us = ENTER_ULPS_DELAY;

    /* Enter Ultra-low Power State (ULPS) */
    err = mipi_dsi_ulps_enter_start();
    handle_error(err, "mipi_dsi_ulps_enter_start failed\r\n");

    /* Wait for ULPS enter completion */
    err = wait_for_mipi_dsi_event(MIPI_DSI_PHY_STATUS_DATA_LANE_ULPS_ENTER);
    handle_error(err, "wait_for_mipi_dsi_event failed\r\n");
    s_ulps_flag = true;
    printf("\r\nEntered Ultra-low Power State (ULPS)");

    /* Wait about 8 seconds */
    while (!g_irq_state)
    {
        timedelay_us--;
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);

        /* Check for time elapse */
        if (RESET_VALUE == timedelay_us)
        {
            /* Display off */
            R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_BACKLIGHT, BSP_IO_LEVEL_LOW);
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function exits Ultra-low Power State (ULPS) and turns on the backlight.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void mipi_dsi_ulps_exit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Exit Ultra-low Power State (ULPS) */
    err = mipi_dsi_ulps_exit_start();
    handle_error(err, "mipi_dsi_ulps_exit_start failed\r\n");

    /* Wait for a ULPS event */
    err = wait_for_mipi_dsi_event(MIPI_DSI_PHY_STATUS_DATA_LANE_ULPS_EXIT);
    handle_error(err, "wait_for_mipi_dsi_event failed\r\n");
    s_ulps_flag = false;

    if (g_timer_overflow)
    {
        printf("\r\nAuto exited Ultra-low Power State (ULPS)");
    }
    else
    {
        printf("\r\nExited Ultra-low Power State (ULPS) due to touch with co-ordinates x: %u; y: %u.",
               touch_coordinates[0].x, touch_coordinates[0].y);
    }

    s_guide_displayed = true;
    printf(EP_GUIDE);

    /* Display on */
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_DISPLAY_BACKLIGHT, BSP_IO_LEVEL_HIGH);
}

/*******************************************************************************************************************//**
 * @brief     This function reads the user selection and sets the time to enter Ultra-Low Power State (ULPS).
 * @param[in] None.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
static fsp_err_t mipi_dsi_set_display_time(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (uart_console_has_char())
    {
        int read_data = getchar();
        bool valid_input = true;

        switch (read_data)
        {
            /* Set 5 seconds to enter Ultra-Low Power State (ULPS) */
            case SELECT_5S:
            {
                printf(MIPI_DSI_INFO_5S);
                period_sec = GPT_DESIRED_PERIOD_5SEC;
                s_guide_displayed = false;
                break;
            }

            /* Set 15 seconds to enter Ultra-Low Power State (ULPS) */
            case SELECT_15S:
            {
                printf(MIPI_DSI_INFO_15S);
                period_sec = GPT_DESIRED_PERIOD_15SEC;
                s_guide_displayed = false;
                break;
            }

            /* Set 30 seconds to enter Ultra-Low Power State (ULPS) */
            case SELECT_30S:
            {
                printf(MIPI_DSI_INFO_30S);
                period_sec = GPT_DESIRED_PERIOD_30SEC;
                s_guide_displayed = false;
                break;
            }

            /* Stop timer to always display */
            case SELECT_DISABLE_ULPS:
            {
                printf(MIPI_DSI_INFO_DISABLE_ULPS);
                g_timer_overflow = false;

                err = mipi_dsi_gpt_stop();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
                s_guide_displayed = true;
                printf(EP_GUIDE);
                break;
            }

            default:
            {
                valid_input = false;
                if (!s_guide_displayed)
                {
                    printf("\r\nInvalid option selected");
                }

                s_guide_displayed = false;
                printf(EP_MENU);
                break;
            }
        }

        if (valid_input && (SELECT_DISABLE_ULPS != read_data))
        {
            /* Set display time */
            err = timer_period_set(period_sec);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function draws 8-color bands into a framebuffer.
 * @param[in] framebuffer               Pointer to the frame buffer.
 * @retval    None.
 **********************************************************************************************************************/
static void display_draw(uint32_t * framebuffer)
{
    /* Draw buffer */
    uint32_t color[COLOR_BAND_COUNT] = {BLUE, LIME, RED, BLACK, WHITE, YELLOW, AQUA, MAGENTA};
    uint16_t bit_width = s_hz_size / COLOR_BAND_COUNT;

    for (uint32_t y = 0; y < s_vr_size; y++)
    {
        for (uint32_t x = 0; x < s_hz_size; x++)
        {
            uint32_t bit = x / bit_width;
            framebuffer[x] = color[bit];
        }
        framebuffer += s_hstride;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related
 *            modules, and entering a trap state.
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
        mipi_dsi_deinit();
        __BKPT(0);
    }
}
