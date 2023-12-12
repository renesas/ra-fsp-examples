/***********************************************************************************************************************
 * File Name    : ceu.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "ceu.h"
#include "ov3640.h"
#include "math.h"

/* External variable */
extern const sensor_reg_t g_ov3640_sxga[];
extern const sensor_reg_t g_ov3640_vga[];
#if OV3640_TEST_PATTERN
extern const uint32_t g_ov3640_vga_color_one_line[];
extern const uint32_t g_ov3640_sxga_color_one_line[];
#endif

/* Global variable */
uint8_t g_image_vga_sram [ VGA_WIDTH * VGA_HEIGHT * BYTE_PER_PIXEL ] BSP_ALIGN_VARIABLE(8);
uint8_t g_image_sxga_sdram [ SXGA_WIDTH * SXGA_HEIGHT * BYTE_PER_PIXEL ] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint32_t g_image_width = RESET_VALUE;
uint32_t g_image_height = RESET_VALUE;
uint8_t * gp_image_buffer = NULL;
sensor_reg_t const * gp_ov3640_resolution = NULL;
capture_instance_t const * gp_ceu_instance = NULL;
volatile bool g_capture_ready = false;

/* Functions declarations */
#if OV3640_TEST_PATTERN
static fsp_err_t ceu_check_image (uint8_t * const p_buffer, uint32_t width, uint32_t height);

/*******************************************************************************************************************//**
 *  @brief      This function compares the image data with the color bar.
 *  @param[in]  p_buffer   pointer to an image buffer
 *  @param[in]  width      width of image
 *  @param[in]  height     height of image
 *  @retval     FSP_SUCCESS   Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ceu_check_image (uint8_t * const p_buffer, uint32_t width, uint32_t height)
{
    fsp_err_t   err                                 = FSP_SUCCESS;
    uint32_t    * p_one_line                        = NULL;
    uint32_t    * p_refer                           = NULL;
    uint32_t    * p_check                           = (uint32_t *)p_buffer;
    uint32_t    count                               = RESET_VALUE;
    float       rate                                = 0.0;
    char        str_rate[MATCH_RATE_STRING_LEN_MAX] = {RESET_VALUE};

    /* Get one line data pointer at SXGA resolution */
    if(SXGA_WIDTH == width && SXGA_HEIGHT == height)
    {
        p_one_line = (uint32_t *)g_ov3640_sxga_color_one_line;
    }

    /* Get one line data pointer at VGA resolution */
    else if (VGA_WIDTH == width && VGA_HEIGHT == height)
    {
        p_one_line = (uint32_t *)g_ov3640_vga_color_one_line;
    }

    /* Invalid resolution */
    else
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_ARGUMENT, " ** Width of image is not supported ** \r\n");
    }

    /* Check data by column */
    for(uint32_t y = 0; y < height; y++)
    {
        /* Reset p_check pointer at start of one line data */
        p_refer = p_one_line;

        /* Check data by row */
        for(uint32_t x = 0; x < (width / 2) ; x ++)
        {
            if(*p_refer == *p_check)
            {
                count ++;
            }

            /* Next address */
            p_check ++;
            p_refer ++;
        }
    }
    /* Calculate accuracy rate */
    rate = (float)count / (float)(width / 2 * height) * 100;

    /* Check accuracy rate of image data */
    if(MATCH_RATE_MIN < rate)
    {
        /* Convert float value to string */
        sprintf(str_rate,"%.2f", rate);
        APP_PRINT("\r\nImage data matches color bars to accuracy ratio: %s%%\r\n", str_rate);
    }
    else
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_DATA, "Image data does not match color bars");
    }
    return err;
}
#endif

/*******************************************************************************************************************//**
 *  @brief      ceu sxga callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void g_ceu_sxga_callback (capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event )
    {
        g_capture_ready = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief      ceu vga callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void g_ceu_vga_callback (capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event )
    {
        g_capture_ready = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief      ceu operation function
 *  @param[in]  p_instance : ceu instance pointer
 *  @param[in]  p_buffer : image buffer pointer
 *  @param[in]  width : width of image
 *  @param[in]  height : height of image
 *  @retval     FSP_SUCCESS   Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ceu_operation (capture_instance_t const * p_instance, uint8_t * const p_buffer, uint32_t width, uint32_t height)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t ceu_timeout = UINT8_MAX;

    /* Initialize CEU module with the configuration specified by CEU instance pointer */
    err = R_CEU_Open(p_instance->p_ctrl, p_instance->p_cfg);
    APP_ERR_RETURN(err, " ** R_CEU_Open API FAILED ** \r\n");

    /* Clean image buffer */
    memset(p_buffer, RESET_VALUE, width * height * BYTE_PER_PIXEL);

    /* Print capture operation start */
    APP_PRINT("\r\nImage Capturing Operation started\r\n");

    /* Start capture image and store it in the buffer specified by image buffer pointer */
    err = R_CEU_CaptureStart(p_instance->p_ctrl, p_buffer);
    APP_ERR_RETURN(err, " ** R_CEU_CaptureStart API FAILED ** \r\n");

    /*  Wait until CEU callback triggers */
    while (true != g_capture_ready)
    {
        /* Start checking for time out to avoid infinite loop */
        ceu_timeout --;
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

        /* Check for time elapse*/
        if (RESET_VALUE == ceu_timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, " ** CEU Callback event not received ** \r\n");
        }
    }

    /* Print success notice */
    APP_PRINT("\r\nCEU Capture Successful !\r\n");

#if OV3640_TEST_PATTERN
    /* Check image data */
    err = ceu_check_image(p_buffer, width, height);
    APP_ERR_RETURN(err, " ** ceu_check_image FAILED ** \r\n");
#endif

    /* Reset capture flag */
    g_capture_ready = false;

    /* Close opened CEU instance */
    err = R_CEU_Close(p_instance->p_ctrl);
    APP_ERR_RETURN(err, " ** R_CEU_Close API FAILED ** \r\n");
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      selection_menu function
 *  @param[in]  None
 *  @retval     None
 **********************************************************************************************************************/
void selection_menu (void)
{
    uint8_t rtt_read[BUFFER_SIZE_DOWN] = {RESET_VALUE};

    /* Select type of memory to store the image buffer */
    APP_PRINT(MAIN_MENU);

    /* RTT input handle */
    while(true)
    {
        if(APP_CHECK_DATA)
        {
            /* Clean RTT Viewer input buffer */
            memset(rtt_read, NULL_CHAR, BUFFER_SIZE_DOWN);

            /* Read RTT Viewer input data */
            APP_READ(rtt_read);

            /* Check select SDRAM to store image buffer */
            if(SELECT_SDRAM_CHAR == rtt_read[INDEX_CHECK])
            {
                g_image_width = SXGA_WIDTH;
                g_image_height = SXGA_HEIGHT;
                gp_image_buffer = g_image_sxga_sdram;
                gp_ov3640_resolution = g_ov3640_sxga;
                gp_ceu_instance = &g_ceu_sxga;
                APP_PRINT("\r\nSelected SDRAM to store the image buffer."
                          "\r\nImage resolution is fixed at SXGA 1280x960.\r\n");
                return;
            }
            /* Check select SRAM to store image buffer */
            else if(SELECT_SRAM_CHAR == rtt_read[INDEX_CHECK])
            {
                g_image_width = VGA_WIDTH;
                g_image_height = VGA_HEIGHT;
                gp_image_buffer = g_image_vga_sram;
                gp_ov3640_resolution = g_ov3640_vga;
                gp_ceu_instance = &g_ceu_vga;
                APP_PRINT("\r\nSelected SRAM to store the image buffer."
                          "\r\nImage resolution is fixed at VGA 640x480.\r\n");
                return;
            }
            /* Selected is invalid */
            else
            {
                APP_PRINT("\r\nSelected is invalid !\r\n");
                APP_PRINT(MAIN_MENU);
            }
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function handle error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   status    error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error (fsp_err_t err,  const char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Close opened CEU module*/
        if(0U != g_ceu_vga_ctrl.open)
        {
            if(FSP_SUCCESS != R_CEU_Close(&g_ceu_vga_ctrl))
            {
                APP_ERR_PRINT("R_CEU_Close FAILED\r\n");
            }
        }
        if(0U != g_ceu_sxga_ctrl.open)
        {
            if(FSP_SUCCESS != R_CEU_Close(&g_ceu_sxga_ctrl))
            {
                APP_ERR_PRINT("R_CEU_Close FAILED\r\n");
            }
        }

        /* Close opened I2C Master module*/
        if(0U != g_i2c_master_for_ov3640_ctrl.open)
        {
            if(FSP_SUCCESS != R_IIC_MASTER_Close(&g_i2c_master_for_ov3640_ctrl))
            {
                APP_ERR_PRINT("R_IIC_MASTER_Close FAILED\r\n");
            }
        }

        /* Close opened GPT module*/
        if(0U != g_timer_periodic_ctrl.open)
        {
            if(FSP_SUCCESS != R_GPT_Close(&g_timer_periodic_ctrl))
            {
                APP_ERR_PRINT("R_GPT_Close FAILED\r\n");
            }
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
