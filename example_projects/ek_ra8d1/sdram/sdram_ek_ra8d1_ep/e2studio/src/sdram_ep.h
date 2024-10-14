/***********************************************************************************************************************
 * File Name    : sdram_ep.h
 * Description  : Contains data structures and functions used in sdram_ep.c
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#ifndef SDRAM_EP_H_
#define SDRAM_EP_H_


#include "common_utils.h"


#define EP_INFO             "\r\nThis project demonstrates the basic functionalities of SDRAM on Renesas RA MCUs based"\
                            "\r\non Renesas FSP. The example project writes data to the entire SDRAM using DMAC. It"\
                            "\r\ndemonstrates SDRAM settings and data retention by enabling self-refresh in low power"\
                            "\r\nmode. When the Ultra-Low-Power Timer (ULPT) expires, the MCU returns to normal mode"\
                            "\r\nand reads the SDRAM to ensure the data is still retained. The EP status information"\
                            "\r\nand error messages are displayed on the host PC. Additionally, the error LED will"\
                            "\r\nturn on if an error occurs.\r\n\r\n"

/* Macros for text color */
#define CTRL_RESET                "\x1B[0m"
#define CTRL_TEXT_BRIGHT_GREEN    "\x1B[1;32m"

/* Macros for User LED */
#define LED_ERROR_STATE           (2U)
#define LED_LPM_STATE             (0U)
#define LED_LPM_LIGHT_TIME        (1)


/* Macros for SDRAM transfer size */
#define BLOCK_DATA_SIZE           (4096u)
#define DATA_SIZE                 (4U)
#define NUM_ADDR_FOR_BLOCK        (BLOCK_DATA_SIZE * DATA_SIZE)

/* Macros for SDRAM memory */
#define SDRAM_START_ADDR          (0x68000000UL)
#define SDRAM_SIZE                (0x04000000UL)

#define BUFF_SIZE                 (30U)
#define PRINT_DELAY               (100)


/* Enumeration for led state use in application */
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
}led_power_t;

/* Enumeration for SDRAM transmission direction */
typedef enum e_sdram_trans_dir
{
    WRITE = 0,
    READ,
}sdram_trans_dir_t;

/* Public function declarations */
void sdram_entry (void);

#endif /* SDRAM_EP_H_ */
