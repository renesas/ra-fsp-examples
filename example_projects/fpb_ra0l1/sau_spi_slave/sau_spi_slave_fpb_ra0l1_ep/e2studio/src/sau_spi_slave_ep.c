/***********************************************************************************************************************
 * File Name    : sau_spi_slave_ep.c
 * Description  : Contains data structures and functions used in sau_spi_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "temp_sensor.h"
#include "sau_spi_slave_ep.h"

/* Event flags for Slave board */
static volatile spi_event_t s_slave_event_flag = (spi_event_t) RESET_VALUE;
volatile bool g_active_transfer = false;

/* Private function */
static fsp_err_t wait_transfer_end(void);

/***********************************************************************************************************************
 * Function Name: wait_transfer_end.
 * Description  : This function waits for the data transfer complete event until a timeout occurs.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Transferred successfully.
 *                Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static fsp_err_t wait_transfer_end(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Reset timeout value */
    uint32_t timeout = MAX_COUNT;

    while ((SPI_EVENT_TRANSFER_COMPLETE != s_slave_event_flag) && (--timeout))
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }

    if (RESET_VALUE == timeout)
    {
        /* Other than the transfer complete event, the SAU SPI module only returns a read overflow error */
        if (SPI_EVENT_ERR_READ_OVERFLOW == s_slave_event_flag)
        {
            return FSP_ERR_ASSERTION;
        }

        /* If neither event is detected, the function returns a timeout error */
        return FSP_ERR_TIMEOUT;
    }

    return err;
}
/***********************************************************************************************************************
* End of function wait_transfer_end
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_slave_communicate.
 * Description  : This function reads data from Master board and writes the corresponding data back.
 * Arguments    : p_event                   Pointer to store the user-visible event produced by this exchange.
 *                p_temp_str                Buffer to receive the formatted temperature string when
 *                                          SAU_SPI_SLAVE_EVENT_TEMP_SENT is produced (may be NULL otherwise).
 *                temp_str_size             Size of p_temp_str.
 * Return Value : FSP_SUCCESS               Communicated successfully.
 *                Any other error codes     Failed to communicate.
 **********************************************************************************************************************/
fsp_err_t sau_spi_slave_communicate(sau_spi_slave_event_t * p_event, char * p_temp_str, uint32_t temp_str_size)
{
    fsp_err_t err = FSP_SUCCESS;
    float mcu_temp_c = 0.0f;
    uint16_t adc_data = RESET_VALUE;
    uint8_t spi_rx_buf[FRAME_DATA_SIZE] = {RESET_VALUE};
    static bool s_b_is_configure_success = false;
    static uint8_t s_write_buf[FRAME_DATA_SIZE] = {RESET_VALUE};

    *p_event = SAU_SPI_SLAVE_EVENT_NONE;

    /* Reset Slave_event flag */
    s_slave_event_flag = (spi_event_t) RESET_VALUE;

    /* Read data, store it in spi_rx_buf, and write data in s_write_buf to the response */
    err = R_SAU_SPI_WriteRead(&g_sau_spi_slave_ctrl, &s_write_buf, &spi_rx_buf, FRAME_DATA_SIZE, SPI_BIT_WIDTH_8_BITS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for transfer complete event and handle error in the case of event failure */
    err = wait_transfer_end();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (CFG_WRITE_CMD == spi_rx_buf[0U])
    {
        if (CFG_VALUE_EXPECT == spi_rx_buf[1U])
        {
            /* Store received data to write buffer, which is sent back to Master */
            s_write_buf[1U] = spi_rx_buf[1U];

            *p_event = SAU_SPI_SLAVE_EVENT_CFG_RECEIVED;
        }
    }
    else if (CFG_READ_CMD == spi_rx_buf[0U])
    {
        if (CFG_VALUE_EXPECT == s_write_buf[1U])
        {
            /* Set the flag to notify success to connect to Master board, moving on to transmit temperature data */
            s_b_is_configure_success = true;

            *p_event = SAU_SPI_SLAVE_EVENT_READ_CFG_RECEIVED;
        }
    }
    else if (DATA_READ_CMD == spi_rx_buf[0U] && s_b_is_configure_success)
    {
        /* Clean s_write_buf data */
        memset(s_write_buf, RESET_VALUE, FRAME_DATA_SIZE);

        /* Read the die temperature data by ADC */
        err = adc_read_data(&adc_data);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        mcu_temp_c = MCU_TEMP_TO_C(adc_data);

        /* Convert the float value to a string to display the temperature */
        if (NULL != p_temp_str)
        {
            snprintf(p_temp_str, temp_str_size, "%.04f%s", mcu_temp_c, DEGREE_SYMBOL);
        }

        /* Convert die temperature to raw value and store in
         * s_write_buf to match the register structure of MAX31723 */
        uint32_t raw_value = (uint32_t)(mcu_temp_c * FLOAT_DIVISOR);
        s_write_buf[INTEGER_PART] = (raw_value >> SHIFT_FOUR_BITS) & 0xFF;
        s_write_buf[FRACTIONAL_PART] = (uint8_t)((raw_value & 0x0F) << SHIFT_FOUR_BITS);

        *p_event = SAU_SPI_SLAVE_EVENT_TEMP_SENT;
    }
    else
    {
        /* Do nothing */
    }

    return err;
}
/***********************************************************************************************************************
* End of function sau_spi_slave_communicate
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_slave_init.
 * Description  : This function initializes the SAU SPI Slave module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t sau_spi_slave_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the SAU SPI Slave module */
    err = R_SAU_SPI_Open(&g_sau_spi_slave_ctrl, &g_sau_spi_slave_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function sau_spi_slave_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_slave_deinit.
 * Description  : This function de-initializes the opened SAU SPI Slave module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void sau_spi_slave_deinit(void)
{
    /* De-initialize the opened SAU SPI Slave module */
    if (g_sau_spi_slave_ctrl.open)
    {
        R_SAU_SPI_Close(&g_sau_spi_slave_ctrl);
    }
}
/***********************************************************************************************************************
* End of function sau_spi_slave_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: active_transfer_init
 * Description  : This function initializes the External IRQ module to detect the active transfer signal
 *                from the Master board.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t active_transfer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the External IRQ module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable the External IRQ module to detect the active transfer signal from the Master board */
    err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function active_transfer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: active_transfer_deinit.
 * Description  : This function de-initializes the opened External IRQ module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void active_transfer_deinit(void)
{
    /* De-initialize the opened External IRQ module */
    if (g_external_irq0_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq0_ctrl);
    }
}
/***********************************************************************************************************************
* End of function active_transfer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sau_spi_slave_callback.
 * Description  : This function is a callback for the SAU SPI Slave module.
 * Arguments    : p_args                    Pointer to the callback arguments.
 * Return Value : None.
 **********************************************************************************************************************/
void sau_spi_slave_callback(spi_callback_args_t * p_args)
{
    if (SAU_SPI_CHANNEL == p_args->channel)
    {
        s_slave_event_flag = p_args->event;
    }
}
/***********************************************************************************************************************
* End of function sau_spi_slave_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: active_transfer_callback.
 * Description  : This function is a callback for ICU module and triggers the transfer when the Slave select
 *                pin signal of the Master board transitions from LOW to HIGH.
 * Arguments    : p_args                    Pointer to the callback arguments.
 * Return Value : None.
 **********************************************************************************************************************/
void active_transfer_callback(external_irq_callback_args_t * p_args)
{
    if (DETECT_SS_SIGNAL_CHANNEL == p_args->channel)
    {
        /* Start the transfer when the Slave select pin signal of the Master MCU#2 transitions from LOW to HIGH */
        g_active_transfer = true;
    }
}
/***********************************************************************************************************************
* End of function active_transfer_callback
***********************************************************************************************************************/
