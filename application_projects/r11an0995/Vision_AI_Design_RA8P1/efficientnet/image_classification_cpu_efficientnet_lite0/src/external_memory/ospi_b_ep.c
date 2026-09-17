/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : ospi_b_ep.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#include "common_util.h"
#include "ospi_b_commands.h"
#include "ospi_b_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ospi_b_ep.c
 * @{
 **********************************************************************************************************************/

#define RESET_VALUE (0)

#define OSPI_B_EP_ERR_RETURN(...)

/* External variables */
extern spi_flash_direct_transfer_t g_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX];

/* function declarations*/
static fsp_err_t ospi_b_write_enable ();

/*******************************************************************************************************************//**
 * @brief       This functions enables write and verify the read data.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful operation
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable ()
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer;

    memset(&transfer, RESET_VALUE, sizeof(transfer));

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
                    ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
                                               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_ospi_b_ctrl.spi_protocol)
                    ? g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
                                               : g_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Check Write Enable bit in Status Register */
    if(!(0x02 & transfer.data))
    {
        OSPI_B_EP_ERR_RETURN(FSP_ERR_ABORTED, "Write enable FAILED\r\n");
    }
    return err;
}



/*******************************************************************************************************************//**
 * @brief       This functions initializes OSPI module and Flash device.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t ospi_b_init ()
{
    fsp_err_t                   err             = FSP_SUCCESS;

    err = R_OSPI_B_Open(&g_ospi_b_ctrl, &g_ospi_b_cfg);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_Open API FAILED \r\n");

    /* Reset flash device */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MICROSECONDS);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MICROSECONDS);

    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_SpiProtocolSet API FAILED \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures ospi to extended spi mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful transition to spi operating mode.
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_b_set_protocol_to_spi ()
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer;

    memset(&transfer, RESET_VALUE, sizeof(transfer));

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    OSPI_B_EP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    /* Write to CFR2V Register to Configure spi mode */
    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR2V_OPI];
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    /* Switch OSPI module mode to SPI mode */
    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_SpiProtocolSet API FAILED \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures ospi to opi mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful transition to opi operating mode.
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t ospi_b_set_protocol_to_opi ()
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer;

    memset(&transfer, RESET_VALUE, sizeof(transfer));

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    OSPI_B_EP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    transfer = g_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR2V_SPI];
    transfer.address = 0x200U;
    transfer.data = 0x2U; /* DQS on STR mode */
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = ospi_b_write_enable();
    OSPI_B_EP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    transfer.address = 0x300U;
    transfer.data = 0x5U;           /* 10 dummy cycles for 99M */
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = ospi_b_write_enable();
    OSPI_B_EP_ERR_RETURN(err, "ospi_b_write_enable FAILED \r\n");

    transfer.address = 0x00U;
    transfer.data = 0x2U;   /* DTR */
    err = R_OSPI_B_DirectTransfer(&g_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_B_DirectTransfer API FAILED \r\n");

    err = R_OSPI_B_SpiProtocolSet(&g_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
    OSPI_B_EP_ERR_RETURN(err, "R_OSPI_SpiProtocolSet API FAILED \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup ospi_b_ep.c)
 **********************************************************************************************************************/
