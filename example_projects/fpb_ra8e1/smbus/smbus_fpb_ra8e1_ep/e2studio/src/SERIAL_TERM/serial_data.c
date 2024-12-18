/***********************************************************************************************************************
 * File Name    : serial_data.c
 * Description  : Contains declarations of data structures and functions used in serial_terminal.c
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "serial_data.h"
#include "serial_terminal.h"
#if (BSP_FEATURE_SCI_VERSION == 1U)
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

sci_uart_instance_ctrl_t g_uart_ctrl;

baud_setting_t g_uart_baud_setting =
        {
        /* Baud rate calculated with 0.469% error. */.semr_baudrate_bits_b.abcse = 0,
          .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 53, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme =
                  false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
#if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
    .de_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
          },
  .irda_setting =
  { .ircr_bits_b.ire = 0, .ircr_bits_b.irrxinv = 0, .ircr_bits_b.irtxinv = 0, }, };

/** UART interface configuration */
const uart_cfg_t g_uart_cfg =
{ .channel = SERIAL_CHANNEL, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          serial_callback,
  .p_context = NULL, .p_extend = &g_uart_cfg_extend,
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
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCIX_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCIX_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCIX_TXI)
                .txi_irq             = VECTOR_NUMBER_SCIX_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCIX_TEI)
                .tei_irq             = VECTOR_NUMBER_SCIX_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCIX_ERI)
                .eri_irq             = VECTOR_NUMBER_SCIX_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart =
{ .p_ctrl = &g_uart_ctrl, .p_cfg = &g_uart_cfg, .p_api = &g_uart_on_sci };


#elif (BSP_FEATURE_SCI_VERSION == 2U)

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

sci_b_uart_instance_ctrl_t g_uart_ctrl;

sci_b_baud_setting_t g_uart_baud_setting =
        {
        /* Baud rate calculated with 0.009% error. */.baudrate_bits_b.abcse = 0,
          .baudrate_bits_b.abcs = 0, .baudrate_bits_b.bgdm = 1, .baudrate_bits_b.cks = 0, .baudrate_bits_b.brr = 43, .baudrate_bits_b.mddr =
                  (uint8_t) 173,
          .baudrate_bits_b.brme = true };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_b_uart_extended_cfg_t g_uart_cfg_extend =
{ .clock = SCI_B_UART_CLOCK_INT, .rx_edge_start = SCI_B_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_B_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_B_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart_baud_setting, .flow_control =
          SCI_B_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_B_UART_RS485_DISABLE,
    .polarity = SCI_B_UART_RS485_DE_POLARITY_HIGH,
    .assertion_time = 1,
    .negation_time = 1, } };

/** UART interface configuration */
const uart_cfg_t g_uart_cfg =
{ .channel = SERIAL_CHANNEL, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          serial_callback,
  .p_context = NULL, .p_extend = &g_uart_cfg_extend,
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
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCIX_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCIX_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCIX_TXI)
                .txi_irq             = VECTOR_NUMBER_SCIX_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCIX_TEI)
                .tei_irq             = VECTOR_NUMBER_SCIX_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCIX_ERI)
                .eri_irq             = VECTOR_NUMBER_SCIX_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart =
{ .p_ctrl = &g_uart_ctrl, .p_cfg = &g_uart_cfg, .p_api = &g_uart_on_sci_b };

#endif /* BSP_FEATURE_SCI_VERSION */

#endif /* USE_VIRTUAL_COM */
