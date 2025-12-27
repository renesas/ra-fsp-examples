/***********************************************************************************************************************
 * File Name    : ospi_b_control.c
 * Description  : Contains data structures and functions used in ospi_b_control.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <ospi_b_control.h>

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

    [OSPI_B_TRANSFER_WRITE_CR_00H_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CR_01H_SPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CR_01H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS] =
    {
    .command        = OSPI_B_COMMAND_ENTER_4BYTES_ADDR,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_DUMMY,
    .data_length    = OSPI_B_DATA_DUMMY,
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

    [OSPI_B_TRANSFER_READ_CR_00H_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_CR_01H_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
    .address        = OSPI_B_ADDRESS_CR_01H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },

    [OSPI_B_TRANSFER_READ_DEVICE_ID_SPI] =
    {
    .command        = OSPI_B_COMMAND_READ_DEVICE_ID_SPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_SPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
    .data_length    = OSPI_B_DATA_LENGTH_THREE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI
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

    [OSPI_B_TRANSFER_WRITE_CR_00H_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
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

    [OSPI_B_TRANSFER_READ_CR_00H_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_CR_01H_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CR_01H_REGISTER,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_TWO,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },

    [OSPI_B_TRANSFER_READ_DEVICE_ID_OPI] =
    {
    .command        = OSPI_B_COMMAND_READ_DEVICE_ID_OPI,
    .address        = OSPI_B_ADDRESS_DUMMY,
    .data           = OSPI_B_DATA_DUMMY,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_THREE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },

    [OSPI_B_TRANSFER_ENTER_XIP_MODE_OPI] =
    {
    .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
    .address        = OSPI_B_ADDRESS_CR_06H_REGISTER,
    .data           = OSPI_B_EN_XIP_CODE,
    .command_length = OSPI_B_COMMAND_LENGTH_OPI,
    .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    .data_length    = OSPI_B_DATA_LENGTH_ONE,
    .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_RESET_ENABLE] =
    {
     .command        = OSPI_B_COMMAND_RESET_ENABLE,
     .address        = OSPI_B_ADDRESS_DUMMY,
     .data           = OSPI_B_DATA_DUMMY,
     .command_length = OSPI_B_COMMAND_LENGTH_SPI,
     .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
     .data_length    = OSPI_B_DATA_LENGTH_ZERO,
     .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_RESET_MEMORY] =
    {
     .command        = OSPI_B_COMMAND_RESET_MEMORY,
     .address        = OSPI_B_ADDRESS_DUMMY,
     .data           = OSPI_B_DATA_DUMMY,
     .command_length = OSPI_B_COMMAND_LENGTH_SPI,
     .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
     .data_length    = OSPI_B_DATA_LENGTH_ZERO,
     .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    }
};

/* Function declarations */
static fsp_err_t ospi_b_write_enable(void);
static fsp_err_t ospi_b_wait_operation(uint32_t timeout);
static fsp_err_t ospi_b_setup_calibrate_data(void);


/*******************************************************************************************************************//**
 * @brief       This function enables write and verify the read data.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful operation.
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable(void)
{
    fsp_err_t                   err         = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer    =
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
    APP_ERR_RETURN(err);

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
               ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err);

    /* Check Write Enable bit in Status Register */
    if(OSPI_B_WEN_BIT_MASK != (transfer.data & OSPI_B_WEN_BIT_MASK))
    {
        return FSP_ERR_INVALID_DATA;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function waits until OSPI operation completes.
 * @param[in]   timeout         Maximum waiting time.
 * @retval      FSP_SUCCESS     Upon successful wait OSPI operating.
 * @retval      FSP_ERR_TIMEOUT Upon time out.
 * @retval      Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_wait_operation(uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        R_OSPI_B_StatusGet(&g_ospi_b_ctrl, &status);
        APP_ERR_RETURN(err);
        if(RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
        R_BSP_SoftwareDelay(1, OSPI_B_TIME_UNIT);
        timeout --;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets up the auto-calibrate data for the flash.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_b_setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;
    ospi_b_extended_cfg_t * p_extended_cfg = (ospi_b_extended_cfg_t *)g_ospi_b_cfg.p_extend;
    uint32_t g_autocalibration_data[] =
    {
        0xFFFF0000U,
        0x000800FFU,
        0x00FFF700U,
        0xF700F708U
    };
    /* Verify auto-calibration data */
    if (RESET_VALUE != memcmp((uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
            &g_autocalibration_data, sizeof(g_autocalibration_data)))
    {
        /* Erase the flash sector that stores auto-calibration data */
        err = R_OSPI_B_Erase(&g_ospi_b_ctrl,
                            (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr, OSPI_B_SECTOR_SIZE_4K);
        APP_ERR_RETURN(err);

        /* Wait until erase operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);
        APP_ERR_RETURN(err);

        /* Write auto-calibration data to the flash */
        err = R_OSPI_B_Write(&g_ospi_b_ctrl, (uint8_t *)&g_autocalibration_data,\
                             (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,\
                             sizeof(g_autocalibration_data));
        APP_ERR_RETURN(err);

        /* Wait until write operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);
        APP_ERR_RETURN(err);
    }
    __NOP();
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes OSPI module and Flash device.
 * @param       None.
 * @retval      FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t ospi_b_init(void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode */
    fsp_err_t                   err      = FSP_SUCCESS;
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
    APP_ERR_RETURN(err);

    /* Switch OSPI module to 1S-1S-1S mode to configure flash device */
    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    APP_ERR_RETURN(err);

    /* ensure that exit XIP mode and reset external flash */
    err = xip_exit_res();
    APP_ERR_RETURN(err);

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    APP_ERR_RETURN(err);

    /* Enter 4-bytes address */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RETURN(err);

    /* Write to ADDR 00000001H of CR to configure dummy cycle */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR_01H_SPI];
    transfer.data    = OSPI_B_DATA_SET_CR_01H;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RETURN(err);

    /* Read back and verify CR register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR_01H_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err);

    if(OSPI_B_DATA_SET_CR_01H != (uint8_t)transfer.data)
    {
        return FSP_ERR_INVALID_DATA;
    }

    /* Setup calibrate data */
    err = ospi_b_setup_calibrate_data();
    APP_ERR_RETURN(err);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures OSPI to extended SPI mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful transition to SPI operating mode.
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t ospi_b_set_protocol_to_spi(void)
{
    fsp_err_t                   err      = FSP_SUCCESS;
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
    bsp_octaclk_settings_t      octaclk  =
    {
        .source_clock  = RESET_VALUE,
        .divider       = RESET_VALUE
    };

    if(SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if(SPI_FLASH_PROTOCOL_8D_8D_8D == g_ospi_b_ctrl.spi_protocol)
    {
        /* Transfer write enable command */
        err = ospi_b_write_enable();
        APP_ERR_RETURN(err);

        /* Change to SPI mode */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR_00H_OPI];
        transfer.data = OSPI_B_DATA_SET_SPI_CR_00H;
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RETURN(err);

        /* Change the OCTACLK clock to 88.667 MHz -> OM_n_SCLK 44.33Mhz in SDR mode without OM_DQS */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_3;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to SPI mode */
        err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_RETURN(err);

        /* Read back and verify CR register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR_00H_SPI];
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RETURN(err);

        if(OSPI_B_DATA_SET_SPI_CR_00H != (uint8_t)transfer.data)
        {
            return FSP_ERR_INVALID_DATA;
        }
    }

    else
    {
        APP_ERR_RETURN(err);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures OSPI to OPI mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful transition to OPI operating mode.
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_b_set_protocol_to_opi(void)
{
    fsp_err_t                   err      = FSP_SUCCESS;
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
    bsp_octaclk_settings_t      octaclk  =
    {
        .source_clock  = RESET_VALUE,
        .divider       = RESET_VALUE
    };

    if(SPI_FLASH_PROTOCOL_8D_8D_8D == g_ospi_b_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if(SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
    {

        /* Transfer write enable command */
        err = ospi_b_write_enable();
        APP_ERR_RETURN(err);

        /* Change to DOPI mode */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR_00H_SPI];
        transfer.data    = OSPI_B_DATA_SET_OPI_CR_00H;
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RETURN(err);

        /* Change the OCTACLK clock to 266 MHz -> OM_n_SLCK is 133MHz in DDR mode with OM_DQS */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_1;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to SPI mode */
        err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        APP_ERR_RETURN(err);

        /* Read back and verify CR register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR_00H_OPI];
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RETURN(err);

        if(OSPI_B_DATA_SET_OPI_CR_00H != (uint8_t) transfer.data)
        {
            return FSP_ERR_INVALID_DATA;
        }
    }
    else
    {
        APP_ERR_RETURN(err);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Enables XIP (Execute-In-Place) mode on the connected OSPI flash.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful transition to OPI operating mode.
 * @retval      FSP_ERR_INVALID_DATA    Data read back from external flash does not match expected value.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t xip_enter(void)
{
    fsp_err_t err = FSP_SUCCESS;

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
    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* setup external flash register enable XIP mode */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_ENTER_XIP_MODE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RETURN(err);

    transfer.command        = OSPI_B_COMMAND_RCR_OPI,
    transfer.address        = 0x00000006,
    transfer.data           = OSPI_B_DATA_DUMMY,
    transfer.command_length = OSPI_B_COMMAND_LENGTH_OPI,
    transfer.address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
    transfer.data_length    = OSPI_B_DATA_LENGTH_ONE,
    transfer.dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI;

    /* Read back and verify CR register data */
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err);

    if(OSPI_B_EN_XIP_CODE != (uint8_t)transfer.data)
    {
        return FSP_ERR_INVALID_DATA;
    }

    err = R_OSPI_B_XipEnter(&g_ospi_b_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Exits XIP mode and resets the flash memory.
 *              This function terminates XIP mode by sending the appropriate exit sequence
 *              and performs a software reset of the flash using RESET ENABLE (66h) and RESET MEMORY (99h)
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful transition to OPI operating mode.
 * @retval      FSP_ERR_TIMEOUT         The flash did not become ready within the allowed time.
 *                                      This ensures the function does not enter an infinite loop.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t xip_exit_res(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout = 0xFFFFFFFF;
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

    /* Hard reset external flash */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, OSPI_B_TIME_UNIT);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, OSPI_B_TIME_UNIT);

    /* Exit XIP mode */
    err = R_OSPI_B_XipExit(&g_ospi_b_ctrl);
    APP_ERR_RETURN(err);

    /* Software Reset OPI Flash */
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl,
                                  &g_ospi_b_direct_transfer[OSPI_B_TRANSFER_RESET_ENABLE],
                                  SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);                             // issue Reset Enable command
    APP_ERR_RETURN(err);

    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS); // tSHSL2 at least 50ns

    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl,
                                  &g_ospi_b_direct_transfer[OSPI_B_TRANSFER_RESET_MEMORY],
                                  SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);                             // issue Reset memory command
    APP_ERR_RETURN(err);

    /* Confirm flash reset */
    transfer.command        = OSPI_B_COMMAND_READ_FLAG_STATUS,
    transfer.address        = OSPI_B_ADDRESS_DUMMY,
    transfer.data           = OSPI_B_DATA_DUMMY,
    transfer.command_length = OSPI_B_COMMAND_LENGTH_SPI,
    transfer.address_length = OSPI_B_ADDRESS_DUMMY,
    transfer.data_length    = OSPI_B_DATA_LENGTH_ONE,
    transfer.dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI;
	
	/* Read & verify flag status bit 7 */
    do {

        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ); // issue command read flag status.
        APP_ERR_RETURN(err);
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        timeout--;
    }while(((uint8_t)transfer.data & (1 << 7)) == 0 && timeout > 0);

    if(timeout == 0)
    {
        return FSP_ERR_TIMEOUT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup ospi_b_control.c)
 **********************************************************************************************************************/
