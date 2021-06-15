/***********************************************************************************************************************
 * File Name    : board_qspi.c
 * Description  : Contains macros data structures and functions used in board_qspi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "board_qspi.h"

static struct
{
    uint8_t manufacturer_id;
    uint8_t memory_type;
    uint8_t memory_capacity;
} qspi_device;

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Initializes QSPI.
 *
 * This function initializes QSPI and Micron Serial Flash Memory device on the board.
 *
 * @note This function is executed once after reset.
 **********************************************************************************************************************/
void bsp_qspi_init (void)
{
    R_MSTP->MSTPCRB_b.MSTPB6 = 0u;     // release QSPI from module stop
    while (0 != R_MSTP->MSTPCRB_b.MSTPB6)
    {
        ;                              // wait for module stop bit to become 0
    }

    /* Exit Quad mode in serial Flash device if any:
     * The steps below are to make sure that both serial flash device and QSPI controller are started in SPI mode
     * whether it is power-on reset or MCU reset
     * *//* Setting QSPI protocol to Quad mode */
    R_QSPI->SFMSPC_b.SFMSPI = QSPI_QUAD_SPI_PROTOCOL;

    /* Enter direct comm mode */
    R_QSPI->SFMCMD = 1u;

    /* Exit Quad mode in serial flash device if any
     * This enabling RESET function of RESET#/SIO3 pin
     */
    R_QSPI->SFMCOM = QSPI_COMMAND_EXIT_QPI;

    /* Close SPI bus cycle */
    R_QSPI->SFMCMD = 1u;

    /* Reset QSPI flash device using RESET#/SIO3 pin */
    R_BSP_PinAccessEnable();

    /* Set P310 as I/O port function and driving it low for the specified time to reset QSPI flash device */
    R_PFS->PORT[5].PIN[5].PmnPFS_b.PMR  = 0u;
    R_PFS->PORT[5].PIN[5].PmnPFS_b.PODR = 0u;
    R_PFS->PORT[5].PIN[5].PmnPFS_b.PDR  = 1u;

    /* Keep RESET line low for 50us */
    R_BSP_SoftwareDelay(50u, BSP_DELAY_UNITS_MICROSECONDS);

    /* Set P310 pin back to QSPI function */
    R_PFS->PORT[5].PIN[5].PmnPFS_b.PMR = 1u;

    R_BSP_PinAccessDisable();

    /* Exit direct comm mode to update SPI mode */
    R_QSPI->SFMCMD = 0u;

    /* Setting QSPI protocol in extended SPI mode, now both QSPI controller and serial flash device are in SPI mode*/
    R_QSPI->SFMSPC_b.SFMSPI = QSPI_EXTENDED_SPI_PROTOCOL;

    R_QSPI->SFMCMD = 1u;

    /* Identify the QSPI device */
    R_QSPI->SFMCOM              = QSPI_COMMAND_READ_ID;
    qspi_device.manufacturer_id = (uint8_t) R_QSPI->SFMCOM;
    qspi_device.memory_type     = (uint8_t) R_QSPI->SFMCOM;
    qspi_device.memory_capacity = (uint8_t) R_QSPI->SFMCOM;
    R_QSPI->SFMCMD              = 1u;

    if ((BSP_PRV_MX25_QSPI_MANUFACTURER_ID == qspi_device.manufacturer_id) &&
        (BSP_PRV_MX25_QSPI_MEMORY_TYPE == qspi_device.memory_type) &&
        (BSP_PRV_MX25_QSPI_MEMORY_CAPACITY == qspi_device.memory_capacity))
    {
        /* Enable Quad mode in serial flash device */
        R_QSPI->SFMCOM = QSPI_COMMAND_ENABLE_QPI;

        /* Close SPI Bus cycle */
        R_QSPI->SFMCMD = 1u;

        /* Exit direct comm mode and enter ROM Access mode to set QSPI registers.
         * Now both QSPI Flash controller and serial flash device are in Quad mode */
        R_QSPI->SFMCMD = 0u;

        R_QSPI->SFMSPC = (uint32_t) ((BSP_QSPI_VAL_SFMSDE << 4) | BSP_QSPI_VAL_SFMSPI);
        R_QSPI->SFMSKC = (uint32_t) ((BSP_QSPI_VAL_SFMDTY << 5) | BSP_QSPI_VAL_SFMDV);
        R_QSPI->SFMSDC = (uint32_t) ((BSP_QSPI_VAL_SFMXD << 8) | (BSP_QSPI_VAL_SFMXEN << 7) | \
                                     (BSP_QSPI_VAL_SFMXST << 6) | (BSP_QSPI_VAL_SFMDN));
        R_QSPI->SFMSSC = (uint32_t) ((BSP_QSPI_VAL_SFMSLD << 5) | (BSP_QSPI_VAL_SFMSHD << 4) | \
                                     (BSP_QSPI_VAL_SFMSW));
        R_QSPI->SFMSMD = (uint32_t) ((BSP_QSPI_VAL_SFMPFE << 6) | (BSP_QSPI_VAL_SFMRM));

        /* Enter ROM access mode */
        R_QSPI->SFMCMD = 0u;
    }
    else
    {
        /* Not Macronix MX25L12845G, disable QSPI */
        R_MSTP->MSTPCRB_b.MSTPB6 = 1u; // set QSPI from module stop
    }
}
