/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

#include "r_ioport.h"
#include "bsp_cfg.h"
#include "bsp_pin_cfg.h"

#include "sdram.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* SDRAM size, in bytes */
 #define SDRAM_SIZE                             (32 * 1024 * 1024)

/*
 * Set ACTIVE-to-PRECHARGE command (tRAS) timing
 * e.g. tRAS = 42ns -> 6cycles are needed at SDCLK 120MHz
 *      tRAS = 37ns -> 5cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_TRAS                     (6U)

/*
 * Set ACTIVE-to-READ or WRITE delay tRCD (tRCD) timing
 * e.g. tRCD = 18ns -> 3cycles are needed at SDCLK 120MHz
 *      tRCD = 15ns -> 2cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_TRCD                     (3U)

/*
 * Set PRECHARGE command period (tRP) timing
 * e.g. tRP  = 18ns -> 3cycles are needed at SDCLK 120MHz
 *      tRP  = 15ns -> 2cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_TRP                      (3U)

/*
 * Set WRITE recovery time (tWR) timing
 * e.g. tWR  = 1CLK + 6ns -> 2cycles are needed at SDCLK 120MHz
 *      tWR  = 1CLK + 7ns -> 2cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_TWR                      (2U)

/*
 * Set CAS (READ) latency (CL) timing
 * e.g. CL = 18ns -> 3cycles are needed at SDCLK 120MHz
 * e.g. CL = 15ns -> 2cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_CL                       (3U)

/*
 * Set AUTO REFRESH period (tRFC) timing
 * e.g. tRFC = 60nS -> 8cycles are needed at SDCLK 120MHz
 *      tRFC = 66nS -> 8cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_TRFC                     (8U)

/*
 * Set Average Refresh period
 * e.g. tREF = 64ms/8192rows -> 7.8125us/each row.  937cycles are needed at SDCLK 120MHz
 */
 #define BSP_PRV_SDRAM_REF_CMD_INTERVAL         (937U)

/*
 * Set Auto-Refresh issue times in initialization sequence needed for SDRAM device
 * Typical SDR SDRAM device needs twice of Auto-Refresh command issue
 */
 #define BSP_PRV_SDRAM_SDIR_REF_TIMES           (2U)

/*
 * Set RAW address offset
 * Available settings are
 * 8  : 8-bit
 * 9  : 9-bit
 * 10 : 10-bit
 * 11 : 11-bit
 */
 #define BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET    (9U)

/*
 * Select endian mode for SDRAM address space
 * 0 : Endian of SDRAM address space is the same as the endian of operating mode
 * 1 : Endian of SDRAM address space is not the endian of operating mode
 */
 #define BSP_PRV_SDRAM_ENDIAN_MODE              (0U)

/*
 * Select access mode
 * Typically Continuous access should be enabled to get better SDRAM bandwidth
 * 0: Continuous access is disabled
 * 1: Continuous access is enabled
 */
 #define BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE    (1U)

/*
 * Select bus width
 * 0: 16-bit
 * 1: 32-bit
 * 2: 8-bit
 */
 #define BSP_PRV_SDRAM_BUS_WIDTH                (0U)

 #if ((BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET != 8U) && (BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET != 9U) \
    && (BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET != 10U) && (BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET > 11U))
  #error "BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET must be either of 8,9,10 or 11"
 #endif

 #if ((BSP_PRV_SDRAM_BUS_WIDTH != 0) && (BSP_PRV_SDRAM_BUS_WIDTH != 1U) && (BSP_PRV_SDRAM_BUS_WIDTH != 2U))
  #error "BSP_PRV_SDRAM_BUS_WIDTH must be either of 0(16-bit) or 1(32-bit) or 2(8-bit)"
 #endif

 #if ((BSP_PRV_SDRAM_ENDIAN_MODE != 0) && (BSP_PRV_SDRAM_ENDIAN_MODE != 1))
  #error \
    "BSP_PRV_SDRAM_ENDIAN_MODE must be either of 0(same endian as operating mode) or 2(another endian against operating mode)"
 #endif

 #if ((BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE != 0) && (BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE != 1))
  #error \
    "BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE must be either of 0(continuous access is disabled) or 1(continuous access is enabled)"
 #endif

 #define BSP_PRV_SDRAM_MR_WB_SINGLE_LOC_ACC    (1U) /* MR.M9                : Single Location Access */
 #define BSP_PRV_SDRAM_MR_OP_MODE              (0U) /* MR.M8:M7             : Standard Operation */
 #define BSP_PRV_SDRAM_MR_BT_SEQUENCTIAL       (0U) /* MR.M3 Burst Type     : Sequential */
 #define BSP_PRV_SDRAM_MR_BURST_LENGTH         (0U) /* MR.M2:M0 Burst Length: 0(1 burst) */

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

extern ioport_instance_ctrl_t g_ioport_ctrl;

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/

/** Array of pin configurations. */
static const ioport_pin_cfg_t g_sdram_bsp_pin_cfg_data[] =
{
    {
        .pin     = BSP_IO_PORT_10_PIN_09, // SDCLK
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_09_PIN_08, // SDRAM RAS
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_01_PIN_15, // SDRAM CS
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_09_PIN_09, // SDRAM CAS
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_01_PIN_13, // SDRAM CKE
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_01_PIN_14, // SDRAM WE
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    /*
    {
        .pin     = BSP_IO_PORT_01_PIN_15, // SDRAM SDCS
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    */
    {
        .pin     = BSP_IO_PORT_10_PIN_10, // SDRAM DQM0
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_01_PIN_12, // SDRAM DQM1
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    /*
    {
        .pin     = BSP_IO_PORT_10_PIN_11, // SDRAM DQM2
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_00, // SDRAM DQM3
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    */
///////////////////////////////
    {
        .pin     = BSP_IO_PORT_06_PIN_01, // SDRAM DQ0
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_02, // SDRAM DQ1
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_03, // SDRAM DQ2
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_04, // SDRAM DQ3
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_05, // SDRAM DQ4
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_06, // SDRAM DQ5
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_07, // SDRAM DQ6
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_00, // SDRAM DQ7
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_09, // SDRAM DQ8
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_10, // SDRAM DQ9
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_11, // SDRAM DQ10
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_12, // SDRAM DQ11
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_13, // SDRAM DQ12
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_14, // SDRAM DQ13
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_06_PIN_15, // SDRAM DQ14
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_08, // SDRAM DQ15
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    /*
    {
        .pin     = BSP_IO_PORT_10_PIN_12, // SDRAM DQ16
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_13, // SDRAM DQ17
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_14, // SDRAM DQ18
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_15, // SDRAM DQ19
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_08_PIN_13, // SDRAM DQ20
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_07, // SDRAM DQ21
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_06, // SDRAM DQ22
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_05, // SDRAM DQ23
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_04, // SDRAM DQ24
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_03, // SDRAM DQ25
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_10_PIN_02, // SDRAM DQ26
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_05_PIN_05, // SDRAM DQ27
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_05_PIN_06, // SDRAM DQ28
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_05_PIN_07, // SDRAM DQ29
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_05_PIN_08, // SDRAM DQ30
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_05_PIN_09, // SDRAM DQ31
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
*/
    ///////////////////
    {
        .pin     = BSP_IO_PORT_03_PIN_00, // SDRAM A0
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_01, // SDRAM A1
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_02, // SDRAM A2
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_03, // SDRAM A3
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_04, // SDRAM A4
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_05, // SDRAM A5
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_06, // SDRAM A6
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_07, // SDRAM A7
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_08, // SDRAM A8
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_09, // SDRAM A9
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_10, // SDRAM A10
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_11, // SDRAM A11
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_03_PIN_12, // SDRAM A12
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_09_PIN_05, // SDRAM BA0
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
    {
        .pin     = BSP_IO_PORT_09_PIN_05, // SDRAM BA1
        .pin_cfg =
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
             (uint32_t) IOPORT_PERIPHERAL_BUS),
    },
};

/** Has pointer to pin configuration array and number of pins to be configured. */
const ioport_cfg_t g_sdram_bsp_pin_cfg =
{
    .number_of_pins = (sizeof(g_sdram_bsp_pin_cfg_data)) / sizeof(ioport_pin_cfg_t),
    .p_pin_cfg_data = &g_sdram_bsp_pin_cfg_data[0],
};

/**********************************************************************************************************************
 * Function Name: init_sdram
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void init_sdram (void)
{
    /* Configure pins. */
    R_IOPORT_Close(&g_ioport_ctrl);
    R_IOPORT_Open(&g_ioport_ctrl, &g_sdram_bsp_pin_cfg);

    /** Initialize SDRAM. */
    /** Delay at least 100uS after SDCLK active */
    R_BSP_SoftwareDelay(200U, BSP_DELAY_UNITS_MICROSECONDS);

    /** Setting for SDRAM initialization sequence */
 #if (BSP_PRV_SDRAM_TRP < 3)
    R_BUS->SDRAM.SDIR_b.PRC = 3U;
 #else
    R_BUS->SDRAM.SDIR_b.PRC = BSP_PRV_SDRAM_TRP - 3U;
 #endif

    while (R_BUS->SDRAM.SDSR)
    {
        /* According to h/w manual, need to confirm that all the status bits in SDSR are 0 before SDIR modification. */
    }

    R_BUS->SDRAM.SDIR_b.ARFC = BSP_PRV_SDRAM_SDIR_REF_TIMES;

    while (R_BUS->SDRAM.SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDIR modification. */
    }

 #if (BSP_PRV_SDRAM_TRFC < 3)
    R_BUS->SDRAM.SDIR_b.ARFI = 0U;
 #else
    R_BUS->SDRAM.SDIR_b.ARFI = BSP_PRV_SDRAM_TRFC - 3U;
 #endif

    while (R_BUS->SDRAM.SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDICR modification. */
    }

    /** Start SDRAM initialization sequence.
     * Following operation is automatically done when set SDICR.INIRQ bit.
     * Perform a PRECHARGE ALL command and wait at least tRP time.
     * Issue an AUTO REFRESH command and wait at least tRFC time.
     * Issue an AUTO REFRESH command and wait at least tRFC time.
     */
    R_BUS->SDRAM.SDICR_b.INIRQ = 1U;
    while (R_BUS->SDRAM.SDSR_b.INIST)
    {
        /* Wait the end of initialization sequence. */
    }

    /** Setting for SDRAM controller */
    R_BUS->SDRAM.SDCCR_b.BSIZE  = BSP_PRV_SDRAM_BUS_WIDTH;             /* set SDRAM bus width */
    R_BUS->SDRAM.SDAMOD_b.BE    = BSP_PRV_SDRAM_CONTINUOUS_ACCESSMODE; /* enable continuous access */
    R_BUS->SDRAM.SDCMOD_b.EMODE = BSP_PRV_SDRAM_ENDIAN_MODE;           /* set endian mode for SDRAM address space */

    while (R_BUS->SDRAM.SDSR)
    {
        /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDMOD modification. */
    }

    /** Using LMR command, program the mode register */
    R_BUS->SDRAM.SDMOD = ((((uint16_t) (BSP_PRV_SDRAM_MR_WB_SINGLE_LOC_ACC << 9) |
                            (uint16_t) (BSP_PRV_SDRAM_MR_OP_MODE << 7)) |
                           (uint16_t) (BSP_PRV_SDRAM_CL << 4)) |
                          (uint16_t) (BSP_PRV_SDRAM_MR_BT_SEQUENCTIAL << 3)) |
                         (uint16_t) (BSP_PRV_SDRAM_MR_BURST_LENGTH << 0);

    /** wait at least tMRD time */
    while (R_BUS->SDRAM.SDSR_b.MRSST)
    {
        /* Wait until Mode Register setting done. */
    }

    /** Set timing parameters for SDRAM */
    R_BUS->SDRAM.SDTR_b.RAS = BSP_PRV_SDRAM_TRAS - 1U; /* set ACTIVE-to-PRECHARGE command cycles*/
    R_BUS->SDRAM.SDTR_b.RCD = BSP_PRV_SDRAM_TRCD - 1U; /* set ACTIVE to READ/WRITE delay cycles */
    R_BUS->SDRAM.SDTR_b.RP  = BSP_PRV_SDRAM_TRP - 1U;  /* set PRECHARGE command period cycles */
    R_BUS->SDRAM.SDTR_b.WR  = BSP_PRV_SDRAM_TWR - 1U;  /* set write recovery cycles */
    R_BUS->SDRAM.SDTR_b.CL  = BSP_PRV_SDRAM_CL;        /* set SDRAM column latency cycles */

    /** Set row address offset for target SDRAM */
    R_BUS->SDRAM.SDADR_b.MXC = BSP_PRV_SDRAM_SDADR_ROW_ADDR_OFFSET - 8U;

    R_BUS->SDRAM.SDRFCR_b.REFW = (uint16_t) (BSP_PRV_SDRAM_TRFC - 1U); /* set Auto-Refresh issuing cycle */
    R_BUS->SDRAM.SDRFCR_b.RFC  = BSP_PRV_SDRAM_REF_CMD_INTERVAL - 1U;  /* set Auto-Refresh period */

    /** Start Auto-refresh */
    R_BUS->SDRAM.SDRFEN_b.RFEN = 1U;

    /** Enable SDRAM access */
    R_BUS->SDRAM.SDCCR_b.EXENB = 1U;
}
/**********************************************************************************************************************
 End of function init_sdram
 *********************************************************************************************************************/







