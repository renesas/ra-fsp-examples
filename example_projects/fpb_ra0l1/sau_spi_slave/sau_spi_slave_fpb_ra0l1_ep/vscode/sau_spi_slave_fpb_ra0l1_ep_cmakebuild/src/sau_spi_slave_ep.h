/***********************************************************************************************************************
 * File Name    : sau_spi_slave_ep.h
 * Description  : Contains data structures and functions used in sau_spi_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_SPI_SLAVE_EP_H_
#define SAU_SPI_SLAVE_EP_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* The commands used in this EP are derived from the purpose of sau_spi_master_ep
 * communicating with the PMOD MAX31723PMB1
 * CFG_WRITE_CMD: Write to the configuration register.
 * CFG_READ_CMD: Read from the configuration register.
 * DATA_READ_CMD: Read from the temperature register.
 * CFG_VALUE_EXPECT: The value is set to configuration register. */
#define CFG_WRITE_CMD               (0x80)
#define CFG_READ_CMD                (0x00)
#define DATA_READ_CMD               (0x01)
#define CFG_VALUE_EXPECT            (0x06)

/* Macro for SPI data frame */
#define FRAME_DATA_SIZE             (3U)

/* Max wait count for time-out operation */
#define MAX_COUNT                   (180000000UL)

/* Macros for manipulating the temperature values */
#ifdef BOARD_RA0E3_FPB
#define MCU_TEMP_TO_C(a)            (((((float)(a)) * (-0.977517f)) + 343.182f))
#else
#define MCU_TEMP_TO_C(a)            (((((float)(a)) * (-0.220784f)) + 312.671f))
#endif /* BOARD_RA0E3_FPB */
#define DEGREE_SYMBOL               ("\xB0")
#define FLOAT_DIVISOR               (16.0F)
#define SHIFT_FOUR_BITS             (4U)
#define INTEGER_PART                (2U)
#define FRACTIONAL_PART             (1U)

/* Macros for the used SAU SPI channel */
#ifdef BOARD_RA0E3_FPB
#define DETECT_SS_SIGNAL_CHANNEL    (2U)
#define SAU_SPI_CHANNEL             (3U)
#else
#define DETECT_SS_SIGNAL_CHANNEL    (5U)
#define SAU_SPI_CHANNEL             (0U)
#endif /* BOARD_RA0E3_FPB */

/* Enumeration for user-visible events produced by sau_spi_slave_communicate() */
typedef enum e_sau_spi_slave_event
{
    SAU_SPI_SLAVE_EVENT_NONE = 0,           /* No user-visible event this cycle */
    SAU_SPI_SLAVE_EVENT_CFG_RECEIVED,       /* CFG_WRITE_CMD received as expected */
    SAU_SPI_SLAVE_EVENT_READ_CFG_RECEIVED,  /* CFG_READ_CMD received, ready to transmit temperature */
    SAU_SPI_SLAVE_EVENT_TEMP_SENT,          /* Temperature transmitted */
} sau_spi_slave_event_t;

/* External variable - Defined in sau_spi_slave_ep.c */
extern volatile bool g_active_transfer;

/* Function declarations */
fsp_err_t sau_spi_slave_init(void);
fsp_err_t sau_spi_slave_communicate(sau_spi_slave_event_t * p_event, char * p_temp_str, uint32_t temp_str_size);
void sau_spi_slave_deinit(void);
fsp_err_t active_transfer_init(void);
void active_transfer_deinit(void);

#endif /* SAU_SPI_SLAVE_EP_H_ */
