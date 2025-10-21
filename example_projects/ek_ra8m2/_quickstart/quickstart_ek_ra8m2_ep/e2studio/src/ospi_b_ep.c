/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : ospi_b_ep.c
 * Description  : Contains data structures and functions used in ospi_b_ep.c
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "ospi_b_commands.h"
#include "ospi_b_ep.h"
#include "R7KA8M2JF_core0.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup ospi_b_ep.c
 * @{
 **********************************************************************************************************************/

/* External variable */
extern spi_flash_direct_transfer_t g_ospi_b_direct_transfer[OSPI_B_TRANSFER_MAX];

/* Global variables */
uint8_t g_read_data[OSPI_B_APP_DATA_SIZE] = {RESET_VALUE};

const uint8_t g_write_data[OSPI_B_APP_DATA_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

/* Function declarations */
static fsp_err_t ospi_b_write_enable (void);
static fsp_err_t ospi_b_wait_operation (uint32_t timeout);
static fsp_err_t ospi_b_setup_calibrate_data (void);


/**********************************************************************************************************************
 * Function Name: ospi_b_write_enable
 * Description  : This function enables write and verify the read data.
 * Return Value : FSP_SUCCESS     Upon successful operation.
 *                FSP_ERR_ABORTED Upon incorrect read data.
 *                Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 *********************************************************************************************************************/
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
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
                ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
                : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
                ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
                : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);


    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_write_enable
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_b_wait_operation
 * Description  : This function waits until OSPI operation completes.
 * Argument     : timeout
 * Return Value : FSP_SUCCESS     Upon successful wait OSPI operating.
 *                FSP_ERR_TIMEOUT Upon time out.
 *                Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 *********************************************************************************************************************/
static fsp_err_t ospi_b_wait_operation(uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;

    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        R_OSPI_B_StatusGet(&g_ospi0_ctrl, &status);

        R_BSP_SoftwareDelay(1, OSPI_B_TIME_UNIT);
        timeout--;
    }
    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_wait_operation
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: ospi_b_init
 * Description  : This functions initializes OSPI module and Flash device.
 * Return Value : FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device.
 *                Any other error code apart from FSP_SUCCESS  Unsuccessful open.
 *********************************************************************************************************************/
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
    err = R_OSPI_B_Open(&g_ospi0_ctrl, &g_ospi0_cfg);


    /* Switch OSPI module to 1S-1S-1S mode to configure flash device */
    err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);

    /* Reset flash device */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, OSPI_B_TIME_UNIT);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, OSPI_B_TIME_UNIT);

    /* Transfer write enable command */
    err = ospi_b_write_enable();

    /* Write to ADDR 00000300H of CR2 to configure dummy cycle */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR2_300H_SPI];

    transfer.data    = OSPI_B_DATA_SET_CR2_300H;

    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);

    /* Read back and verify CR2 register data */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR2_300H_SPI];

    err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);

    /* Abort if the received data does not match the expected CR2 value (0x300).
       Casting to uint8_t ensures consistent comparison by matching the expected data type. */
    if (OSPI_B_DATA_SET_CR2_300H != (uint8_t)transfer.data)
    {
        return err;
    }

    /* Setup calibrate data */
    err = ospi_b_setup_calibrate_data();


    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_init
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: ospi_b_set_protocol_to_spi
 * Description  : This function configures OSPI to extended SPI mode.
 * Return Value : FSP_SUCCESS     Upon successful transition to SPI operating mode.
 *                FSP_ERR_ABORTED Upon incorrect read data.
 *                Any other error code apart from FSP_SUCCESS  Unsuccessful operation.
 *********************************************************************************************************************/
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

    if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_8D_8D_8D == g_ospi0_ctrl.spi_protocol)
    {
        /* Transfer write enable command */
        err = ospi_b_write_enable();

        /* Change to SPI mode */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR2_000H_OPI];

        transfer.data = OSPI_B_DATA_SET_SPI_CR2_000H;

        err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);

        /* Change the OCTACLK clock to 100 MHz in SDR mode without OM_DQS */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2R;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_4;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to SPI mode */
        err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);

        /* Read back and verify CR2 register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR2_000H_SPI];

        err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);

        /* Abort if the received data does not match the expected SPI CR2 value.
           Casting to uint8_t ensures type consistency for accurate comparison. */
        if (OSPI_B_DATA_SET_SPI_CR2_000H != (uint8_t)transfer.data)
        {
            err = FSP_ERR_ABORTED;
            return err;
        }
    }

    else
    {
        err = FSP_ERR_ABORTED;
        return err;
    }
    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_set_protocol_to_spi
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       This function configures OSPI to OPI mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful transition to OPI operating mode.
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: ospi_b_set_protocol_to_opi
 * Description  : This function configures OSPI to OPI mode.
 * Return Value : FSP_SUCCESS     Upon successful transition to OPI operating mode.
 *                FSP_ERR_ABORTED Upon incorrect read data.
 *                Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 *********************************************************************************************************************/
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

    if (SPI_FLASH_PROTOCOL_8D_8D_8D == g_ospi0_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi0_ctrl.spi_protocol)
    {
        /* Transfer write enable command */
        err = ospi_b_write_enable();


        /* Change to DOPI mode */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR2_000H_SPI];

        transfer.data    = OSPI_B_DATA_SET_OPI_CR2_000H;

        err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);

        /* Transfer write enable command */
        err = ospi_b_write_enable();


        /* Change the OCTACLK clock to 200 MHz in DDR mode with OM_DQS */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2R;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_1;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to SPI mode */
        err = R_OSPI_B_SpiProtocolSet(&g_ospi0_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);

        /* Read back and verify CR2 register data */
        transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR2_000H_OPI];

        err = R_OSPI_B_DirectTransfer(&g_ospi0_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);


        /* Abort if the received data does not match the expected OPI CR2 value.
           Casting to uint8_t ensures type consistency for reliable comparison. */
        if (OSPI_B_DATA_SET_OPI_CR2_000H != (uint8_t)transfer.data)
        {
            err = FSP_ERR_ABORTED;
            return err;
        }
    }
    else
    {
        err = FSP_ERR_ABORTED;
        return err;
    }
    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_set_protocol_to_opi
 *********************************************************************************************************************/




/**********************************************************************************************************************
 * Function Name: ospi_b_setup_calibrate_data
 * Description  : This function sets up the auto-calibrate data for the flash.
 * Return Value : FSP_SUCCESS Upon successful operation
 *                Any other error code apart from FSP_SUCCESS  Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t ospi_b_setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;


    /* Cast to ospi_b_extended_cfg_t to access extended configuration parameters
       from the generic OSPI configuration structure. */
    ospi_b_extended_cfg_t * p_extended_cfg = (ospi_b_extended_cfg_t *)g_ospi0_cfg.p_extend;

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    uint32_t g_autocalibration_data[] =
    {
        0xFFFF0000U,
        0x000800FFU,
        0x00FFF700U,
        0xF700F708U
    };
#elif defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8M2_EK)
    uint32_t g_autocalibration_data[] =
    {
        0xFFFF0000U,
        0x0800FF00U,
        0xFF0000F7U,
        0x00F708F7U
    };
#else // unsupported device
    uint32_t g_autocalibration_data[] =
    {
        0x00000000U,
        0x00000000U,
        0x00000000U,
        0x00000000U
    };
#endif
    /* Verify auto-calibration data */
    if (RESET_VALUE != memcmp((uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
            &g_autocalibration_data, sizeof(g_autocalibration_data)))
    {
        /* Erase the flash sector that stores auto-calibration data */
        err = R_OSPI_B_Erase(&g_ospi0_ctrl,
                            (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr, OSPI_B_SECTOR_SIZE_4K);


        /* Wait until erase operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);

        /* Write auto-calibration data to the flash */
        err = R_OSPI_B_Write(&g_ospi0_ctrl, (uint8_t *)&g_autocalibration_data, \
                            (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr, \
                            sizeof(g_autocalibration_data));


        /* Wait until write operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);

    }
    __NOP();
    return err;
}
/**********************************************************************************************************************
 End of function ospi_b_setup_calibrate_data
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup ospi_b_ep.c)
 **********************************************************************************************************************/
