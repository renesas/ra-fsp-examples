/***********************************************************************************************************************
 * File Name    : board_qspi.c
 * Description  : Contains data structures and functions used in board_qspi.c and littlefs_qspi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "board_qspi.h"

/* Function declaration */
static fsp_err_t qspi_wait_operation (uint32_t timeout);

/***********************************************************************************************************************
 *  Function Name: qspi_init
 *  Description  : This function initializes QSPI module and Flash device.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful initialization of QSPI module and Flash device
 *                 FSP_ERR_ABORTED Upon incorrect read data
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t qspi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t write_data[DATA_SIZE]  = {RESET_VALUE};
    uint8_t status_reg             = RESET_VALUE;
    uint8_t config_reg             = RESET_VALUE;

    spi_flash_status_t wip = {RESET_VALUE};

    /* Open QSPI module */
    err = R_QSPI_Open(&g_qspi_ctrl, &g_qspi_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_Open API FAILED!!!**\r\n");

    /* Exit Quad mode in QSPI flash device if any
     * This enable RESET function of RESET#/SIO3 pin */
    write_data[ZERO] = EXIT_QPI;
    R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, false);

    /* Set RESET#/SIO3 pin in I/O mode */
    R_IOPORT_PinCfg(&g_ioport_ctrl, RESET_SIO3_PIN, IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH);

    /* Reset QSPI flash device */
    R_IOPORT_PinWrite(&g_ioport_ctrl, RESET_SIO3_PIN, BSP_IO_LEVEL_LOW);

    /* Wait 10 ms */
    R_BSP_SoftwareDelay(QSPI_TIME_RESET, QSPI_TIME_UNIT);
    R_IOPORT_PinWrite(&g_ioport_ctrl, RESET_SIO3_PIN, BSP_IO_LEVEL_HIGH);

    /* Set RESET#/SIO3 pin in peripheral mode */
    R_IOPORT_PinCfg(&g_ioport_ctrl, RESET_SIO3_PIN,  ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_QSPI));

    /* Set QSPI protocol to Extended mode */
    R_QSPI->SFMSPC_b.SFMSPI = QSPI_EXTENDED_SPI_PROTOCOL;

    /* Read Configuration register */
    write_data[ZERO] = READ_CONFIG_REG;
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, true);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

    err = R_QSPI_DirectRead(&g_qspi_ctrl, &config_reg, LENGTH_ONE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectRead API FAILED!!!**\r\n");

    /* Read Status register */
    write_data[ZERO] = READ_STATUS_REG;
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, true);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

    err = R_QSPI_DirectRead(&g_qspi_ctrl, &status_reg, LENGTH_ONE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectRead API FAILED!!!**\r\n");

    /* Set the QE bit in the Status register if it has not been set up yet */
    if (QE_BIT_ENABLE != (status_reg & QE_BIT_ENABLE))
    {
        /* Write Enable */
        write_data[ZERO] = WRITE_ENABLE;
        err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, false);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

        /* Write Status register */
        write_data[ZERO] = WRITE_STATUS_REG;

        /* Set QE bit = 1 */
        write_data[ONE] = (uint8_t)(status_reg | QE_BIT_ENABLE);
        write_data[TWO] = config_reg;
        err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_THREE, false);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

        wip.write_in_progress = true;
        while (wip.write_in_progress)
        {
            err = R_QSPI_StatusGet(&g_qspi_ctrl, &wip);
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_StatusGet API FAILED!!!**\r\n");
        }

        /* Write Disable */
        write_data[ZERO] = WRITE_DISABLE;
        err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, false);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");
    }

    /* Enter 4-byte mode in the QSPI device */
    write_data[ZERO] = ENTER_4BYTE_MODE;
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, false);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

    /* Read Configuration register to verify 4BYTE bit*/
    write_data[ZERO] = READ_CONFIG_REG;
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, true);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

    err = R_QSPI_DirectRead(&g_qspi_ctrl, &config_reg, LENGTH_ONE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectRead API FAILED!!!**\r\n");
    if (FOUR_BYTE_BIT_ENABLE != (config_reg & FOUR_BYTE_BIT_ENABLE))
    {
        err = FSP_ERR_ABORTED;
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nData mismatched when entering 4-byte mode!!!\r\n");
    }

    /* Enable QPI in the QSPI device */
    write_data[ZERO] = ENABLE_QPI;
    err = R_QSPI_DirectWrite(&g_qspi_ctrl, write_data, LENGTH_ONE, false);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_DirectWrite API FAILED!!!**\r\n");

    /* Setting QSPI protocol to Quad mode */
    R_QSPI->SFMSPC_b.SFMSPI = QSPI_QUAD_SPI_PROTOCOL;

    /* Close QSPI module */
    err = R_QSPI_Close(&g_qspi_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_Close API FAILED!!!**\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function qspi_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: erase_qspi_flash
 *  Description  : This function erases the flash memory using lower level flash APIs.
 *  Arguments    : erase_size      The size of flash to be erased
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t erase_qspi_flash(uint32_t erase_size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open QSPI module */
    if (MODULE_CLOSE == g_qspi_ctrl.open)
    {
        err = R_QSPI_Open(&g_qspi_ctrl, &g_qspi_cfg);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_Open API FAILED!!!**\r\n");
    }

    /* Erase flash */
    if (SPI_FLASH_ERASE_SIZE_CHIP_ERASE == erase_size)
    {
        APP_PRINT("\r\n\r\nErase the entire qspi flash...\r\n\r\n");
        APP_PRINT("\r\nThis operation will take several minutes. Please wait!\r\n");

        err = R_QSPI_Erase(&g_qspi_ctrl, (uint8_t *)QSPI_START_ADDRESS, SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_Erase API FAILED!!!**\r\n");

        err = qspi_wait_operation(UINT32_MAX);
        APP_ERR_RET(FSP_SUCCESS != err, err, "qspi_wait_operation failed!!!\r\n");

        APP_PRINT("\r\n\r\nErased entire QSPI flash successfully!\r\n");
        APP_PRINT("\r\nPlease FORMAT FILE SYSTEM after erase the entire QSPI flash!\r\n");
    }
    else if (QSPI_32KB_BLOCK_SIZE == erase_size)
    {
        APP_PRINT("\r\n\r\nErase the first 32 KB of the QSPI flash...\r\n\r\n");

        err = R_QSPI_Erase(&g_qspi_ctrl, (uint8_t *)QSPI_START_ADDRESS, QSPI_32KB_BLOCK_SIZE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_QSPI_Erase API FAILED!!!**\r\n");

        err = qspi_wait_operation(QSPI_TIME_32KB_BLOCK_ERASE);
        APP_ERR_RET(FSP_SUCCESS != err, err, "qspi_wait_operation failed!!!\r\n");

        APP_PRINT("\r\n\r\nErased the first 32 KB of the QSPI flash successfully!\r\n");
        APP_PRINT("\r\nPlease FORMAT FILE SYSTEM after erase the first 32 KB of the QSPI flash!\r\n");
    }
    else
    {
        APP_PRINT("\r\n\r\nThis EP only supports erase entire flash or 32 KB flash!\r\n\r\n");
        return err;
    }

    return err;
}
/***********************************************************************************************************************
* End of function erase_qspi_flash
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: qspi_wait_operation
 *  Description  : This function waits until QSPI operation completes.
 *  Arguments    : timeout         Maximum waiting time
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 FSP_ERR_TIMEOUT Upon time out
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t qspi_wait_operation(uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;

    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        R_QSPI_StatusGet(&g_qspi_ctrl, &status);
        APP_ERR_RET(FSP_SUCCESS != err, err, "**R_QSPI_StatusGet API FAILED!!!**\r\n");
        if(RESET_VALUE == timeout)
        {
            err = FSP_ERR_TIMEOUT;
            APP_ERR_RET(FSP_SUCCESS != err, err, "QSPI time out occurred!!!\r\n");
        }
        R_BSP_SoftwareDelay(1, QSPI_TIME_UNIT);
        timeout --;

        APP_PRINT(RESET_VALUE == (timeout % PERIODIC_PRINT_OUT) ? ".":"");
        APP_PRINT(RESET_VALUE == (timeout % PERIODIC_LINE_FEED) ? "\r\n":"");
    }

    return err;
}
/***********************************************************************************************************************
* End of function qspi_wait_operation
***********************************************************************************************************************/
