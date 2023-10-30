/***********************************************************************************************************************
 * File Name    : spi_ep.h
 * Description  : Contains declarations of data structures and functions used in spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
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
#define WRITE_AND_READ  (1)
#define WRITE_READ      (2)
#define EXIT            (3)

/* Macro for miscellaneous */
#define DELAY_ONE_SEC   (1)

/* Array indexing MACRO */
#define ARRAY_INDEX      (0)

/* MACRO for checking if no byte is received */
#define BYTES_RECEIVED_ZERO  (0)

/* SPI buffer length */
#define BUFF_LEN (32)

/* Max wait count for time-out operation */
#define MAX_COUNT (0xFFFFFF)

/* Min wait count for time-out operation */
#define MIN_COUNT (0)

/* MACRO for checking if two buffers are equal */
#define BUFF_EQUAL (0)

/* MACRO for null character */
#define NULL_CHAR   ('\0')

/* Macro to convert 32 bits into bytes */
#define BITS_TO_BYTES   (4U)

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/*
 * function declarations
 */
fsp_err_t spi_init(void);
fsp_err_t spi_write_and_read(void);
fsp_err_t spi_write_read(void);
fsp_err_t spi_exit_demo(void);
void spi_clean_up(void);


/** @} */



#endif /* SPI_EP_H_ */
