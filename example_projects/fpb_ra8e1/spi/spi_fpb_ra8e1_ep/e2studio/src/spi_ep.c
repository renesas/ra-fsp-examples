/***********************************************************************************************************************
 * File Name    : spi_ep.c
 * Description  : Contains data structures and functions used in spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "spi_ep.h"

/* Master transfer event completion flag */
volatile spi_event_t g_master_event_flag = (spi_event_t) RESET_VALUE;

/* Slave transfer event completion flag */
volatile spi_event_t g_slave_event_flag = (spi_event_t) RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     This function initializes SPI Master and Slave modules.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t spi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize SPI Master module */
    err = SPI_OPEN(&g_spi_master_ctrl, &g_spi_master_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize SPI Slave module */
    err = SPI_OPEN(&g_spi_slave_ctrl, &g_spi_slave_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function transmits data to a SPI device.
 * @param[in] p_api_ctrl                Pointer to control structure.
 * @param[in] p_src                     Buffer to transmit data.
 * @param[in] length                    Number of transfers.
 * @param[in] bit_width                 Data frame size.
 * @retval    FSP_SUCCESS               Transmitted successfully.
 * @retval    Any other error codes     Failed to transmit.
 **********************************************************************************************************************/
fsp_err_t spi_write(spi_ctrl_t * const p_api_ctrl, void const * p_src, uint32_t const length,
                    spi_bit_width_t const bit_width)
{
    fsp_err_t err = FSP_SUCCESS;

    err = SPI_WRITE(p_api_ctrl, p_src, length, bit_width);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function receives data from a SPI device.
 * @param[in]  p_api_ctrl               Pointer to control structure.
 * @param[out] p_dest                   Buffer to store received data.
 * @param[in]  length                   Number of transfers.
 * @param[in]  bit_width                Data frame size.
 * @retval     FSP_SUCCESS              Received successfully.
 * @retval     Any other error codes    Failed to receive.
 **********************************************************************************************************************/
fsp_err_t spi_read(spi_ctrl_t * const p_api_ctrl, void * p_dest, uint32_t const length,
                   spi_bit_width_t const bit_width)
{
    fsp_err_t err = FSP_SUCCESS;

    err = SPI_READ(p_api_ctrl, p_dest, length, bit_width);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function simultaneously transmits and receives data.
 * @param[in]  p_api_ctrl               Pointer to control structure.
 * @param[in]  p_src                    Buffer to transmit data.
 * @param[out] p_dest                   Buffer to store received data.
 * @param[in]  length                   Number of transfers.
 * @param[in]  bit_width                Data frame size.
 * @retval     FSP_SUCCESS              Transmitted and received successfully.
 * @retval     Any other error codes    Failed to transmit or receive.
 **********************************************************************************************************************/
fsp_err_t spi_write_read(spi_ctrl_t * const p_api_ctrl, void const * p_src, void * p_dest,
                         uint32_t const length, spi_bit_width_t const bit_width)
{
    fsp_err_t err = FSP_SUCCESS;

    err = SPI_WRITE_READ(p_api_ctrl, p_src, p_dest, length, bit_width);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes all the opened SPI modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void spi_deinit(void)
{
    if (g_spi_master_ctrl.open)
    {
        /* De-initialize the opened SPI Master module */
        SPI_CLOSE(&g_spi_master_ctrl);
    }

    if (g_spi_slave_ctrl.open)
    {
        /* De-initialize the opened SPI Slave module */
        SPI_CLOSE(&g_spi_slave_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is the callback for the SPI Master module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void spi_master_callback(spi_callback_args_t * p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_master_event_flag = SPI_EVENT_TRANSFER_COMPLETE;
    }
    else
    {
        g_master_event_flag = SPI_EVENT_TRANSFER_ABORTED;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is the callback for the SPI Slave module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void spi_slave_callback(spi_callback_args_t * p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_slave_event_flag = SPI_EVENT_TRANSFER_COMPLETE;
    }
    else
    {
        g_slave_event_flag = SPI_EVENT_TRANSFER_ABORTED;
    }
}
