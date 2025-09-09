/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "camera_control.h"
#include "ov5640_cfg.h"
#include "hal_data.h"
#include "stdlib.h"
#include "common_util.h"
#include "time_counter.h"

/***************************************************************************************************************************
 * Macro definitions
 * Refer to the application note for the physical definition of these values
 ***************************************************************************************************************************/

#define OV5640_CLOCK_HZ_TARGET (OV5640_HSCLK_HZ)

#define OV5640_FLIP_IMAGE   (0)
#define OV5640_MIRROR_IMAGE (0)

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/


/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
uint8_t * gp_next_buffer;

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
static uint32_t last_vin_frame_end = 0;
static uint32_t sys_clk_hz = 0;
static uint32_t mipi_clk_hz = 0;

/**
  * @brief  Set MIPI VirtualChannel
  * @param  pObj  pointer to component object
  * @param  vchannel virtual channel for Mipi Mode
  * @retval Component status
  */
void OV5640_SetMIPIVirtualChannel(uint32_t vchannel)
{
  uint8_t tmp;

  tmp = ov5640_read_reg(0x4814);

  tmp = tmp & (uint8_t)~(3 << 6);
  tmp = tmp | (uint8_t)(vchannel << 6);
  ov5640_write_reg(0x4814, tmp);
}

void OV5640_stream_on(void)
{
    ov5640_write_reg(0x4202, 0x00);
}

void OV5640_stream_off(void)
{
    ov5640_write_reg(0x4202, 0x0f);
}

/*********************************************************************************************************************
 *  set the camera clock
 *  This function set the camera clcok.
 *  @param      None
 *  @retval     None
***********************************************************************************************************************/


/* Set camera clock
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
*/


static uint32_t ov5640_configure_clocks()
{

    uint8_t scaling_enabled = ((ov5640_read_reg(0x5001) >> 5) & 0x01); // SDE on, scale on, UV average off, CMX on, AWB on
    uint8_t mipi_bit_div = (0x0F & ov5640_read_reg(0x3034)) / 5;

    static uint8_t pclk_sclk_ratio = 0;
    static uint8_t mipi_sclk_ratio = 0;


    /*                      |    Range      | Conditions |
     * ---------------------|---------------|------------|
     * PLL Base
     * - pll_multiplier     |  4 to 252     | Even integer when > 127 |
     * - pll_root_div       |  1 or 2       | On/Off            |
     * - pll_pre_div        |  1,2,3,4,6,8  |
     *
     * System Clock
     * - sys_clock_div      |  0(16) to 15  | Range Unspecified |
     * - sclk2x_root_div    |  1, 2, 4, 8   |                   |
     * - sclk_root_div      |  1, 2, 4, 8   |                   |
     *
     * MIPI Clock
     * - pclk_root_div      |  1, 2, 4, 8   |                   |
     * - mipi_div           |  0(16) to 15  | Range Unspecified |
     */
    uint8_t clock_settings[8]; // Temp location for 'best match' --- In same order as above

    // Set actual values for these, not register values
    int32_t pll_multiplier = 4;     // Bit[7:0]: PLL multiplier (4~252, even integer from 128+)
    uint8_t pll_root_div = 1;       // Bit[4]: PLL root divider --- 0: Bypass, 1: Divided by 2
    int32_t pll_pre_div = 1;        // Bit[3:0]: PLL pre-divider: 1,2,3,4,6,8

    uint8_t sclk2x_root_div = 1;    // Bit[3:2]: sclk2x root divider (00: 1, 01: 2, 10: 4, 11: 8)
    uint8_t sclk_root_div = 1;      // Bit[1:0]: SCLK root divider (00: 1, 01: 2, 10: 4, 11: 8)
    uint8_t sys_clock_div = 1;      // Bit[7:4]: System clock divider

    uint8_t mipi_clock_div = 1;     // Bit[3:0]: Scale divider for MIPI
    uint8_t pclk_root_div = 1;      // Bit[5:4]: PCLK root divider (00: 1, 01: 2, 10: 4, 11: 8)


    /* Find clock settings */
    uint32_t best_clock_error = 0xFFFFFFFF;
    for(pll_root_div = 2; pll_root_div>0; pll_root_div--)
    {   // TODO: For now (sys_clock_div == mipi_clock_div) and (sclk_root_div == pclk_root_div)
        for(sclk_root_div = (uint8_t)1; sclk_root_div <= (uint8_t)8; sclk_root_div = (uint8_t)(sclk_root_div << 1))
        {
            for(pclk_root_div = (uint8_t)1; pclk_root_div <= (uint8_t)8; pclk_root_div = (uint8_t)(pclk_root_div << 1))
            {
            // TODO: Question for ArduCam -- pclk_root_div
                for(sys_clock_div = 14; sys_clock_div > 0; sys_clock_div--)
                {
                    for(mipi_clock_div = 14; mipi_clock_div > 0; mipi_clock_div--)
                    {
                        for(pll_pre_div = 1; pll_pre_div<=8; pll_pre_div += ((pll_pre_div >= 4) ? 2 : 1))
                        {
                            for(pll_multiplier = 4; pll_multiplier <= 252; pll_multiplier += ((pll_multiplier >= 128) ? 2 : 1))
                            {
                                uint32_t pre_div_frequency = (uint32_t)(pll_multiplier * (MIPI_XCLK_HZ / (pll_pre_div * pll_root_div)));
                                if(800000000 < pre_div_frequency) // Base frequency cannot be more than 800 MHz
                                    break;

                                uint32_t post_div_frequency = pre_div_frequency / (pclk_root_div * mipi_clock_div);

                                uint32_t clock_error = (uint32_t)abs((int)OV5640_CLOCK_HZ_TARGET - (int)post_div_frequency);

                                /* =================== Parameter Checking ===================
                                 * In order for the sensor to operate correctly the ratio of
                                 * SCLK:PCLK:MIPI RATE must be:
                                 *  - 1:2:8 when the scalar in the ISP is not enabled,
                                 *  - 1:1:4 when the scalar in the ISP is enabled.
                                 *  - MIPI rate doesn't matter in DVP mode.
                                 * The ratio of these different clocks is maintained by the constant div values
                                 * below, with PCLK div being selected based on if the mode is using the scalar. */

                                uint32_t sys_pll = (uint32_t)(pll_multiplier * (MIPI_XCLK_HZ / (pll_pre_div * sys_clock_div)));
                                uint32_t mipi_rate = sys_pll / mipi_clock_div;
                                uint32_t sys_pll_root = sys_pll / pll_root_div;
                                uint32_t bit_div = sys_pll_root / mipi_bit_div;
                                uint32_t sys_clk = bit_div / sclk_root_div;
                                uint32_t mipi_pclk = bit_div / (mipi_clock_div * pclk_root_div);

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
                                        clock_settings[0] = (uint8_t)pll_multiplier;
                                        clock_settings[1] = pll_root_div;
                                        clock_settings[2] = (uint8_t)pll_pre_div;

                                        clock_settings[3] = sclk2x_root_div;
                                        clock_settings[4] = sclk_root_div;
                                        clock_settings[5] = sys_clock_div;

                                        clock_settings[6] = mipi_clock_div;
                                        clock_settings[7] = pclk_root_div;
                                    }
                                }
                            }
                        }
                    }
                }
            } // sys_clock_div
        } // sys_clock_div
    }
    // Restore best match (for readability)
    pll_multiplier  = clock_settings[0];
    pll_root_div    = clock_settings[1];
    pll_pre_div     = clock_settings[2];

    sclk2x_root_div = clock_settings[3];
    sclk_root_div   = clock_settings[4];
    sys_clock_div   = clock_settings[5];

    mipi_clock_div  = clock_settings[6];
    pclk_root_div   = clock_settings[7];


    uint32_t sys_pll = (uint32_t)(pll_multiplier * (MIPI_XCLK_HZ / (pll_pre_div * sys_clock_div)));
    uint32_t mipi_rate = sys_pll / mipi_clock_div;
    uint32_t sys_pll_root = sys_pll / pll_root_div;
    uint32_t bit_div = sys_pll_root / mipi_bit_div;
    uint32_t sys_clk = bit_div / sclk_root_div;
    uint32_t mipi_pclk = bit_div / (mipi_clock_div * pclk_root_div);

    pclk_sclk_ratio = (uint8_t)(mipi_pclk / sys_clk);
    mipi_sclk_ratio = (uint8_t)(mipi_rate / sys_clk);

    uint32_t base_pll_hz = (uint32_t)(pll_multiplier * (MIPI_XCLK_HZ / (pll_root_div * pll_pre_div)));
    assert(800000000 >= base_pll_hz);
    sys_clk_hz = base_pll_hz / (uint32_t)(sys_clock_div  * sclk2x_root_div * sclk_root_div); (void)sys_clk_hz;
    mipi_clk_hz = base_pll_hz / (uint32_t)(mipi_clock_div * pclk_root_div);
    assert(sys_clk_hz >= mipi_clk_hz);
    int32_t clock_error = abs((int32_t)(mipi_clk_hz - OV5640_CLOCK_HZ_TARGET));
    assert( clock_error < (OV5640_CLOCK_HZ_TARGET/200)); // Clock within 0.1%

    /* 0x3035 SC PLL CONTRL1 0x11 RW (Default: 0x11)
     * TODO: 0 is div 16 according to linux driver */
    uint8_t reg_3035_value = (uint8_t)(sys_clock_div << 4) | (uint8_t)(mipi_clock_div << 0);
    ov5640_write_reg(0x3035, reg_3035_value); // Sys Clock Div: 1 --- MIPI Clock Div: 2

    /* 0x3036 SC PLL CONTRL2 0x69 RW  (Default: 0x38)*/
    uint8_t reg_3036_value = (uint8_t)pll_multiplier;
    ov5640_write_reg(0x3036, reg_3036_value);  // PLL Multiplier: 140

    /* 0x3037 SC PLL CONTRL3 0x03 RW (Default: 0x13)*/
    uint8_t reg_3037_value = (uint8_t)((pll_root_div==2) << 4) | (uint8_t)(((uint8_t)pll_pre_div) << 0);
    ov5640_write_reg(0x3037, reg_3037_value); // PLL 2x-root-div: Enabled --- PLL Pre-div: 3
    assert((pll_pre_div == 1) || (pll_pre_div == 2) || \
           (pll_pre_div == 3) || (pll_pre_div == 4) || \
           (pll_pre_div == 6) || (pll_pre_div == 8));

    /* 0x3108 SYSTEM ROOT DIVIDER 0x16 RW - Pad Clock Divider for SCCB Clock (Default: 0x01)*/
    uint8_t reg_3108_div_lut[] = {0xF, 0, 1, 0xF, 2, 0xF, 0xF, 0xF, 3}; // 1, 2, 4, 8 are valid
    uint8_t reg_3108_value = (uint8_t)(reg_3108_div_lut[pclk_root_div] << 4 | \
                             (uint8_t)((uint8_t)reg_3108_div_lut[sclk2x_root_div] << 2) | \
                             reg_3108_div_lut[sclk_root_div] << 0);

    ov5640_write_reg(0x3108, reg_3108_value); // system divider
    assert(reg_3108_div_lut[pclk_root_div] < 0xF);
    assert(reg_3108_div_lut[sclk2x_root_div] < 0xF);
    assert(reg_3108_div_lut[sclk_root_div] < 0xF);
    return mipi_clk_hz;
}
/**
 * @brief       Initialize OV5640 Hardware
 * @param       none
 * @retval      OV5640_EOK   : OV5640 initialization success
 *              OV5640_ERROR : communication error，OV5640 initialization failed
 *              OV5640_ENOMEM: memory failure，OV5640 initialization failed
 */
uint8_t ov5640_init(void)
{
    /* Initialize GPT module */
    R_GPT_Open(&g_timer_camera_xclk_ctrl, &g_timer_camera_xclk_cfg);
//
//    /* Start GPT module to provide the 24MHz clock frequency output for the camera clock source */
    R_GPT_Start(&g_timer_camera_xclk_ctrl);
    /* Enter power-down and reset state */
    ov5640_hw_init();               /* OV5640 hardware initialization */
    delay_ms(300);

    /* Exit power-down and reset state */
    ov5640_exit_power_down();       /* OV5640 exit power down mode */

    /* Reset using GPIO */
    ov5640_hw_reset();
    ov5640_sw_reset();              /* OV5640 software reset */
    delay_ms(20); // Register access permitted after 20 ms

    /* Reset using Software registers */
    uint8_t reg3103;
    uint8_t reg3008;

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
    uint8_t reg_3034_value = (uint8_t)(pll_charge_pump_mode << 4) | (uint8_t)(mipi_bits << 0);
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
    OV5640_SetMIPIVirtualChannel(0x00);

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

    const float sensor_max_x = 2592;
    const float sensor_max_y = 1944;

    uint16_t sensor_cropped_x = (uint16_t)(2*IMAGE_WIDTH);
    uint16_t sensor_cropped_y = (uint16_t)(2*IMAGE_HEIGHT);
    sensor_cropped_x = (uint16_t)(sensor_cropped_x < sensor_max_x ? sensor_max_x: sensor_cropped_x);
    sensor_cropped_y = (uint16_t)(sensor_cropped_y < sensor_max_y ? sensor_max_y: sensor_cropped_y);

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

#include <math.h>
    /* TODO: THe subtraction here is from the 'scanned' area, to increase FPS */
#define FPS_TARGET ((float)110)
#define PIXEL_X_Y_RATIO (((float)sensor_max_x/sensor_max_y))
#define VTS sqrt((((float)sys_clk_hz / FPS_TARGET) / PIXEL_X_Y_RATIO))
#define HTS (VTS*PIXEL_X_Y_RATIO)
    ov5640_write_reg(0x380c, (uint8_t)(((uint32_t)(HTS))>>8)); // HTS
    ov5640_write_reg(0x380d, (uint8_t)((uint32_t)(HTS)));      // HTS
    ov5640_write_reg(0x380e, (uint8_t)(((uint32_t)(VTS))>>8)); // VTS
    ov5640_write_reg(0x380f, (uint8_t)((uint32_t)(VTS))); // VTS
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

/* select the nex VIN output buffer */
void display_next_buffer_set(uint8_t* next_buffer)
{
    gp_next_buffer = next_buffer;
}

/*
*  VIN and MIPI_CSI callback functions
*/

  //__attribute__ ((section(".itcm_from_flash"), aligned(8192), used))
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
                  display_next_buffer_set(p_args->p_buffer);
                  application_processing_time.camera_image_capture_time_ms = TimeCounter_CountValueConvertToMs(last_vin_frame_end, TimeCounter_CurrentCountGet());
                 last_vin_frame_end = TimeCounter_CurrentCountGet();

              }

              break;
          }

          default:
          {
              /* Do nothing */
              break;
          }
      }
  }
  //__attribute__ ((section(".dtcm_from_flash"), aligned(8192), used))
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
              uint8_t channel_idx = p_args->event_idx;
              FSP_PARAMETER_NOT_USED(channel_idx);
              break;
          }
          default:
              break;
      }
  }
