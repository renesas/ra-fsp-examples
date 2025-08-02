/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_sdram.h
 * Version      : 1.0
 * Description  : sdram interface source code to configure / support SDRAM.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

#include "r_ioport.h"
#include "bsp_cfg.h"
#include "bsp_pin_cfg.h"
#include "r_typedefs.h"

#include "board_sdram.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

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

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
static bool_t is_initialised = false;

/**********************************************************************************************************************
 * Function Name: is_sdram_initialised
 * Description  : Helper function  to determine if SDRAM has been initialised. Exact used is irrelevant to the user.
 * Argument     : NONE.
 * Return Value : initisation has occurred
 *********************************************************************************************************************/
bool_t is_sdram_initialised(void)
{
    if(true == is_initialised)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}
/**********************************************************************************************************************
 End of function is_sdram_initialised
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: bsp_sdram_init
 * Description  : Initialises SDRAM. has guard to prevent code from executing twice.
 *                Uses BSP_CFG_SDRAM_ENABLED to protect against execution when FSP has set SDRAM Support to Enabled
 * Argument     : NONE.
 * Return Value : NONE.
 *********************************************************************************************************************/
void bsp_sdram_init (void)
{
#if BSP_CFG_SDRAM_ENABLED
    is_initialised = true;
#else
    if(false == is_sdram_initialised())
    {
        /** Setting for SDRAM initialization sequence */
    #if (BSP_PRV_SDRAM_TRP < 3)
        R_BUS->SDRAM.SDIR_b.PRC = 3U;
    #else
        R_BUS->SDRAM.SDIR_b.PRC = BSP_PRV_SDRAM_TRP - 3U;
    #endif

        while (R_BUS->SDRAM.SDSR)
        {
            /* According to h/w maual, need to confirm that all the status bits in SDSR are 0 before SDIR modification. */
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

        is_initialised = true;
    }
#endif // BSP_CFG_SDRAM_ENABLED
}
/**********************************************************************************************************************
 End of function bsp_sdram_init
 *********************************************************************************************************************/
