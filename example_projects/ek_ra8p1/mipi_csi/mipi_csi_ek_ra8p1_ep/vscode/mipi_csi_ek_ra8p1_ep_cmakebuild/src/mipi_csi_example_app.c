/***********************************************************************************************************************
 * File Name    : mipi_csi_example_app.c
 * Description  : Example application layer for the MIPI CSI EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: mipi_csi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mipi_csi_ep.h"
#include "mipi_csi_example_app.h"
#include "console/uart_console.h"

/* Private variable */
static bool s_is_set_resolution = false;

/* Private functions */
static fsp_err_t csi_check_image(uint8_t * const p_buffer, uint32_t width, uint32_t height);
static fsp_err_t vin_resolution_set(void);
static fsp_err_t camera_mode_selection(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: mipi_csi_example_app_run.
 * Description  : Single entry-point for the MIPI CSI example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void mipi_csi_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

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

    /* Initialize IIC module to control the on-board switch and the camera sensor */
    err = i2c_control_init();
    handle_error(err, "i2c_control_init failed\r\n");

    /* Set the MIPI_SEL_PIN to high for using MIPI CSI */
    err = set_switch_state(MIPI_SEL_PIN, HIGH_STATE);
    handle_error(err, "set_switch_state failed\r\n");

    /* Setup the camera with MIPI CSI configuration */
    err = camera_open();
    handle_error(err, "camera_open failed\r\n");

    /* Start VIN module with default configuration */
    err = vin_camera_start(&g_vin_cfg);
    handle_error(err, "vin_camera_start failed\r\n");

    /* Main loop */
    while (true)
    {
        /* User selects the camera resolution */
        err = vin_resolution_set();
        handle_error(err, "vin_resolution_set failed\r\n");

        while (s_is_set_resolution)
        {
            /* User selects the mode of camera */
            err = camera_mode_selection();
            handle_error(err, "camera_mode_selection failed\r\n");

#if (DISPLAY_OUTPUT == 1U)
            do
            {
                g_vsync_flag = false;

                /* Wait for a Vsync event */
                while (!g_vsync_flag);

                /* Update new frame for GLCDC display */
                err = glcdc_buffer_change((uint8_t * const) gp_next_buffer);
                if (FSP_ERR_INVALID_UPDATE_TIMING != err)
                {
                    handle_error(err, "glcdc_buffer_change failed\r\n");
                }
            }
            while (FSP_ERR_INVALID_UPDATE_TIMING == err);
#endif /* DISPLAY_OUTPUT */
        }
    }
}
/***********************************************************************************************************************
* End of function mipi_csi_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: csi_check_image.
 * Description  : This function compares the image data with the color bars.
 * Arguments    : p_buffer                  Pointer to an image buffer.
 *                width                     Width of image.
 *                height                    Height of image.
 * Return Value : FSP_SUCCESS               Checked successfully.
 *                Any other error codes     Failed to check.
 **********************************************************************************************************************/
static fsp_err_t csi_check_image(uint8_t * const p_buffer, uint32_t width, uint32_t height)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t stride_width = RESET_VALUE;
    uint32_t shift_bytes = RESET_VALUE;
    uint32_t * p_one_line = NULL;
    uint32_t * p_refer = NULL;
    uint32_t * p_check = (uint32_t *)p_buffer;
    uint32_t count = RESET_VALUE;
    float rate = 0.0F;

    /* Calculate the necessary shift bytes when changing the resolution */
    stride_width = g_vin_cfg_extend.input_ctrl.preclip.pixel_end + 1;
    shift_bytes = (stride_width - width) / 2;

    /* Get one line data pointer at 1024x600 resolution */
    if (g_camera_profiles[RES_1024x600].width == width && g_camera_profiles[RES_1024x600].height == height)
    {
        p_one_line = (uint32_t *) g_test_pattern_color_one_line_1024x600;
    }

    /* Get one line data pointer at VGA resolution */
    else if (g_camera_profiles[RES_VGA].width == width && g_camera_profiles[RES_VGA].height == height)
    {
        p_one_line = (uint32_t *) g_test_pattern_color_one_line_vga;
    }

    /* Get one line data pointer at QVGA resolution */
    else if (g_camera_profiles[RES_QVGA].width == width && g_camera_profiles[RES_QVGA].height == height)
    {
        p_one_line = (uint32_t *) g_test_pattern_color_one_line_qvga;
    }

    else
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Check data by column */
    for (uint32_t y = 0; y < height; y++)
    {
        /* Reset p_refer pointer at start of one line data */
        p_refer = p_one_line;

        /* Check data by row */
        for (uint32_t x = 0; x < (width / 2) ; x++)
        {
            /* Compare one pixel value pointed to by p_refer and p_check */
            if (*p_refer == *p_check)
            {
                count++;
            }

            /* Next pixel (2 bytes per pixel) */
            p_check++;
            p_refer++;
        }

        /* Skip the padded area at the end of each line in the buffer */
        p_check += shift_bytes;
    }

    /* Calculate accuracy rate */
    rate = (float)count / (float)(width / 2 * height) * 100;

    /* Check accuracy rate of image data */
    if (MATCH_RATE_MIN < rate)
    {
        printf("\r\nImage data matches color bars to accuracy ratio: %.2f%%", rate);
    }
    else
    {
        return FSP_ERR_INVALID_DATA;
    }

    return err;
}
/***********************************************************************************************************************
* End of function csi_check_image
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: vin_resolution_set.
 * Description  : This function configures the VIN and GLCDC modules with the selected resolution.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Configured successfully.
 *                Any other error codes     Failed to configure.
 **********************************************************************************************************************/
static fsp_err_t vin_resolution_set(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t input_value = RESET_VALUE;
    char user_input_buf[BUFFER_SIZE] = {NULL_CHAR};

    printf(EP_MENU);

    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    /* Read user input data from terminal */
    user_input(user_input_buf, (int)sizeof(user_input_buf));

    /* Convert to integer value */
    input_value = (uint8_t)atoi(user_input_buf);

    if ((input_value < RES_MAX) && (input_value > RESET_VALUE))
    {
        /* Update the image size according to the selected resolution */
        g_image_width = g_camera_profiles[input_value].width;
        g_image_height = g_camera_profiles[input_value].height;

#if (DISPLAY_OUTPUT == 1U)
        /* Re-initialize and start the GLCDC module with selected resolution */
        err = glcdc_init();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
#endif /* DISPLAY_OUTPUT */

        /* Clear old images in SDRAM */
        memset(vin_image_buffer_1, RESET_VALUE, VIN_BYTES_PER_FRAME);
        memset(vin_image_buffer_2, RESET_VALUE, VIN_BYTES_PER_FRAME);
        memset(vin_image_buffer_3, RESET_VALUE, VIN_BYTES_PER_FRAME);

        /* Scale the output image via VIN module */
        err = vin_scale_image(g_image_width, g_image_height);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Start VIN with run-time configuration */
        err = vin_camera_start(&g_vin_cfg_run_time);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Set the s_is_set_resolution flag to exit the resolution selection and proceed to the camera mode selection */
        s_is_set_resolution = true;

        printf(SUB_MENU);
    }
    else
    {
        printf("\r\nSelected resolution is invalid!");
    }

    return err;
}
/***********************************************************************************************************************
* End of function vin_resolution_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: camera_mode_selection.
 * Description  : This function handles camera mode selection.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t camera_mode_selection(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char user_input_buf[BUFFER_SIZE] = {NULL_CHAR};
    uint8_t input_value = RESET_VALUE;

    if (uart_console_has_char())
    {
        /* Read user input data from terminal */
        user_input(user_input_buf, (int)sizeof(user_input_buf));
        input_value = (uint8_t)atoi(user_input_buf);

        if (SELECT_LIVE_CAMERA == input_value)
        {
            /* Change to live camera mode */
            err = camera_write_array(live_camera);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            printf("\r\nLive camera streaming started");
            printf(SUB_MENU);
        }
        else if (SELECT_TEST_PATTERN == input_value)
        {
            /* Change to camera test pattern mode */
            err = camera_write_array(color_bar_test_pattern);
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Check image data */
            err = csi_check_image((uint8_t *)gp_next_buffer, g_image_width, g_image_height);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            printf(SUB_MENU);
        }
        else if (BACK_TO_MAIN_MENU == input_value)
        {
            /* Clear s_is_set_resolution flag to return to main menu */
            s_is_set_resolution = false;
            return FSP_SUCCESS;
        }
        /* Selected is invalid */
        else
        {
            printf("\r\nSelected mode is invalid!");
            printf(SUB_MENU);
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function camera_mode_selection
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: user_input.
 * Description  : This function reads a line of user input from stdin and removes trailing newline characters.
 * Arguments    : buf                       Pointer to buffer where the input string is stored.
 *                len                       Maximum number of characters to read (buffer size).
 * Return Value : None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}
/***********************************************************************************************************************
* End of function user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        mipi_csi_deinit();
        i2c_control_deinit();
        camera_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
