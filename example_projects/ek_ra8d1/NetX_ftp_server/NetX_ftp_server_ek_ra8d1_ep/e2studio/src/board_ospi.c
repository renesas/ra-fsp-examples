/***********************************************************************************************************************
 * File Name    : board_ospi.c
 * Description  : Contains macros data structures and functions used in board_ospi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "board_ospi.h"

/* External variables */
spi_flash_direct_transfer_t g_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX] =
{
 /* Transfer structure for SPI mode */
    [OSPI_B_TRANSFER_WRITE_ENABLE_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_ENABLE_SPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_ZERO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR2V_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
    .data           = OSPI_B_DATA_CFR2V_REGISTER,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR3V_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
    .data           = OSPI_B_DATA_CFR3V_REGISTER,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_READ_STATUS_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_STATUS_SPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI
    },
    [OSPI_B_TRANSFER_READ_CFR2V_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_CFR3V_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },

    /* Transfer structure for OPI mode */
    [OSPI_B_TRANSFER_WRITE_ENABLE_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_ENABLE_OPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_ZERO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_READ_STATUS_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_STATUS_OPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       This functions enables write and verify the read data.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful operation
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable (void)
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        = {RESET_VALUE};

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
            ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
            : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_DirectTransfer API FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
            ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
            : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_DirectTransfer API FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Check Write Enable bit in Status Register */
    if(OSPI_B_WEN_BIT_MASK != (transfer.data & OSPI_B_WEN_BIT_MASK))
    {
        APP_PRINT("Write enable FAILED\r\n");
        APP_ERR_TRAP(err);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes OSPI module and Flash device.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t ospi_b_init (void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode */
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        = {RESET_VALUE};

    /* Switch OSPI module to 1S-1S-1S mode to configure flash device */
    err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_SpiProtocolSet API FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Reset flash device by driving OM_RESET pin */
    R_XSPI->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, OSPI_B_TIME_UNIT);
    R_XSPI->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, OSPI_B_TIME_UNIT);

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("ospi_b_write_enable FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Write to CFR2V to configure Address Byte Length and Memory Array Read Latency */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR2V_SPI];
    transfer.address_length = OSPI_B_ADDRESS_LENGTH_THREE;  // default Address Byte Length is 3 bytes
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_DirectTransfer API FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("ospi_b_write_enable FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Write to CFR3V to configure Volatile Register Read Latency */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR3V_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_DirectTransfer API FAILED \r\n");
        APP_ERR_TRAP(err);
    }
    /* Read back and verify CFR2V register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CFR2V_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_DirectTransfer API FAILED \r\n");
        APP_ERR_TRAP(err);
    }
    if(OSPI_B_DATA_CFR2V_REGISTER != (uint8_t)transfer.data)
    {
        APP_PRINT("Verify CFR2V register data FAILED\r\n");
        APP_ERR_TRAP(err);
    }

    /* Read back and verify CFR3V register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CFR3V_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_PRINT("R_OSPI_B_DirectTransfer API FAILED \r\n");
        APP_ERR_TRAP(err);
    }
    if(OSPI_B_DATA_CFR3V_REGISTER != (uint8_t)transfer.data)
    {
        APP_PRINT("Verify CFR3V register data FAILED\r\n");
        APP_ERR_TRAP(err);
    }

    return err;
}
