/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : camera_thread_entry.c
 * Version      : 1.0
 * Description  : I/F for FreeRTOS thread camera_thread.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "hal_data.h"
#include "vector_data.h"

#include "camera_thread.h"
#include "common_data.h"
#include "common_init.h"
#include "common_utils.h"

#include "bsp_api.h"
#include "board_hw_cfg.h"
#include "ov5640_cfg.h"

#include "display_thread_entry.h"
#include "../ra/fsp/inc/instances/r_mipi_phy.h"

#include "jlink_console.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define REG_PIDH    (0x300a)
#define REG_PIDL    (0x300b)

/**********************************************************************************************************************
 * Debug and similar configuration / documentation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Validated resolutions:
 *  --- Camera configured for YUV422 format
 *  --- Camera SysClk and MIPIClk both at 280MHz
 *
 *                |  RGB565     | RGB888
 * ---------------|-------------------------------------
 * - 640  x 480   |  Success    |
 * - 1024 x 600   |  Success    |
 * - 1280 x 960   |  Success    | VIN FIFO Overflow
 * - 1280 x 1024  |  Success    | VIN FIFO Overflow
 * - 1920 x 1080  |  Fail       | Fail
 * 
 *********************************************************************************************************************/

#define IMAGE_WIDTH         ((uint32_t)(g_plcd_display_cfg.input[0].hsize))
#define IMAGE_HEIGHT        ((uint32_t)(g_plcd_display_cfg.input[0].vsize))
#define OV5640_FLIP_IMAGE   (0)
#define OV5640_MIRROR_IMAGE (0)

#define MIPI_NUM_LANES (2)
#define MIPI_XCLK_HZ (24000000) // 20 MHz

/* Input ns and UI, output ns */
#define MIPI_DPHYTIM_CALC(sf, ns, ui) ((uint16_t)(((float)(ui) * (float)OV5640_HS_CLK_UI_NS) + ((float)(ns) * (sf))))

/* Input ns, output ns */
#define MIPI_DPHYTIM_NS_TO_PCLKA(ns)  ((uint8_t)(((float)(ns) / (float)g_pclka_period_ns) - 1.0f))

/**********************************************************************************************************************
 OV5640 MIPI Image Configuration
 *********************************************************************************************************************/
#define OV5640_CONTINUOUS_CLOCK       (1) // Disabling continuous clock makes timing more difficult - Not recommended

/**********************************************************************************************************************
 * OV5640 Test Image Configuration:
 *********************************************************************************************************************/
/* Not all combinations interact with one another */
#define OV5640_TEST_IMAGE_ENABLE      (0)
#define OV5640_TEST_IMAGE_ROLLING     (0)
#define OV5640_TEST_IMAGE_TRANSPARENT (0)
#define OV5640_TEST_IMAGE_SQUARE      (0)
#define OV5640_TEST_IMAGE_BAR_STYLE   (1) /* 0: 8-Bar, 1: Gradual Vertical Mode 1, 2: Gradual Horizontal, 3: Gradual vertical Mode 2 */
#define OV5640_TEST_IMAGE_TYPE        (0) /* 0: Color Bar, 1: Random Data, 2, Squares, 3: Black */

/**********************************************************************************************************************
 * Camera Config Macros
 *********************************************************************************************************************/
/* Only 1 */
#define INPUT_FORMAT_YUV422_8_BIT  (1) // 0x3034 == 0x18
#define INPUT_FORMAT_YUV422_10_BIT (0) // 0x3034 == 0x1A

#define INPUT_FORMAT_RAW8   (0)
#define INPUT_FORMAT_RGB888 (0)

/* Up to 1 - no conversion if none */
#define OUTPUT_FORMAT_YUV     (0)
#define OUTPUT_FORMAT_RGB565  (1)
#define OUTPUT_FORMAT_RGB1555 (0)
#define OUTPUT_FORMAT_RGB888  (0)
#define OUTPUT_FORMAT_RGB8888 (0)

/* YUV422 configuration is checked asserted below */
#define BYTES_PER_PIXEL_INPUT  (2U)

#if OUTPUT_FORMAT_YUV
    #define BYTES_PER_PIXEL_OUTPUT (2)
#elif OUTPUT_FORMAT_RGB565
    #define BYTES_PER_PIXEL_OUTPUT (2)
#elif OUTPUT_FORMAT_RGB1555
    #define BYTES_PER_PIXEL_OUTPUT (2)
#elif OUTPUT_FORMAT_RGB888
    #define BYTES_PER_PIXEL_OUTPUT (4)
#elif OUTPUT_FORMAT_RGB8888
    #define BYTES_PER_PIXEL_OUTPUT (4)
#else
    #define BYTES_PER_PIXEL_OUTPUT (2)
#endif

#define BYTE_PER_PIXEL      (2U) /* YUV422 configuration is check asserted below */

#if OUTPUT_FORMAT_RGB565 || OUTPUT_FORMAT_RGB1555 || OUTPUT_FORMAT_RGB888 || OUTPUT_FORMAT_RGB8888
    #define OUTPUT_FORMAT_RGB (1)
#else
    #define OUTPUT_FORMAT_RGB (0)
#endif

/*====================================================================================================
 * Camera Timing Macros
 ====================================================================================================*/
/* When changing the clock settings then this define should be turned ON. Running the code will then
 * recalculate the appropriate clocks for the camera. this will take about 10 seconds each time it is
 * run. The settings can then be obtained by reading clock_settings which is an array containing the
 * settings for all eight required clocks. This information should then be manually set in the code
 * below and then this define can be turned back on. */
#define COMPUTE_CLOCKS_CONFIG (OFF_CFG_)

#if COMPUTE_CLOCKS_CONFIG == OFF_CFG_

    #define OV5640_HSCLK_MHZ    (185)
    #define PHY_T_INIT       (75001)
    #define PHY_T_CLK_MISS   (50)//20)   /* ns */
    #define PHY_T_CLK_SETTLE (200)//150)  /* ns */
    #define PHY_T_HS_SETT    (80)//125)  /* ns */

    #define PHY_T_LP_EXIT    (108)  /* ns (104 clock lane /115 data lane) */

    #define PHY_T_CLK_PREP   (85)   /* ns (before CLK start) */
    #define PHY_T_CLK_ZERO   (548)  /* ns */
    #define PHY_T_CLK_PRE    (122)  /* ns (before DataN LPX) */
    #define PHY_T_CLK_POST   (366)  /* ns */
    #define PHY_T_CLK_TRAIL  (99)   /* ns */

    #define PHY_T_HS_PREP    (74)   /* ns */
    #define PHY_T_HS_ZERO    (143)  /* ns */
    #define PHY_T_HS_TRAIL   (96)   /* ns */
    #define PHY_T_HS_EXIT    (13)   /* ns */

    #if OV5640_HSCLK_MHZ < 70
        #define OV5640_MIPI_GLOBAL_TIMING_VALUE (0x44)
    #else
        #define OV5640_MIPI_GLOBAL_TIMING_VALUE (0x0a)
    #endif //  OV5640_HSCLK_MHZ

#else

    #define OV5640_HSCLK_MHZ    (185)

    #if OV5640_HSCLK_MHZ < 70
        #define OV5640_MIPI_GLOBAL_TIMING_VALUE (0x44)
        #define JDG_SKEW_OFFSET_CLK             (0)
        #define JDG_SKEW_OFFSET_DATA            (0)

        #define PHY_T_INIT       (75001)
        #define PHY_T_CLK_MISS   (300)      /* ns */
        #define PHY_T_CLK_SETTLE (500)      /* ns */
        #define PHY_T_HS_SETT    (100)      /* ns */

        #define PHY_T_LP_EXIT    (190)      /* ns (190 clock lane / 253 data lane) */

        #define PHY_T_CLK_PREP   (45)       /* ns (before CLK start) */
        #define PHY_T_CLK_ZERO   (456)      /* ns */
        #define PHY_T_CLK_PRE    (523)      /* ns (before DataN LPX) */
        #define PHY_T_CLK_POST   (678)      /* ns */
        #define PHY_T_CLK_TRAIL  (57)       /* ns */

        #define PHY_T_HS_PREP    (75)       /* ns */
        #define PHY_T_HS_ZERO    (157)      /* ns */
        #define PHY_T_HS_TRAIL   (123)      /* ns */
        #define PHY_T_HS_EXIT    (13)       /* ns */
    #else
        #define OV5640_MIPI_GLOBAL_TIMING_VALUE (0x0a)
        #define JDG_SKEW_OFFSET_CLK             (3)
        #define JDG_SKEW_OFFSET_DATA            (0)

        #define PHY_T_INIT       (75001)
        #define PHY_T_CLK_MISS   (300)      /* ns */
        #define PHY_T_CLK_SETTLE (500)      /* ns */
        #define PHY_T_HS_SETT    (200)      /* ns */

        #define PHY_T_LP_EXIT    (320)      /* ns (144 clock lane / 170 data lane) */

        #define PHY_T_CLK_PREP   (100)      /* ns (before CLK start) */
        #define PHY_T_CLK_ZERO   (690)      /* ns */
        #define PHY_T_CLK_PRE    (215)      /* ns (before DataN LPX) */
        #define PHY_T_CLK_POST   (420)      /* ns */
        #define PHY_T_CLK_TRAIL  (59)       /* ns */

        #define PHY_T_HS_PREP    (174)      /* ns */
        #define PHY_T_HS_ZERO    (220)      /* ns */
        #define PHY_T_HS_TRAIL   (235)      /* ns */
        #define PHY_T_HS_EXIT    (13)       /* ns */
    #endif
#endif

#define OV5640_HSCLK_HZ         (OV5640_HSCLK_MHZ * 1000000)
#define OV5640_HS_CLK_UI_NS     ((1000000000/OV5640_HSCLK_HZ)/2)

//=======================================================================================================================================
//                                                            Other Macros
//=======================================================================================================================================
#define IMAGE_BYTES     (IMAGE_WIDTH*IMAGE_HEIGHT*BYTES_PER_PIXEL_OUTPUT)

/* 10 lines buffer between image frames for better memory view */
#define IMAGE_PADDING   (IMAGE_WIDTH*BYTES_PER_PIXEL_OUTPUT * 10)

/* in line delay function caller */
#define delay_ms(x)     (R_BSP_SoftwareDelay((x), BSP_DELAY_UNITS_MILLISECONDS))

#define OV5640_CLOCK_HZ_TARGET (OV5640_HSCLK_HZ)
/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/* #define COMPUTE_CLOCKS_CONFIG (OFF_CFG_) block */
/* The following settings can be computed by setting COMPUTE_CLOCKS_CONFIG to ON_CFG_, the recording
 * any changes to update the following (8) variables. */
 
/* 0x3035 SC PLL CONTRL1 0x11 RW (Default: 0x11)                                                        */
static uint8_t s_sys_clock_div  = 1;    /* Bit[7:4]: System clock divider                               */
static uint8_t s_mipi_clock_div = 2;    /* Bit[3:0]: Scale divider for MIPI                             */

/* 0x3036 SC PLL CONTRL2 0xF6 RW  (Default: 0x38)                                                       */
static uint8_t s_pll_multiplier = 246;  /* Bit[7:0]: PLL multiplier (4~252, even integer from 128+)     */

/* 0x3037 SC PLL CONTRL3 0x01 RW (Default: 0x13)                                                        */
static uint8_t s_pll_root_div   = 1u;   /* Bit[4]: PLL root divider --- 0: Bypass, 1: Divided by 2      */
static uint8_t pll_pre_div      = 8u;   /* Bit[3:0]: PLL pre-divider: 1,2,3,4,6,8                       */

/* 0x3108 SYSTEM ROOT DIVIDER 0x16 RW - Pad Clock Divider for SCCB Clock (Default: 0x01)                */
static uint8_t pclk_root_div    = 2;    /* Bit[5:4]: PCLK root divider (00: 1, 01: 2, 10: 4, 11: 8)     */
static uint8_t sclk2x_root_div  = 1;    /* Bit[3:2]: sclk2x root divider (00: 1, 01: 2, 10: 4, 11: 8)   */
static uint8_t sclk_root_div    = 4;    /* Bit[1:0]: SCLK root divider (00: 1, 01: 2, 10: 4, 11: 8)     */

/* ENDOF #define COMPUTE_CLOCKS_CONFIG (OFF_CFG_) block */


static uint32_t g_pclka_hz          = 0;
static uint32_t g_pclka_mhz         = 100000000U;
static float    g_pclka_period_ns   = 0;

static uint32_t ov5640_configure_clocks(void);
static void     ov5640_stream_on(void);
static void     ov5640_stream_off(void);

static bool_t   camera_state_determined = false;
static bool_t   s_camera_detected       = false;

static uint32_t sys_clk_hz  = 0;
static uint32_t mipi_clk_hz = 0;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

volatile uint32_t xw = 0; /* IMAGE_WIDTH; */
volatile uint32_t xh = 0; /* IMAGE_HEIGHT;*/

uint8_t ov5640_init(void);
void    OV5640_EnableMIPIMode(void);
void    ov5640_set_mipi_virtual_channel(uint32_t vchannel);
void    OV5640_DspMIPIModeSettings(void);

/**********************************************************************************************************************
 Functions
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: camera_thread_entry
 * Description  : Camera Thread entry function
 *                Note: pvParameters contains TaskHandle_t
 * Argument     : pvParameters - unused but required by I/F
 * Return Value : NONE
 *********************************************************************************************************************/
void camera_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    xw = IMAGE_WIDTH;
    xh = IMAGE_HEIGHT;

    s_camera_detected = false;
    camera_state_determined = false;

    /* Wait for system up (from manu_thread_entry) */
    while (!system_up())
    {
        vTaskDelay (100);
    }

    while(!jlink_configured())
    {
        vTaskDelay(100);
    }

    /* Open video input (VIN) driver */
    R_VIN_Open(&g_vin0_ctrl, &g_vin0_cfg);
    
    /* Init globals */
    memset(fb_foreground, 0x55, sizeof(fb_foreground));
    g_pclka_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKA);
    g_pclka_mhz = (uint8_t) (g_pclka_hz / 1000000);
    g_pclka_period_ns = 1000000000.0f / ((float)g_pclka_hz);


    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_01_PIN_08, BSP_IO_LEVEL_LOW);
    R_BSP_PinAccessDisable();

    if(g_board_i2c_master_ctrl.open == 0)
    {
        __BKPT(0);
    }
    else
    {
        ;
    }

    /** Setup The Camera **/
    ov5640_init();
    ov5640_stream_off();
    ov5640_write_reg(0x3008, 0x42); // software power down

    R_BSP_SoftwareDelay((uint32_t)1, BSP_DELAY_UNITS_MILLISECONDS);
    R_VIN_CaptureStart(&g_vin0_ctrl, NULL);
    R_BSP_SoftwareDelay((uint32_t)1, BSP_DELAY_UNITS_MILLISECONDS);

    ov5640_write_reg(0x3008, 0x02); // wake up
    ov5640_stream_on();

    s_camera_detected = true;
    camera_state_determined = true;
    while (1)
    {
        capture_status_t capture_status;
        R_VIN_StatusGet(&g_vin0_ctrl, &capture_status);
        if(capture_status.state == CAPTURE_STATE_IN_PROGRESS)
        {
            // TODO Add processing outside capture here
        }
    vTaskDelay (1);
    }
}
/**********************************************************************************************************************
 End of function camera_thread_entry
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: check_ov5640_camera_connected
 * Description  : Wait until the camera state has been determined
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
bool_t check_ov5640_camera_connected(void)
{
    while (false == camera_state_determined)
    {
        vTaskDelay(100);
    }
    return (s_camera_detected);
}
/**********************************************************************************************************************
 End of function check_ov5640_camera_connected
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_set_mipi_virtual_channel
 * Description  : Set MIPI VirtualChannel
 * Argument     : vchannel virtual channel for Mipi Mode
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_set_mipi_virtual_channel(uint32_t vchannel)
{
    uint8_t tmp;

    tmp = ov5640_read_reg(0x4814);

    tmp = tmp & (uint8_t)~(3 << 6);
    tmp = tmp | (uint8_t)(vchannel << 6);

    ov5640_write_reg(0x4814, tmp);
}
/**********************************************************************************************************************
 End of function ov5640_set_mipi_virtual_channel
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_stream_on
 * Description  : Turn camera stream on
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_stream_on(void)
{
    ov5640_write_reg(0x4202, 0x00);
}
/**********************************************************************************************************************
 End of function ov5640_stream_on
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_stream_off
 * Description  : Turn camera stream off
 * Argument     : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_stream_off(void)
{
    ov5640_write_reg(0x4202, 0x0f);
}
/**********************************************************************************************************************
 End of function ov5640_stream_off
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_init
 * Description  : Initialize the OV5640 module
 * Argument     : NONE
 * Return Value : OV5640_EOK : OV5640 module initialization successful
 *                OV5640_ERROR : Communication error, OV5640 module initialization failed
 *                OV5640_ENOMEM: The OV5640 module failed to initialize due to insufficient memory
 *********************************************************************************************************************/
uint8_t ov5640_init(void)
{
    uint8_t reg_val = 0xFF;

    /* Initialize GPT module */
    R_GPT_Open(&g_timer_camera_xclk_ctrl, &g_timer_camera_xclk_cfg);

    /* Start GPT module to provide the 24MHz clock frequency output for the camera clock source */
    R_GPT_Start(&g_timer_camera_xclk_ctrl);

    /* Enter power-down and reset state */
    ov5640_hw_init();               /* OV5640模块硬件初始化 */
    delay_ms(300);

    /* Exit power-down and reset state */
    ov5640_exit_power_down();       /* OV5640模块退出掉电模式 */

    /* Reset using GPIO */
    ov5640_hw_reset();
    ov5640_sw_reset();              /* OV5640模块软件复位 */
    delay_ms(20); // Register access permitted after 20 ms

    /* Check Camera is connected */
    reg_val = ov5640_read_reg(REG_PIDH); // PIDH  PID MSB

    /* Discard first read in reg_val */
    reg_val = ov5640_read_reg(REG_PIDL); // PIDH  PID LSB REV2c - 0x4C, REV2a = 0x41, REV1a=0x40 otherwise error

    if ((reg_val == 0x40) || (reg_val == 0x41) || (reg_val == 0x4C))
    {
        /* Valid Camera */
        ;
    }
    else
    {
        s_camera_detected = false;
        camera_state_determined = true;

        /* Halt the camera setup and block the thread */
        while(1)
        {
            vTaskDelay(100);
        }
    }

    /* Reset using Software registers */
    volatile uint8_t reg3103;
    volatile uint8_t reg3008;

    reg3103 = ov5640_read_reg(0x3103);
    reg3103 = reg3103 & (uint8_t)~(0x01 << 1);
    ov5640_write_reg(0x3103, reg3103);

    reg3008 = ov5640_read_reg(0x3008);
    reg3008 |= (0x01 << 7); // Software reset (should do the same thing as HW-GPIO reset above)
    ov5640_write_reg(0x3008, reg3008);
    delay_ms(300);

    //
    //OV5640 setting Version History
    //dated 04/08/2010 A02
    //--Based on v08 release
    //
    //dated 04/20/2010 A03
    //--Based on V10 release
    //
    //dated 04/22/2010 A04
    //--Based on V10 release
    //--updated ccr & awb setting
    //
    //dated 04/22/2010 A06
    //--Based on A05 release
    //--Add pg setting
    //
    //dated 05/19/2011 A09
    //--changed pchg 3708 setting
    ov5640_write_reg(0x3103, 0x11); // SCCB system control
    ov5640_write_reg(0x3008, 0x82); // software reset
    // delay 5ms
    delay_ms(5);
    ov5640_write_reg(0x3008, 0x42); // software power down
    ov5640_write_reg(0x3103, 0x03); // SCCB system control
    ov5640_write_reg(0x3017, 0x00); // set Frex, Vsync, Href, PCLK, D[9:6] input
    ov5640_write_reg(0x3018, 0x00); // set d[5:0], GPIO[1:0] input
    ov5640_write_reg(0x3034, 0x18); // MIPI 8-bit mode
    ov5640_write_reg(0x3037, 0x13); // PLL
    ov5640_write_reg(0x3108, 0x01); // system divider
    ov5640_write_reg(0x3630, 0x36);
    ov5640_write_reg(0x3631, 0x0e);
    ov5640_write_reg(0x3632, 0xe2);
    ov5640_write_reg(0x3633, 0x12);
    ov5640_write_reg(0x3621, 0xe0);
    ov5640_write_reg(0x3704, 0xa0);
    ov5640_write_reg(0x3703, 0x5a);
    ov5640_write_reg(0x3715, 0x78);
    ov5640_write_reg(0x3717, 0x01);
    ov5640_write_reg(0x370b, 0x60);
    ov5640_write_reg(0x3705, 0x1a);
    ov5640_write_reg(0x3905, 0x02);
    ov5640_write_reg(0x3906, 0x10);
    ov5640_write_reg(0x3901, 0x0a);
    ov5640_write_reg(0x3731, 0x12);
    ov5640_write_reg(0x3600, 0x08); // VCM debug mode
    ov5640_write_reg(0x3601, 0x33); // VCM debug mode
    ov5640_write_reg(0x302d, 0x60); // system control
    ov5640_write_reg(0x3620, 0x52);
    ov5640_write_reg(0x371b, 0x20);
    ov5640_write_reg(0x471c, 0x50);
    ov5640_write_reg(0x3a13, 0x43); // AGC pre-gain, 0x40 = 1x
    ov5640_write_reg(0x3a18, 0x00); // gain ceiling
    ov5640_write_reg(0x3a19, 0xf8); // gain ceiling
    ov5640_write_reg(0x3635, 0x13);
    ov5640_write_reg(0x3636, 0x03);
    ov5640_write_reg(0x3634, 0x40);
    ov5640_write_reg(0x3622, 0x01);
    // 50Hz/60Hz
    ov5640_write_reg(0x3c01, 0x34); // 50/60Hz
    ov5640_write_reg(0x3c04, 0x28); // threshold for low sum
    ov5640_write_reg(0x3c05, 0x98); // threshold for high sum
    ov5640_write_reg(0x3c06, 0x00); // light meter 1 threshold high
    ov5640_write_reg(0x3c08, 0x00); // light meter 2 threshold high
    ov5640_write_reg(0x3c09, 0x1c); // light meter 2 threshold low
    ov5640_write_reg(0x3c0a, 0x9c); // sample number high
    ov5640_write_reg(0x3c0b, 0x40); // sample number low
    // timing
    ov5640_write_reg(0x3800, 0x00); // HS
    ov5640_write_reg(0x3801, 0x00); // HS
    ov5640_write_reg(0x3802, 0x00); // VS
    ov5640_write_reg(0x3803, 0x04); // VS
    ov5640_write_reg(0x3804, 0x0a); // HW
    ov5640_write_reg(0x3805, 0x3f); // HW -- Full Resolution
    ov5640_write_reg(0x3806, 0x07); // VW
    ov5640_write_reg(0x3807, 0x9F); // VW -- Full Resolution
    ov5640_write_reg(0x3810, 0x00); // H offset high
    ov5640_write_reg(0x3811, 0x10); // H offset low
    ov5640_write_reg(0x3812, 0x00); // V offset high
    ov5640_write_reg(0x3813, 0x00); // V offset
    ov5640_write_reg(0x3708, 0x64);
    ov5640_write_reg(0x3a08, 0x01); // B50
    ov5640_write_reg(0x4001, 0x02); // BLC start line
    ov5640_write_reg(0x4005, 0x1a); // BLC always update
    ov5640_write_reg(0x3000, 0x00); // system reset 0
    ov5640_write_reg(0x3002, 0x1c); // system reset 2
    ov5640_write_reg(0x3004, 0xff); // clock enable 00
    ov5640_write_reg(0x3006, 0xc3); // clock enable 2
#if (MIPI_NUM_LANES == 2)
    ov5640_write_reg(0x300e, 0x45); // MIPI control, 2 lane, MIPI enable
#elif
#error Untested Configuration
    ov5640_write_reg(0x300e, 0x25); // MIPI control, 1 lane, MIPI enable
#else
    #error Invalid lane configuration
#endif

    ov5640_write_reg(0x302e, 0x08);
uint8_t mipi_bits = 8;            // 0x3034 Bit[3:0]: MIPI bit mode (8 or 10)
#if INPUT_FORMAT_YUV422_8_BIT
    ov5640_write_reg(0x4300, 0x32); // YUV 422, YUYV
#elif INPUT_FORMAT_YUV422_10_BIT
    #error Untested Configuration
    enum ov5640_format_t format = ov5640_yuv422;
    uint8_t mipi_bits = 10;            // 0x3034 Bit[3:0]: MIPI bit mode (8 or 10)
    ov5640_write_reg(0x4300, 0x32); // YUV 422, YUYV
#elif INPUT_FORMAT_RAW8
     #error Untested Configuration
     ov5640_write_reg(0x4300, 0xF8); // RAW
#elif INPUT_FORMAT_RGB888
    #error Untested Configuration
    ov5640_write_reg(0x4300, 0x22); // YUV444/RGB888
#endif
    /* 0x3034 SC PLL CONTRL0 0x1A RW  (Default: 0x18)*/
    uint8_t pll_charge_pump_mode = 1; // Bit[6:4]: PLL charge pump control
    uint8_t reg_3034_value = pll_charge_pump_mode << 4 | mipi_bits << 0;
    ov5640_write_reg(0x3034, reg_3034_value); // MIPI 8-bit mode

    ov5640_write_reg(0x501f, 0x00); // ISP YUV 422
    ov5640_write_reg(0x4407, 0x04); // JPEG QS
    ov5640_write_reg(0x440e, 0x00);
    ov5640_write_reg(0x5000, 0xa7); // ISP control, Lenc on, gamma on, BPC on, WPC on, CIP on
    // AWB
    ov5640_write_reg(0x5180, 0xff);
    ov5640_write_reg(0x5181, 0xf2);
    ov5640_write_reg(0x5182, 0x00);
    ov5640_write_reg(0x5183, 0x14);
    ov5640_write_reg(0x5184, 0x25);
    ov5640_write_reg(0x5185, 0x24);
    ov5640_write_reg(0x5186, 0x09);
    ov5640_write_reg(0x5187, 0x09);
    ov5640_write_reg(0x5188, 0x09);
    ov5640_write_reg(0x5189, 0x75);
    ov5640_write_reg(0x518a, 0x54);
    ov5640_write_reg(0x518b, 0xe0);
    ov5640_write_reg(0x518c, 0xb2);
    ov5640_write_reg(0x518d, 0x42);
    ov5640_write_reg(0x518e, 0x3d);
    ov5640_write_reg(0x518f, 0x56);
    ov5640_write_reg(0x5190, 0x46);
    ov5640_write_reg(0x5191, 0xf8);
    ov5640_write_reg(0x5192, 0x04);
    ov5640_write_reg(0x5193, 0x70);
    ov5640_write_reg(0x5194, 0xf0);
    ov5640_write_reg(0x5195, 0xf0);
    ov5640_write_reg(0x5196, 0x03);
    ov5640_write_reg(0x5197, 0x01);
    ov5640_write_reg(0x5198, 0x04);
    ov5640_write_reg(0x5199, 0x12);
    ov5640_write_reg(0x519a, 0x04);
    ov5640_write_reg(0x519b, 0x00);
    ov5640_write_reg(0x519c, 0x06);
    ov5640_write_reg(0x519d, 0x82);
    ov5640_write_reg(0x519e, 0x38);
    // color matrix
    ov5640_write_reg(0x5381, 0x1e);
    ov5640_write_reg(0x5382, 0x5b);
    ov5640_write_reg(0x5383, 0x08);
    ov5640_write_reg(0x5384, 0x0a);
    ov5640_write_reg(0x5385, 0x7e);
    ov5640_write_reg(0x5386, 0x88);
    ov5640_write_reg(0x5387, 0x7c);
    ov5640_write_reg(0x5388, 0x6c);
    ov5640_write_reg(0x5389, 0x10);
    ov5640_write_reg(0x538a, 0x01);
    ov5640_write_reg(0x538b, 0x98);
    // CIP
    ov5640_write_reg(0x5300, 0x08); // sharpen MT th1
    ov5640_write_reg(0x5301, 0x30); // sharpen MT th2
    ov5640_write_reg(0x5302, 0x10); // sharpen MT offset 1
    ov5640_write_reg(0x5303, 0x00); // sharpen MT offset 2
    ov5640_write_reg(0x5304, 0x08); // DNS threshold 1
    ov5640_write_reg(0x5305, 0x30); // DNS threshold 2
    ov5640_write_reg(0x5306, 0x08); // DNS offset 1
    ov5640_write_reg(0x5307, 0x16); // DNS offset 2
    ov5640_write_reg(0x5309, 0x08); // sharpen TH th1
    ov5640_write_reg(0x530a, 0x30); // sharpen TH th2
    ov5640_write_reg(0x530b, 0x04); // sharpen TH offset 1
    ov5640_write_reg(0x530c, 0x06); // sharpen Th offset 2
    // gamma
    ov5640_write_reg(0x5480, 0x01);
    ov5640_write_reg(0x5481, 0x08);
    ov5640_write_reg(0x5482, 0x14);
    ov5640_write_reg(0x5483, 0x28);
    ov5640_write_reg(0x5484, 0x51);
    ov5640_write_reg(0x5485, 0x65);
    ov5640_write_reg(0x5486, 0x71);
    ov5640_write_reg(0x5487, 0x7d);
    ov5640_write_reg(0x5488, 0x87);
    ov5640_write_reg(0x5489, 0x91);
    ov5640_write_reg(0x548a, 0x9a);
    ov5640_write_reg(0x548b, 0xaa);
    ov5640_write_reg(0x548c, 0xb8);
    ov5640_write_reg(0x548d, 0xcd);
    ov5640_write_reg(0x548e, 0xdd);
    ov5640_write_reg(0x548f, 0xea);
    ov5640_write_reg(0x5490, 0x1d);
    // UV adjust
    ov5640_write_reg(0x5580, 0x06); // sat on, contrast on
    ov5640_write_reg(0x5583, 0x40); // sat U
    ov5640_write_reg(0x5584, 0x10); // sat V
    ov5640_write_reg(0x5589, 0x10); // UV adjust th1
    ov5640_write_reg(0x558a, 0x00); // UV adjust th2[8]
    ov5640_write_reg(0x558b, 0xf8); // UV adjust th2[7:0]
    ov5640_write_reg(0x501d, 0x04); // enable manual offset of contrast
    // lens correction
    ov5640_write_reg(0x5800, 0x23);
    ov5640_write_reg(0x5801, 0x14);
    ov5640_write_reg(0x5802, 0x0f);
    ov5640_write_reg(0x5803, 0x0f);
    ov5640_write_reg(0x5804, 0x12);
    ov5640_write_reg(0x5805, 0x26);
    ov5640_write_reg(0x5806, 0x0c);
    ov5640_write_reg(0x5807, 0x08);
    ov5640_write_reg(0x5808, 0x05);
    ov5640_write_reg(0x5809, 0x05);
    ov5640_write_reg(0x580a, 0x08);
    ov5640_write_reg(0x580b, 0x0d);
    ov5640_write_reg(0x580c, 0x08);
    ov5640_write_reg(0x580d, 0x03);
    ov5640_write_reg(0x580e, 0x00);
    ov5640_write_reg(0x580f, 0x00);
    ov5640_write_reg(0x5810, 0x03);
    ov5640_write_reg(0x5811, 0x09);
    ov5640_write_reg(0x5812, 0x07);
    ov5640_write_reg(0x5813, 0x03);
    ov5640_write_reg(0x5814, 0x00);
    ov5640_write_reg(0x5815, 0x01);
    ov5640_write_reg(0x5816, 0x03);
    ov5640_write_reg(0x5817, 0x08);
    ov5640_write_reg(0x5818, 0x0d);
    ov5640_write_reg(0x5819, 0x08);
    ov5640_write_reg(0x581a, 0x05);
    ov5640_write_reg(0x581b, 0x06);
    ov5640_write_reg(0x581c, 0x08);
    ov5640_write_reg(0x581d, 0x0e);
    ov5640_write_reg(0x581e, 0x29);
    ov5640_write_reg(0x581f, 0x17);
    ov5640_write_reg(0x5820, 0x11);
    ov5640_write_reg(0x5821, 0x11);
    ov5640_write_reg(0x5822, 0x15);
    ov5640_write_reg(0x5823, 0x28);
    ov5640_write_reg(0x5824, 0x46);
    ov5640_write_reg(0x5825, 0x26);
    ov5640_write_reg(0x5826, 0x08);
    ov5640_write_reg(0x5827, 0x26);
    ov5640_write_reg(0x5828, 0x64);
    ov5640_write_reg(0x5829, 0x26);
    ov5640_write_reg(0x582a, 0x24);
    ov5640_write_reg(0x582b, 0x22);
    ov5640_write_reg(0x582c, 0x24);
    ov5640_write_reg(0x582d, 0x24);
    ov5640_write_reg(0x582e, 0x06);
    ov5640_write_reg(0x582f, 0x22);
    ov5640_write_reg(0x5830, 0x40);
    ov5640_write_reg(0x5831, 0x42);
    ov5640_write_reg(0x5832, 0x24);
    ov5640_write_reg(0x5833, 0x26);
    ov5640_write_reg(0x5834, 0x24);
    ov5640_write_reg(0x5835, 0x22);
    ov5640_write_reg(0x5836, 0x22);
    ov5640_write_reg(0x5837, 0x26);
    ov5640_write_reg(0x5838, 0x44);
    ov5640_write_reg(0x5839, 0x24);
    ov5640_write_reg(0x583a, 0x26);
    ov5640_write_reg(0x583b, 0x28);
    ov5640_write_reg(0x583c, 0x42);
    ov5640_write_reg(0x583d, 0xce);
    ov5640_write_reg(0x5025, 0x00);
    ov5640_write_reg(0x3a0f, 0x30); // stable in high
    ov5640_write_reg(0x3a10, 0x28); // stable in low
    ov5640_write_reg(0x3a1b, 0x30); // stable out high
    ov5640_write_reg(0x3a1e, 0x26); // stable out low
    ov5640_write_reg(0x3a11, 0x60); // fast zone high
    ov5640_write_reg(0x3a1f, 0x14); // fast zone low

    /*=====================================================================================*/
    /*======================================== JDG ========================================*/
    /*=====================================================================================*/
    ov5640_write_reg(0x503d, (OV5640_TEST_IMAGE_ENABLE << 7) | \
                             (OV5640_TEST_IMAGE_ROLLING << 6) | \
                             (OV5640_TEST_IMAGE_TRANSPARENT << 5) | \
                             (OV5640_TEST_IMAGE_SQUARE << 4) | \
                             (OV5640_TEST_IMAGE_BAR_STYLE << 2) | \
                             (OV5640_TEST_IMAGE_TYPE << 0)); // Test image
    ov5640_write_reg(0x4800, 0x04 | (!OV5640_CONTINUOUS_CLOCK << 5));
    ov5640_write_reg(0x3007, 0XFB); // Disable DVP PCLK camera Output
    ov5640_write_reg(0x3017, 0x00); // Disable DVP PCLK camera Output
    ov5640_write_reg(0x301D, 0xFF); //
    ov5640_write_reg(0x5001, 0xa3); // SDE on, scale on, UV average off, CMX on, AWB on

    // JDG - Clock related settings
    ov5640_configure_clocks();
    ov5640_set_mipi_virtual_channel(0x00);

    /*====================================================================================*/
    /*======================================== OG ========================================*/
    /*====================================================================================*/
    ov5640_write_reg(0x3008, 0x02); // wake up

    //input 24M
    //output vga 30fps bit rate 224M bps
    //Set in ov5640_configure_clocks ov5640_write_reg(0x3035, 0x14); // pll
    //Set in ov5640_configure_clocks ov5640_write_reg(0x3036, 0x38); // pll
    ov5640_write_reg(0x3c07, 0x08); // light meter 1 threshold
#if OV5640_FLIP_IMAGE
    ov5640_write_reg(0x3820, 0x43); // ISP flip off, sensor flip off
#else
    ov5640_write_reg(0x3820, 0x40); // ISP flip off, sensor flip off
#endif

#if OV5640_MIRROR_IMAGE
    ov5640_write_reg(0x3821, 0x07); // ISP mirror on, sensor mirror on, binning enable
#else
    ov5640_write_reg(0x3821, 0x01); // ISP mirror on, sensor mirror on, binning enable
#endif
    // timing
    ov5640_write_reg(0x3814, 0x31); // X inc
    ov5640_write_reg(0x3815, 0x31); // Y inc
    ov5640_write_reg(0x3803, 0x04); // VS
    const float sensor_max_x = 2642;
    const float sensor_max_y = 1952;
    const float default_ratio = sensor_max_x / sensor_max_y;
    float x_y_ratio = (IMAGE_WIDTH / (float)IMAGE_HEIGHT);
    assert(x_y_ratio > 1); // cropped calculations fail if image is taller than it is wide
    /* Scale According to ratio - keeping resolution as large as possible
     * x/y = ratio
     * x = ratio * y
     * y = x/ratio */

#define CROP_SENSOR_Y (190)
#define CROP_SENSOR_X ((default_ratio) * CROP_SENSOR_Y)
    uint16_t sensor_cropped_x = (uint16_t)((sensor_max_y-CROP_SENSOR_X) * (x_y_ratio / default_ratio));
    uint16_t sensor_cropped_y = (uint16_t)((sensor_max_x-CROP_SENSOR_Y) / x_y_ratio);

    uint16_t cropped_pixels_x = (uint16_t)sensor_max_x - sensor_cropped_x;
    uint16_t cropped_pixels_y = (uint16_t)sensor_max_y - sensor_cropped_y;
    uint16_t sensor_x_start = cropped_pixels_x / 2;
    uint16_t sensor_x_end = (uint16_t)(sensor_max_x - (cropped_pixels_x / 2) - 1); // Setting is one-indexed
    uint16_t sensor_y_start = 4 + cropped_pixels_y / 2;
    uint16_t sensor_y_end = (uint16_t)(sensor_max_y - (cropped_pixels_y / 2) - 1); // Setting is one-indexed
    ov5640_write_reg(0x3800, (uint8_t)(sensor_x_start>>8)); // HS
    ov5640_write_reg(0x3801, (uint8_t)sensor_x_start); // HS
    ov5640_write_reg(0x3802, (uint8_t)(sensor_y_start>>8)); // VS
    ov5640_write_reg(0x3803, (uint8_t)sensor_y_start); // VS
    ov5640_write_reg(0x3804, (uint8_t)(sensor_x_end>>8)); // HW
    ov5640_write_reg(0x3805, (uint8_t)sensor_x_end);      // HW -- Full Resolution: 2624
    ov5640_write_reg(0x3806, (uint8_t)(sensor_y_end>>8)); // VW
    ov5640_write_reg(0x3807, (uint8_t)sensor_y_end);      // VW -- Full Resolution: 1952 (1.344 Ratio)

    ov5640_write_reg(0x3808, (uint8_t)(IMAGE_WIDTH>>8));  // DVPHO - Horizontal
    ov5640_write_reg(0x3809, (uint8_t)IMAGE_WIDTH);       // DVPHO - Horizontal
    ov5640_write_reg(0x380a, (uint8_t)(IMAGE_HEIGHT>>8)); // DVPVO - Vertical
    ov5640_write_reg(0x380b, (uint8_t)IMAGE_HEIGHT);      // DVPVO - Vertical

    /* The subtraction here is from the 'scanned' area, to increase FPS */
    ov5640_write_reg(0x380c, (uint8_t)(((uint32_t)(sensor_cropped_x-93))>>8)); // HTS
    ov5640_write_reg(0x380d, (uint8_t)((uint32_t)(sensor_cropped_x-93)));      // HTS
    ov5640_write_reg(0x380e, (uint8_t)(((uint32_t)(sensor_cropped_y-714))>>8)); // VTS
    ov5640_write_reg(0x380f, (uint8_t)((uint32_t)(sensor_cropped_y-714))); // VTS
    ov5640_write_reg(0x3813, 0x06); // V offset
    ov5640_write_reg(0x3618, 0x00);
    ov5640_write_reg(0x3612, 0x29);
    ov5640_write_reg(0x3709, 0x52);
    ov5640_write_reg(0x370c, 0x03);
    ov5640_write_reg(0x3a02, 0x03); // 60Hz max exposure
    ov5640_write_reg(0x3a03, 0xd8); // 60Hz max exposure
    ov5640_write_reg(0x3a09, 0x27); // B50 low
    ov5640_write_reg(0x3a0a, 0x00); // B60 high
    ov5640_write_reg(0x3a0b, 0xf6); // B60 low
    ov5640_write_reg(0x3a0e, 0x03); // B50 max
    ov5640_write_reg(0x3a0d, 0x04); // B60 max
    ov5640_write_reg(0x3a14, 0x03); // 50Hz max exposure
    ov5640_write_reg(0x3a15, 0xd8); // 50Hz max exposure
    ov5640_write_reg(0x4004, 0x02); // BLC line number
    ov5640_write_reg(0x4713, 0x03); // JPEG mode 3
    ov5640_write_reg(0x460b, 0x35); // debug
    ov5640_write_reg(0x460c, 0x22); // VFIFO, PCLK manual
    ov5640_write_reg(0x4837, OV5640_MIPI_GLOBAL_TIMING_VALUE); // MIPI global timing
    ov5640_write_reg(0x3824, 0x02); // PCLK divider
    ov5640_write_reg(0x5001, 0xa3); // SDE on, scale on, UV average off, CMX on, AWB on

    return 0;
}
/**********************************************************************************************************************
 End of function ov5640_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vin0_callback
 * Description  : Application level callback interface from VIN (Video In) driver.
 * Argument     : capture_callback_args_t - CAPTURE callback function parameter data
 * Return Value : NONE
 *********************************************************************************************************************/
void vin0_callback (capture_callback_args_t * p_args)
{
    vin_event_t            event            = (vin_event_t) p_args->event;
    vin_module_status_t    module_status    = (vin_module_status_t) p_args->event_status;
    vin_interrupt_status_t interrupt_status = (vin_interrupt_status_t) p_args->interrupt_status;
    FSP_PARAMETER_NOT_USED(module_status);

    switch (event)
    {
        case VIN_EVENT_NOTIFY:
        {
            if (interrupt_status.bits.frame_complete)
            {
                /* Tell display thread camera image has updated */
                display_next_buffer_set(p_args->p_buffer);
            }
            break;
        }

        case VIN_EVENT_ERROR:
        {
            // Process Error based on interrupt and module status;
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}
/**********************************************************************************************************************
 End of function vin0_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mipi_csi0_callback
 * Description  : Application level callback interface from MIPI Camera Serial Interface (CSI) driver.
 * Argument     : mipi_csi_callback_args_t - CSI callback function parameter data
 * Return Value : NONE
 *********************************************************************************************************************/
void mipi_csi0_callback (mipi_csi_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case MIPI_CSI_EVENT_DATA_LANE:
        {
            mipi_csi_data_lane_status_t data = p_args->event_data.data_lane_status;
            uint8_t lane_idx = p_args->event_idx;
            FSP_PARAMETER_NOT_USED(data);
            FSP_PARAMETER_NOT_USED(lane_idx);
            break;
        }

        case MIPI_CSI_EVENT_FRAME_DATA:
        {
            mipi_csi_data_lane_status_t data = p_args->event_data.data_lane_status;
            FSP_PARAMETER_NOT_USED(data);
            break;
        }

        case MIPI_CSI_EVENT_POWER:
        {
            mipi_csi_data_lane_status_t data = p_args->event_data.data_lane_status;
            FSP_PARAMETER_NOT_USED(data);
            break;
        }

        case MIPI_CSI_EVENT_SHORT_PACKET_FIFO:
        {
            mipi_csi_data_lane_status_t data = p_args->event_data.data_lane_status;
            FSP_PARAMETER_NOT_USED(data);
            break;
        }

        case MIPI_CSI_EVENT_VIRTUAL_CHANNEL:
        {
            mipi_csi_data_lane_status_t data = p_args->event_data.data_lane_status;
            uint8_t channel_idx = p_args->event_idx;
            FSP_PARAMETER_NOT_USED(channel_idx);
            if(data.bits.err_control || data.bits.err_escape)
            {
                ;
            }
            break;
        }

        default:
            break;
    }
}
/**********************************************************************************************************************
 End of function mipi_csi0_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Set camera clock
 *
 * Note from: https://patchwork.kernel.org/project/linux-media/patch/1539797508-127629-1-git-send-email-sam@elite-embedded.com/
 *
 * The current best guess of the clock tree, as reverse engineered by several
 * people on the media mailing list:
 *
 *   +--------------+
 *   |  Ext. Clock  |
 *   +------+-------+
 *          |
 *   +------+-------+ - reg 0x3037[3:0] for the pre-divider
 *   | System PLL   | - reg 0x3036 for the multiplier
 *   +--+-----------+ - reg 0x3035[7:4] for the system divider
 *      |
 *      |   +--------------+
 *      |---+  MIPI Rate   | - reg 0x3035[3:0] for the MIPI root divider
 *      |   +--------------+
 *      |
 *   +--+-----------+
 *   | PLL Root Div | - (reg 0x3037[4])+1 for the root divider
 *   +--+-----------+
 *          |
 *   +------+-------+
 *   | MIPI Bit Div | - reg 0x3034[3:0]/4 for divider when in MIPI mode, else 1
 *   +--+-----------+
 *      |
 *      |   +--------------+
 *      |---+     SCLK     | - log2(reg 0x3108[1:0]) for the root divider
 *      |   +--------------+
 *      |
 *   +--+-----------+ - reg 0x3035[3:0] for the MIPI root divider
 *   |    PCLK      | - log2(reg 0x3108[5:4]) for the DVP root divider
 *   +--------------+
 *
 * Not all limitations of register values are documented above, see ov5640
 * datasheet.
 *
 * In order for the sensor to operate correctly the ratio of
 * SCLK:PCLK:MIPI RATE must be 1:2:8 when the scalar in the ISP is not
 * enabled, and 1:1:4 when it is enabled (MIPI rate doesn't matter in DVP mode).
 * The ratio of these different clocks is maintained by the constant div values
 * below, with PCLK div being selected based on if the mode is using the scalar.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_configure_clocks
 * Description  : Configure the clocks either by calculating the required values or by using the pre-configured
 *                settings calculated during a previous run.
 * Argument     : NONE
 * Return Value : mipi_clk_hz - calculated mipi_clk_hz, for validation or reference.
 *********************************************************************************************************************/
static uint32_t ov5640_configure_clocks()
{
#if COMPUTE_CLOCKS_CONFIG

    uint8_t mipi_bit_div = (0x0F & ov5640_read_reg(0x3034)) / 5;

    /*                      |    Range      | Conditions              |
     * ---------------------|---------------|-------------------------|
     * PLL Base             |               |                         |
     * - s_pll_multiplier   |  4 to 252     | Even integer when > 127 |
     * - s_pll_root_div     |  1 or 2       | On/Off                  |
     * - pll_pre_div        |  1,2,3,4,6,8  |                         |
     *______________________|_______________|_________________________|
     * System Clock                                                   |
     * - s_sys_clock_div    |  0(16) to 15  | Range Unspecified       |
     * - sclk2x_root_div    |  1, 2, 4, 8   |                         |
     * - sclk_root_div      |  1, 2, 4, 8   |                         |
     * _____________________|_______________|_________________________|
     * MIPI Clock                                                     |
     * - pclk_root_div      |  1, 2, 4, 8   |                         |
     * - mipi_div           |  0(16) to 15  | Range Unspecified       |
     */

    uint8_t scaling_enabled = ((ov5640_read_reg(0x5001) >> 5) & 0x01); // SDE on, scale on, UV average off, CMX on, AWB on
    uint8_t clock_settings[8]; // Temp location for 'best match' --- In same order as above

    // Set actual values for these, not register values
    s_pll_multiplier = 4;   // Bit[7:0]: PLL multiplier (4~252, even integer from 128+)
    s_pll_root_div = 1;     // Bit[4]:   PLL root divider --- 0: Bypass, 1: Divided by 2
    pll_pre_div = 1;        // Bit[3:0]: PLL pre-divider: 1,2,3,4,6,8

    sclk2x_root_div = 1;    // Bit[3:2]: sclk2x root divider (00: 1, 01: 2, 10: 4, 11: 8)
    sclk_root_div = 1;      // Bit[1:0]: SCLK root divider (00: 1, 01: 2, 10: 4, 11: 8)
    s_sys_clock_div = 1;    // Bit[7:4]: System clock divider

    s_mipi_clock_div = 1;   // Bit[3:0]: Scale divider for MIPI
    pclk_root_div = 1;      // Bit[5:4]: PCLK root divider (00: 1, 01: 2, 10: 4, 11: 8)

    static uint8_t pclk_sclk_ratio = 0;
    static uint8_t mipi_sclk_ratio = 0;

    /* Find clock settings */
    uint32_t best_clock_error = 0xFFFFFFFF;
    for(s_pll_root_div = 2; s_pll_root_div>0; s_pll_root_div--)
    {   // INFO: For now (s_sys_clock_div == s_mipi_clock_div) and (sclk_root_div == pclk_root_div)
        for(sclk_root_div = 1; sclk_root_div <= 8; sclk_root_div = sclk_root_div << 1)
        {
            for(pclk_root_div = 1; pclk_root_div <= 8; pclk_root_div = pclk_root_div << 1)
            {
            // INFO: Question for ArduCam -- pclk_root_div
                for(s_sys_clock_div = 14; s_sys_clock_div > 0; s_sys_clock_div--)
                {
                    for(s_mipi_clock_div = 14; s_mipi_clock_div > 0; s_mipi_clock_div--)
                    {
                        uint8_t pll_div_inc = (pll_pre_div >= 4) ? 2 : 1;
                        for(pll_pre_div = 1; pll_pre_div<=8; pll_pre_div += pll_div_inc)
                        {
                            uint8_t s_pll_inc = (s_pll_multiplier >= 128) ? 2 : 1;
                            for(s_pll_multiplier = 4; s_pll_multiplier <= 252; s_pll_multiplier += s_pll_inc)
                            {
                                uint32_t pre_div_frequency = (uint32_t)(s_pll_multiplier * (MIPI_XCLK_HZ / (pll_pre_div * s_pll_root_div)));
                                if(800000000 < pre_div_frequency) // Base frequency cannot be more than 800 MHz
                                    break;

                                uint32_t post_div_frequency = pre_div_frequency / (pclk_root_div * s_mipi_clock_div);

                                uint32_t clock_error = (uint32_t)abs((int)OV5640_CLOCK_HZ_TARGET - (int)post_div_frequency);

                                /* =================== Parameter Checking ===================
                                 * In order for the sensor to operate correctly the ratio of
                                 * SCLK:PCLK:MIPI RATE must be:
                                 *  - 1:2:8 when the scalar in the ISP is not enabled,
                                 *  - 1:1:4 when the scalar in the ISP is enabled.
                                 *  - MIPI rate doesn't matter in DVP mode.
                                 * The ratio of these different clocks is maintained by the constant div values
                                 * below, with PCLK div being selected based on if the mode is using the scalar. */

                                uint32_t sys_pll = (uint32_t)(s_pll_multiplier * (MIPI_XCLK_HZ / (pll_pre_div * s_sys_clock_div)));
                                uint32_t mipi_rate = sys_pll / s_mipi_clock_div;
                                uint32_t sys_pll_root = sys_pll / s_pll_root_div;
                                uint32_t bit_div = sys_pll_root / mipi_bit_div;
                                uint32_t sys_clk = bit_div / sclk_root_div;
                                uint32_t mipi_pclk = bit_div / (s_mipi_clock_div * pclk_root_div);

                                if((mipi_pclk % sys_clk) || (mipi_rate % sys_clk))
                                {
                                    // Must be even multiple
                                    continue;
                                }
                                pclk_sclk_ratio = (uint8_t)(mipi_pclk / sys_clk);
                                mipi_sclk_ratio = (uint8_t)(mipi_rate / sys_clk);

                                if((!scaling_enabled  && (8 == mipi_sclk_ratio) && (2 == pclk_sclk_ratio)) ||
                                   (scaling_enabled && (2 == mipi_sclk_ratio) && (1 == pclk_sclk_ratio)))
                                   {
                                // Update best match
                                    if(clock_error <= best_clock_error)
                                    {
                                        best_clock_error = clock_error;
                                        clock_settings[0] = (uint8_t)s_pll_multiplier;
                                        clock_settings[1] = s_pll_root_div;
                                        clock_settings[2] = (uint8_t)pll_pre_div;

                                        clock_settings[3] = sclk2x_root_div;
                                        clock_settings[4] = sclk_root_div;
                                        clock_settings[5] = s_sys_clock_div;

                                        clock_settings[6] = s_mipi_clock_div;
                                        clock_settings[7] = pclk_root_div;
                                    }
                                }
                            }
                        }
                    }
                }
            } // s_sys_clock_div
        } // s_sys_clock_div
    }
    // Restore best match (for readability)
    s_pll_multiplier  = clock_settings[0];
    s_pll_root_div    = clock_settings[1];
    pll_pre_div     = clock_settings[2];

    sclk2x_root_div = clock_settings[3];
    sclk_root_div   = clock_settings[4];
    s_sys_clock_div   = clock_settings[5];

    s_mipi_clock_div  = clock_settings[6];
    pclk_root_div   = clock_settings[7];

    /* TODO Remember to update DEFAULTS in section Camera Timing Macros
     * to ensure new settings are persistent when COMPUTE_CLOCKS_CONFIG
     * is set to _OFF_CFG */

#endif
    uint32_t base_pll_hz = (uint32_t)(s_pll_multiplier * (MIPI_XCLK_HZ / (s_pll_root_div * pll_pre_div)));

    assert(800000000 >= base_pll_hz);
    sys_clk_hz = base_pll_hz / (uint32_t)(s_sys_clock_div  * sclk2x_root_div * sclk_root_div); (void)sys_clk_hz;
    mipi_clk_hz = base_pll_hz / (uint32_t)(s_mipi_clock_div * pclk_root_div);
    assert(sys_clk_hz >= mipi_clk_hz);
    int32_t clock_error = abs((int32_t)(mipi_clk_hz - OV5640_CLOCK_HZ_TARGET));
    int32_t clock_target = (OV5640_CLOCK_HZ_TARGET/200); // Clock within 0.1%
    assert( clock_error < clock_target ); // Clock within 0.1%

    /* 0x3035 SC PLL CONTRL1 0x11 RW (Default: 0x11)
     * INFO: 0 is div 16 according to linux driver */
    uint8_t reg_3035_value = s_sys_clock_div << 4 | s_mipi_clock_div << 0;
    ov5640_write_reg(0x3035, reg_3035_value); // Sys Clock Div: 1 --- MIPI Clock Div: 2

    /* 0x3036 SC PLL CONTRL2 0x69 RW  (Default: 0x38)*/
    uint8_t reg_3036_value = (uint8_t)s_pll_multiplier;
    ov5640_write_reg(0x3036, reg_3036_value);  // PLL Multiplier: 140

    /* 0x3037 SC PLL CONTRL3 0x03 RW (Default: 0x13)*/
    uint8_t reg_3037_value = (s_pll_root_div==2) << 4 | ((uint8_t)pll_pre_div) << 0;
    ov5640_write_reg(0x3037, reg_3037_value); // PLL 2x-root-div: Enabled --- PLL Pre-div: 3
    assert((pll_pre_div == 1) || (pll_pre_div == 2) || \
           (pll_pre_div == 3) || (pll_pre_div == 4) || \
           (pll_pre_div == 6) || (pll_pre_div == 8));

    /* 0x3108 SYSTEM ROOT DIVIDER 0x16 RW - Pad Clock Divider for SCCB Clock (Default: 0x01)*/
    uint8_t reg_3108_div_lut[] = {0xF, 0, 1, 0xF, 2, 0xF, 0xF, 0xF, 3}; // 1, 2, 4, 8 are valid
    uint8_t reg_3108_value = reg_3108_div_lut[pclk_root_div] << 4 | \
                             reg_3108_div_lut[sclk2x_root_div] << 2 | \
                             reg_3108_div_lut[sclk_root_div] << 0;
    ov5640_write_reg(0x3108, reg_3108_value); // system divider
    assert(reg_3108_div_lut[pclk_root_div] < 0xF);
    assert(reg_3108_div_lut[sclk2x_root_div] < 0xF);
    assert(reg_3108_div_lut[sclk_root_div] < 0xF);
    return mipi_clk_hz;
}
/**********************************************************************************************************************
 End of function ov5640_configure_clocks
 *********************************************************************************************************************/

