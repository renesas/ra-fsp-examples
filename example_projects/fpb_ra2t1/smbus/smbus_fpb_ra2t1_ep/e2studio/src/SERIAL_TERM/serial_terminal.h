/***********************************************************************************************************************
 * File Name    : serial_terminal.h
 * Description  : Contains declarations of data structures and functions used in serial_terminal.c
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#if (USE_VIRTUAL_COM == 1)

#ifndef SER_TERM_SERIAL_TERMINAL_H_
#define SER_TERM_SERIAL_TERMINAL_H_

/* Public function declarations */
#include "serial_data.h"

fsp_err_t uart_init (void);
void uart_deinit (void);
uint8_t serial_write_message (char * format, ...);

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macro for UART */
#define UART_EP_INFO_SIZE               (512U)
#define UART_ERROR_CODE_SIZE            (32U)
#define UART_ERROR_INFO_SIZE            (128U)
#define UART_TIMEOUT_ONE_BYTE           (1000)
#define UART_CHAR_TIMEOUT_VALUE         (1U)
#define UART_CHAR_TIMEOUT_UNIT          (BSP_DELAY_UNITS_MICROSECONDS)
#define BUFFER_SIZE_UP                  (2048)
#define BUFFER_SIZE_DOWN                (16)

/* Macro for ASCII character */
#define ENTER_KEY                 ('\r')

/* Board-specific macros for UART hardware configuration */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
  #define SERIAL_CHANNEL                    (9U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_10_PIN_15)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_10_PIN_14)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
#elif defined(BOARD_RA6E2_EK) || defined(BOARD_RA4E2_EK)
  #define SERIAL_CHANNEL                    (0U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_04_PIN_10)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_04_PIN_11)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8)
#elif defined(BOARD_RA2A2_EK) || defined(BOARD_RA2E3_FPB) || defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK)
  #define SERIAL_CHANNEL                    (9U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_10)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_09)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
#elif defined(BOARD_RA2T1_FPB)
  #define SERIAL_CHANNEL                    (9U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_10)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_09)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
#elif defined(BOARD_RA8T1_MCK)
  #define SERIAL_CHANNEL                    (3U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_03_PIN_09)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_03_PIN_10)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
#elif defined(BOARD_RA8E1_FPB)
  #define SERIAL_CHANNEL                    (9U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_01)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_02)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
#endif

/* Macros for the UART baud rate configuration */
#define SERIAL_BAUD_RATE                    (115200U)
#define SERIAL_ERR_X1000                    (5000U)
#define SERIAL_MODULATION                   (0U)

/* Macros for setting interrupt */
#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI0_RXI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI1_RXI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI2_RXI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI3_RXI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI4_RXI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI5_RXI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI6_RXI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI7_RXI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI8_RXI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_RXI)
    #define VECTOR_NUMBER_SCIX_RXI (VECTOR_NUMBER_SCI9_RXI)
  #endif
#endif

#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI0_TXI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI1_TXI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI2_TXI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI3_TXI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI4_TXI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI5_TXI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI6_TXI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI7_TXI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI8_TXI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_TXI)
    #define VECTOR_NUMBER_SCIX_TXI (VECTOR_NUMBER_SCI9_TXI)
  #endif
#endif

#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI0_TEI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI1_TEI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI2_TEI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI3_TEI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI4_TEI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI5_TEI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI6_TEI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI7_TEI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI8_TEI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_TEI)
    #define VECTOR_NUMBER_SCIX_TEI (VECTOR_NUMBER_SCI9_TEI)
  #endif
#endif

#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI0_ERI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI1_ERI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI2_ERI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI3_ERI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI4_ERI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI5_ERI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI6_ERI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI7_ERI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI8_ERI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_ERI)
    #define VECTOR_NUMBER_SCIX_ERI (VECTOR_NUMBER_SCI9_ERI)
  #endif
#endif

#endif /* SER_TERM_SERIAL_TERMINAL_H_ */
#endif /* USE_VIRTUAL_COM */
