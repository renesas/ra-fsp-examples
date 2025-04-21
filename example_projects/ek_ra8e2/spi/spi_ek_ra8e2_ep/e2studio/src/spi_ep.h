/***********************************************************************************************************************
 * File Name    : spi_ep.h
 * Description  : Contains declarations of data structures and functions used in spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SPI_EP_H_
#define SPI_EP_H_

/*******************************************************************************************************************//**
 * @ingroup spi_ep
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros for commands to be received through RTT input */
#define WRITE_AND_READ          (1)
#define WRITE_READ              (2)
#define EXIT                    (3)

/* Macro for miscellaneous */
#define DELAY_ONE_SEC           (1)

/* Array indexing MACRO */
#define ARRAY_INDEX             (0)

/* Macro for checking if no byte is received */
#define BYTES_RECEIVED_ZERO     (0)

/* SPI buffer length */
#define BUFF_LEN                (32)

/* Max wait count for time-out operation */
#define MAX_COUNT               (0xFFFFFF)

/* Min wait count for time-out operation */
#define MIN_COUNT               (0)

/* Macro for checking if two buffers are equal */
#define BUFF_EQUAL              (0)

/* Macro for null character */
#define NULL_CHAR               ('\0')

/* Macro to convert 32 bits into bytes */
#define BITS_TO_BYTES           (4U)

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/* Function declarations */
fsp_err_t spi_init(void);
fsp_err_t spi_write_and_read(void);
fsp_err_t spi_write_read(void);
fsp_err_t spi_exit_demo(void);
void spi_clean_up(void);

/** @} */

#endif /* SPI_EP_H_ */
