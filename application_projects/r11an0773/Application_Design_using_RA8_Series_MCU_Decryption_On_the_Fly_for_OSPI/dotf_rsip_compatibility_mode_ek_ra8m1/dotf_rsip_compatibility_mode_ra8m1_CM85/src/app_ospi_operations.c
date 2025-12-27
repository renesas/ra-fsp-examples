/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "app_common_util.h"
#include "app_ospi_operations.h"
#include "app_runtime_encryption.h"

static const spi_flash_direct_transfer_t g_ospi_b_direct_transfer[OSPI_B_MODE_MAX][OSPI_B_TRANSFER_MAX] =
{
     [OSPI_B_MODE_SPI] =
     {
        [OSPI_B_TRANSFER_WRITE_ENABLE] =
        {
             .command        = OSPI_B_COMMAND_WRITE_ENABLE_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_DUMMY,
             .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_ZERO
        },

        [OSPI_B_TRANSFER_WRITE_CFR2V] =
        {
             .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
             .data           = OSPI_B_DATA_CFR2V_REGISTER,
             .data_length    = OSPI_B_DATA_LENGTH_ONE,
        },

        [OSPI_B_TRANSFER_WRITE_CFR3V] =
        {
             .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
             .data           = OSPI_B_DATA_CFR3V_REGISTER,
             .data_length    = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_WRITE_CFR5V] =
        {
             .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI,
             .data           = OSPI_B_DATA_SET_OPI_CFR5V_REGISTER,
             .data_length    = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_STATUS] =
        {
             .command        = OSPI_B_COMMAND_READ_STATUS_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_DUMMY,
             .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_CFR2V] =
        {
             .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_CFR3V] =
        {
             .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_ONE
        },

        [OSPI_B_TRANSFER_READ_CFR5V] =
        {
             .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
             .command_length = OSPI_B_COMMAND_LENGTH_SPI,
             .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_ONE
        },
     },

     [OSPI_B_MODE_OPI] =
     {
        [OSPI_B_TRANSFER_WRITE_ENABLE] =
        {
            .command        = OSPI_B_COMMAND_WRITE_ENABLE_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address        = OSPI_B_ADDRESS_DUMMY,
            .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
            .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI,
            .data           = OSPI_B_DATA_DUMMY,
            .data_length    = OSPI_B_DATA_LENGTH_ZERO
        },

        [OSPI_B_TRANSFER_WRITE_CFR2V] =
        {
            .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address        = OSPI_B_ADDRESS_CFR2V_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI,
            .data           = OSPI_B_DATA_CFR2V_REGISTER,
            .data_length    = OSPI_B_DATA_LENGTH_TWO
        },

        [OSPI_B_TRANSFER_WRITE_CFR3V] =
        {
             .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
             .command_length = OSPI_B_COMMAND_LENGTH_OPI,
             .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI,
             .data           = OSPI_B_DATA_CFR3V_REGISTER,
             .data_length    = OSPI_B_DATA_LENGTH_TWO
        },

        [OSPI_B_TRANSFER_WRITE_CFR5V] =
        {
            .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI,
            .data           = OSPI_B_DATA_DUMMY,
            .data_length    = OSPI_B_DATA_LENGTH_TWO
        },

        [OSPI_B_TRANSFER_READ_STATUS] =
        {
             .command        = OSPI_B_COMMAND_READ_STATUS_OPI,
             .command_length = OSPI_B_COMMAND_LENGTH_OPI,
             .address        = OSPI_B_ADDRESS_DUMMY,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_TWO
        },

        [OSPI_B_TRANSFER_READ_CFR2V] =
        {
             .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
             .command_length = OSPI_B_COMMAND_LENGTH_OPI,
             .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_TWO
        },

        [OSPI_B_TRANSFER_READ_CFR3V] =
        {
            .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
            .command_length = OSPI_B_COMMAND_LENGTH_OPI,
            .address        = OSPI_B_ADDRESS_CFR3V_REGISTER,
            .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
            .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI,
            .data           = OSPI_B_DATA_DUMMY,
            .data_length    = OSPI_B_DATA_LENGTH_TWO
        },

        [OSPI_B_TRANSFER_READ_CFR5V] =
        {
             .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
             .command_length = OSPI_B_COMMAND_LENGTH_OPI,
             .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
             .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
             .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI,
             .data           = OSPI_B_DATA_DUMMY,
             .data_length    = OSPI_B_DATA_LENGTH_TWO
        }
     },
};

static fsp_err_t ospi_b_write_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address        = RESET_VALUE,
        .address_length = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE,
        .data           = RESET_VALUE,
        .data_length    = RESET_VALUE,
    };

    /* Transfer write enable command */
    if (g_ospi_b_ctrl.spi_protocol == SPI_FLASH_PROTOCOL_EXTENDED_SPI)
    {
        transfer =  g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_WRITE_ENABLE];
    }
    else
    {
        transfer =  g_ospi_b_direct_transfer[OSPI_B_MODE_OPI][OSPI_B_TRANSFER_WRITE_ENABLE];
    }

    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    /* Read Status Register */
    if (g_ospi_b_ctrl.spi_protocol == SPI_FLASH_PROTOCOL_EXTENDED_SPI)
    {
        transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_READ_STATUS];
    }
    else
    {
        transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_OPI][OSPI_B_TRANSFER_READ_STATUS];
    }

    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    if ((transfer.data & OSPI_B_WEL_BIT_MASK) != OSPI_B_WEL_BIT_MASK)
    {
        app_handle_error(APP_ERR_COMMON, APP_OSPI_WRITE_ENABLE);
        OSPI_B_ERR_RETURN(FSP_ERR_WRITE_FAILED, return FSP_ERR_WRITE_FAILED);
    }

    return err;
}

static fsp_err_t ospi_b_wait_operation(uint32_t timeout)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        err = R_OSPI_B_StatusGet(&g_ospi_b_ctrl, &status);
        app_handle_error(err, APP_OSPI_STATUSGET);
        OSPI_B_ERR_RETURN(err, return err);

        if (RESET_VALUE == timeout)
        {
            app_handle_error(APP_ERR_COMMON, APP_OSPI_OPERATION_TIMEOUT);
            OSPI_B_ERR_RETURN(FSP_ERR_TIMEOUT, return FSP_ERR_TIMEOUT);
        }

        R_BSP_SoftwareDelay(1, OSPI_B_TIME_UNIT);
        timeout--;
    }

    return err;
}

static fsp_err_t ospi_b_write_page(uint8_t * const p_src, uint8_t * const p_dest, uint32_t len)
{
    OSPI_B_CHECK_IF_NULL(p_src, return FSP_ERR_INVALID_ARGUMENT);
    OSPI_B_CHECK_IF_NULL(p_dest, return FSP_ERR_INVALID_ARGUMENT);

    fsp_err_t err = FSP_SUCCESS;

    uint32_t page_size = g_ospi_b_ctrl.p_cfg->page_size_bytes;

    uint32_t pages_to_write = len / page_size;
    uint32_t pages_written  = 0;

    while (RESET_VALUE != pages_to_write)
    {
        err = R_OSPI_B_Write(&g_ospi_b_ctrl ,
                            (uint8_t *) ((uint32_t) p_src + (pages_written * page_size)),
                            (uint8_t *) (p_dest + (pages_written * page_size)), page_size);

        app_handle_error(err, APP_OSPI_WRITE);
        OSPI_B_ERR_RETURN(err, return err);

        err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);
        OSPI_B_ERR_RETURN(err, return err);

        pages_to_write--;
        pages_written++;
    }

    return err;
}

static fsp_err_t ospi_b_erase(uint8_t * const p_address)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t sector_size = RESET_VALUE;

    sector_size = (OSPI_B_SECTOR_4K_END_ADDRESS < (uint32_t)p_address) ?
                OSPI_B_SECTOR_SIZE_256K : OSPI_B_SECTOR_SIZE_4K;

    err = R_OSPI_B_Erase(&g_ospi_b_ctrl, p_address, sector_size);
    app_handle_error(err, APP_OSPI_ERASE);
    OSPI_B_ERR_RETURN(err, return err);

    err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);
    OSPI_B_ERR_RETURN(err, return err);

    return err;
}

fsp_err_t ospi_b_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address        = RESET_VALUE,
        .address_length = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE,
        .data           = RESET_VALUE,
        .data_length    = RESET_VALUE,
    };

    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
    app_handle_error(err, APP_OSPI_OPEN);
    OSPI_B_ERR_RETURN(err, return err);

    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    app_handle_error(err, APP_OSPI_SPI_PROTOCOL_SET);
    OSPI_B_ERR_RETURN(err, return err);

    /* Reset flash device by driving OM_RESET pin */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, OSPI_B_TIME_UNIT);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, OSPI_B_TIME_UNIT);

    /* Configure DDR sampling window extend */
    R_XSPI0->LIOCFGCS_b[1].DDRSMPEX = 0x7;

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    app_handle_error(err, APP_OSPI_WRITE_ENABLE);
    OSPI_B_ERR_RETURN(err, return err);

    /* Write to CFR2V to configure Address Byte Length and Memory Array Read Latency */
    transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_WRITE_CFR2V];
    transfer.address_length = OSPI_B_ADDRESS_LENGTH_THREE;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl , &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    app_handle_error(err, APP_OSPI_WRITE_ENABLE);
    OSPI_B_ERR_RETURN(err, return err);

    /* Write to CFR3V to configure Volatile Register Read Latency */
    transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_WRITE_CFR3V];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl , &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
    OSPI_B_ERR_RETURN(err, return err);

    /* Read back and verify CFR2V register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_READ_CFR2V];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl , &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if ((uint8_t)transfer.data != OSPI_B_DATA_CFR2V_REGISTER)
    {
        app_handle_error(APP_ERR_COMMON, APP_OSPI_READ);
        OSPI_B_ERR_RETURN(FSP_ERR_WRITE_FAILED, return err);
    }

    /* Read back and verify CFR3V register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_READ_CFR3V];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl , &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if ((uint8_t)transfer.data != OSPI_B_DATA_CFR3V_REGISTER)
    {
        app_handle_error(APP_ERR_COMMON, APP_OSPI_READ);
        OSPI_B_ERR_RETURN(FSP_ERR_WRITE_FAILED, return err);
    }

    if (g_ospi_b_ctrl.spi_protocol == SPI_FLASH_PROTOCOL_EXTENDED_SPI)
    {
        /* Switch 8D mode*/
        err = ospi_b_write_enable();
        app_handle_error(err, APP_OSPI_WRITE_ENABLE);
        OSPI_B_ERR_RETURN(err, return err);

        transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_SPI][OSPI_B_TRANSFER_WRITE_CFR5V];
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl , &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        app_handle_error(err, APP_OSPI_DIRECT_TRANSFER);
        OSPI_B_ERR_RETURN(err, return err);

        err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        app_handle_error(err, APP_OSPI_SPI_PROTOCOL_SET);
        OSPI_B_ERR_RETURN(err, return err);

        /* Read back and verify CFR5V register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_MODE_OPI][OSPI_B_TRANSFER_READ_CFR5V];
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl , &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if ((uint8_t)transfer.data != OSPI_B_DATA_SET_OPI_CFR5V_REGISTER)
        {
            app_handle_error(APP_ERR_COMMON, APP_OSPI_READ);
            OSPI_B_ERR_RETURN(FSP_ERR_WRITE_FAILED, return err);
        }
    }

    return err;
}

fsp_err_t write_encrypted_data_to_ospi(void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint32_t encrypted_addr = (uint32_t)(ENCRYPTED_START_ADDRESS);

    err = ospi_b_erase((uint8_t*)encrypted_addr);
    OSPI_B_ERR_RETURN(err, return err);

    ospi_b_write_page((uint8_t *)encrypted_data, (uint8_t *)encrypted_addr, sizeof(encrypted_data));
    OSPI_B_ERR_RETURN(err, return err);

    return err;
}
