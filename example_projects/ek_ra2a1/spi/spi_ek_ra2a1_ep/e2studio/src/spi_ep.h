/***********************************************************************************************************************
 * File Name    : spi_ep.h
 * Description  : Contains declarations of data structures and functions used in spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

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
#define BUFF_LEN 32

/* Max wait count for time-out operation */
#define MAX_COUNT 0xFFFFFF

/* Min wait count for time-out operation */
#define MIN_COUNT (0)

/* MACRO for checking if two buffers are equal */
#define BUFF_EQUAL (0)

/* MACRO for null character */
#define NULL_CHAR   '\0'

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
