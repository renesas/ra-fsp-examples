/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "camera_layer.h"
#include "hal_data.h"
#include "board_cfg.h"
#include "r_ceu.h"

#include "r_capture_api.h"

// #define USE_DEBUG_BREAKPOINTS 1

#define REG_CLKRC                    0x3011
#define REG_PIDH                     0x300a
#define REG_PIDL                     0x300b
#define REG_SYS                      0x3012
#define SYS_RESET                    0x80

#define CAM_PWDN_PIN                 BSP_IO_PORT_07_PIN_04
#define BSP_I2C_SLAVE_ADDR_CAMERA    (0x3C)   /*Slave address for OV3640 Camera Module */

#define OV7740_REG_TABLE             (ov_reg_t *) OV7740_VGA_YUV422
#define CAM_DATA_READY               (1 << 0) /* UART Console has initiated and is ready for I/O */

// #define OV7740_REG_TABLE                 (ov_reg_t *)OV7740_VGA_YUV422
// #define JPEG_BUFFER_SIZE                 (8192)

// OV7740 registers
#define OV7740_PIDH                      (0x0A)
#define OV7740_PIDH_DEFAULT              (0x77)

#define OV7740_PIDL                      (0x0B)
#define OV7740_PIDL_DEFAULT              (0x42)

#define OV7740_REG0C_MAX_EXPOSURE_Pos    (1)
#define OV7740_REG0C_MAX_EXPOSURE_Msk    (0x3u << OV7740_REG0C_MAX_EXPOSURE_Pos) // (OV7740_REG0C) Max exposure = frame length - limit x 2
#define OV7740_REG0C_MAX_EXPOSURE(value)    ((OV7740_R, {0x335d, 0x30} \
                                              EG0C_MAX_EXPOSURE_Msk &((value) << OV7740_REG0C_MAX_EXPOSURE_Pos)))

#define OV7740_REG0C_MIRROR_ENABLE       (0x1u << 6)                             /**< \brief (OV7740_REG0C) Mirror enable */
#define OV7740_REG0C_FLIP_ENABLE         (0x1u << 7)                             /**< \brief (OV7740_REG0C) Flip enable */
#define OV7740_REG0C_YUV_SWAP_ENABLE     (0x0u << 4)                             /**< \brief (OV7740_REG0C) output UYVYUYVY */
#define OV7740_REG0C_YUV_BIT_SWAP        (0x1u << 3)                             /**< \brief (OV7740_REG0C) output [Y2,Y3…Y8,Y9,Y1,Y0] */

#define RANGE_LIMIT(x)    (x > 255 ? 255 : (x < 0 ? 0 : x))

static void bsp_camera_write_array(st_ov_reg_t * array);

static st_ov_reg_t ov3640_fmt_yuv422_vga[] =
{
    {0x3002, 0x06  }, {0x3003, 0x1F}, {0x3001, 0x12}, {0x304d, 0x45},
    {0x30aa, 0x45  }, {0x30B0, 0xff}, {0x30B1, 0xff}, {0x30B2, 0x10},
    {0x30d7, 0x10  }, {0x3047, 0x00}, {0x3018, 0x60}, {0x3019, 0x58},
    {0x301A, 0xa1  }, {0x3087, 0x02}, {0x3082, 0x20}, {0x303C, 0x08},
    {0x303d, 0x18  }, {0x303e, 0x06},
    {0x303f, 0x0c  }, {0x3030, 0x62}, {0x3031, 0x26}, {0x3032, 0xe6},
    {0x3033, 0x6e  }, {0x3034, 0xea}, {0x3035, 0xae}, {0x3036, 0xa6},
    {0x3037, 0x6a  }, {0x3015, 0x12}, {0x3013, 0xfd}, {0x3104, 0x02},
    {0x3105, 0xfd  }, {0x3106, 0x00}, {0x3107, 0xff}, {0x3308, 0xa5},
    {0x3316, 0xff  }, {0x3317, 0x00}, {0x3087, 0x02}, {0x3082, 0x20},
    {0x3300, 0x13  }, {0x3301, 0xd6}, {0x3302, 0xef}, {0x30B8, 0x20},
    {0x30B9, 0x17  }, {0x30BA, 0x04}, {0x30BB, 0x08},

    {0x3507, 0x06  },
    {0x350a, 0x4f  }, {0x3600, 0xc4}, {0x332B, 0x00}, {0x332D, 0x45},
    {0x332D, 0x60  }, {0x332F, 0x03},
    {0x3100, 0x02  }, {0x3304, 0xfc}, {0x3400, 0x01}, {0x3404, 0x11}, /* RGB565 ceu bit flip 32-bit only */
    {0x3601, 0x01  }, {0x302a, 0x06}, {0x302b, 0x20},
    {0x300E, 0x32  }, {0x300F, 0x21}, {0x3010, 0x21}, {0x3011, 0x01}, /* QXGA PLL setting*/
    {0x304c, 0x81  },
    {0x3602, 0x22  }, {0x361E, 0x00}, {0x3622, 0x18}, {0x3623, 0x69}, /* CSI setting */
    {0x3626, 0x00  }, {0x3627, 0xf0}, {0x3628, 0x00}, {0x3629, 0x26},
    {0x362A, 0x00  }, {0x362B, 0x5f}, {0x362C, 0xd0}, {0x362D, 0x3c},
    {0x3632, 0x10  }, {0x3633, 0x28}, {0x3603, 0x4d}, {0x364C, 0x04},
    {0x309e, 0x00  },
    {0x3020, 0x01  }, {0x3021, 0x1d}, {0x3022, 0x00}, {0x3023, 0x0a}, /* crop window setting*/
    {0x3024, 0x08  }, {0x3025, 0x18}, {0x3026, 0x06}, {0x3027, 0x0c},
    {0x335f, 0x68  }, {0x3360, 0x18}, {0x3361, 0x0c},
    {0x307c, 0x10  }, {0x3023, 0x0a}, {0x3090, 0xc0},                 // 11 Flip, 12 mirror, 13 mirror and flip
    {0x3362, 0x12  }, {0x3363, 0x88}, {0x3364, 0xe4}, {0x3403, 0x42}, /* VGA */
    {0x3088, 0x02  }, {0x3089, 0x80}, {0x308a, 0x01}, {0x308b, 0xe0},
    {0x3355, 0x04  }, {0x3354, 0x01}, {0x335e, 0x00},                 /* brightness */
    {0x3355, 0x04  }, {0x335c, 0x20}, {0x335d, 0x20},                 /* contrast */

    /* Disable Test Pattern Mode */
    {0x306c, 0x10  },                                                 // Disable color bar mode normal image
    {0x307b, 0x40  },                                                 // Disable colour bar pattern
    {0x307d, 0x20  },                                                 // Colour bar Disable

    /* End of file marker (0xFFFF) */
    {0xffff, 0x00ff}
};

static st_ov_reg_t ov3640_fmt_yuv422_vga_test_mode[] =
{
    {0x3002, 0x06  }, {0x3003, 0x1F}, {0x3001, 0x12}, {0x304d, 0x45},
    {0x30aa, 0x45  }, {0x30B0, 0xff}, {0x30B1, 0xff}, {0x30B2, 0x10},
    {0x30d7, 0x10  }, {0x3047, 0x00}, {0x3018, 0x60}, {0x3019, 0x58},
    {0x301A, 0xa1  }, {0x3087, 0x02}, {0x3082, 0x20}, {0x303C, 0x08},
    {0x303d, 0x18  }, {0x303e, 0x06},
    {0x303f, 0x0c  }, {0x3030, 0x62}, {0x3031, 0x26}, {0x3032, 0xe6},
    {0x3033, 0x6e  }, {0x3034, 0xea}, {0x3035, 0xae}, {0x3036, 0xa6},
    {0x3037, 0x6a  }, {0x3015, 0x12}, {0x3013, 0xfd}, {0x3104, 0x02},
    {0x3105, 0xfd  }, {0x3106, 0x00}, {0x3107, 0xff}, {0x3308, 0xa5},
    {0x3316, 0xff  }, {0x3317, 0x00}, {0x3087, 0x02}, {0x3082, 0x20},
    {0x3300, 0x13  }, {0x3301, 0xd6}, {0x3302, 0xef}, {0x30B8, 0x20},
    {0x30B9, 0x17  }, {0x30BA, 0x04}, {0x30BB, 0x08},
    {0x307c, 0x12  }, {0x3023, 0x09}, {0x3090, 0xc0},                 // 11 Flip, 12 mirror, 13 mirroe and flip
    {0x3507, 0x06  },
    {0x350a, 0x4f  }, {0x3600, 0xc4}, {0x332B, 0x00}, {0x332D, 0x45},
    {0x332D, 0x60  }, {0x332F, 0x03},
    {0x3100, 0x02  }, {0x3304, 0xfc}, {0x3400, 0x00}, {0x3404, 0x02}, /* YUV422 */
    {0x3601, 0x01  }, {0x302a, 0x06}, {0x302b, 0x20},
    {0x300E, 0x32  }, {0x300F, 0x21}, {0x3010, 0x21}, {0x3011, 0x01}, /* QXGA PLL setting*/
    {0x304c, 0x81  },
    {0x3602, 0x22  }, {0x361E, 0x00}, {0x3622, 0x18}, {0x3623, 0x69}, /* CSI setting */
    {0x3626, 0x00  }, {0x3627, 0xf0}, {0x3628, 0x00}, {0x3629, 0x26},
    {0x362A, 0x00  }, {0x362B, 0x5f}, {0x362C, 0xd0}, {0x362D, 0x3c},
    {0x3632, 0x10  }, {0x3633, 0x28}, {0x3603, 0x4d}, {0x364C, 0x04},
    {0x309e, 0x00  },
    {0x3020, 0x01  }, {0x3021, 0x1d}, {0x3022, 0x00}, {0x3023, 0x0a}, /* crop window setting*/
    {0x3024, 0x08  }, {0x3025, 0x18}, {0x3026, 0x06}, {0x3027, 0x0c},
    {0x335f, 0x68  }, {0x3360, 0x18}, {0x3361, 0x0c},
    {0x3362, 0x12  }, {0x3363, 0x88}, {0x3364, 0xe4}, {0x3403, 0x42}, /* VGA */
    {0x3088, 0x02  }, {0x3089, 0x80}, {0x308a, 0x01}, {0x308b, 0xe0},
    {0x3355, 0x04  }, {0x3354, 0x01}, {0x335e, 0x30},                 /* brightness */
    {0x3355, 0x04  }, {0x335c, 0x20}, {0x335d, 0x30},                 /* contrast */

    /* Enable Test Pattern Mode */
    {0x306c, 0x00  },                                                 // Enable color bar mode
    {0x307b, 0x42  },                                                 // Enable colour bar pattern
    {0x307d, 0x80  },                                                 // Colour bar Enable

    /* End of file marker (0xFFFF) */
    {0xffff, 0x00ff}
};

static capture_event_t g_last_cam_event = CEU_EVENT_NONE; ///< Event causing the callback
bool g_capture_ready = false;
bool image_processed = true;

#define USE_SDRAM    (1)
#ifdef USE_SDRAM
uint8_t bsp_camera_out_buffer[2][BSP_CAM_WIDTH * BSP_CAM_HEIGHT *
                                 BSP_CAM_BYTE_PER_PIXEL] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t bsp_camera_out_buffer565[BSP_CAM_WIDTH * BSP_CAM_HEIGHT *
                                 BSP_CAM_BYTE_PER_PIXEL] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t bsp_camera_out_buffer888[BSP_CAM_WIDTH * BSP_CAM_HEIGHT *
                                 3] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t bsp_det_model_ip_buffer888[192 * 192 * 3] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t bsp_det_crop_model_ip_buffer888[240 * 240 * 3] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t bsp_camera_out_rot_buffer565[2][BSP_CAM_WIDTH * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL] BSP_PLACE_IN_SECTION(
    ".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t throw_away_buffer[BSP_CAM_WIDTH * BSP_CAM_HEIGHT *
                          BSP_CAM_BYTE_PER_PIXEL] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);

uint8_t bsp_cls_model_ip_buffer888[224 * 224 * 3] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);

#else
uint8_t bsp_camera_out_buffer[BSP_CAM_WIDTH * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL]  BSP_ALIGN_VARIABLE(8);
uint8_t bsp_camera_out_buffer565[BSP_CAM_WIDTH * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL] BSP_ALIGN_VARIABLE(8);
uint8_t bsp_camera_out_buffer888[BSP_CAM_WIDTH * BSP_CAM_HEIGHT * 3] BSP_ALIGN_VARIABLE(8);
uint8_t bsp_det_model_ip_buffer888[192 * 192 * 3] BSP_ALIGN_VARIABLE(8);
uint8_t bsp_rec_model_ip_buffer888[224 * 224 * 3] BSP_ALIGN_VARIABLE(8);
#endif

uint8_t g_rgb_buffer = 0;              // double buffering current display buffer
uint8_t s_ceu_buffer = 0;              // double buffering current capture buffer

void rot90_clock(uint8_t * input_image, uint8_t * output_image, int n_ch, int ip_w, int ip_h);
void bsp_camera_rgb565_to_rgb888(const void * inbuf, void * outbuf, uint16_t width, uint16_t height);

/**********************************************************************************************************************
 * Function Name: bsp_camera_write_array
 * Description  : .
 * Argument     : array
 * Return Value : .
 *********************************************************************************************************************/
static void bsp_camera_write_array (st_ov_reg_t * array)
{
#if USE_DEBUG_BREAKPOINTS
    uint8_t value;
    R_BSP_PinAccessEnable();
#endif

    while (0xFFFF != array->reg_num)
    {
        wrSensorReg16_8(array->reg_num, array->value);

#if USE_DEBUG_BREAKPOINTS
        rdSensorReg16_8(array->reg_num, &value);

        if (value == array->value)
        {
// printf_colour ("Write ADDR:[0x%04x] Data:[[GREEN]0x%02x[WHITE]]\r\n",array->reg_num,array->value);
        }
        else
        {
            __BKPT(0);

// printf_colour ("Write ADDR:[0x%04x] Data:[[RED]0x%02x[WHITE]]\r\n",array->reg_num,array->value);
        }
#endif
        array++;
    }

#if USE_DEBUG_BREAKPOINTS
    TURN_RED_OFF;

    R_BSP_PinAccessDisable();
#endif
}

/**********************************************************************************************************************
 * End of function bsp_camera_write_array
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: bsp_camera_rgb565_to_rgb888
 * Description  : .
 * Argument     : .
 * Return Value : .
 *********************************************************************************************************************/
void bsp_camera_rgb565_to_rgb888 (const void * inbuf, void * outbuf, uint16_t width, uint16_t height)
{
    uint32_t rows, columns;

    register uint16_t * in_data  = (uint16_t *) inbuf;
    register uint8_t  * out_data = (uint8_t *) outbuf;

    for (rows = 0; rows < height; rows++)
    {
        for (columns = 0; columns < width; columns++)
        {
            *out_data++ = (uint8_t) ((*in_data >> 8) & 0xF8);
            *out_data++ = (uint8_t) (((0x07E0 & *in_data) >> 3) & 0xFC);
            *out_data++ = (uint8_t) (((0x001F & *in_data)) << 3);

            in_data++;
        }
    }
}

/**********************************************************************************************************************
 * End of function bsp_camera_write_array
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: camera_init
 * Description  : .
 * Argument     : use_test_mode
 * Return Value : .
 *********************************************************************************************************************/
bool_t camera_init (bool_t use_test_mode)
{
// fsp_err_t status = FSP_ERR_NOT_OPEN;
    byte   reg_val1          = 0;
    byte   reg_val2          = 0;
    bool_t initialised_state = false;

    R_GPT_Open(&g_cam_clk_ctrl, &g_cam_clk_cfg);
    R_GPT_Start(&g_cam_clk_ctrl);

    /* Hardware reset module */
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, CAM_PWDN_PIN, (bsp_io_level_t) POWER_DOWN);
    R_BSP_SoftwareDelay(40, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, CAM_PWDN_PIN, (bsp_io_level_t) POWER_UP);
    R_BSP_PinAccessDisable();

    i2c_master_status_t i2c_status;
    R_IIC_MASTER_StatusGet(&g_i2c_master1_ctrl, &i2c_status);

    /* only open if not opened */
    if (i2c_status.open != true)       // I2c not open
    {
        R_IIC_MASTER_Open(&g_i2c_master1_ctrl, &g_i2c_master1_cfg);
    }

    R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, BSP_I2C_SLAVE_ADDR_CAMERA, I2C_MASTER_ADDR_MODE_7BIT);
    rdSensorReg16_8(REG_PIDH, &reg_val1); // PIDH  PID MSB
    rdSensorReg16_8(REG_PIDL, &reg_val2); // PIDH  PID LSB REV2c - 0x4C, REV2a = 0x41, REV1a=0x40 otherwise error

    if ((reg_val2 == 0x40) || (reg_val2 == 0x41) || (reg_val2 == 0x4C))
    {
        /* Valid Camera */

        /* Open camera module */
        R_CEU_Open(&g_ceu_ctrl, &g_ceu_cfg);

        if (use_test_mode == false)
        {
            /* VGA TEST PATTERN */
            bsp_camera_write_array((st_ov_reg_t *) &ov3640_fmt_yuv422_vga_test_mode);
        }
        else
        {
            /* LIVE DATA */
            bsp_camera_write_array((st_ov_reg_t *) &ov3640_fmt_yuv422_vga);
        }

        initialised_state = true;
        R_CEU_CaptureStart(&g_ceu_ctrl, bsp_camera_out_buffer[s_ceu_buffer]);
    }

    return initialised_state;
}

/**********************************************************************************************************************
 * End of function camera_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: rot90_clock
 * Description  : .
 * Arguments    : input_image
 *              : output_image
 *              : n_ch
 *              : ip_w
 *              : ip_h
 * Return Value : .
 *********************************************************************************************************************/
void rot90_clock (uint8_t * input_image, uint8_t * output_image, int n_ch, int ip_w, int ip_h)
{
    register int row_times_ip_w = 0;
    register int temp           = 0;
    register int temp_2         = 0;

    register int src_idx = 0;
    register int dst_idx = 0;
    register int ipwxnch = ip_w * n_ch;

    for (int row = 0; row < ip_h; row++)
    {
        row_times_ip_w = row * ipwxnch;
        temp           = (ip_h - row - 1) * n_ch;

        for (int col = 0; col < ip_w; col++)
        {
            temp_2  = col * n_ch;
            src_idx = row_times_ip_w + temp_2;
            dst_idx = temp + temp_2 * ip_h;

            output_image[dst_idx + 0] = input_image[src_idx + 0];
            output_image[dst_idx + 1] = input_image[src_idx + 1];
        }
    }
}

/**********************************************************************************************************************
 * End of function rot90_clock
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: bsp_camera_capture_image
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void bsp_camera_capture_image (void)
{
    fsp_err_t err;

#if USE_DEBUG_BREAKPOINTS
    if (FSP_SUCCESS != err)
    {
        __BKPT(0);
    }

#else
    (void) err;
#endif

    rot90_clock(bsp_camera_out_buffer[s_ceu_buffer],
                bsp_camera_out_rot_buffer565[s_ceu_buffer],
                2,
                BSP_CAM_WIDTH,
                BSP_CAM_HEIGHT);

// g_rgb_buffer = s_ceu_buffer;
}

/**********************************************************************************************************************
 * End of function bsp_camera_capture_image
 *********************************************************************************************************************/
void g_ceu_user_callback (capture_callback_args_t * p_args)
{
    BaseType_t xHigherPriorityTaskWoken, xResult;

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
    xHigherPriorityTaskWoken = pdFALSE;
    if (CEU_EVENT_FRAME_END & p_args->event)
    {
        xResult = xEventGroupSetBitsFromISR(g_update_console_event, CAM_DATA_READY, &xHigherPriorityTaskWoken);

        /* Was the message posted successfully? */
        if (xResult != pdFAIL)
        {
            /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
             * switch should be requested.  The macro used is port specific and will
             * be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
             * the documentation page for the port being used. */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        R_CEU_CaptureStart(&g_ceu_ctrl, bsp_camera_out_buffer[s_ceu_buffer]);
        g_rgb_buffer = s_ceu_buffer;
        s_ceu_buffer = s_ceu_buffer == 0 ? 1 : 0;
    }

    g_last_cam_event = p_args->event;
}

/**********************************************************************************************************************
 * End of function g_ceu_user_callback
 *********************************************************************************************************************/
