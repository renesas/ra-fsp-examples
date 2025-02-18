/***********************************************************************************************************************
 * File Name    : setup_qspi.c
 * Description  : Contains data structures and functions used in setup_qspi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "setup_qspi.h"

fsp_err_t setup_qspi(spi_flash_ctrl_t * p_ctrl, spi_flash_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t write_data[8];
    uint8_t read_data[8];

    uint8_t status_reg;
    uint8_t config_reg;


    err = R_QSPI_Open(p_ctrl, p_cfg);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    /*
     * Exit Quad mode in QSPI flash device if any
     * This enabling RESET function of RESET#/SIO3 pin
     */
#ifdef QSPI_DEVICE_MX25L
    write_data[0] = EXIT_QPI;
    R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
#endif
    /* Set RESET#/SIO3 pin in I/O mode*/
    R_IOPORT_PinCfg(&g_ioport_ctrl, QSPI_SIO3, IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH );

    /* Reset QSPI flash device */
    R_IOPORT_PinWrite(&g_ioport_ctrl, QSPI_SIO3, BSP_IO_LEVEL_LOW );
    /** Wait 10 ms. */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, QSPI_SIO3, BSP_IO_LEVEL_HIGH );


    /* Set RESET#/SIO3 pin in peripheral mode*/
    R_IOPORT_PinCfg(&g_ioport_ctrl, QSPI_SIO3,  ((uint32_t) IOPORT_CFG_DRIVE_HIGH | \
            (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_QSPI) );

    /* Setting QSPI protocol to Extended mode */
    R_QSPI->SFMSPC_b.SFMSPI = QSPI_EXTENDED_SPI_PROTOCOL;

    write_data[0] = RDID; /* Read ID */
    memset(read_data, 0, sizeof(read_data));
    err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, true);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    err = R_QSPI_DirectRead(p_ctrl, read_data, 3);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    if ((MX_MANUFACTURER_ID != read_data[0]) || (MEMORY_TYPE != read_data[1]) || (MEMORY_DENSITY != read_data[2]))
    {
      return FSP_ERR_ASSERTION;
    }
#ifdef QSPI_DEVICE_MX25L
    write_data[0] = READ_CONFIG_REG;  /* Read Config register */
    err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, true);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    err = R_QSPI_DirectRead(p_ctrl, &config_reg, 1);
    if (FSP_SUCCESS != err)
    {
      return err;
    }
    write_data[0] = READ_STATUS_REG; /* Read Status register */
#elif QSPI_DEVICE_AT25SF128A
    write_data[0] = READ_STATUS_REG_2; /* Read Status register */
#endif
    err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, true);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    err = R_QSPI_DirectRead(p_ctrl, &status_reg, 1);
    if (FSP_SUCCESS != err)
    {
      return err;
    }
#ifdef QSPI_DEVICE_MX25L
    if ((1<<6) != (status_reg & (1<<6))) /* Set the QE bit in the status register if not already set */
    {
        write_data[0] = WRITE_ENABLE; /* Write Enable */
        err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
        if (FSP_SUCCESS != err)
        {
          return err;
        }

        write_data[0] = WRITE_STATUS_REG; /* Write Status register */
        write_data[1] = (uint8_t)(status_reg | (1<<6)); /* Set QE bit = 1 */
        write_data[2] = config_reg;
        err = R_QSPI_DirectWrite(p_ctrl, write_data, 3, false);

        if (FSP_SUCCESS != err)
        {
          return err;
        }

        spi_flash_status_t wip;
        wip.write_in_progress = 1;
        while (1 == wip.write_in_progress)
        {
          err = R_QSPI_StatusGet(p_ctrl, &wip);
          if (FSP_SUCCESS != err)
          {
              return err;
          }
        }

        write_data[0] = WRITE_DISABLE; /* Write disable */
        err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
        if (FSP_SUCCESS != err)
        {
          return err;
        }
    }
    write_data[0] = ENABLE_4BYTE; /* Enable 4 Byte addressing in the QSPI device */
    err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    write_data[0] = ENABLE_QPI; /* Enable QPI in the QSPI device */
    err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    /* Setting QSPI protocol to Quad mode */
    R_QSPI->SFMSPC_b.SFMSPI = QSPI_QUAD_SPI_PROTOCOL;

    /* Check we can still communicate with the QSPI memory */
    write_data[0] = QPIID; /* Read ID in QPI */
    memset(read_data, 0, sizeof(read_data));
    err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, true);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    err = R_QSPI_DirectRead(p_ctrl, read_data, 3);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    if ((MX_MANUFACTURER_ID != read_data[0]) || (MEMORY_TYPE != read_data[1]) || (MEMORY_DENSITY != read_data[2]))
    {
      return FSP_ERR_ASSERTION;
    }
#elif QSPI_DEVICE_AT25SF128A
    if ((1<<1) != (status_reg & (1<<1))) /* Set the QE bit in the status register if not already set */
    {
        write_data[0] = WRITE_ENABLE; /* Write Enable */
        err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
        if (FSP_SUCCESS != err)
        {
          return err;
        }

        write_data[0] = WRITE_STATUS_REG_2; /* Write Status register */
        write_data[1] = (uint8_t)(status_reg | (1<<1)); /* Set QE bit = 1 */
        err = R_QSPI_DirectWrite(p_ctrl, write_data, 2, false);
        if (FSP_SUCCESS != err)
        {
          return err;
        }

        spi_flash_status_t wip;
        wip.write_in_progress = 1;
        while (1 == wip.write_in_progress)
        {
          err = R_QSPI_StatusGet(p_ctrl, &wip);
          if (FSP_SUCCESS != err)
          {
              return err;
          }
        }

        write_data[0] = WRITE_DISABLE; /* Write disable */
        err = R_QSPI_DirectWrite(p_ctrl, write_data, 1, false);
        if (FSP_SUCCESS != err)
        {
          return err;
        }
    }
#endif

    err = R_QSPI_Close(p_ctrl);
    if (FSP_SUCCESS != err)
    {
      return err;
    }

    return err;

}
