/***********************************************************************************************************************
 * File Name    : sci_spi_ep.c
 * Description  : Contains data structures and functions used in sci_spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sci_spi_ep.h"

/* Master transfer event completion flag */
volatile spi_event_t g_master_event_flag = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function initializes SCI SPI module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t sci_spi_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open SCI SPI module */
    err = R_SCI_SPI_OPEN(&g_spi_ctrl, &g_spi_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function transmits data to a SPI device.
 * @param[in] p_api_ctrl                Pointer to the control structure.
 * @param[in] p_src                     Pointer to the source buffer.
 * @param[in] length                    The number of bytes to transfer.
 * @param[in] bit_width                 Invalid for SCI_SPI (Set to SPI_BIT_WIDTH_8_BITS).
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
fsp_err_t sci_spi_write(spi_ctrl_t * const p_api_ctrl, void const * p_src, uint32_t const length,
                        spi_bit_width_t const bit_width)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit data */
    err = R_SCI_SPI_WRITE(p_api_ctrl, p_src, length, bit_width);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function receives data from an SPI device.
 * @param[in]  p_api_ctrl                Pointer to the control structure.
 * @param[out] p_dest                    Pointer to the destination buffer.
 * @param[in]  length                    The number of bytes to transfer.
 * @param[in]  bit_width                 Invalid for SCI_SPI (Set to SPI_BIT_WIDTH_8_BITS).
 * @retval     FSP_SUCCESS               Received successfully.
 * @retval     Any other error codes     Failed to receive.
 **********************************************************************************************************************/
fsp_err_t sci_spi_read(spi_ctrl_t * const p_api_ctrl, void * p_dest, uint32_t const length,
                       spi_bit_width_t const bit_width)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Receive data */
    err = R_SCI_SPI_READ(p_api_ctrl, p_dest, length, bit_width);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function simultaneously transmits data to SPI device while receiving data from SPI device.
 * @param[in]  p_api_ctrl               Pointer to the control structure.
 * @param[in]  p_src                    Pointer to the source buffer.
 * @param[out] p_dest                   Pointer to the destination buffer.
 * @param[in]  length                   The number of bytes to transfer.
 * @param[in]  bit_width                Invalid for SCI_SPI (Set to SPI_BIT_WIDTH_8_BITS).
 * @retval     FSP_SUCCESS              Transmitted and received successfully.
 * @retval     Any other error codes    Failed to transmit or receive.
 **********************************************************************************************************************/
fsp_err_t sci_spi_write_read(spi_ctrl_t * const p_api_ctrl, void const * p_src, void * p_dest, uint32_t const length,
                             spi_bit_width_t const bit_width)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit and receive data */
    err = R_SCI_SPI_WRITE_READ(p_api_ctrl, p_src, p_dest, length, bit_width);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes SCI SPI module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sci_spi_deinit(void)
{
    if (g_spi_ctrl.open)
    {
        /* Close the opened SCI SPI module */
        R_SCI_SPI_CLOSE(&g_spi_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function asserts the CS (Chip Select) pin for the SPI device.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Asserted successfully.
 * @retval    Any other error codes     Failed to assert.
 **********************************************************************************************************************/
fsp_err_t cs_assert(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Assert CS pin */
    err = R_IOPORT_PinWrite(&g_ioport_ctrl, CS_PIN, BSP_IO_LEVEL_HIGH);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-asserts the CS (Chip Select) pin for the SPI device.
 * @param[in] None.
 * @retval    FSP_SUCCESS               De-asserted successfully.
 * @retval    Any other error codes     Failed to de-assert.
 **********************************************************************************************************************/
fsp_err_t cs_de_assert(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* De-assert CS pin */
    err = R_IOPORT_PinWrite(&g_ioport_ctrl, CS_PIN, BSP_IO_LEVEL_LOW);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     sci_spi_callback function.
 * @param[in] p_args        Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void sci_spi_callback(spi_callback_args_t *p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_master_event_flag = SPI_EVENT_TRANSFER_COMPLETE;
    }
    else
    {
        /* Update the flag here to capture and handle all other error events */
        g_master_event_flag = SPI_EVENT_TRANSFER_ABORTED;
    }
}
