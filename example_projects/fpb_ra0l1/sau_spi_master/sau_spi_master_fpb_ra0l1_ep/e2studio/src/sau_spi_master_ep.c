/***********************************************************************************************************************
 * File Name    : sau_spi_master_ep.c
 * Description  : Contains data structures and functions used in sau_spi_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sau_spi_master_ep.h"

/* Master transfer event completion flag */
volatile spi_event_t g_master_event_flag = (spi_event_t)RESET_VALUE;

/***********************************************************************************************************************
 * Function Name: sensor_config
 * Description  : This function configures temperature sensor and verifies configuration.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Sensor configured successfully.
 *                Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t sensor_config(void)
{
    fsp_err_t err = FSP_SUCCESS;
    const uint8_t config_sensor[DATA_FRAME_SIZE] = {CFG_WRITE_ADDR, SENSOR_RES_CFG};
    uint8_t read_config_addr = CFG_READ_ADDR;
    uint8_t read_config_data[DATA_FRAME_SIZE] = {RESET_VALUE};

    /* Assert Slave select pin to trigger the slave device initiates the first transfer */
    SS_ASSERT(SS_PIN);

    /* Reset Slave select pin for next transfer */
    SS_DE_ASSERT(SS_PIN);

    /* Make sure the CS pin is de-asserted */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /* Configure temperature sensor */
    err = sau_spi_write(config_sensor, DATA_FRAME_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back configured temperature sensor register */
    err = sau_spi_write_read(&read_config_addr, read_config_data, DATA_FRAME_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Verify sensor configuration */
    if (config_sensor[1] != read_config_data[1])
    {
        /* Incorrect configuration of temperature sensor */
        return FSP_ERR_INVALID_HW_CONDITION;
    }

    return err;
}
/***********************************************************************************************************************
* End of function sensor_config
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_master_init
 * Description  : This function initializes the SAU SPI Master module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t sau_spi_master_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open SAU SPI Master module */
    err = R_SAU_SPI_Open(&g_sau_spi_ctrl, &g_sau_spi_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function sau_spi_master_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_write
 * Description  : This function writes data through SAU SPI.
 * Arguments    : p_src                     Pointer to source data buffer.
 *                length                    Number of bytes to write.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t sau_spi_write(void const *p_src, uint32_t length)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Assert slave select pin to start configuration write */
    SS_ASSERT(SS_PIN);

    /* Reset SPI master event flag */
    g_master_event_flag = (spi_event_t)RESET_VALUE;

    /* Transmit data over SAU SPI */
    err = R_SAU_SPI_Write(&g_sau_spi_ctrl, p_src, length, SPI_BIT_WIDTH_8_BITS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for transfer complete event */
    err = wait_sau_spi_transfer_complete();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* De-assert slave select pin to stop data transfer */
    SS_DE_ASSERT(SS_PIN);

    /* Make sure the CS pin is de-asserted */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    return err;
}
/***********************************************************************************************************************
* End of function sau_spi_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_write_read
 * Description  : This function writes and reads data through SAU SPI.
 * Arguments    : p_src                     Pointer to source data buffer.
 *                p_dest                    Pointer to destination data buffer.
 *                length                    Number of bytes to transfer.
 * Return Value : FSP_SUCCESS               Transferred successfully.
 *                Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
fsp_err_t sau_spi_write_read(void const *p_src, void *p_dest, uint32_t length)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Assert slave select pin to start configuration write */
    SS_ASSERT(SS_PIN);

    /* Reset SPI master event flag */
    g_master_event_flag = (spi_event_t)RESET_VALUE;

    /* Transmit and receive data over SAU SPI */
    err = R_SAU_SPI_WriteRead(&g_sau_spi_ctrl, p_src, p_dest, length, SPI_BIT_WIDTH_8_BITS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for transfer complete event */
    err = wait_sau_spi_transfer_complete();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* De-assert slave select pin to stop data transfer */
    SS_DE_ASSERT(SS_PIN);

    /* Make sure the CS pin is de-asserted */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    return err;
}
/***********************************************************************************************************************
* End of function sau_spi_write_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_master_deinit
 * Description  : This function closes the opened SAU SPI Master module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void sau_spi_master_deinit(void)
{
    /* Close the opened SAU SPI Master module */
    if (g_sau_spi_ctrl.open)
    {
        R_SAU_SPI_Close(&g_sau_spi_ctrl);
    }
}
/***********************************************************************************************************************
* End of function sau_spi_master_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: wait_sau_spi_transfer_complete
 * Description  : This function waits for the data transfer complete event until a timeout occurs.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Transferred successfully.
 *                Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
fsp_err_t wait_sau_spi_transfer_complete(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t timeout = UINT8_MAX;

    while (SPI_EVENT_TRANSFER_COMPLETE != g_master_event_flag && --timeout)
    {
        R_BSP_SoftwareDelay(SPI_TIMEOUT_DIV, SPI_TIMEOUT_UNIT);
    }

    if (RESET_VALUE == timeout)
    {
       /* Return a timeout error if the SPI operation fails to complete */
       err = FSP_ERR_TIMEOUT;
    }

    return err;
}
/***********************************************************************************************************************
* End of function wait_sau_spi_transfer_complete
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_callback
 * Description  : This function is the callback for a SAU SPI Master module.
 * Arguments    : p_args                    Pointer to the callback function parameter data.
 * Return Value : None.
 **********************************************************************************************************************/
void sau_spi_callback(spi_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_master_event_flag = p_args->event;
    }
}
/***********************************************************************************************************************
* End of function sau_spi_callback
***********************************************************************************************************************/
