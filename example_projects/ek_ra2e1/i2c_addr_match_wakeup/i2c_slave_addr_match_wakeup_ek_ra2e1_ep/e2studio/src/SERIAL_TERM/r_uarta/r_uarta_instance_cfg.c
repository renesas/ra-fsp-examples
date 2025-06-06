/***********************************************************************************************************************
* File Name    : r_uarta_instance_cfg.c
* Description  : Contains macros, data structures, and common functions used for the UART configuration.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (USE_SERIAL_TERM == 1)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

#if BSP_PERIPHERAL_UARTA_PRESENT
#include "r_uarta_instance_cfg.h"
#include "../serial.h"
/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

uarta_instance_ctrl_t g_serial_ctrl;

uarta_baud_setting_t g_serial_baud_setting =
{
#if (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC)

  /* Baud rate calculated with Acutal_Error 0.22%. */
  /* The permissible baud rate error range during reception: -4.71% ~ 5.20% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOSC,
  .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1, .brgca = 87, .delay_time = 1
#elif (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_HOCO)

/* Baud rate calculated with Acutal_Error 0.08%. */
/* The permissible baud rate error range during reception: -4.73% ~ 5.22% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_HOCO
, .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1
, .brgca = 139
, .delay_time = 1
 #elif (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MOCO)

/* Baud rate calculated with Acutal_Error 2.12%. */
/* The permissible baud rate error range during reception: -4.49% ~ 4.94% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOCO
, .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1
, .brgca = 17
, .delay_time = 1
 #elif ((BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CFG_FSXP_SOURCE) || (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_LOCO))

/* Baud rate calculated with Acutal_Error 100%. */
/* The permissible baud rate error range during reception: Invalid Range Error */
  .utanck_clock_b.utasel = 0
, .utanck_clock_b.utanck = 0
, .brgca = 0
, .delay_time = 31
 #elif (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK)

/* Baud rate calculated with Acutal_Error 100%. */
/* The permissible baud rate error range during reception: Invalid Range Error */
  .utanck_clock_b.utasel = 0
, .utanck_clock_b.utanck = 0
, .brgca = 0
, .delay_time = 31
 #endif
        };

/** UART extended configuration for UART on UARTA HAL driver */
const uarta_extended_cfg_t g_serial_cfg_extend =
{ .transfer_dir = UARTA_DIR_BIT_LSB_FIRST, .transfer_level = UARTA_ALV_BIT_POSITIVE_LOGIC, .clock_output =
          UARTA_CLOCK_OUTPUT_DISABLED,
  .p_baud_setting = &g_serial_baud_setting, };

/** UART interface configuration */
const uart_cfg_t g_serial_cfg =
{ .channel = 0, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          serial_callback,
  .p_context = NULL, .p_extend = &g_serial_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
.rxi_ipl = (5), .txi_ipl = (5), .eri_ipl = (5),
#if defined(VECTOR_NUMBER_UARTX_RXI)
.rxi_irq             = VECTOR_NUMBER_UARTX_RXI,
#else
.rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTX_TXI)
.txi_irq             = VECTOR_NUMBER_UARTX_TXI,
#else
.txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTX_TEI)
.tei_irq             = VECTOR_NUMBER_UARTX_TEI,
#else
.tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTX_ERI)
.eri_irq             = VECTOR_NUMBER_UARTX_ERI,
#else
.eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_serial =
{ .p_ctrl = &g_serial_ctrl, .p_cfg = &g_serial_cfg, .p_api = &g_uart_on_uarta };

#endif /* BSP_PERIPHERAL_UARTA_PRESENT */
#endif /* USE_SERIAL_TERM */
