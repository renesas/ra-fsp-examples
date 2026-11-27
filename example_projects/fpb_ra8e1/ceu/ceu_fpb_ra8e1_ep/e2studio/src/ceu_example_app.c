/***********************************************************************************************************************
 * File Name    : ceu_example_app.c
 * Description  : Example application layer for the CEU EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: ceu_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camera_sensor.h"
#include "ceu_example_app.h"
#include "console/uart_console.h"

#if defined (BOARD_RA8E1_FPB)
static uint8_t s_image_qvga_sram[QVGA_WIDTH * QVGA_HEIGHT * BYTE_PER_PIXEL] BSP_ALIGN_VARIABLE(8);
#else
static uint8_t s_image_vga_sram[VGA_WIDTH * VGA_HEIGHT * BYTE_PER_PIXEL] BSP_ALIGN_VARIABLE(8);
static uint8_t s_image_sxga_sdram[SXGA_WIDTH * SXGA_HEIGHT * BYTE_PER_PIXEL] BSP_PLACE_IN_SECTION(".sdram")
               BSP_ALIGN_VARIABLE(8);
#endif /* BOARD_RA8E1_FPB */

static uint32_t s_image_width = RESET_VALUE;
static uint32_t s_image_height = RESET_VALUE;
static uint8_t * sp_image_buffer = NULL;
static sensor_reg_t const * sp_cam_resolution = NULL;
static capture_instance_t const * sp_ceu_instance = NULL;

/* Private functions */
static void ceu_operation(capture_instance_t const * p_instance, uint8_t * p_buffer, uint32_t width, uint32_t height);
#if (CAM_TEST_PATTERN)
static fsp_err_t ceu_check_image(uint8_t * const p_buffer, uint32_t width, uint32_t height);
#endif /* CAM_TEST_PATTERN */
static void select_capture_setting(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the CEU example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ceu_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

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
    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MICROSECONDS);

    /* Release the reset pin */
    R_IOPORT_PinWrite(&g_ioport_ctrl, PIN_CAM_RESET, BSP_IO_LEVEL_HIGH);

    /* Initialize GPT module */
    err = gpt_init(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
    handle_error(err, "gpt_init failed\r\n");

    /* Start GPT module to provide the 24MHz clock frequency output for the camera clock source */
    err = gpt_start(&g_timer_periodic_ctrl);
    handle_error(err, "gpt_start failed\r\n");

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /* Initialize IIC Master module */
    err = iic_master_init(&g_i2c_master_for_cam_ctrl, &g_i2c_master_for_cam_cfg);
    handle_error(err, "iic_master_init failed\r\n");

    /* Initialize camera sensor with output format as YUV422 */
    err = camera_open();
    handle_error(err, "camera_open failed\r\n");

    while (true)
    {
        /* Select capture settings */
        select_capture_setting();

        /* Set camera resolution specified by camera array pointer */
        err = camera_set_resolution(sp_cam_resolution);
        handle_error(err, "camera_set_resolution failed\r\n");

        /* Perform CEU operation specified by CEU instance pointer and store image in buffer
         * specified by image buffer pointer */
        ceu_operation(sp_ceu_instance, sp_image_buffer, s_image_width, s_image_height);
    }
}

/*******************************************************************************************************************//**
 * @brief      This function performs CEU operation.
 * @param[in]  p_instance               Pointer to the CEU instance.
 * @param[in]  p_buffer                 Pointer to an image buffer.
 * @param[in]  width                    Width of the image.
 * @param[in]  height                   Height of the image.
 * @retval     None.
 **********************************************************************************************************************/
static void ceu_operation(capture_instance_t const * p_instance, uint8_t * const p_buffer, uint32_t width,
                          uint32_t height)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t ceu_timeout = UINT8_MAX;

    /* Initialize the CEU module using the configuration specified by the CEU instance pointer */
    err = ceu_init(p_instance->p_ctrl, p_instance->p_cfg);
    handle_error(err, "ceu_init failed\r\n");

    /* Clear image buffer */
    memset(p_buffer, RESET_VALUE, width * height * BYTE_PER_PIXEL);

    /* Print capture operation start */
    printf("\r\nImage capturing operation started");

    /* Start image capture and store the image in the specified buffer */
    err = ceu_capture_start(p_instance->p_ctrl, p_buffer);
    handle_error(err, "ceu_capture_start failed\r\n");

    /* Wait until the CEU callback is triggered */
    while (true != g_capture_ready)
    {
        /* Start checking for time out to avoid infinite loop */
        ceu_timeout--;
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

        /* Check for timeout */
        if (RESET_VALUE == ceu_timeout)
        {
            handle_error(FSP_ERR_TIMEOUT, "CEU callback event did not occur\r\n");
        }
    }

    /* Print success notice */
    printf("\r\nCEU captured successfully!\r\n");

#if (CAM_TEST_PATTERN)
    /* Check image data */
    err = ceu_check_image(p_buffer, width, height);
    handle_error(err, "ceu_check_image failed\r\n");
#endif /* CAM_TEST_PATTERN */

    /* Reset capture flag */
    g_capture_ready = false;

    /* Close the opened CEU instance */
    ceu_deinit(p_instance->p_ctrl);
}

#if (CAM_TEST_PATTERN)
/*******************************************************************************************************************//**
 * @brief     This function compares the image data with the color bar.
 * @param[in] p_buffer                  Pointer to an image buffer.
 * @param[in] width                     Width of the image.
 * @param[in] height                    Height of the image.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t ceu_check_image(uint8_t * const p_buffer, uint32_t width, uint32_t height)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t * p_one_line = NULL;
    uint32_t * p_refer = NULL;
    uint32_t * p_check = (uint32_t *) p_buffer;
    uint32_t count = RESET_VALUE;
    float rate = 0.0F;

#if defined (BOARD_RA8E1_FPB)
    /* Get one line data pointer at QVGA resolution */
    if ((QVGA_WIDTH == width) && (QVGA_HEIGHT == height))
    {
        p_one_line = (uint32_t *) g_cam_qvga_color_one_line;
    }
#else
    /* Get one line data pointer at SXGA resolution */
    if ((SXGA_WIDTH == width) && (SXGA_HEIGHT == height))
    {
        p_one_line = (uint32_t *) g_cam_sxga_color_one_line;
    }

    /* Get one line data pointer at VGA resolution */
    else if ((VGA_WIDTH == width) && (VGA_HEIGHT == height))
    {
        p_one_line = (uint32_t *) g_cam_vga_color_one_line;
    }
#endif /* BOARD_RA8E1_FPB */

    /* Invalid resolution */
    else
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Check data by column */
    for (uint32_t y = 0; y < height; y++)
    {
        /* Reset p_check pointer at start of one line data */
        p_refer = p_one_line;

        /* Check data by row */
        for (uint32_t x = 0; x < (width / 2); x++)
        {
            if (*p_refer == *p_check)
            {
                count++;
            }

            /* Next address */
            p_check++;
            p_refer++;
        }
    }

    /* Calculate accuracy rate */
    rate = (float) count / (float) (width / 2 * height) * 100;

    /* Print the accuracy ratio value */
    printf("\r\nImage data matches color bars with an accuracy ratio of %.2f%%\r\n", rate);

    /* Verify image data accuracy */
    if (MATCH_RATE_MIN > rate)
    {
        return FSP_ERR_INVALID_DATA;
    }

    return err;
}
#endif /* CAM_TEST_PATTERN */

/*******************************************************************************************************************//**
 * @brief     This function selects the image capture settings.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void select_capture_setting(void)
{
    /* Print the main menu */
    printf(MAIN_MENU);

    while (true)
    {
        if (uart_console_has_char())
        {
#if defined (BOARD_RA8E1_FPB)
            /* Read and discard the key that triggered uart_console_has_char() */
            getchar();
            s_image_width = QVGA_WIDTH;
            s_image_height = QVGA_HEIGHT;
            sp_image_buffer = s_image_qvga_sram;
            sp_cam_resolution = g_cam_qvga;
            sp_ceu_instance = &g_ceu_qvga;
            printf(QVGA_INFO);
            return;
#else
            uint8_t user_input = RESET_VALUE;

            /* Get user input */
            user_input = (uint8_t) getchar();

            if (SELECT_SDRAM == user_input)
            {
                s_image_width = SXGA_WIDTH;
                s_image_height = SXGA_HEIGHT;
                sp_image_buffer = s_image_sxga_sdram;
                sp_cam_resolution = g_cam_sxga;
                sp_ceu_instance = &g_ceu_sxga;
                printf(SXGA_INFO);
                return;
            }
            else if (SELECT_SRAM == user_input)
            {
                s_image_width = VGA_WIDTH;
                s_image_height = VGA_HEIGHT;
                sp_image_buffer = s_image_vga_sram;
                sp_cam_resolution = g_cam_vga;
                sp_ceu_instance = &g_ceu_vga;
                printf(VGA_INFO);
                return;
            }
            else
            {
                printf("\r\nInvalid input!\r\n");
                printf(MAIN_MENU);
            }
#endif /* BOARD_RA8E1_FPB */
        }
    }
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
#if defined (BOARD_RA8E1_FPB)
        ceu_deinit(&g_ceu_qvga_ctrl);
#else
        ceu_deinit(&g_ceu_vga_ctrl);
        ceu_deinit(&g_ceu_sxga_ctrl);
#endif /* BOARD_RA8E1_FPB */
        iic_master_deinit(&g_i2c_master_for_cam_ctrl);
        gpt_deinit(&g_timer_periodic_ctrl);
        __BKPT(0);
    }
}
