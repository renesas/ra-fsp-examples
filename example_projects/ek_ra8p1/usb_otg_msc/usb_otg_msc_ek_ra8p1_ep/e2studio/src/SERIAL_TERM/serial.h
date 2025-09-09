/***********************************************************************************************************************
* File Name    : serial.h
* Description  : Contains macros, data structures, and common functions used for serial.c
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SERIAL_H_
#define SERIAL_H_

#if (USE_VIRTUAL_COM == 1)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include <vector_data.h>
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Board-specific macros for UART hardware configuration */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
  #define SERIAL_CHANNEL                    (9U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_10_PIN_15)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_10_PIN_14)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                             (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
  #define SERIAL_PIN_RX_CFG                  (SERIAL_PIN_CFG)
  #define SERIAL_PIN_TX_CFG                  (SERIAL_PIN_CFG)
#elif defined(BOARD_RA6E2_EK) || defined(BOARD_RA4E2_EK)
  #define SERIAL_CHANNEL                    (0U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_04_PIN_10)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_04_PIN_11)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                             (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8)
  #define SERIAL_PIN_RX_CFG                  (SERIAL_PIN_CFG)
  #define SERIAL_PIN_TX_CFG                  (SERIAL_PIN_CFG)
#elif defined(BOARD_RA2E3_FPB) || defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK)
  #define SERIAL_CHANNEL                    (9U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_10)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_09)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                             (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
  #define SERIAL_PIN_RX_CFG                  (SERIAL_PIN_CFG)
  #define SERIAL_PIN_TX_CFG                  (SERIAL_PIN_CFG)
#elif defined(BOARD_RA8T1_MCK)
  #define SERIAL_CHANNEL                    (3U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_03_PIN_09)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_03_PIN_10)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                             (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_SCI1_3_5_7_9)
  #define SERIAL_PIN_RX_CFG                  (SERIAL_PIN_CFG)
  #define SERIAL_PIN_TX_CFG                  (SERIAL_PIN_CFG)
#elif defined(BOARD_RA0E1_FPB)
  #define SERIAL_CHANNEL                    (0U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_02_PIN_07)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_02_PIN_08)
  #define SERIAL_PIN_RX_CFG                 ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_UARTA2)
  #define SERIAL_PIN_TX_CFG                 ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_UARTA2_OUT)
#elif defined(BOARD_RA0E2_FPB)
  #define SERIAL_CHANNEL                    (0U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_00)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_01)
  #define SERIAL_PIN_RX_CFG                 ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_UARTA5)
  #define SERIAL_PIN_TX_CFG                 ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_UARTA5_OUT)

#elif defined(BOARD_RA8P1_EK)
  #define SERIAL_CHANNEL                    (8U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_13_PIN_03)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_13_PIN_02)
  #define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
                                            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                            (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8)
  #define SERIAL_PIN_RX_CFG                  (SERIAL_PIN_CFG)
  #define SERIAL_PIN_TX_CFG                  (SERIAL_PIN_CFG)

#elif defined(BOARD_RA0L1_FPB)
  #define SERIAL_CHANNEL                    (1U)
  #define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_05)
  #define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_06)
  #define SERIAL_PIN_RX_CFG                 ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_UARTA4)
  #define SERIAL_PIN_TX_CFG                 ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                             (uint32_t) IOPORT_PERIPHERAL_UARTA4_OUT)

#endif

/* Macros for the UART baud rate configuration */
#define SERIAL_BAUD_RATE                    (115200U)
#define SERIAL_ERR_X1000                    (4000U)
#define SERIAL_MODULATION                   (0U)

/* Macros for time-out operation */
#define SAFETY_FACTOR                       (2U)
#define SERIAL_BIT_FRAME                    (10U)
#define SECOND_TO_MICROSECOND               (1000000U)
#define SERIAL_TIME_US                      (SECOND_TO_MICROSECOND / (SERIAL_BAUD_RATE / SERIAL_BIT_FRAME))

/* Macros for the UART software configuration */
#define SERIAL_TX_MAX_SIZE                  (2048U)
#define SERIAL_RX_MAX_SIZE                  (64U)

/* Macros for special character definitions */
#define SERIAL_CHAR_BS                      (0x08)
#define SERIAL_CHAR_LF                      (0x0A)
#define SERIAL_CHAR_CR                      (0x0D)

/* Macros for special values */
#define SERIAL_TRUE                         (1U)
#define SERIAL_FALSE                        (0U)
#define SERIAL_DATA_ZERO                    (0x00)

/* Macros for setting interrupt */
#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI0_RXI)
  #elif defined (VECTOR_NUMBER_UARTA0_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_UARTA0_RXI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI1_RXI)
  #elif defined (VECTOR_NUMBER_UARTA1_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_UARTA1_RXI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI2_RXI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI3_RXI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI4_RXI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI5_RXI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI6_RXI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI7_RXI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI8_RXI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_SCI9_RXI)
  #endif
#endif

#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI0_TXI)
  #elif defined (VECTOR_NUMBER_UARTA0_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_UARTA0_TXI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI1_TXI)
  #elif defined (VECTOR_NUMBER_UARTA1_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_UARTA1_TXI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI2_TXI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI3_TXI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI4_TXI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI5_TXI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI6_TXI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI7_TXI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI8_TXI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_SCI9_TXI)
  #endif
#endif

#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI0_TEI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI1_TEI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI2_TEI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI3_TEI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI4_TEI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI5_TEI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI6_TEI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI7_TEI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI8_TEI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_SCI9_TEI)
  #endif
#endif

#if SERIAL_CHANNEL == 0
  #if defined(VECTOR_NUMBER_SCI0_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI0_ERI)
  #elif defined (VECTOR_NUMBER_UARTA0_ERRI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_UARTA0_ERRI)
  #endif
#elif SERIAL_CHANNEL == 1
  #if defined(VECTOR_NUMBER_SCI1_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI1_ERI)
  #elif defined (VECTOR_NUMBER_UARTA1_ERRI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_UARTA1_ERRI)
  #endif
#elif SERIAL_CHANNEL == 2
  #if defined(VECTOR_NUMBER_SCI2_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI2_ERI)
  #endif
#elif SERIAL_CHANNEL == 3
  #if defined(VECTOR_NUMBER_SCI3_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI3_ERI)
  #endif
#elif SERIAL_CHANNEL == 4
  #if defined(VECTOR_NUMBER_SCI4_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI4_ERI)
  #endif
#elif SERIAL_CHANNEL == 5
  #if defined(VECTOR_NUMBER_SCI5_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI5_ERI)
  #endif
#elif SERIAL_CHANNEL == 6
  #if defined(VECTOR_NUMBER_SCI6_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI6_ERI)
  #endif
#elif SERIAL_CHANNEL == 7
  #if defined(VECTOR_NUMBER_SCI7_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI7_ERI)
  #endif
#elif SERIAL_CHANNEL == 8
  #if defined(VECTOR_NUMBER_SCI8_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI8_ERI)
  #endif
#elif SERIAL_CHANNEL == 9
  #if defined(VECTOR_NUMBER_SCI9_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_SCI9_ERI)
  #endif
#endif

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
uint32_t serial_init(void);
void serial_deinit(void);
uint32_t serial_printf(char * p_format, ...);
uint32_t serial_read(void * const p_buffer, uint32_t buffer_size);
uint32_t serial_has_data(void);
uint32_t serial_has_key(void);

#endif /* USE_VIRTUAL_COM */
#endif /* SERIAL_H_ */
