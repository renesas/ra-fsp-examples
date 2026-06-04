/***********************************************************************************************************************
 * File Name    : ov3640.c
 * Description  : Contains data structures and functions setup OV3640 camera used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ov3640.h"
#include "common_utils.h"
#include "camera_thread.h"

/*******************************************************************************************************************//**
 * @addtogroup ceu_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
static volatile i2c_master_event_t g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

/* OV3640 YUV format Preview
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_fmt_YUV422 [] = {
    {0x304d, 0x45}, {0x30a7, 0x5e}, {0x3087, 0x16}, {0x309c, 0x1a},
    {0x30a2, 0xe4}, {0x30aa, 0x42}, {0x30b0, 0xff}, {0x30b1, 0xff},
    {0x30b2, 0x10}, {0x300e, 0x32}, {0x300f, 0x21}, {0x3010, 0x20},
    {0x3011, 0x04}, {0x304c, 0x81}, {0x30d7, 0x10}, {0x30d9, 0x0d},
    {0x30db, 0x08}, {0x3016, 0x82}, {0x3018, 0x38}, {0x3019, 0x30},
    {0x301a, 0x61}, {0x307d, 0x00}, {0x3087, 0x02}, {0x3082, 0x20},
    {0x3015, 0x12}, {0x3014, 0x04}, {0x3013, 0xf7}, {0x303c, 0x08},
    {0x303d, 0x18}, {0x303e, 0x06}, {0x303f, 0x0c}, {0x3030, 0x62},
    {0x3031, 0x26}, {0x3032, 0xe6}, {0x3033, 0x6e}, {0x3034, 0xea},
    {0x3035, 0xae}, {0x3036, 0xa6}, {0x3037, 0x6a}, {0x3104, 0x02},
    {0x3105, 0xfd}, {0x3106, 0x00}, {0x3107, 0xff}, {0x3300, 0x12},
    {0x3301, 0xde}, {0x3302, 0xef}, {0x3316, 0xff}, {0x3317, 0x00},
    {0x3312, 0x26}, {0x3314, 0x42}, {0x3313, 0x2b}, {0x3315, 0x42},
    {0x3310, 0xd0}, {0x3311, 0xbd}, {0x330c, 0x18}, {0x330d, 0x18},
    {0x330e, 0x56}, {0x330f, 0x5c}, {0x330b, 0x1c}, {0x3306, 0x5c},
    {0x3307, 0x11}, {0x336a, 0x52}, {0x3370, 0x46}, {0x3376, 0x38},
    {0x3300, 0x13}, {0x30b8, 0x20}, {0x30b9, 0x17}, {0x30ba, 0x04},
    {0x30bb, 0x08}, {0x3507, 0x06}, {0x350a, 0x4f}, {0x3100, 0x02},
    {0x3301, 0xde}, {0x3304, 0x00}, {0x3400, 0x00}, {0x3404, 0x00},
    {0x335f, 0x68}, {0x3360, 0x18}, {0x3361, 0x0c}, {0x3362, 0x12},
    {0x3363, 0x88}, {0x3364, 0xe4}, {0x3403, 0x42}, {0x3088, 0x02},
    {0x3089, 0x80}, {0x308a, 0x01}, {0x308b, 0xe0}, {0x308d, 0x04},
    {0x3086, 0x03}, {0x3086, 0x00}, {0x3011, 0x00}, {0x304c, 0x85},
    {0x3600, 0xc0},
    {OV3640_END_OF_ARRAY, 0xff}     //  End of file marker (0xFFFF)
};

/* OV3640 JPEG format Preview
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_fmt_JPEG [] = {
    // Unknown
    {0x304d, 0x45},

    // Reserved
    {0x30a7, 0x5e},

    // Timing control 11
    {0x3087, 0x16},

    // Unknown
    {0x309c, 0x1a},

    // Reserved
    {0x30a2, 0xe4},
    {0x30aa, 0x42},

    // IO Control
    {0x30b0, 0xff}, // IO Control 0
    {0x30b1, 0xff}, // IO Control 1
    {0x30b2, 0x10}, // IO Control 2

    // Clock configure
    {0x300e, 0x32}, // PLL Control 1
    {0x300f, 0x21}, // PLL Control 2
    {0x3010, 0x20}, // PLL Control 3
    {0x3011, 0x00}, // Clock rate control
    {0x304c, 0x81}, // Unknown

    // Unknown
    {0x30d7, 0x10},
    {0x30d9, 0x0d},
    {0x30db, 0x08},

    // Reserved
    {0x3016, 0x82},

    // Determine High and Low Threshold for stable operating region of Luminance, also for outside of control zone
    {0x3018, 0x38}, // Luminance Signal/Histogram High Range for AEC/AGC operation
    {0x3019, 0x30}, // Luminance Signal/Histogram Low Range for AEC/AGC operation
    {0x301a, 0x61}, // Fast Mode Large Step Range Thresholds - effective only in AEC/AGC fast mode

    //Color bar disable
    {0x307d, 0x00},

    // Timing control 11
    {0x3087, 0x02},

    // Reserved
    {0x3082, 0x20},

    // Auto control
    {0x3015, 0x12}, // Auto control 3 - AGC 8x, allow 1 dummy frame
    {0x3014, 0x04}, // Auto control 2 - Banding 60Hz, Disable banding auto detection, Enable banding AEC smooth switch, Enable auto exposure
    {0x3013, 0xf7}, // Auto control 1 - Faster AEC correction, Big step, minimum exposure, auto disable banding filter, AGC mode enable, AEC mode enable

    // Average section width (AHW)
    {0x303c, 0x02},
    {0x303d, 0x04},

    // Average section height (AVH)
    {0x303e, 0x02},
    {0x303f, 0x04},

    // Average section weighting
    {0x3030, 0x62},
    {0x3031, 0x26},
    {0x3032, 0xe6},
    {0x3033, 0x6e},
    {0x3034, 0xea},
    {0x3035, 0xae},
    {0x3036, 0xa6},
    {0x3037, 0x6a},

    // ISP System Control - reset, on/off pipeline module
    {0x3104, 0x02}, // Gated reset0
    {0x3105, 0xfd}, // Gated clk0
    {0x3106, 0x00}, // Gated reset1
    {0x3107, 0xff}, // Gated clk1

    // DSP Control
    {0x3300, 0x12}, // DSP Control 0
    {0x3301, 0xde}, // DSP Control 1
    {0x3302, 0xef}, // DSP Control 2
    {0x336a, 0x52}, // R_A1
    {0x3370, 0x46}, // G_A1
    {0x3376, 0x38}, // B_A1
    {0x3300, 0x13}, // DSP Control 0 - Lenc control - correct light variations enable

    // Unknown
    {0x3316, 0xff},
    {0x3317, 0x00},
    {0x3312, 0x26},
    {0x3314, 0x42},
    {0x3313, 0x2b},
    {0x3315, 0x42},
    {0x3310, 0xd0},
    {0x3311, 0xbd},
    {0x330c, 0x18},
    {0x330d, 0x18},
    {0x330e, 0x56},
    {0x330f, 0x5c},
    {0x330b, 0x1c},

    // Reserved
    {0x3306, 0x5c},
    {0x3307, 0x11},

    // UV Adjust setting
    {0x30b8, 0x20},
    {0x30b9, 0x17},
    {0x30ba, 0x04},
    {0x30bb, 0x08},

    // Unknown
    {0x3507, 0x04}, {0x350a, 0x4f},

    // Compression Related
    {0x3100, 0x32}, // Compression Enable
    {0x3301, 0xde}, // Enable a Reserved bit
    {0x3304, 0x00}, // Disable CMX_bias, GMA_bias, lENC_bias and their bias plus

    // Format MUX
    {0x3400, 0x02}, // FMT_MUX_CTRL0 - Format select: ISP YUV422
    {0x3404, 0x22}, // FMT_CTRL00

    // DSP Control
    {0x335f, 0x68}, // SIZE_IN_MISC - using default value
    {0x3360, 0x18}, // HSIZE_IN_L - using default value
    {0x3361, 0x0c}, // VSIZE_IN_L - using default value
    {0x3362, 0x12}, // SIZE_OUT_MISC
    {0x3363, 0x88}, // HSIZE_OUT_L
    {0x3364, 0xe4}, // HSIZE_OUT_L

    // ISP_PAD_CTRL2 - using default value
    {0x3403, 0x42},

    // Timing control 13
    {0x308d, 0x04}, // OUT_I2C_susp option 2

    // Timing control 10
    {0x3086, 0x03},
    {0x3086, 0x00}, // using default value

    // Don't know why duplicate define
    {0x304c, 0x84}, // Unknown
    {0x3011, 0x00}, // Clock rate control

    // Unknown
    {0x3500, 0x00},

    // OUT_TOP Control register
    {0x3600, 0x42}, // OUT_CTRL00 - DVP Control - Vsyns mode2 select
    {0x3610, 0x00}, // WIDTH_CTRL - width manual
    {0x3611, 0x20}, // OUT_CTRL11

    {OV3640_END_OF_ARRAY, 0xff}     //  End of file marker (0xFFFF)
};



/* OV3640 VGA resolution register configuration
 * The below values are different from the values published from the camera module manufacturer 's datasheet.
 * Please consult the manufacturer for the recommended values for your end application. */
const sensor_reg_t g_ov3640_VGA[] =
{
    // DSP Control
    {0x335f, 0x68}, // SIZE_IN_MISC - using default value
    {0x3360, 0x18}, // HSIZE_IN_L = 818 = 2072 - default
    {0x3361, 0x0c}, // VSIZE_IN_L = 60c = 1548 - default
    {0x3362, 0x12}, // SIZE_OUT_MISC
    {0x3363, 0x88}, // HSIZE_OUT_L = 288 = 648
    {0x3364, 0xe4}, // VSIZE_OUT_L = 1e4 = 484

    // ISP_PAD_CTRL2
    {0x3403, 0x42},

    // ISP X-direction Output Size = 640
    {0x3088, 0x02},
    {0x3089, 0x80},
    // ISP Y-direction Output Size = 480
    {0x308a, 0x01},
    {0x308b, 0xe0},

    // VSYNC Pulse Option
    {0x3075, 0x00},

    // Mirror and flip function control
    {0x307C, 0x12}, // Mirror - Timing control 6
    {0x3090, 0xc8}, // Flip
    {0x3023, 0x0a}, // B/R row adjustment - Vertical window Start 8 LSBs - using default value

    // Auto control
    {0x3015, 0x12}, // Auto control 3 - AGC 8x, allow 1 dummy frame
    {0x3014, 0x04}, // Auto control 2 - Banding 60Hz, Disable banding auto detection, Enable banding AEC smooth switch, Enable auto exposure
    {0x3013, 0xf7}, // Auto control 1 - Faster AEC correction, Big step, minimum exposure, auto disable banding filter, AGC mode enable, AEC mode enable

    // Banding - using default value
    {0x3070, 0x00}, // 50Hz banding 8 MSBs
    {0x3071, 0xeb}, // 50Hz banding 8 LSBs
    {0x3072, 0x00}, // 60Hz banding 8 MSBs
    {0x3073, 0xc4}, // 60Hz banding 8 LSBs

    // AECG_MAX50 - using default value
    {0x301c, 0x05},

    // AECG_MAX60 - default 0x07
    {0x301d, 0x06}, // Max banding for 60Hz in terms of row exposure

    {OV3640_END_OF_ARRAY, 0xff}      //  End of file marker (0xFFFF)
};

/* Functions declarations */
static fsp_err_t i2c_master_wait_event (const i2c_master_event_t i2c_event);
static fsp_err_t ov3640_reg_write (uint16_t address, uint8_t data);
static fsp_err_t ov3640_reg_read (uint16_t address, uint8_t * p_data);
static fsp_err_t ov3640_write_array (sensor_reg_t const * p_array);
static fsp_err_t ov3640_software_reset (void);
static void ov3640_power (ov3640_power_t power_state);

/*******************************************************************************************************************//**
 * @brief       Wait for i2c master event.
 * @param[in]   i2c_event : i2c master events need to wait for
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t i2c_master_wait_event (const i2c_master_event_t i2c_event)
{
    fsp_err_t err      = FSP_SUCCESS;
    uint8_t   time_out = UINT8_MAX;

    /* Wait until event occur */
    while(i2c_event != g_i2c_event_for_ov3640)
    {
        /* Check for aborted event */
        if(I2C_MASTER_EVENT_ABORTED == g_i2c_event_for_ov3640)
        {
            APP_ERR_RET(FSP_ERR_TRANSFER_ABORTED,
                           " ** Error EVENT_ABORTED received during perform i2c operation **\r\n");
        }
        else
        {
            /* Start checking for time out to avoid infinite loop */
            time_out --;
            R_BSP_SoftwareDelay(OV3640_I2C_TIMEOUT_UNIT, BSP_DELAY_UNITS_MICROSECONDS);

            /* Check for time elapse*/
            if (RESET_VALUE == time_out)
            {
                APP_ERR_RET(FSP_ERR_TIMEOUT,
                               " ** I2C Master Callback event not received **\r\n");
            }
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Write data to camera register.
 * @param[in]   address : Register address
 * @param[in]   data : data to be written
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_reg_write (uint16_t address, uint8_t data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[3];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);
    i2c_buffer[2] = data;

    /* Reset callback event */
    g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_ov3640_ctrl, i2c_buffer, 3, false);
    APP_ERR_RET(err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RET(err, " ** i2c_master_wait_event FAILED ** \r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Read data from camera register.
 * @param[in]   address : Register address
 * @param[in]   p_data : pointer used to store register data
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_reg_read (uint16_t address, uint8_t * p_data)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t i2c_buffer[2];

    /* Prepare data for write operation */
    i2c_buffer[0] = (uint8_t)((address >> 8 ) & 0xFF);
    i2c_buffer[1] = (uint8_t)(address & 0xFF);

    /* Reset callback event */
    g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C write operation */
    err = R_IIC_MASTER_Write(&g_i2c_master_for_ov3640_ctrl, i2c_buffer, 2, true);
    APP_ERR_RET(err, " ** R_IIC_MASTER_Write API FAILED ** \r\n");

    /* Wait until write transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_TX_COMPLETE);
    APP_ERR_RET(err, " ** i2c_master_wait_event FAILED ** \r\n");

    /* Reset callback event */
    g_i2c_event_for_ov3640 = (i2c_master_event_t)RESET_VALUE;

    /* Perform I2C read operation */
    err = R_IIC_MASTER_Read(&g_i2c_master_for_ov3640_ctrl, p_data, 1, false);
    APP_ERR_RET(err, " ** R_IIC_MASTER_Read API FAILED ** \r\n");

    /* Wait until read transmission complete */
    err = i2c_master_wait_event (I2C_MASTER_EVENT_RX_COMPLETE);
    APP_ERR_RET(err, " ** i2c_master_wait_event FAILED ** \r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Write data to camera registers.
 * @param[in]   p_array : pointer to array of structures containing a register address and value.
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_write_array (sensor_reg_t const * p_array)
{
    fsp_err_t err   = FSP_SUCCESS;
    uint8_t   value = RESET_VALUE;

    /* Check end of register array */
    while (OV3640_END_OF_ARRAY != p_array->reg)
    {
        /* Write data to the camera register */
        err = ov3640_reg_write(p_array->reg, p_array->val);
        APP_ERR_RET(err, " ** ov3640_reg_write FAILED ** \r\n");

        /* Read-back data from the camera register */
        err = ov3640_reg_read (p_array->reg, &value);
        APP_ERR_RET(err, " ** ov3640_reg_read FAILED ** \r\n");

        /* Compare data written and data read-back */
        if(value != p_array->val)
        {
            APP_ERR_RET(FSP_ERR_ASSERTION, " ** Data Write and read-back data do not match ** \r\n");
        }

        /* Select next register in register array */
        p_array ++;
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief       Software reset the camera
 * @param       None
 * @retval      FSP_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
static fsp_err_t ov3640_software_reset (void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = ov3640_reg_write (OV3640_RESET_ADDRESS, OV3640_RESET_VALUE);
    APP_ERR_RET(err, " ** ov3640_reg_write FAILED ** \r\n");
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Power on the camera
 * @param[in]   power_state : Power state wants to set the camera
 * @retval      None
 **********************************************************************************************************************/
static void ov3640_power (ov3640_power_t power_state)
{
    R_BSP_PinAccessEnable();
    R_BSP_PinWrite(OV3640_CAM_PWR_ON, (bsp_io_level_t)power_state);
    R_BSP_PinAccessDisable();
}

/*******************************************************************************************************************//**
 * @brief       Initialization the camera.
 * @param       None
 * @retval      FSP_SUCCESS    Upon successful initialization.
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov3640_open (sensor_reg_t const * p_array)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Power on the camera */
    ov3640_power (OV3640_POWER_ON);

    /* Software reset the camera */
    err = ov3640_software_reset();
    APP_ERR_RET(err, " ** ov3640_software_reset FAILED ** \r\n");

    /* write array register initialize OV3640 */
    err = ov3640_write_array(p_array);
    APP_ERR_RET(err, " ** open ov3640_write_array FAILED ** \r\n");

    /* Delay 50ms to complete write array register */
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Configure camera resolution.
 * @param[in]   p_array : pointer to array of structures containing a register address and value for set resolution
 * @retval      FSP_SUCCESS    Upon successful initialization.
 * @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t ov3640_set_resolution (sensor_reg_t const * p_array)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Configure the OV3640 camera resolution as selected resolution */
    err = ov3640_write_array(p_array);
    APP_ERR_RET(err, " ** ov3640_write_array FAILED ** \r\n");

    /* Delay 50ms to complete write array register */
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    return err;

    /* Software reset the camera */
    err = ov3640_software_reset();
    APP_ERR_RET(err, " ** ov3640_software_reset FAILED ** \r\n");
}

/*******************************************************************************************************************//**
 *  @brief      iic_master callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void g_i2c_master_for_ov3640_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_i2c_event_for_ov3640 = p_args->event;
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup ceu_ep)
 **********************************************************************************************************************/
