/***********************************************************************************************************************
 * File Name    : board_ospi.c
 * Description  : Contains macros data structures and functions used in board_ospi.c and littlefs_ospi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "board_ospi.h"

/* Function declarations */
static fsp_err_t ospi_b_write_enable (void);
static fsp_err_t ospi_b_wait_operation (uint32_t timeout);
static fsp_err_t ospi_b_setup_calibrate_data (void);
static fsp_err_t ospi_b_set_protocol_to_opi (void);

/* External variables */
spi_flash_direct_transfer_t g_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX] =
{
    /* Transfer structure for SPI mode */
    [OSPI_B_TRANSFER_ENTER_4_ADDR_SPI] =
    {
        .command        = OSPI_B_COMMAND_ENTER_4_ADDR_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ZERO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_EXIT_4_ADDR_SPI] =
    {
        .command        = OSPI_B_COMMAND_EXIT_4_ADDR_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ZERO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
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
    [OSPI_B_TRANSFER_WRITE_REG_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CFR5V_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
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
    [OSPI_B_TRANSFER_READ_VOL_REG_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_NON_VOL_REG_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI
    },
    [OSPI_B_TRANSFER_READ_DEVICE_ID_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_DEVICE_ID_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_FOUR,
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
    [OSPI_B_TRANSFER_WRITE_REG_OPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
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
    [OSPI_B_TRANSFER_READ_VOL_REG_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_CFR5V_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
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
        .data_length    = OSPI_B_DATA_LENGTH_FOUR,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },
};

/***********************************************************************************************************************
 *  Function Name: ospi_b_write_enable
 *  Description  : This function enables write and verify the read data.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 FSP_ERR_ABORTED Upon incorrect read data
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable(void)
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        = {RESET_VALUE};

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
            ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
            : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
            ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
            : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");

    /* Check Write Enable bit in Status Register */
    if (OSPI_B_WEN_BIT_MASK != (transfer.data & OSPI_B_WEN_BIT_MASK))
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Write enable FAILED!!!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_write_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_b_wait_operation
 *  Description  : This function waits until OSPI operation completes.
 *  Arguments    : timeout         Maximum waiting time
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 FSP_ERR_TIMEOUT Upon time out
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
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
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_StatusGet API FAILED!!!**\r\n");
        if (RESET_VALUE == timeout)
        {
            err = FSP_ERR_TIMEOUT;
            APP_ERR_RET(FSP_SUCCESS != err, err, "OSPI time out occurred!!!\r\n");
        }
        R_BSP_SoftwareDelay(1, OSPI_B_TIME_UNIT);
        timeout --;

        APP_PRINT(RESET_VALUE == (timeout % PERIODIC_PRINT_OUT) ? ".":"");
        APP_PRINT(RESET_VALUE == (timeout % PERIODIC_LINE_FEED) ? "\r\n":"");
    }
    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_wait_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_b_init
 *  Description  : This function initializes OSPI module and Flash device.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_b_init (void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode first */
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        = {RESET_VALUE};

    spi_flash_cfg_t g_ospi_init_cfg = {RESET_VALUE};
    memcpy(&g_ospi_init_cfg, &g_ospi_b_cfg, sizeof(spi_flash_cfg_t));
    g_ospi_init_cfg.spi_protocol = SPI_FLASH_PROTOCOL_1S_1S_1S;

    /* Open OSPI module */
    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_init_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Open API FAILED!!!**\r\n");

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_write_enable FAILED!!!\r\n");

    /* Transfer enter 4 byte address command */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_ENTER_4_ADDR_SPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");

    /* Read back and verify CFR2V register data to confirm 4 byte address mode */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
    transfer.address = OSPI_B_ADDRESS_CFR2V_REGISTER;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");
    if (OSPI_B_DATA_CFR2V_REGISTER != (uint8_t)transfer.data)
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Verify CFR2V register data FAILED!!!\r\n");
    }

    /* Read back and verify CFR3V register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
    transfer.address = OSPI_B_ADDRESS_CFR3V_REGISTER;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");
    if (OSPI_B_DATA_DEFAULT_CFR3V_REGISTER != (uint8_t)transfer.data)
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Verify CFR3V register data FAILED!!!\r\n");
    }

    /* Read back and verify CFR4V register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
    transfer.address = OSPI_B_ADDRESS_CFR4V_REGISTER;
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");
    if (OSPI_B_DATA_CFR4V_REGISTER != (uint8_t)transfer.data)
    {
        if (OSPI_B_DATA_DEFAULT_CFR4V_REGISTER != (uint8_t)transfer.data)
        {
            err = FSP_ERR_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "Verify CFR4V register default data FAILED!!!\r\n");
        }
        APP_PRINT("The CFR4V register still in default data\r\n");

        /* Transfer write enable command */
        err = ospi_b_write_enable();
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_write_enable FAILED!!!\r\n");

        /* Write to CFR4N to configure Error Correction Code */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_REG_SPI];
        transfer.address = OSPI_B_ADDRESS_CFR4N_REGISTER;
        transfer.data = OSPI_B_DATA_CFR4V_REGISTER;

        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");

        err = ospi_b_wait_operation(UINT16_MAX);
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_wait_operation FAILED!!!\r\n");

        /* Read back and verify CFR4V register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
        transfer.address = OSPI_B_ADDRESS_CFR4V_REGISTER;

        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");
        if (OSPI_B_DATA_CFR4V_REGISTER != (uint8_t)transfer.data)
        {
            err = FSP_ERR_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "Verify CFR4V register data FAILED!!!\r\n");
        }

        APP_PRINT("Erase Chip after configure Error Correction Code\r\n");

        err = R_OSPI_B_Erase(&g_ospi_b_ctrl, (uint8_t *)OSPI_B_CS1_START_ADDRESS, SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Erase API FAILED!!!**\r\n");

        err = ospi_b_wait_operation(UINT32_MAX);
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_wait_operation FAILED!!!\r\n");
    }

    /* Setup calibrate data */
    err = ospi_b_setup_calibrate_data();
    APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_setup_calibrate_data FAILED!!!\r\n");

    /* Configure OSPI flash to OPI mode */
    err = ospi_b_set_protocol_to_opi();
    APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_set_protocol_to_opi FAILED!!!\r\n");

    /* Close OSPI flash */
    err = R_OSPI_B_Close(&g_ospi_b_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Close API FAILED!!!**\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: erase_ospi_flash
 *  Description  : This function erases the flash memory using lower level flash APIs.
 *  Arguments    : erase_size      The size of flash to be erased
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t erase_ospi_flash(uint32_t erase_size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open OSPI module */
    if (MODULE_CLOSE == g_ospi_b_ctrl.open)
    {
        err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Open API FAILED!!!**\r\n");
    }

    /* Erase flash */
    if (SPI_FLASH_ERASE_SIZE_CHIP_ERASE == erase_size)
    {
        APP_PRINT("\r\n\r\nErase the entire ospi flash...\r\n\r\n");
        APP_PRINT("\r\nThis operation will take several minutes. Please wait!\r\n");

        err = R_OSPI_B_Erase(&g_ospi_b_ctrl, (uint8_t*) OSPI_B_CS1_START_ADDRESS, erase_size);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Erase API FAILED!!!**\r\n");

        err = ospi_b_wait_operation(UINT32_MAX);
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_wait_operation FAILED!!!\r\n");
    }
    else if (OSPI_B_32K_SIZE == erase_size)
    {
        APP_PRINT("\r\n\r\nErase the first 32 KB of the OSPI flash...\r\n\r\n");

        uint8_t * p_current_add = (uint8_t*) OSPI_B_CS1_START_ADDRESS;
        for (uint8_t i = 0; i < (erase_size/OSPI_B_SECTOR_4K_SIZE); ++i)
        {
            err = R_OSPI_B_Erase(&g_ospi_b_ctrl, p_current_add, OSPI_B_SECTOR_4K_SIZE);
            APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Erase API FAILED!!!**\r\n");

            err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);
            APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_wait_operation FAILED!!!\r\n");

            p_current_add += OSPI_B_SECTOR_4K_SIZE;
        }
    }
    else
    {
        APP_PRINT("\r\n\r\nThis EP only supports erase entire flash or 32 KB flash!\r\n\r\n");
        return err;
    }

    /* Re-setup calibrate data after erase OSPI Flash */
    err = ospi_b_setup_calibrate_data();
    APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_setup_calibrate_data FAILED!!!\r\n");

    /* Erase OSPI flash successful */
    if (SPI_FLASH_ERASE_SIZE_CHIP_ERASE == erase_size)
    {
        APP_PRINT("\r\n\r\nErased entire OSPI flash successfully!\r\n");
        APP_PRINT("\r\nPlease FORMAT FILE SYSTEM after erase the entire OSPI flash!\r\n");
    }
    else
    {
        APP_PRINT("\r\n\r\nErased the first 32 KB of the OSPI flash successfully!\r\n");
        APP_PRINT("\r\nPlease FORMAT FILE SYSTEM after erase the first 32 KB of the OSPI flash!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function erase_ospi_flash
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_b_setup_calibrate_data
 *  Description  : This function sets up the auto-calibrate data for the flash.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_b_setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Calibration pattern data, which is set in both the OSPI_B Command Calibration Control register
     * and OSPI_B_AUTO_CALIB_ADDRESS. Refer to the OSPI_B_PRV_AUTOCALIBRATION_PREAMBLE_PATTERN in r_ospi_b.c */
    uint32_t g_auto_calibration_data[] =
    {
        0xFFFF0000U,
        0x000800FFU,
        0x00FFF700U,
        0xF700F708U
    };

    int32_t compare_result = memcmp((uint8_t *)OSPI_B_AUTO_CALIB_ADDRESS,
                                    &g_auto_calibration_data, sizeof(g_auto_calibration_data));

    /* Verify auto-calibration data */
    if (RESET_VALUE != compare_result)
    {
        /* Erase the flash sector that stores auto-calibration data */
        err = R_OSPI_B_Erase(&g_ospi_b_ctrl,
                              (uint8_t *)OSPI_B_AUTO_CALIB_ADDRESS, OSPI_B_SECTOR_4K_SIZE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Erase API FAILED!!!**\r\n");

        /* Wait until erase operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_wait_operation FAILED!!!\r\n");

        /* Write auto-calibration data to the flash */
        err = R_OSPI_B_Write(&g_ospi_b_ctrl, (uint8_t *)&g_auto_calibration_data,
                             (uint8_t *)OSPI_B_AUTO_CALIB_ADDRESS, sizeof(g_auto_calibration_data));
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_Write API FAILED!!!**\r\n");

        /* Wait until write operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_wait_operation FAILED!!!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_setup_calibrate_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_b_set_protocol_to_opi
 *  Description  : This function configures ospi to opi mode.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_b_set_protocol_to_opi(void)
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer = {RESET_VALUE};

    if (SPI_FLASH_PROTOCOL_8D_8D_8D == g_ospi_b_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
    {
        /* Transfer write enable command */
        err = ospi_b_write_enable();
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_b_write_enable FAILED!!!\r\n");

        /* Write to CFR5V Register to Configure flash device interface mode */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR5V_SPI];
        transfer.data = OSPI_B_DATA_SET_OPI_CFR5V_REGISTER;
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");

        /* Switch OSPI module mode to OPI mode */
        err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_SpiProtocolSet API FAILED!!!**\r\n");

        /* Read back and verify CFR5V register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CFR5V_OPI];
        err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_B_DirectTransfer API FAILED!!!**\r\n");
        if (OSPI_B_DATA_SET_OPI_CFR5V_REGISTER != (uint8_t)transfer.data)
        {
            err = FSP_ERR_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "Verify CFR5V register data FAILED!!!\r\n");
        }
    }
    else
    {
        err = FSP_ERR_INVALID_MODE;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Flash device do not support this mode!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_set_protocol_to_opi
***********************************************************************************************************************/
