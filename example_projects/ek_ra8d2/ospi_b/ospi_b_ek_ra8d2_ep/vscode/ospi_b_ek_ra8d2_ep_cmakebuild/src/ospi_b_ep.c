/***********************************************************************************************************************
 * File Name    : ospi_b_ep.c
 * Description  : Contains data structures and functions used in ospi_b_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "ospi_b_ep.h"

/* Global variables */
uint8_t g_read_data [OSPI_B_APP_DATA_SIZE] = {RESET_VALUE};
const uint8_t g_write_data [OSPI_B_APP_DATA_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

spi_flash_direct_transfer_t g_transfer =
{
    .command = RESET_VALUE,
    .address = RESET_VALUE,
    .data = RESET_VALUE,
    .command_length = RESET_VALUE,
    .address_length = RESET_VALUE,
    .data_length = RESET_VALUE,
    .dummy_cycles = RESET_VALUE
};

/* Function declarations */
static void ospi_b_flash_reset(void);
#if !(defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK))
static fsp_err_t ospi_b_config_register(uint32_t write_cmd, uint32_t read_cmd, uint8_t value);
#endif /* ! BOARD_RA8D1_EK || ! BOARD_RA8M1_EK */
static fsp_err_t ospi_b_wait_operation(uint32_t timeout);
static fsp_err_t ospi_b_setup_calibrate_data(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes OSPI module and Flash device.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t ospi_b_init(void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode */
    fsp_err_t err = FSP_SUCCESS;
    memset(&g_transfer, 0, sizeof(g_transfer));

    /* Open OSPI module */
    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Switch OSPI module to 1S-1S-1S mode to configure flash device */
    err = ospi_b_spi_protocol(SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset flash */
    ospi_b_flash_reset();

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    /* Write to CFR2V to configure Address Byte Length and Memory Array Read Latency */
    g_transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR2V_SPI];
    g_transfer.address_length = OSPI_B_ADDRESS_LENGTH_THREE;  /* Default Address Byte Length is 3 bytes */
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Write to CFR3V to configure Volatile Register Read Latency */
    g_transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR3V_SPI];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back and verify CFR2V register data */
    g_transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CFR2V_SPI];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_CFR2V_REGISTER != (uint8_t)g_transfer.data)
    {
        return FSP_ERR_ABORTED;
    }

    /* Read back and verify CFR3V register data */
    g_transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CFR3V_SPI];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_CFR3V_REGISTER != (uint8_t)g_transfer.data)
    {
        return FSP_ERR_ABORTED;
    }

#elif defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8M2_EK)
    /* Write to ADDR 00000300H of CR2 to configure dummy cycle */
    /* Read back and verify CR2 register data */
    err = ospi_b_config_register(
            OSPI_B_TRANSFER_WRITE_CR2_300H_SPI,
            OSPI_B_TRANSFER_READ_CR2_300H_SPI,
            OSPI_B_DATA_SET_CR2_300H);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#elif defined (BOARD_RA8D2_EK) || defined (BOARD_RA8T2_EK)
    /* Enter 4-bytes address */
    g_transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Write to ADDR 00000001H of CR to configure dummy cycle */
    /* Read back and verify CR register data */
    err = ospi_b_config_register(
           OSPI_B_TRANSFER_WRITE_CR_01H_SPI,
           OSPI_B_TRANSFER_READ_CR_01H_SPI,
           OSPI_B_DATA_SET_CR_01H);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA8D1_EK || BOARD_RA8M1_EK || BOARD_RA8E2_EK || BOARD_RA8P1_EK || BOARD_RA8M2_EK ||
        BOARD_RA8D2_EK || BOARD_RA8T2_EK */

    return ospi_b_setup_calibrate_data();
}

/*******************************************************************************************************************//**
 * @brief     This function resets the connected flash device.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void ospi_b_flash_reset(void)
{
#if defined (BOARD_RA8T2_EK)
    R_XSPI1->LIOCTL_b.RSTCS0 = 0;
#else
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
#endif /* BOARD_RA8T2_EK */

    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_PULSE, BSP_DELAY_UNITS_MICROSECONDS);

#if defined (BOARD_RA8T2_EK)
    R_XSPI1->LIOCTL_b.RSTCS0 = 1;
#else
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
#endif /* BOARD_RA8T2_EK */

    R_BSP_SoftwareDelay(OSPI_B_TIME_RESET_SETUP, BSP_DELAY_UNITS_MICROSECONDS);
}

#if!(defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK))
/*******************************************************************************************************************//**
 * @brief     This function writes a value to a flash configuration register and verifies the written value.
 * @param[in] write_cmd                 Index for register write.
 * @param[in] read_cmd                  Index for register read.
 * @param[in] value                     Expected register value.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_config_register(uint32_t write_cmd, uint32_t read_cmd, uint8_t value)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer = g_ospi_b_direct_transfer[write_cmd];
    g_transfer.data = value;
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    g_transfer = g_ospi_b_direct_transfer[read_cmd];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return ((uint8_t) g_transfer.data == value) ? FSP_SUCCESS : FSP_ERR_ABORTED;
}
#endif /* ! BOARD_RA8D1_EK || ! BOARD_RA8M1_EK */

/*******************************************************************************************************************//**
 * @brief      This function reads flash device ID.
 * @param[out] p_id                      Pointer to store device ID.
 * @retval     FSP_SUCCESS               Read successfully.
 * @retval     Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t ospi_b_read_device_id (uint32_t * const p_id)
{
    fsp_err_t err = FSP_SUCCESS;
    memset(&g_transfer, 0, sizeof(g_transfer));

    /* Read and check flash device ID */
    g_transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
               ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_DEVICE_ID_SPI]
               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_DEVICE_ID_OPI];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    if ((OSPI_B_DEVICE_ID != g_transfer.data) && (OSPI_B_DEVICE_HL_ID != g_transfer.data))
#elif defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) ||\
      defined (BOARD_RA8T2_EK)
    if (OSPI_B_DEVICE_ID != g_transfer.data)
#endif /* BOARD_RA8D1_EK || BOARD_RA8M1_EK || BOARD_RA8E2_EK || BOARD_RA8P1_EK || BOARD_RA8M2_EK ||
        BOARD_RA8D2_EK || BOARD_RA8T2_EK */
    {
        return FSP_ERR_ABORTED;
    }

    /* Get flash device ID */
    *p_id = g_transfer.data;

    return err;
}

/*******************************************************************************************************************//**
 * @brief         This function reads/writes raw data directly with the OctaFlash
 * @param[in,out] p_transfer            Pointer to transfer configuration structure.
 * @param[in]     direction             Transfer direction.
 * @retval        FSP_SUCCESS           Read/written successfully.
 * @retval        Any other error codes Failed to read/write.
 **********************************************************************************************************************/
fsp_err_t ospi_b_transfer(spi_flash_direct_transfer_t * const p_transfer, spi_flash_direct_transfer_dir_t direction)
{
    return R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, p_transfer, direction);
}

/*******************************************************************************************************************//**
 * @brief     This function sets the SPI protocol.
 * @param[in] spi_protocol              Target SPI protocol.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t ospi_b_spi_protocol(spi_flash_protocol_t spi_protocol)
{
    return R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, spi_protocol);
}

/*******************************************************************************************************************//**
 * @brief     This function enables write and verifies the read data.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled and verified successfully.
 * @retval    Any other error codes     Failed to enable or verify.
 **********************************************************************************************************************/
fsp_err_t ospi_b_write_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;
    memset(&g_transfer, 0, sizeof(g_transfer));

    /* Transfer write enable command */
    g_transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
               ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read Status Register */
    g_transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
               ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = ospi_b_transfer(&g_transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check write enable bit in status register */
    if (OSPI_B_WEN_BIT_MASK != (g_transfer.data & OSPI_B_WEN_BIT_MASK))
    {
        return FSP_ERR_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function performs a write operation on the flash device.
 * @param[in]  p_address                Pointer to flash device memory address.
 * @param[out] p_time                   Pointer to store execute time.
 * @retval     FSP_SUCCESS              Written successfully.
 * @retval     Any other error codes    Failed to write.
 **********************************************************************************************************************/
fsp_err_t ospi_b_write_operation(uint8_t * const p_address, uint32_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t erase_time = RESET_VALUE;

    /* Erase sector before write data to flash device */
    err = ospi_b_erase_operation(p_address, &erase_time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start measure */
    err = timer_start_measure();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Write data to flash device */
    err = R_OSPI_B_Write(&g_ospi_b_ctrl, g_write_data, p_address, OSPI_B_APP_DATA_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait until write operation completes */
    err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return timer_get_measure(p_time);
}

/*******************************************************************************************************************//**
 * @brief      This function performs a read operation on the flash device.
 * @param[in]  p_address                Pointer to flash device memory address.
 * @param[out] p_time                   Pointer to store execute time.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t ospi_b_read_operation(uint8_t * const p_address, uint32_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clean read buffer */
    memset(&g_read_data, RESET_VALUE, OSPI_B_APP_DATA_SIZE);

    /* Start measure */
    err = timer_start_measure();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read data from flash device */
    memcpy(&g_read_data, p_address, OSPI_B_APP_DATA_SIZE);

    return timer_get_measure(p_time);
}

/*******************************************************************************************************************//**
 * @brief      This function erases sector on the flash device.
 * @param[in]  p_address                Pointer to flash device memory address.
 * @param[out] p_time                   Pointer to store execute time.
 * @retval     FSP_SUCCESS              Erased successfully.
 * @retval     Any other error codes    Failed to erase.
 **********************************************************************************************************************/
fsp_err_t ospi_b_erase_operation(uint8_t * const p_address, uint32_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t sector_size = RESET_VALUE;
    uint32_t erase_timeout = RESET_VALUE;

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK)
    /* Check sector size according to input address pointer, described in S28HS512T data sheet */
    if (OSPI_B_SECTOR_4K_END_ADDRESS < (uint32_t)p_address)
    {
        sector_size = OSPI_B_SECTOR_SIZE_256K;
        erase_timeout = OSPI_B_TIME_ERASE_256K;
    }
    else
    {
        sector_size = OSPI_B_SECTOR_SIZE_4K;
        erase_timeout = OSPI_B_TIME_ERASE_4K;
    }
#elif defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) ||\
      defined (BOARD_RA8T2_EK)
    sector_size = OSPI_B_SECTOR_SIZE_4K;
    erase_timeout = OSPI_B_TIME_ERASE_4K;
#endif  /* BOARD_RA8D1_EK || BOARD_RA8M1_EK || BOARD_RA8E2_EK || BOARD_RA8P1_EK || BOARD_RA8M2_EK ||
        BOARD_RA8D2_EK || BOARD_RA8T2_EK */

    /* Start measure */
    err = timer_start_measure();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Perform erase sector */
    err = R_OSPI_B_Erase(&g_ospi_b_ctrl, p_address, sector_size);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait till operation completes */
    err = ospi_b_wait_operation(erase_timeout);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return timer_get_measure(p_time);
}

/*******************************************************************************************************************//**
 * @brief     This function waits until OSPI operation completes.
 * @param[in] timeout                   Maximum waiting time.
 * @retval    FSP_SUCCESS               SPI operation completes successfully.
 * @retval    Any other error codes     Timeout during OSPI operation completes.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_wait_operation(uint32_t timeout)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        err = R_OSPI_B_StatusGet(&g_ospi_b_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);
        timeout--;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets up the auto-calibrate data for the flash.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Calibrated successfully.
 * @retval    Any other error codes     Failed to calibrate.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;
    ospi_b_extended_cfg_t * p_extended_cfg = (ospi_b_extended_cfg_t *)g_ospi_b_cfg.p_extend;

#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8T2_EK)
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
#endif /* BOARD_RA8D1_EK || BOARD_RA8M1_EK || BOARD_RA8E2_EK || BOARD_RA8P1_EK || BOARD_RA8M2_EK ||
        BOARD_RA8D2_EK || BOARD_RA8T2_EK */

    /* Verify auto-calibration data */
    if (RESET_VALUE != memcmp((uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
            &g_autocalibration_data, sizeof(g_autocalibration_data)))
    {
        /* Erase the flash sector that stores auto-calibration data */
        err = R_OSPI_B_Erase(&g_ospi_b_ctrl,
                            (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr, OSPI_B_SECTOR_SIZE_4K);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait until erase operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Write auto-calibration data to the flash */
        err = R_OSPI_B_Write(&g_ospi_b_ctrl, (uint8_t *)&g_autocalibration_data,
                             (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
                             sizeof(g_autocalibration_data));
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait until write operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes opened OSPI_B module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ospi_b_deinit(void)
{
    if (g_ospi_b_ctrl.open)
    {
        /* Close the opened OSPI_B */
        R_OSPI_B_Close(&g_ospi_b_ctrl);
    }
}
