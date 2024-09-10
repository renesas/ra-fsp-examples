/***********************************************************************************************************************
 * File Name    : serial_data.c
 * Description  : Contains declarations of data structures and functions used in serial_terminal.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#include "serial_data.h"

/* DTC instance control */
dtc_instance_ctrl_t g_transfer1_ctrl;
dtc_instance_ctrl_t g_transfer0_ctrl;

/* DTC information */
transfer_info_t g_transfer1_info =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = 0,
  .length = 0, };

transfer_info_t g_transfer0_info =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = 0,
  .length = 0, };

/* DTC instance configure */
const dtc_extended_cfg_t g_transfer1_cfg_extend =
{ .activation_source = VECTOR_NUMBER_UARTA0_RXI, };

const transfer_cfg_t g_transfer1_cfg =
{
  .p_info = &g_transfer1_info,
  .p_extend = &g_transfer1_cfg_extend,
};

const dtc_extended_cfg_t g_transfer0_cfg_extend =
{ .activation_source = VECTOR_NUMBER_UARTA0_TXI, };

const transfer_cfg_t g_transfer0_cfg =
{
  .p_info = &g_transfer0_info,
  .p_extend = &g_transfer0_cfg_extend,
};

/* DTC instance */
const transfer_instance_t g_transfer1 =
{ .p_ctrl = &g_transfer1_ctrl, .p_cfg = &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };

const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };

/* UARTA instance control */
uarta_instance_ctrl_t g_uart_ctrl;

/* Configure UARTA clock */
uarta_baud_setting_t g_uart_baud_setting =
{
#if (BSP_CFG_UARTA_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC)

  /* Baud rate calculated with Acutal_Error 0.22%. */
  /* The permissible baud rate error range during reception: -4.71% ~ 5.20% */
  .uta0ck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOSC,
  .uta0ck_clock_b.uta0ck = UARTA_CLOCK_DIV_1, .brgca = 87
#elif (BSP_CFG_UARTA_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_HOCO)

/* Baud rate calculated with Acutal_Error 0.08%. */
/* The permissible baud rate error range during reception: -4.73% ~ 5.22% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_HOCO
, .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1
, .brgca = 139
 #elif (BSP_CFG_UARTA_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MOCO)

/* Baud rate calculated with Acutal_Error 2.12%. */
/* The permissible baud rate error range during reception: -4.49% ~ 4.94% */
  .uta0ck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOCO
, .uta0ck_clock_b.uta0ck = UARTA_CLOCK_DIV_1
, .brgca = 17
 #elif (BSP_CFG_UARTA_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_FSXP)

/* Baud rate calculated with Acutal_Error 100%. */
/* The permissible baud rate error range during reception: Invalid Range Error */
  .uta0ck_clock_b.utasel = 0
, .uta0ck_clock_b.uta0ck = 0
, .brgca = 0
 #endif
};

/* UART extended configuration for UART on UARTA HAL driver */
const uarta_extended_cfg_t g_uart_cfg_extend =
{.transfer_dir = UARTA_DIR_BIT_LSB_FIRST, .transfer_level = UARTA_ALV_BIT_POSITIVE_LOGIC,
  .p_baud_setting = &g_uart_baud_setting, };

/* UART interface configuration */
const uart_cfg_t g_uart_cfg =
{ .channel = 0, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          serial_callback,
  .p_context = NULL, .p_extend = &g_uart_cfg_extend,
  .p_transfer_tx = &g_transfer0,
  .p_transfer_rx = &g_transfer1,
  .rxi_ipl = (2),
  .txi_ipl = (2), .eri_ipl = (2),

#if defined(VECTOR_NUMBER_UARTA0_RXI)
                .rxi_irq             = VECTOR_NUMBER_UARTA0_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTA0_TXI)
                .txi_irq             = VECTOR_NUMBER_UARTA0_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTA0_ERRI)
                .eri_irq             = VECTOR_NUMBER_UARTA0_ERRI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
};

/* UARTA instance */
const uart_instance_t g_uart =
{ .p_ctrl = &g_uart_ctrl, .p_cfg = &g_uart_cfg, .p_api = &g_uart_on_uarta };
#endif /* USE_VIRTUAL_COM */
