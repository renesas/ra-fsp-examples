/***********************************************************************************************************************
 * File Name    : transfer_initialise.c
 * Description  : Contains data structures and functions used in transfer_initialise.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "transfer_initialise.h"

/*******************************************************************************************************************//**
 * @addtogroup transfer_initialise
 * @{
 **********************************************************************************************************************/

/* Variable to flag for printing data on RTT console */
volatile bool g_send_data_to_console_flag = false;

/* Destination array to which the DMAC writes GPT timer values */
uint32_t g_dest_data[DEST_DATA_SIZE];

/* Source data that will be transferred by the DMAC g_transfer_led_blink.
 * This data will be transferred to the IOPORT PCNTR1 register,
 * which configures the pin direction and output level.
 * The register is a 32-bit register - bits [15:0] = direction (input/output)
 *                                   - bits [31:16] = output level (high/low)
 * The user LEDs on board are connected to I/O pins
 * Therefore, writing the array values to the IOPORT PCNTR1 register sets the pins
 * as outputs and sets pin[1] high, pin[0] low (for example). */

#if defined(BOARD_RA6M1_EK)
/* LED Port data array for EK-RA6M1 board */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,
 0x00001000,0x10001000
};
#elif defined(BOARD_RA4M1_EK) || defined(BOARD_RA6M2_EK) || defined(BOARD_RA4W1_EK)
/* LED Port data array for EK-RA4M1, EK-RA4W1 and EK-RA6M2 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,0x00000040,0x00400040,
 0x00000040,0x00400040
};
#elif defined(BOARD_RA6M4_EK) || defined (BOARD_RA4M3_EK) || defined (BOARD_RA8P1_EK) ||\
	  defined (BOARD_RA8M2_EK)
/* LED Port data array for EK-RA6M4, EK-RA4M3, EK-RA8P1 and EK-RA8M2 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,0x00000001,0x00010001,
 0x00000001,0x00010001
};
#elif defined (BOARD_RA4E2_EK)
/* LED Port data array for EK-RA4E2 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
  0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,0x10001000,0x00001000,
  0x10001000,0x00001000
};
#elif defined (BOARD_RA6E2_EK)
/* LED Port data array for EK-RA6E2 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,
 0x20002000,0x00002000
};
#elif defined(BOARD_RA6T1_RSSK)
/* LED Port data array for RSSK-RA6T1 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
  0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,
  0x00000100,0x01000100
};
#elif defined (BOARD_RA4M2_EK) || defined (BOARD_RA8E2_EK)
/* LED Port data array for EK-RA4M2 and EK-RA8E2 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,
 0x00000020,0x00200020
};
#elif defined (BOARD_RA6M5_EK)
/* LED Port data array for EK-RA6M5 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,0x01000100,0x00000100,
 0x01000100,0x00000100
};
#elif defined (BOARD_RA6E1_FPB) || defined (BOARD_RA4E1_FPB) || defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
/* LED Port data array for FPB-RA6E1, FPB-RA4E1, EK-RA8D1 and EK-RA8M1 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,
 0x00000080,0x00800080
};
#elif defined (BOARD_RA6T2_MCK)
/* LED Port data array for MCK-RA6T2 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,0x00000002,0x00020002,
 0x00000002,0x00020002
};
#elif defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK)
/* LED Port data array for MCK-RA4T1 and MCK-RA6T3 board */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,0x00002000,0x20002000,
 0x00002000,0x20002000
};
#elif defined (BOARD_RA8T1_MCK)
/* LED Port data array for MCK-RA8T1 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,0x00005000,0x50005000,
 0x00005000,0x50005000,
};
#elif defined (BOARD_RA8E1_FPB)
/* LED Port data array for FPB-RA8E1 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,0x00000010,0x00100010,
 0x00000010,0x00100010,
};
#elif defined (BOARD_RA4L1_EK)
/* LED Port data array for EK-RA4L1 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,0x00000200,0x02000200,
 0x00000200,0x02000200,
};
#elif defined (BOARD_RA4C1_EK)
/* LED Port data array for EK-RA4C1 */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,0x00000400,0x04000400,
 0x00000400,0x04000400,
};
#elif defined (BOARD_RA8T2_MCK)
/* LED Port data array for BOARD_RA8T2_MCK */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,0x00004000,0x40004000,
 0x00004000,0x40004000
 };
#else
/* LED Port data array for EK-RA6M3 and EK-RA6M3G board */
uint32_t g_source_data[SOURCE_DATA_SIZE]=
{
 0x00000007,0x00070007,
 0x00000007,0x00010007,0x00020007,0x00030007,0x00040007,0x00050007,0x00060007,0x00070007,
 0x00000007,0x00070007,0x00000007,0x00070007,0x00000007,0x00070007,0x00000007,0x00070007,
 0x00000007,0x00010007,0x00000007,0x00010007,0x00000007,0x00010007,0x00000007,0x00010007,
 0x00000007,0x00020007,0x00000007,0x00020007,0x00000007,0x00020007,0x00000007,0x00020007,
 0x00000007,0x00040007,0x00000007,0x00040007,0x00000007,0x00040007,0x00000007,0x00040007,
 0x00000007,0x00070007,0x00000007,0x00070007,0x00000007,0x00070007,0x00000007,0x00070007,
 0x00000007,0x00010007,0x00020007,0x00030007,0x00040007,0x00050007,0x00060007,0x00070007,
 0x00000007,0x00070007
};
#endif

/*******************************************************************************************************************//**
 *  @brief      Initializing DMAC transfer instance based on transfer unit and enable for transfer.
 *  @param[in]  p_transfer_ctl       Transfer instance control structure.
 *  @param[in]  p_transfer_cfg       Transfer instance configuration structure.
 *  @param[in]  transfer_unit        DMAC transfer unit.
 *  @retval     FSP_SUCCESS          On successful initialization of dmac_transfer.
 *  @retval     Any other error code apart from FSP_SUCCESS if Unsuccessful open or enable.
 **********************************************************************************************************************/
fsp_err_t dmac_transfer_init(dmac_instance_ctrl_t *const p_transfer_ctl,
                             transfer_cfg_t const *const p_transfer_cfg, uint8_t transfer_unit)
{
    /* Variable to help handle error codes */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open the transfer instance */
    fsp_err = R_DMAC_Open(p_transfer_ctl, p_transfer_cfg);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("** R_DMAC_Open API for transfer%d failed **\r\n", transfer_unit);
        return fsp_err;
    }

    /* Enable transfer for the configured activation source */
    fsp_err = R_DMAC_Enable(p_transfer_ctl);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        /* If R_DMAC_Open succeeded but enabling fails, immediate cleanup is required.
         * Since close is done here, cleanup in dmac_transfer_deinit for open is not required. */
        err = R_DMAC_Close(p_transfer_ctl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* DMAC Close failure message */
            APP_ERR_PRINT("** R_DMAC_Close for transfer%d API failed **\r\n", transfer_unit);
        }
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief     De-initializing DMAC transfer instance based on transfer unit and enable for transfer.
 *  @param[in] p_transfer_ctl       Transfer instance control structure.
 *  @param[in] transfer_unit        DMAC transfer unit.
 *  @retval    None
 **********************************************************************************************************************/
void dmac_transfer_deinit(dmac_instance_ctrl_t *const p_transfer_ctl, uint8_t transfer_unit)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_DMAC_Close(p_transfer_ctl);
    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* DMAC Close failure message */
        APP_ERR_PRINT("** R_DMAC_Close for transfer%d API failed **\r\n", transfer_unit);
    }
}

/*******************************************************************************************************************//**
 *  @brief          Start DMAC transfer.
 *  @param[in]      transfer_ctrl_t
 *  @retval         FSP_SUCCESS      Upon successful start of DMAC.
 *  @retval         Any other error code apart from FSP_SUCCESS is Unsuccessful start.
 **********************************************************************************************************************/
fsp_err_t dmac_transfer_software_start(transfer_ctrl_t * const p_transfer_ctrl)
{

    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Start DMAC transfer by software */
    fsp_err = R_DMAC_SoftwareStart(p_transfer_ctrl, TRANSFER_START_MODE_SINGLE);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("\r\nDMAC transfer software start failed.\r\n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       Print DMAC data transferred by g_transfer_write_ioport on user RTT input.
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void dmac_transfer_print_data(void)
{
    APP_PRINT("\r\n*** Data transferred through g_transfer_gpt_timer ***\r\n");
    APP_PRINT("data[0]=%05d\n", (int)g_dest_data[0]);
    APP_PRINT("data[1]=%05d\n", (int)g_dest_data[1]);
    APP_PRINT("data[2]=%05d\n", (int)g_dest_data[2]);
    APP_PRINT("data[3]=%05d\n", (int)g_dest_data[3]);
    APP_PRINT("data[4]=%05d\n", (int)g_dest_data[4]);
    APP_PRINT("data[5]=%05d\n", (int)g_dest_data[5]);
    APP_PRINT("data[6]=%05d\n", (int)g_dest_data[6]);
    APP_PRINT("data[7]=%05d\n", (int)g_dest_data[7]);
    APP_PRINT("-------------\n");
}

/*******************************************************************************************************************//**
 *  @brief       Set the source and destination address for the DMAC transfer instance.
 *  @param[IN]   p_config       Transfer instance configuration structure.
 *  @param[IN]   p_src          Source address.
 *  @param[IN]   p_dest         Destination address.
 *  @retval      None
 **********************************************************************************************************************/
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest)
{
    p_config->p_info->p_src = (void *) p_src;
    p_config->p_info->p_dest = (void *) p_dest;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup transfer_initialise)
 **********************************************************************************************************************/
