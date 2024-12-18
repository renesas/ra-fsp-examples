/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "qspi_ep.h"
#include "r_qspi.h"
#include "downloader_thread.h"
#include "qspi_operations.h"

/*******************************************************************************************************************//**
 *  @brief       Close QSPI module
 *  @param[IN]   spi_protocol mode
 *  @retval      None
 **********************************************************************************************************************/
void deinit_qspi(const spi_flash_protocol_t spi_protocol_mode)
{
    fsp_err_t error = FSP_SUCCESS;

    /* if QPI is active mode then Exit QPI mode from flash device before QSPI close */
    if (SPI_FLASH_PROTOCOL_QPI == spi_protocol_mode)
    {
        uint8_t data_exit_qpi = QSPI_MX25L_CMD_EXIT_QPI_MODE;

        error = R_QSPI_DirectWrite(&g_qspi0_ctrl, &data_exit_qpi, ONE_BYTE, false);
        if (FSP_SUCCESS != error)
        {
            while(1);
        }
    }

       /* close QSPI module */
    error = R_QSPI_Close(&g_qspi0_ctrl);
    if (FSP_SUCCESS != error)
    {
        while(1);
    }


}
/*******************************************************************************************************************//**
 *  @brief       wait for QSPI flash device status register to get idle till operation is in progress
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS or any other possible error codes
 **********************************************************************************************************************/
fsp_err_t get_flash_status(void)
{
    spi_flash_status_t status = {.write_in_progress = true};
    int32_t time_out          = (INT32_MAX);
    fsp_err_t err             = FSP_SUCCESS;

    do
    {
        /* Get status from QSPI flash device */
        err = R_QSPI_StatusGet(&g_qspi0_ctrl, &status);
        if (FSP_SUCCESS!= err)
        {
            return err;
        }

        /* Decrement time out to avoid infinite loop in case of consistent failure */
        --time_out;

        if ( 0 >= time_out)
        {
            return FSP_ERR_TIMEOUT;
        }

    }while (false != status.write_in_progress);

    return err;
}

void init_qspi(void)
{
    fsp_err_t err                                  = FSP_SUCCESS;

       uint8_t   data_sreg[SREG_SIZE]                 = STATUS_REG_PAYLOAD;

       /* Banner information */

       {
           /* open QSPI in extended SPI mode */
           err = R_QSPI_Open(&g_qspi0_ctrl, &g_qspi0_cfg);
           if (FSP_SUCCESS != err)
           {
               while(1);
           }
       }

       /* write enable for further operations */
       err = R_QSPI_DirectWrite(&g_qspi0_ctrl, &(g_qspi0_cfg.write_enable_command), ONE_BYTE, false);
       if (FSP_SUCCESS != err)
       {

           /* close QSPI module which is currently in extended SPI mode only */
           deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

       }
       else
       {
           err = get_flash_status();
           if (FSP_SUCCESS != err)
           {

               /* close QSPI module which is currently in extended SPI mode only */
               deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

           }
       }

       /*
        * write QSPI flash status register
        * This is required to make sure the device is ready for general
        * read write operation,
        * This performs settings such as physical reset,WP hardware pin disable,
        * block protection lock bits clearing.
        * for more details please refer Mx25L data sheet.
        */
       err = R_QSPI_DirectWrite(&g_qspi0_ctrl, data_sreg, SREG_SIZE, false);
       if (FSP_SUCCESS != err)
       {

           /* close QSPI module which is currently in extended SPI mode only */
           deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

       }
       else
       {
           err = get_flash_status();
           if (FSP_SUCCESS != err)
           {

               /* close QSPI module which is currently in extended SPI mode only */
               deinit_qspi(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

           }
       }

}


