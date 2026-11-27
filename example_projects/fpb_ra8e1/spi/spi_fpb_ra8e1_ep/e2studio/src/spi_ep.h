/***********************************************************************************************************************
 * File Name    : spi_ep.h
 * Description  : Contains declarations of data structures and functions used in spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SPI_EP_H_
#define SPI_EP_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                                     (0x00)

/* Macros for the command to be received through the terminal */
#define WRITE_AND_READ                                  (1U)
#define WRITE_READ                                      (2U)
#define EXIT                                            (3U)

/* Macro for the buffer size */
#define BUFFER_SIZE                                     (16U)

/* Maximum wait count for time-out operation */
#define MAX_COUNT                                       (0xFFFFFF)

/* Minimum wait count for time-out operation */
#define MIN_COUNT                                       (0U)

/* Macro for checking if two buffers are equal */
#define BUFF_EQUAL                                      (0U)

/* Macro for null character */
#define NULL_CHAR                                       ('\0')

/* Macro to convert 32 bits into bytes */
#define BITS_TO_BYTES                                   (4U)

/* Macros for used functions based on specific SPI version */
#if (BSP_PERIPHERAL_SPI_B_PRESENT)
#define MODULE_NAME                                     "r_spi_b"
#define MODULE_PADDING                                  "             "
#define SPI_OPEN(ctrl, cfg)                             (R_SPI_B_Open((ctrl), (cfg)))
#define SPI_READ(ctrl, dest, len, width)                (R_SPI_B_Read((ctrl), (dest), (len), (width)))
#define SPI_WRITE(ctrl, src, len, width)                (R_SPI_B_Write((ctrl), (src), (len), (width)))
#define SPI_WRITE_READ(ctrl, src, dest, len, width)     (R_SPI_B_WriteRead((ctrl), (src), (dest), (len), (width)))
#define SPI_CLOSE(ctrl)                                 (R_SPI_B_Close(ctrl))
#elif (BSP_PERIPHERAL_SPI_PRESENT)
#define MODULE_NAME                                     "r_spi"
#define MODULE_PADDING                                  "               "
#define SPI_OPEN(ctrl, cfg)                             (R_SPI_Open((ctrl), (cfg)))
#define SPI_READ(ctrl, dest, len, width)                (R_SPI_Read((ctrl), (dest), (len), (width)))
#define SPI_WRITE(ctrl, src, len, width)                (R_SPI_Write((ctrl), (src), (len), (width)))
#define SPI_WRITE_READ(ctrl, src, dest, len, width)     (R_SPI_WriteRead((ctrl), (src), (dest), (len), (width)))
#define SPI_CLOSE(ctrl)                                 (R_SPI_Close(ctrl))
#endif /* SPI version */

/* Extern variables - Defined in spi_ep.c */
extern volatile spi_event_t g_master_event_flag;
extern volatile spi_event_t g_slave_event_flag;

/* Function declarations */
fsp_err_t spi_init(void);
fsp_err_t spi_write(spi_ctrl_t * const p_api_ctrl, void const * p_src, uint32_t const length,
                    spi_bit_width_t const bit_width);
fsp_err_t spi_read(spi_ctrl_t * const p_api_ctrl, void * p_dest, uint32_t const length,
                   spi_bit_width_t const bit_width);
fsp_err_t spi_write_read(spi_ctrl_t * const p_api_ctrl, void const * p_src, void * p_dest,
                         uint32_t const length, spi_bit_width_t const bit_width);
void spi_deinit(void);

#endif /* SPI_EP_H_ */
