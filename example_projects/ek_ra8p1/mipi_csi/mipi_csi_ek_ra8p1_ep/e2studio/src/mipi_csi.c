/***********************************************************************************************************************
 * File Name    : mipi_csi.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "mipi_csi.h"
#include "math.h"

/* External variables */
extern sensor_reg_t live_camera;
extern sensor_reg_t color_bar_test_pattern;
extern const uint32_t g_test_pattern_color_one_line_1024x600[];
extern const uint32_t g_test_pattern_color_one_line_vga[];
extern const uint32_t g_test_pattern_color_one_line_qvga[];
extern const camera_config_t camera_profiles[RES_MAX];

extern const vin_extended_cfg_t g_vin_cfg_extend;

extern volatile uint8_t g_vsync_flag;

/* Global variables */
uint8_t * gp_next_buffer;
uint16_t g_image_width = RESET_VALUE;
uint16_t g_image_height = RESET_VALUE;
_Bool g_is_set_resolution = false;

vin_extended_cfg_t g_vin_cfg_run_time_extend;
capture_cfg_t g_vin_cfg_run_time;

static fsp_err_t camera_mode_selection (void);
static fsp_err_t csi_check_image (uint8_t * const p_buffer, uint32_t width, uint32_t height);
static fsp_err_t vin_resolution_set (void);
static fsp_err_t vin_camera_start(capture_cfg_t const * p_cfg);
static fsp_err_t vin_scale_image(uint16_t new_width, uint16_t new_height);

/***********************************************************************************************************************
 *  Function Name: mipi_csi_ep_entry
 *  Description  : This function is used to start MIPI CSI example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void mipi_csi_ep_entry(void)
{
    fsp_pack_version_t  version = {RESET_VALUE};
    fsp_err_t           err     = FSP_SUCCESS;

    /* Initialize the terminal */
    TERM_INIT();

    /* Version get API for FSP pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT (EP_INFO);

    /* Initialize IIC module to control the switch onboard and the camera sensor */
    err = i2c_control_init();
    handle_error(err, "i2c_control_init FAILED \r\n");

    /* Set the MIPI_SEL_PIN to high for using MIPI CSI */
    err = set_switch_state(MIPI_SEL_PIN, HIGH_STATE);
    handle_error(err, "set_switch_logic FAILED \r\n");

    /* Setup the camera with MIPI CSI configuration */
    err = camera_open();
    handle_error(err, " ** camera_open FAILED ** \r\n");

    /* Start VIN module with default configuration */
    err = vin_camera_start(&g_vin_cfg);
    handle_error(err, "camera_vin_start FAILED \r\n");

    /* Main loop */
    while(true)
    {
        /* User selects the camera resolution */
        err = vin_resolution_set();
        handle_error(err, " camera_resolution_set FAILED \r\n");

        while(g_is_set_resolution)
        {
            /* User selects the mode of camera */
            err = camera_mode_selection();
            handle_error(err, " camera_mode_selection FAILED \r\n");

#if (DISPLAY_OUTPUT == 1U)
            do
            {
                g_vsync_flag = RESET_FLAG;
                /* Wait for a Vsync event */
                while(!g_vsync_flag);

                /* Update new frame for GLCDC display */
                err = R_GLCDC_BufferChange(&g_display_ctrl, (uint8_t * const) gp_next_buffer, DISPLAY_FRAME_LAYER_1);
                if (FSP_ERR_INVALID_UPDATE_TIMING != err)
                {
                    handle_error(err, "** R_GLCDC_BufferChange API FAILED **\r\n");
                }
            }
            while (FSP_ERR_INVALID_UPDATE_TIMING == err);
#endif /* DISPLAY_OUTPUT */
        }

    }
}
/***********************************************************************************************************************
* End of function mipi_csi_ep_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: csi_check_image
 *  Description  : This function is used to compare the image data with the color bar.
 *  Arguments    : p_buffer       Pointer to an image buffer
 *                 width          Width of image
 *                 height         Height of image
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t csi_check_image (uint8_t * const p_buffer, uint32_t width, uint32_t height)
{
    fsp_err_t   err                                 = FSP_SUCCESS;

    uint32_t    stride_width                        = RESET_VALUE;
    uint32_t    shift_bytes                         = RESET_VALUE;
    uint32_t    * p_one_line                        = NULL;
    uint32_t    * p_refer                           = NULL;
    uint32_t    * p_check                           = (uint32_t *)p_buffer;
    uint32_t    count                               = RESET_VALUE;
    float       rate                                = 0.0;
    char        str_rate[MATCH_RATE_STRING_LEN_MAX] = {RESET_VALUE};

    /* Calculate the necessary shift bytes when changing the resolution */
    stride_width = g_vin_cfg_extend.input_ctrl.preclip.pixel_end + 1;
    shift_bytes = (stride_width - width) / 2;

    /* Get one line data pointer at 1024x600 resolution */
    if (camera_profiles[RES_1024x600].width == width && camera_profiles[RES_1024x600].height == height)
    {
        p_one_line = (uint32_t *) g_test_pattern_color_one_line_1024x600;
    }
    /* Get one line data pointer at VGA resolution */
    else if (camera_profiles[RES_VGA].width == width && camera_profiles[RES_VGA].height == height)
    {
        p_one_line = (uint32_t *) g_test_pattern_color_one_line_vga;
    }
    /* Get one line data pointer at QVGA resolution */
    else if (camera_profiles[RES_QVGA].width == width && camera_profiles[RES_QVGA].height == height)
    {
        p_one_line = (uint32_t *) g_test_pattern_color_one_line_qvga;
    }
    else
    {
        err = FSP_ERR_INVALID_ARGUMENT;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Resolution of image is not supported\r\n");
    }

    /* Check data by column */
    for(uint32_t y = 0; y < height; y++)
    {
        /* Reset p_refer pointer at start of one line data */
        p_refer = p_one_line;

        /* Check data by row */
        for(uint32_t x = 0; x < (width / 2) ; x ++)
        {
            /* Compare one pixel value pointed to by p_refer and p_check */
            if(*p_refer == *p_check)
            {
                count ++;
            }

            /* Next pixel (2 bytes per pixel) */
            p_check ++;
            p_refer ++;
        }
        /* Skip the padded area at the end of each line in the buffer */
        p_check += shift_bytes;
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
        err = FSP_ERR_INVALID_DATA;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Image data does not match color bars\r\n");
    }
    return err;
}
/***********************************************************************************************************************
* End of function csi_check_image
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: vin_callback
 *  Description  : This function is used to get captured image from VIN callback
 *  Arguments    : p_args       Pointer to callback argument
 *  Return Value : None
 **********************************************************************************************************************/
void vin_callback (capture_callback_args_t * p_args)
 {
     vin_module_status_t    module_status    = (vin_module_status_t) p_args->event_status;
     vin_interrupt_status_t interrupt_status = (vin_interrupt_status_t) p_args->interrupt_status;
     FSP_PARAMETER_NOT_USED(module_status);

     switch (p_args->event)
     {
         case VIN_EVENT_NOTIFY:
         {
             if (interrupt_status.bits.frame_complete)
             {
                 gp_next_buffer = p_args->p_buffer;
             }

             break;
         }

         case VIN_EVENT_ERROR:
         {
             break;
         }

         default:
         {
             /* Do nothing */
             break;
         }
     }
 }
/***********************************************************************************************************************
* End of function vin_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: mipi_csi0_callback
 *  Description  : This function is used to handle MIPI CSI event
 *  Arguments    : p_args      Pointer to callback argument
 *  Return Value : None
 **********************************************************************************************************************/
 void mipi_csi0_callback (mipi_csi_callback_args_t * p_args)
 {
     switch (p_args->event)
     {
         /*
          * In this project, the application does not rely on any MIPI CSI events.
          * FRAME_DATA event is ignored because VIN automatically handles captured frames.
          * DATA_LANE event status changes are not monitored (stable single-lane setup).
          * VIRTUAL_CHANNEL event is unused as only one channel 0 is active.
          * POWER and SHORT_PACKET_FIFO events are ignored for simplicity.
          */
         case MIPI_CSI_EVENT_DATA_LANE:
         case MIPI_CSI_EVENT_FRAME_DATA:
         case MIPI_CSI_EVENT_POWER:
         case MIPI_CSI_EVENT_SHORT_PACKET_FIFO:
         case MIPI_CSI_EVENT_VIRTUAL_CHANNEL:
             break;

         default:
             break;
     }
 }
 /***********************************************************************************************************************
* End of function mipi_csi0_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: vin_resolution_set
 *  Description  : This function is used to configure the VIN, GLCDC with the selected resolution
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
 static fsp_err_t vin_resolution_set (void)
 {
     fsp_err_t err = FSP_SUCCESS;

     uint8_t input_value = RESET_VALUE;
     uint8_t user_input[TERM_BUFFER_SIZE + 1] = {RESET_VALUE};

     APP_PRINT(MAIN_MENU);

     while (!APP_CHECK_DATA) {;}

     /* Clean user input buffer */
     memset(user_input, NULL_CHAR, sizeof(user_input));

     /* Read user input data from terminal */
     TERM_READ(user_input, sizeof(user_input));

     /* Convert to integer value */
     input_value = (uint8_t)atoi((char*) &user_input[0]);

     if (input_value < RES_MAX && input_value > RESET_VALUE)
     {
         /* Update the image size according to the selected resolution */
         g_image_width = camera_profiles[input_value].width;
         g_image_height = camera_profiles[input_value].height;

#if (DISPLAY_OUTPUT == 1U)
         /* Re-Initialize and start the GLCDC module with selected resolution */
         err = glcdc_init();
         APP_ERR_RET(FSP_SUCCESS != err, err, "glcdc_init FAILED \r\n");
#endif /* DISPLAY_OUTPUT */

         /* Clear old images in SDRAM */
         memset(vin_image_buffer_1, RESET_VALUE, VIN_BYTES_PER_FRAME);
         memset(vin_image_buffer_2, RESET_VALUE, VIN_BYTES_PER_FRAME);
         memset(vin_image_buffer_3, RESET_VALUE, VIN_BYTES_PER_FRAME);

         /* Scale the output image via VIN module */
         err = vin_scale_image(g_image_width, g_image_height);
         APP_ERR_RET(FSP_SUCCESS != err, err, "vin_scale_image FAILED \r\n");

         /* Start VIN with run-time configuration */
         err = vin_camera_start(&g_vin_cfg_run_time);
         APP_ERR_RET( FSP_SUCCESS != err, err, " ** vin_camera_start FAILED ** \r\n");

         /* Set the g_is_set_resolution flag to exit the resolution selection and proceed to the camera mode selection. */
         g_is_set_resolution = true;

         APP_PRINT(CAMERA_MODE_SELECTION);

     }
     else
     {
         APP_PRINT("\r\nSelected resolution is invalid !\r\n");
     }

     return err;
 }
 /***********************************************************************************************************************
* End of function vin_resolution_set
***********************************************************************************************************************/

 /***********************************************************************************************************************
 *  Function Name: vin_camera_start
 *  Description  : This function is used to initialize the VIN module with inputed configuration
 *  Arguments    : p_cfg          Pointer to selected VIN configuration
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t vin_camera_start(capture_cfg_t const * p_cfg)
 {
     fsp_err_t err;

     /* Sanity-check the input pointer */
     if (p_cfg == NULL)
     {
         return FSP_ERR_INVALID_ARGUMENT;
     }

     /* Stop streaming before de-initialing VIN module */
     err = camera_stream_off();
     APP_ERR_RET( FSP_SUCCESS != err, err, " ** camera_stream_off FAILED ** \r\n");

     /* De-Initialize VIN module if it was opened */
     if (MODULE_CLOSE != g_vin_ctrl.open)
     {
         err = R_VIN_Close(&g_vin_ctrl);
         APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_VIN_Close API FAILED ** \r\n");
     }

     /* Power down camera after de-initialing VIN module */
     err = write_reg_16bit(SYS_CTRL0_REG, SYS_CTRL0_SW_PWDN);
     APP_ERR_RET( FSP_SUCCESS != err, err, "write_reg_16bit to power down camera failed\r\n");

     /* Initialize VIN with the provided configuration */
     err = R_VIN_Open(&g_vin_ctrl, p_cfg);
     APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_VIN_Open API FAILED ** \r\n");

     /* Start VIN capture engine */
     err = R_VIN_CaptureStart(&g_vin_ctrl, NULL);
     APP_ERR_RET( FSP_SUCCESS != err, err, " ** R_VIN_CaptureStart API FAILED ** \r\n");

     /* Start the camera sensor streaming */
     err = camera_stream_on();
     APP_ERR_RET( FSP_SUCCESS != err, err, " ** camera_stream_on FAILED ** \r\n");

     return err;
 }
/***********************************************************************************************************************
* End of function vin_camera_start
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: vin_scale_image
 *  Description  : This function is used to change the VIN parameters according to the inputed size
 *  Arguments    : new_width      Target width of image
 *                 new_height     Target height of image
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t vin_scale_image(uint16_t new_width, uint16_t new_height)
 {
     /* Validate arguments (avoid division by zero later) */
     if ((new_width == 0U) || (new_height == 0U))
     {
         return FSP_ERR_INVALID_ARGUMENT;
     }

     /* Copy the default extended configuration into the runtime copy */
     g_vin_cfg_run_time_extend = g_vin_cfg_extend;

     /* Fetch old dimensions and masks from the default (template) config */
     const uint32_t old_v  = (uint32_t) g_vin_cfg_extend.conversion_data.uds_clipping_bits.cl_vsize;
     const uint32_t old_h  = (uint32_t) g_vin_cfg_extend.conversion_data.uds_clipping_bits.cl_hsize;
     const uint16_t old_vm = (uint16_t) g_vin_cfg_extend.conversion_data.uds_scale_bits.vertical_mask;
     const uint16_t old_hm = (uint16_t) g_vin_cfg_extend.conversion_data.uds_scale_bits.horizontal_mask;

     /* Recalculate the scale masks: new_mask = (old_mask * old_size) / new_size */
     g_vin_cfg_run_time_extend.conversion_data.uds_scale_bits.vertical_mask =
         (uint16_t) ( ((uint32_t) old_vm * old_v) / new_height );

     g_vin_cfg_run_time_extend.conversion_data.uds_scale_bits.horizontal_mask =
         (uint16_t) ( ((uint32_t) old_hm * old_h) / new_width );

     /* Update clipping sizes to match the new target dimensions */
     g_vin_cfg_run_time_extend.conversion_data.uds_clipping_bits.cl_vsize = (uint16_t) new_height;
     g_vin_cfg_run_time_extend.conversion_data.uds_clipping_bits.cl_hsize = (uint16_t) new_width;

     /* Copy the default main configuration into the runtime copy */
     g_vin_cfg_run_time = g_vin_cfg;

     /* Point p_extend at the freshly updated runtime-extended configuration */
     g_vin_cfg_run_time.p_extend = &g_vin_cfg_run_time_extend;

     return FSP_SUCCESS;
 }
/***********************************************************************************************************************
* End of function vin_scale_image
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: camera_mode_selection
 *  Description  : This function is used to enable or disable test pattern mode of camera sensor
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t camera_mode_selection (void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t user_input[TERM_BUFFER_SIZE + 1] = {RESET_VALUE};

    if(APP_CHECK_DATA)
    {
        /* Clean user input buffer */
        memset(user_input, NULL_CHAR, sizeof(user_input));

        /* Read user input data from terminal */
        TERM_READ(user_input, sizeof(user_input));

        if(SELECT_LIVE_CAMERA == user_input[INDEX_CHECK])
        {
            /* Change to live camera mode */
            err = camera_write_array(&live_camera);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Change to live camera mode FAILED \r\n");

            APP_PRINT("\r\nLive camera streaming started\r\n");
        }
        else if(SELECT_TEST_PATTERN == user_input[INDEX_CHECK])
        {
            /* Change to camera test pattern mode */
            err = camera_write_array(&color_bar_test_pattern);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Change to camera test mode FAILED \r\n");

            /* Check image data */
            err = csi_check_image(gp_next_buffer, g_image_width, g_image_height);
            APP_ERR_RET(FSP_SUCCESS != err, err, "csi_check_image FAILED \r\n");
        }
        else if (BACK_TO_MAIN_MENU == user_input[INDEX_CHECK])
        {
            /* Clear g_is_set_resolution flag to return to main menu */
            g_is_set_resolution = false;
            return FSP_SUCCESS;
        }
        /* Selected is invalid */
        else
        {
            APP_PRINT("\r\nSelected mode is invalid !\r\n");
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function camera_mode_selection
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : This function close all opened modules, print and trap error.
 *  Arguments    : err            error code
 *                 err_str        error string
 *  Return Value : None
 **********************************************************************************************************************/
void handle_error (fsp_err_t err, char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Close opened VIN module*/
        if(0U != g_vin_ctrl.open)
        {
            if(FSP_SUCCESS != R_VIN_Close(&g_vin_ctrl))
            {
                APP_ERR_PRINT("R_VIN_Close FAILED\r\n");
            }
        }

        /* Close opened I2C Master module*/
        if(0U != g_i2c_master_for_peripheral_ctrl.open)
        {
            if(FSP_SUCCESS != R_IIC_MASTER_Close(&g_i2c_master_for_peripheral_ctrl))
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
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
