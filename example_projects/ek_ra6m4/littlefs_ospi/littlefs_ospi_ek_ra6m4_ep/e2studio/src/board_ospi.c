/***********************************************************************************************************************
 * File Name    : board_ospi.c
 * Description  : Contains macros data structures and functions used in board_ospi.c and littlefs_ospi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "board_ospi.h"

/* Function declarations */
static fsp_err_t ospi_write_enable(void);
static fsp_err_t ospi_wait_operation(uint32_t timeout);
static fsp_err_t ospi_set_protocol_to_opi(void);

/* External variables */
spi_flash_direct_transfer_t g_ospi_direct_transfer [OSPI_COMMAND_MAX] =
{
     /* Transfer structure for SPI mode */
     [OSPI_COMMAND_WRITE_ENABLE_SPI_MODE] =
     {
          .command        = OSPI_WRITE_ENABLE_COMMAND_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_ZERO,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_WRITE_DISABLE_SPI_MODE] =
     {
          .command        = OSPI_WRITE_DISABLE_COMMAND_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_ZERO,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_READ_STATUS_SPI_MODE] =
     {
          .command        = OSPI_READ_STATUS_COMMAND_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_ZERO,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_SECTOR_ERASE_SPI_MODE] =
     {
          .command        = OSPI_ERASE_COMMAND_SECTOR_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_SECTOR_ERASE_3SPI_MODE] =
     {
          .command        = OSPI_ERASE_COMMAND_SECTOR_3SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_THREE,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_BLOCK_ERASE_SPI_MODE] =
     {
          .command        = OSPI_ERASE_COMMAND_BLOCK_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_BLOCK_ERASE_3SPI_MODE] =
     {
          .command        = OSPI_ERASE_COMMAND_BLOCK_3SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_THREE,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_WRITE_CR2_SPI_MODE] =
     {
          .command        = OSPI_COMMAND_WCR2_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_WRITE_CR2_3SPI_MODE] =
     {
          .command        = OSPI_COMMAND_WCR2_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_READ_CR2_SPI_MODE] =
     {
          .command        = OSPI_COMMAND_RCR2_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_READ_CR2_3SPI_MODE] =
     {
          .command        = OSPI_COMMAND_RCR2_SPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_ONE,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_ZERO
     },

     /* Transfer structure for OPI mode */
     [OSPI_COMMAND_WRITE_CR2_OPI_MODE] =
     {
          .command        = OSPI_COMMAND_WCR2_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_READ_CR2_OPI_MODE] =
     {
          .command        = OSPI_COMMAND_RCR2_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_FOUR
     },
     [OSPI_COMMAND_WRITE_ENABLE_OPI_MODE] =
     {
          .command        = OSPI_WRITE_ENABLE_COMMAND_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_ZERO,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_WRITE_DISABLE_OPI_MODE] =
     {
          .command        = OSPI_WRITE_DISABLE_COMMAND_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_ZERO,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_SECTOR_ERASE_OPI_MODE] =
     {
          .command        = OSPI_ERASE_COMMAND_SECTOR_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_BLOCK_ERASE_OPI_MODE] =
     {
          .command        = OSPI_ERASE_COMMAND_BLOCK_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ZERO,
          .dummy_cycles   = LENGTH_ZERO
     },
     [OSPI_COMMAND_READ_STATUS_OPI_MODE] =
     {
          .command        = OSPI_READ_STATUS_COMMAND_OPI_MODE,
          .address        = OSPI_CR2_ADDRESS_HEX_0,
          .data           = OSPI_CR2_DATA_HEX_00,
          .command_length = LENGTH_TWO,
          .address_length = LENGTH_FOUR,
          .data_length    = LENGTH_ONE,
          .dummy_cycles   = LENGTH_FOUR
     }
};

/***********************************************************************************************************************
 *  Function Name: ospi_write_enable
 *  Description  : This function enables write and verify the read data.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 FSP_ERR_ABORTED Upon incorrect read data.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t ospi_write_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer = {RESET_VALUE};

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_ctrl.spi_protocol)
               ? g_ospi_direct_transfer[OSPI_COMMAND_WRITE_ENABLE_SPI_MODE]
               : g_ospi_direct_transfer[OSPI_COMMAND_WRITE_ENABLE_OPI_MODE];
    err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_DirectTransfer API FAILED while Writing Enable!!!**\r\n");

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_ctrl.spi_protocol)
               ? g_ospi_direct_transfer[OSPI_COMMAND_READ_STATUS_SPI_MODE]
               : g_ospi_direct_transfer[OSPI_COMMAND_READ_STATUS_OPI_MODE];
    err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RET(FSP_SUCCESS != err, err,
                "\r\n**R_OSPI_DirectTransfer API FAILED while Reading back status register!!!**\r\n");

    /* Check Write Enable bit in Status Register */
    if (OSPI_WEN_BIT_MASK != (transfer.data & OSPI_WEN_BIT_MASK))
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Write enable failed!!!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_write_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_wait_operation
 *  Description  : This function waits until OSPI operation completes.
 *  Arguments    : timeout         Maximum waiting time.
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 FSP_ERR_TIMEOUT Upon time out.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t ospi_wait_operation(uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;

    while (status.write_in_progress)
    {
        /* Get device status */
        R_OSPI_StatusGet(&g_ospi_ctrl, &status);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_StatusGet API FAILED!!!**\r\n");
        if (RESET_VALUE == timeout)
        {
            err = FSP_ERR_TIMEOUT;
            APP_ERR_RET(FSP_SUCCESS != err, err, "OSPI time out occurred!!!\r\n");
        }
        R_BSP_SoftwareDelay(1, OSPI_TIME_UNIT);
        timeout --;

        APP_PRINT(RESET_VALUE == (timeout % PERIODIC_PRINT_OUT) ? ".":"");
        APP_PRINT(RESET_VALUE == (timeout % PERIODIC_LINE_FEED) ? "\r\n":"");
    }
    return err;
}
/***********************************************************************************************************************
* End of function ospi_wait_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_init
 *  Description  : This function initializes OSPI module and Flash device.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t ospi_init(void)
{
    fsp_err_t   err = FSP_SUCCESS;

    bsp_octaclk_settings_t octaclk  = {RESET_VALUE};

    /* Upon power-on or reset, the device defaults to SPI mode.
     * To ensure proper operation, the OCTACLK clock should be configured to 100 MHz in SPI mode.
     * Because OM_SCLK (OM_SCLK = OCTACLK/2) only support 50MHz as max. */
    octaclk.source_clock = BSP_CFG_OCTACLK_SOURCE; /* 200MHz */
    octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_2;
    R_BSP_OctaclkUpdate(&octaclk);

    /* In the current OSPI device model, the default status of the device is set to SPI mode.
     * Therefore, it is necessary to initialize the OSPI driver module in SPI mode
     * to ensure compatibility with the device's default SPI protocol. */
    spi_flash_cfg_t g_ospi_init_cfg = {RESET_VALUE};
    memcpy(&g_ospi_init_cfg, &g_ospi_cfg, sizeof(spi_flash_cfg_t));
    g_ospi_init_cfg.spi_protocol = SPI_FLASH_PROTOCOL_EXTENDED_SPI;

    /* Open OSPI module */
    err = R_OSPI_Open(&g_ospi_ctrl, &g_ospi_init_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_Open API FAILED!!!**\r\n");

    /* Enable Write */
    err = ospi_write_enable();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nospi_write_enable failed!!!\r\n");

    /* Set the OCTACLK clock to 200 MHz, as specified by the default configuration.*/
    octaclk.source_clock = BSP_CFG_OCTACLK_SOURCE; /* 200MHz */
    octaclk.divider      = BSP_CFG_OCTACLK_DIV;
    R_BSP_OctaclkUpdate(&octaclk);

    /* Configure OSPI flash to OPI mode */
    err = ospi_set_protocol_to_opi();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nospi_set_protocol_to_opi failed!!!\r\n");

    /* Close OSPI flash */
    err = R_OSPI_Close(&g_ospi_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_Close API FAILED!!!**\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function ospi_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: erase_ospi_flash
 *  Description  : This function erases the flash memory using lower level flash APIs.
 *  Arguments    : erase_size      The size of flash to be erased.
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t erase_ospi_flash(uint32_t erase_size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open OSPI module */
    if (MODULE_CLOSE == g_ospi_ctrl.open)
    {
        err = R_OSPI_Open(&g_ospi_ctrl, &g_ospi_cfg);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_Open API FAILED!!!**\r\n");
    }

    /* Erase flash */
    if (SPI_FLASH_ERASE_SIZE_CHIP_ERASE == erase_size)
    {
        APP_PRINT("\r\n\r\nErase the entire ospi flash...\r\n\r\n");
        APP_PRINT("\r\nThis operation will take several minutes. Please wait!\r\n");

        err = R_OSPI_Erase(&g_ospi_ctrl, (uint8_t *)OSPI_CS0_START_ADDRESS, SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_Erase API FAILED!!!**\r\n");

        err = ospi_wait_operation(UINT32_MAX);
        APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_wait_operation failed!!!\r\n");

        APP_PRINT("\r\n\r\nErased entire OSPI flash successfully!\r\n");
        APP_PRINT("\r\nPlease FORMAT FILE SYSTEM after erase the entire OSPI flash!\r\n");
    }
    else if (OSPI_32K_SIZE == erase_size)
    {
        APP_PRINT("\r\n\r\nErase the first 32 KB of the OSPI flash...\r\n\r\n");

        uint8_t * p_current_add = (uint8_t*) OSPI_CS1_START_ADDRESS;
        for (uint8_t i = 0; i < (erase_size/OSPI_SECTOR_SIZE); ++i)
        {
            err = R_OSPI_Erase(&g_ospi_ctrl, p_current_add, OSPI_SECTOR_SIZE);
            APP_ERR_RET(FSP_SUCCESS != err, err, "**R_OSPI_Erase API FAILED!!!**\r\n");

            err = ospi_wait_operation(OSPI_TIME_ERASE_4K);
            APP_ERR_RET(FSP_SUCCESS != err, err, "ospi_wait_operation failed!!!\r\n");

            p_current_add += OSPI_SECTOR_SIZE;
        }

        APP_PRINT("\r\n\r\nErased the first 32 KB of the OSPI flash successfully!\r\n");
        APP_PRINT("\r\nPlease FORMAT FILE SYSTEM after erase the first 32 KB of the OSPI flash!\r\n");
    }
    else
    {
        APP_PRINT("\r\n\r\nThis EP only supports erase entire flash or 32 KB flash!\r\n\r\n");
        return err;
    }

    return err;
}
/***********************************************************************************************************************
* End of function erase_ospi_flash
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: ospi_set_protocol_to_opi
 *  Description  : This function configures OSPI to OPI mode.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation.
 *                 FSP_ERR_ABORTED  Upon incorrect read data.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t ospi_set_protocol_to_opi(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    if (SPI_FLASH_PROTOCOL_DOPI == g_ospi_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_ctrl.spi_protocol)
    {
        /* Write Enable */
        err = ospi_write_enable();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nospi_write_enable failed in SPI mode!!!\r\n");

        /* Change dummy cycles according to requested frequency */
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_300;
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].data = OSPI_CR2_DATA_HEX_05;

        /* Write configuration register */
        err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE],
                                    SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_DirectTransfer API FAILED while writing Write"\
                    "configuration register in spi mode!!!**\r\n");

        /* Verify Dummy cycle */
        g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_300;

        /* Read configuration register */
        err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE],
                                    SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_DirectTransfer API FAILED while reading Write"\
                    "configuration register in spi mode!!!**\r\n");

        /* Check read data */
        if (OSPI_CR2_DATA_HEX_05 !=
            (g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_SPI_MODE].data & OSPI_CR2_DATA_HEX_05))
        {
            err = FSP_ERR_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nData mismatched in SPI mode!!!\r\n");
        }

        /* Write Enable in extended SPI mode */
        err = ospi_write_enable();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nospi_write_enable failed in extended spi mode!!!\r\n");

        /* The OctaFlash chip is in SPI mode. Change DOPI mode */
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].address = OSPI_CR2_ADDRESS_HEX_0;
        g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE].data = DOPI_MODE; /* DTR OPI enable */

        /* Write configuration register */
        err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_WRITE_CR2_SPI_MODE],
                                    SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_DirectTransfer API FAILED while writing Write"\
                    "configuration register in opi mode!!!**\r\n");

        /* Transition the OSPI peripheral to DOPI mode. This will initiate auto calibration as MDTR is 0 */
        err = R_OSPI_SpiProtocolSet(&g_ospi_ctrl, SPI_FLASH_PROTOCOL_DOPI);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_SpiProtocolSet API FAILED!!!**\r\n");

        /* Verify that the chip is in requested OPI mode */
        g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_OPI_MODE].address = OSPI_CR2_ADDRESS_HEX_0;
        err = R_OSPI_DirectTransfer(&g_ospi_ctrl, &g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_OPI_MODE],
                                    SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_OSPI_DirectTransfer API FAILED while reading"\
                    "configuration register in opi mode!!!**\r\n");

        /* Check for read data */
        if ((DOPI_MODE) != (g_ospi_direct_transfer[OSPI_COMMAND_READ_CR2_OPI_MODE].data & DOPI_MODE))
        {
            err = FSP_ERR_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nData mismatched while reading mode!!!\r\n");
        }
    }
    else
    {
        err = FSP_ERR_INVALID_MODE;
        APP_ERR_RET(FSP_SUCCESS != err, err, "Flash device do not support this mode!!!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_set_protocol_to_opi
***********************************************************************************************************************/
