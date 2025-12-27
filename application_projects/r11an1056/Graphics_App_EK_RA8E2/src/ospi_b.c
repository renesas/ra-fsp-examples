/***********************************************************************************************************************
 * File Name    : ospi_b.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ospi_b.h"

extern spi_flash_direct_transfer_t g_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX];

/*******************************************************************************************************************//**
 * @brief       This functions enables write and verify the read data.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful operation
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_b_write_enable (void)
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data           = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
            ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
            : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_TRAP(err);

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
            ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
            : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_TRAP(err);

    /* Check Write Enable bit in Status Register */
    if(OSPI_B_WEN_BIT_MASK != (transfer.data & OSPI_B_WEN_BIT_MASK))
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
    return err;
}

fsp_err_t ospi_b_init(void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode */
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data           = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Open OSPI module */
    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    /* Configure DDR sampling window extend */
    g_ospi_b_ctrl.p_reg->LIOCFGCS_b[1].DDRSMPEX = 0x7;

    /* Switch OSPI module to 1S-1S-1S mode to configure flash device */
    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    /* Reset flash device by driving OM_RESET pin */
    g_ospi_b_ctrl.p_reg->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, OSPI_B_TIME_UNIT);
    g_ospi_b_ctrl.p_reg->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, OSPI_B_TIME_UNIT);

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
    /* Write to ADDR 00000300H of CR2 to configure dummy cycle */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR2_300H_SPI];
    transfer.data    = OSPI_B_DATA_SET_CR2_300H;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    /* Read back and verify CR2 register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR2_300H_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    if(OSPI_B_DATA_SET_CR2_300H != (uint8_t)transfer.data)
    {
        if(FSP_SUCCESS != err)
        {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }
    }

    return err;
}

