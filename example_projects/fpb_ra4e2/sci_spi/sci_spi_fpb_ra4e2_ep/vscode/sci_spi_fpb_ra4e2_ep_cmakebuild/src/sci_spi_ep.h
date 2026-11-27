/***********************************************************************************************************************
 * File Name    : sci_spi_ep.h
 * Description  : Contains declarations of data structures and functions used in sci_spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_SPI_EP_H_
#define SCI_SPI_EP_H_

#include "hal_data.h"

/* Slave Select pin for the specific board to enable data transfer */
#if defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA4M1_EK) || defined (BOARD_RA6T1_RSSK) ||\
    defined (BOARD_RA2L1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2E2_EK) || defined (BOARD_RA2E3_FPB)  ||\
    defined (BOARD_RA4E1_FPB) || defined (BOARD_RA2L2_EK) || defined (BOARD_RA2T1_FPB)
#define CS_PIN  (BSP_IO_PORT_01_PIN_03)
#elif defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define CS_PIN  (BSP_IO_PORT_02_PIN_05)
#elif defined (BOARD_RA2A1_EK)
#define CS_PIN  (BSP_IO_PORT_04_PIN_10)
#elif defined (BOARD_RA2E1_EK)
#define CS_PIN  (BSP_IO_PORT_04_PIN_03)
#elif defined (BOARD_RA4E2_EK) || defined (BOARD_RA6E2_EK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA4T1_MCK) ||\
      defined (BOARD_RA4E2_FPB)
#define CS_PIN  (BSP_IO_PORT_03_PIN_01)
#elif defined (BOARD_RA6T2_MCK)
#define CS_PIN  (BSP_IO_PORT_10_PIN_12)
#elif defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define CS_PIN  (BSP_IO_PORT_10_PIN_05)
#elif defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
#define CS_PIN  (BSP_IO_PORT_06_PIN_04)
#elif defined (BOARD_RA2A2_EK)
#define CS_PIN  (BSP_IO_PORT_05_PIN_03)
#elif defined (BOARD_RA8E1_FPB)
#define CS_PIN  (BSP_IO_PORT_08_PIN_04)
#elif defined (BOARD_RA4L1_EK) || defined (BOARD_RA4C1_EK)
#define CS_PIN  (BSP_IO_PORT_01_PIN_12)
#elif defined (BOARD_RA8E2_EK)
#define CS_PIN  (BSP_IO_PORT_03_PIN_14)
#elif defined (BOARD_RA8T2_MCK)
#define CS_PIN  (BSP_IO_PORT_07_PIN_10)
#else
#define CS_PIN  (BSP_IO_PORT_04_PIN_13)
#endif

#if (BSP_PERIPHERAL_SCI_B_PRESENT)
#define MODULE_NAME                                         "r_sci_b_spi"
#define MODULE_PADDING                                      "  "
#define R_SCI_SPI_OPEN(ctrl, cfg)                           (R_SCI_B_SPI_Open((ctrl), (cfg)))
#define R_SCI_SPI_WRITE(ctrl, src, len, width)              (R_SCI_B_SPI_Write((ctrl), (src), (len), (width)))
#define R_SCI_SPI_READ(ctrl, dest, len, width)              (R_SCI_B_SPI_Read((ctrl), (dest), (len), (width)))
#define R_SCI_SPI_WRITE_READ(ctrl, src, dest, len, width)   (R_SCI_B_SPI_WriteRead((ctrl), (src), (dest), (len),\
                                                                                   (width)))
#define R_SCI_SPI_CLOSE(ctrl)                               (R_SCI_B_SPI_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_PRESENT)
#define MODULE_NAME                                         "r_sci_spi"
#define MODULE_PADDING                                      "    "
#define R_SCI_SPI_OPEN(ctrl, cfg)                           (R_SCI_SPI_Open((ctrl), (cfg)))
#define R_SCI_SPI_WRITE(ctrl, src, len, width)              (R_SCI_SPI_Write((ctrl), (src), (len), (width)))
#define R_SCI_SPI_READ(ctrl, dest, len, width)              (R_SCI_SPI_Read((ctrl), (dest), (len), (width)))
#define R_SCI_SPI_WRITE_READ(ctrl, src, dest, len, width)   (R_SCI_SPI_WriteRead((ctrl), (src), (dest), (len),\
                                                                                 (width)))
#define R_SCI_SPI_CLOSE(ctrl)                               (R_SCI_SPI_Close(ctrl))
#endif /* BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT */

/* Macro for reset value */
#define RESET_VALUE             (0x00)

/* Macro for conversion time for 12-bit resolution */
#define CONVERSION_TIME         (200U)

/* Macro for the terminal print delay */
#define PRINT_DELAY             (2U)

/* Master transfer event completion flag */
extern volatile spi_event_t g_master_event_flag;

/* Function declarations */
fsp_err_t sci_spi_init(void);
fsp_err_t sci_spi_write(spi_ctrl_t * const p_api_ctrl, void const * p_src, uint32_t const length,
                        spi_bit_width_t const bit_width);
fsp_err_t sci_spi_read(spi_ctrl_t * const p_api_ctrl, void * p_dest, uint32_t const length,
                       spi_bit_width_t const bit_width);
fsp_err_t sci_spi_write_read(spi_ctrl_t * const p_api_ctrl, void const * p_src, void * p_dest, uint32_t const length,
                             spi_bit_width_t const bit_width);
fsp_err_t cs_assert(void);
fsp_err_t cs_de_assert(void);
void sci_spi_deinit(void);

#endif /* SCI_SPI_EP_H_ */
