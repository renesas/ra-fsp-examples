/***********************************************************************************************************************
 * File Name    : ospi_b_ep.h
 * Description  : Contains data structures and functions used in ospi_b_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_B_EP_H_
#define OSPI_B_EP_H_

#include "hal_data.h"
#include "ospi_b_commands.h"
#include "gpt_ep.h"

/* Macros for flash device */
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
#define OSPI_B_DEVICE_ID                    (0x0F1A5B34)    /* For S28HS512T flash */
#define OSPI_B_DEVICE_HL_ID                 (0x0F1A5A34)    /* For S28HL512T flash */
#define OSPI_B_MANUFACTURER_ID              (0x34)
#define OSPI_B_DEVICE_ID_TYPE               (0x5B)          /* For S28HS512T flash */
#define OSPI_B_DEVICE_HL_ID_TYPE            (0x5A)          /* For S28HL512T flash */
#define OSPI_B_DEVICE_ID_DENSITY            (0x1A)
#elif defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8M2_EK)
#define OSPI_B_DEVICE_ID                    (0x3A86C2)      /* For MX25LW51245G flash */
#elif defined (BOARD_RA8D2_EK) || defined (BOARD_RA8T2_EK)
#define OSPI_B_DEVICE_ID                    (0x1A5A9D)      /* For IS25LX512M flash */
#endif /* BOARD_RA8D1_EK || BOARD_RA8M1_EK || BOARD_RA8E2_EK || BOARD_RA8P1_EK || BOARD_RA8M2_EK ||
        BOARD_RA8D2_EK || BOARD_RA8T2_EK */

/* Flash device sector size */
#define OSPI_B_SECTOR_SIZE_4K               (0x1000)
#define OSPI_B_SECTOR_SIZE_256K             (0x40000)
#define OSPI_B_SECTOR_4K_END_ADDRESS        (0x9001FFFF)

/* Flash device timing */
#define OSPI_B_TIME_RESET_SETUP             (2U)             /* Type 50ns */
#define OSPI_B_TIME_RESET_PULSE             (1000U)          /* Type 500us */
#define OSPI_B_TIME_ERASE_256K              (1500000U)       /* Type 256KB sector is 331 KBps -> Type 0.773s */
#define OSPI_B_TIME_ERASE_4K                (100000U)        /* Type 4KB sector is 95 KBps -> Type 0.042s */
#define OSPI_B_TIME_WRITE                   (10000U)         /* Type 256B page (4KB/256KB) is 595/533 KBps -> Type */

/* Flash device status bit */
#define OSPI_B_WEN_BIT_MASK                 (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                (0x00000001)

/* Flash device address space mapping */
#if defined (BOARD_RA8T2_EK)
#define OSPI_B_CS0_START_ADDRESS            (0x70000000)
#define OSPI_B_CS1_START_ADDRESS            (0x78000000)
#else
#define OSPI_B_CS0_START_ADDRESS            (0x80000000)
#define OSPI_B_CS1_START_ADDRESS            (0x90000000)
#endif /* BOARD_RA8T2_EK */

#define OSPI_B_APP_ADDRESS(sector_no)       ((uint8_t *)(OSPI_B_CS1_START_ADDRESS +\
                                                ((sector_no) * OSPI_B_SECTOR_SIZE_4K)))
#define OSPI_B_SECTOR_FIRST                 (0U)
#define OSPI_B_SECTOR_SECOND                (1U)
#define OSPI_B_APP_DATA_SIZE                (64U)

/* Macros for user selections */
#define SPI_MODE                            ('1')
#define OPI_MODE                            ('2')
#define WRITE_OPERATION                     ('1')
#define READ_OPERATION                      ('2')
#define ERASE_OPERATION                     ('3')
#define EXIT                                ('4')
#define APP_MAX_MODE                        (2U)
#define APP_SPI_MODE                        "1S-1S-1S Protocol Mode"
#define APP_OPI_MODE                        "8D-8D-8D Protocol Mode"

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
#define SPI_CLK_SRC                         (BSP_CLOCKS_SOURCE_CLOCK_PLL2P)
#define SPI_CLK_DIV                         (BSP_CLOCKS_OCTA_CLOCK_DIV_4)
#define OPI_CLK_DIV                         (BSP_CLOCKS_OCTA_CLOCK_DIV_2)
#define SPI_REGISTER                        (OSPI_B_DATA_SET_SPI_CFR5V_REGISTER)
#define OPI_REGISTER                        (OSPI_B_DATA_SET_OPI_CFR5V_REGISTER)
#define OPI_WRITE                           (OSPI_B_TRANSFER_WRITE_CFR5V_OPI)
#define SPI_READ                            (OSPI_B_TRANSFER_READ_CFR5V_SPI)
#define OPI_READ                            (OSPI_B_TRANSFER_READ_CFR5V_OPI)
#define SPI_WRITE                           (OSPI_B_TRANSFER_WRITE_CFR5V_SPI)
#elif defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8M2_EK)
#define SPI_CLK_SRC                         (BSP_CLOCKS_SOURCE_CLOCK_PLL2P)
#define SPI_CLK_DIV                         (BSP_CLOCKS_OCTA_CLOCK_DIV_3)
#define OPI_CLK_DIV                         (BSP_CLOCKS_OCTA_CLOCK_DIV_2)
#define SPI_REGISTER                        (OSPI_B_DATA_SET_SPI_CR2_000H)
#define OPI_REGISTER                        (OSPI_B_DATA_SET_OPI_CR2_000H)
#define OPI_WRITE                           (OSPI_B_TRANSFER_WRITE_CR2_000H_OPI)
#define OPI_READ                            (OSPI_B_TRANSFER_READ_CR2_000H_OPI)
#define SPI_READ                            (OSPI_B_TRANSFER_READ_CR2_000H_SPI)
#define SPI_WRITE                           (OSPI_B_TRANSFER_WRITE_CR2_000H_SPI)
#elif defined (BOARD_RA8D2_EK) || defined (BOARD_RA8T2_EK)
#define SPI_CLK_SRC                         (BSP_CLOCKS_SOURCE_CLOCK_PLL2Q)
#define SPI_CLK_DIV                         (BSP_CLOCKS_OCTA_CLOCK_DIV_1)
#define OPI_CLK_DIV                         (BSP_CLOCKS_OCTA_CLOCK_DIV_1)
#define SPI_REGISTER                        (OSPI_B_DATA_SET_SPI_CR_00H)
#define OPI_REGISTER                        (OSPI_B_DATA_SET_OPI_CR_00H)
#define OPI_WRITE                           (OSPI_B_TRANSFER_WRITE_CR_00H_OPI)
#define OPI_READ                            (OSPI_B_TRANSFER_READ_CR_00H_OPI)
#define SPI_READ                            (OSPI_B_TRANSFER_READ_CR_00H_SPI)
#define SPI_WRITE                           (OSPI_B_TRANSFER_WRITE_CR_00H_SPI)
#endif /* BOARD_RA8D1_EK || BOARD_RA8M1_EK || BOARD_RA8E2_EK || BOARD_RA8P1_EK || BOARD_RA8M2_EK ||
        BOARD_RA8D2_EK || BOARD_RA8T2_EK */

/* External variables - Defined in ospi_b_ep.c */
extern spi_flash_direct_transfer_t g_transfer;
extern uint8_t g_read_data [OSPI_B_APP_DATA_SIZE];
extern const uint8_t g_write_data [OSPI_B_APP_DATA_SIZE];

/* Function declarations */
fsp_err_t ospi_b_init(void);
fsp_err_t ospi_b_transfer(spi_flash_direct_transfer_t * const p_transfer, spi_flash_direct_transfer_dir_t direction);
fsp_err_t ospi_b_spi_protocol(spi_flash_protocol_t spi_protocol);
fsp_err_t ospi_b_write_enable(void);
fsp_err_t ospi_b_write_operation(uint8_t * const p_address, uint32_t * const p_time);
fsp_err_t ospi_b_read_operation(uint8_t * const p_address, uint32_t * const p_time);
fsp_err_t ospi_b_erase_operation(uint8_t * const p_address, uint32_t * const p_time);
fsp_err_t ospi_b_read_device_id(uint32_t * const p_id);
void ospi_b_deinit(void);

#endif /* OSPI_B_EP_H_ */
