/***********************************************************************************************************************
 * File Name    : sau_spi_master_ep.h
 * Description  : Contains macros and function declarations used in sau_spi_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_SPI_MASTER_EP_H_
#define SAU_SPI_MASTER_EP_H_

#include "hal_data.h"

/* Macros for PMOD MAX31723PMB1 register addresses */
#define CFG_WRITE_ADDR          (0x80)
#define CFG_READ_ADDR           (0x00)
#define DATA_READ_ADDR          (0x01)

/* Macros for sensor configuration */
#define SENSOR_RES_CFG          (0x06)
#define DATA_FRAME_SIZE         (3U)

/* Macros for SPI transfer */
#define SPI_TIMEOUT_UNIT        (BSP_DELAY_UNITS_MILLISECONDS)
#define SPI_TIMEOUT_DIV         (1U)

/* Macros for delay timer */
#define CONVERSION_TIME         (200U)
#define CONVERSION_TIME_UNIT    (BSP_DELAY_UNITS_MILLISECONDS)
#define PRINT_DELAY             (2U)

/* Macros for temperature calculation */
#define INTEGER_PART            (2U)
#define FRACTIONAL_PART         (1U)
#define FLOAT_DIVISOR           (16.0F)
#define SHIFT_FOUR_BITS         (4U)
#define DEGREE_SYMBOL           "\xB0"
#define BUFF_SIZE               (30U)

/* Macro for slave select pin */
#if defined(BOARD_RA0E1_FPB) || defined(BOARD_RA0L1_FPB)
#define SS_PIN                  (BSP_IO_PORT_01_PIN_03)
#elif defined(BOARD_RA0E2_FPB)
#define SS_PIN                  (BSP_IO_PORT_02_PIN_07)
#elif defined(BOARD_RA0E3_FPB)
#define SS_PIN                  (BSP_IO_PORT_01_PIN_12)
#endif /* SS_PIN */

#define SS_DE_ASSERT(x)         (R_IOPORT_PinWrite(&g_ioport_ctrl, (x), BSP_IO_LEVEL_LOW))
#define SS_ASSERT(x)            (R_IOPORT_PinWrite(&g_ioport_ctrl, (x), BSP_IO_LEVEL_HIGH))

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Extern variable - defined in sau_spi_master_ep.c */
extern volatile spi_event_t g_master_event_flag;

/* Function declarations */
fsp_err_t sau_spi_master_init(void);
void sau_spi_master_deinit(void);
fsp_err_t wait_sau_spi_transfer_complete(void);
fsp_err_t sau_spi_write(void const *p_src, uint32_t length);
fsp_err_t sau_spi_write_read(void const *p_src, void *p_dest, uint32_t length);
fsp_err_t sensor_config(void);

#endif /* SAU_SPI_MASTER_EP_H_ */
