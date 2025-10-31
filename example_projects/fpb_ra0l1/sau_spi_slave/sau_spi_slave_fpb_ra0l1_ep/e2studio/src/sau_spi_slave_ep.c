/***********************************************************************************************************************
 * File Name    : sau_spi_slave_ep.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "temp_sensor.h"
#include "sau_spi_slave_ep.h"

/* Event flags for Slave board */
static volatile spi_event_t g_slave_event_flag = (spi_event_t) RESET_VALUE;
static volatile bool g_active_transfer = false;

/* User defined functions */
static fsp_err_t wait_transfer_end(void);
static fsp_err_t communicate_with_master_board(void);
static void sau_spi_slave_deinit(void);
static void active_transfer_deinit(void);
static void handle_error(fsp_err_t err, const char *err_str);

/***********************************************************************************************************************
 *  Function Name: sau_spi_slave_entry
 *  Description  : This function is used to start the SAU SPI Slave example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void sau_spi_slave_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

    /* Get FSP version */
    R_FSP_VersionGet(&version);

    /* Initialize the UART module first to print logs to the serial terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Error trap */
        ERROR_TRAP;
    }

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize, configure, and start the ADC module */
    err = adc_init();
    handle_error(err, "adc_init failed\r\n");

    /* Initialize the SPI Slave module */
    err = R_SAU_SPI_Open(&g_sau_spi_slave_ctrl, &g_sau_spi_slave_cfg);
    handle_error(err, "** R_SAU_SPI_Open API failed **\r\n");

    /* Initialize the ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    handle_error(err, "** R_ICU_ExternalIrqOpen API failed **\r\n");

    /* Enable the ICU module to detect the active transfer signal from the Master board */
    err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);
    handle_error(err, "** R_ICU_ExternalIrqEnable API failed **\r\n");

    while(true)
    {
        if(g_active_transfer)
        {
            /* Reset g_active_transfer flag */
            g_active_transfer = false;

            /* Receive data and transmit the corresponding data to the Master board */
            err = communicate_with_master_board();
            handle_error(err, "communicate_with_master_board failed\r\n");
        }
        else
        {
            /* Delay to reduce CPU usage while waiting for the active transfer flag */
            R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
        }
    }
}
/***********************************************************************************************************************
* End of function sau_spi_slave_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: wait_transfer_end
 *  Description  : This function waits data transfer complete event until timeout occurs and handles error.
 *  Return Value : FSP_SUCCESS          Upon successful operation.
 *                 FSP_ERR_TIMEOUT      Upon execution time expiration.
 **********************************************************************************************************************/
static fsp_err_t wait_transfer_end(void)
{
    /* Reset timeout value */
    uint32_t timeout = MAX_COUNT;

    while((SPI_EVENT_TRANSFER_COMPLETE != g_slave_event_flag) && (--timeout) )
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }

    if (RESET_VALUE == timeout)
    {
        /* Other than the transfer complete event, the SAU SPI module only returns a read overflow error */
        if (SPI_EVENT_ERR_READ_OVERFLOW == g_slave_event_flag)
        {
            APP_ERR_RET(FSP_ERR_ASSERTION != FSP_SUCCESS, FSP_ERR_ASSERTION,
                        "SAU SPI transfer occurs the read overflow error\r\n");
        }

        /* If neither event is detected, the function returns a timeout error */
         APP_ERR_RET(FSP_ERR_TIMEOUT != FSP_SUCCESS, FSP_ERR_TIMEOUT,
                     "SAU SPI transfer exceeds the timeout period\r\n");
    }

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function wait_transfer_end
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: communicate_with_master_board
 *  Description  : This function reads data from Master board and writes the corresponding data back.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS      Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t communicate_with_master_board(void)
{
    fsp_err_t err = FSP_SUCCESS;
    float mcu_temp_c                          = 0.0f;
    uint16_t adc_data                         = RESET_VALUE;
    uint8_t spi_rx_buf[FRAME_DATA_SIZE]       = {RESET_VALUE};
    char temp_buff[30]                        = {RESET_VALUE};

    static bool is_configure_success          = false;
    static uint8_t write_buf[FRAME_DATA_SIZE] = {RESET_VALUE};

    /* Reset Slave_event flag */
    g_slave_event_flag = (spi_event_t) RESET_VALUE;

    /* Read data, store it in spi_rx_buf, and write data in write_buf to the response */
    err = R_SAU_SPI_WriteRead(&g_sau_spi_slave_ctrl, &write_buf, &spi_rx_buf, FRAME_DATA_SIZE, SPI_BIT_WIDTH_8_BITS);
    APP_ERR_RET(FSP_SUCCESS != err , err, "** R_SAU_SPI_WriteRead API failed **\r\n");

    /* Waiting for transfer complete event and handle error in the case of event failure */
    err = wait_transfer_end();
    APP_ERR_RET(FSP_SUCCESS != err , err, "wait_transfer_end failed.\r\n");

    if (CFG_WRITE_CMD == spi_rx_buf[INDEX_ZERO])
    {
        if (CFG_VALUE_EXPECT == spi_rx_buf[INDEX_ONE])
        {
            APP_PRINT("\r\nReceived data as expected.");

            /* Store received data to write buffer, which is sent back to Master */
            write_buf[INDEX_ONE] = spi_rx_buf[INDEX_ONE];
        }
    }
    else if (CFG_READ_CMD == spi_rx_buf[INDEX_ZERO])
    {
        if (CFG_VALUE_EXPECT == write_buf[INDEX_ONE])
        {
            APP_PRINT("\r\nStart transmitting the MCU temperature to Master.\r\n");

            /* Set the flag to notify success to connect to Master board, moving on to transmit temperature data */
            is_configure_success = true;
        }
    }

    else if (DATA_READ_CMD == spi_rx_buf[INDEX_ZERO] && is_configure_success)
    {
        /* Clean write_buf data */
        memset(write_buf, RESET_VALUE, FRAME_DATA_SIZE);

        /* Read the die temperature data by ADC */
        err = adc_read_data(&adc_data);
        APP_ERR_RET(FSP_SUCCESS != err, err, "adc_read_data failed\r\n");

        mcu_temp_c = MCU_TEMP_TO_C(adc_data);

        /* Convert the float value to a string to display the temperature */
        snprintf(temp_buff, sizeof(temp_buff), "%.04f%s", mcu_temp_c, DEGREE_SYMBOL);
        APP_PRINT("\r\nMCU Temperature Reading from ADC: %sC", temp_buff);

        /* Convert die temperature to raw value and store in
         * write_buf to match the register structure of MAX31723 */
        uint32_t raw_value = (uint32_t)(mcu_temp_c * FLOAT_DIVISOR);
        write_buf[INTERGER_PART] = (raw_value >> SHIFT_FOUR_BITS) & 0xFF;
        write_buf[FRACTIONAL_PART] = (raw_value & 0x0F) << SHIFT_FOUR_BITS;
    }

    else
    {
        /* Do nothing */
    }

    return err;
}
/***********************************************************************************************************************
* End of function communicate_with_master_board
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sau_spi_slave_callback
 *  Description  : SAU SPI Slave callback function.
 *  Arguments    : p_args       Pointer to the callback arguments.
 *  Return Value : None
 **********************************************************************************************************************/
void sau_spi_slave_callback(spi_callback_args_t * p_args)
{
    if (SAU_SPI_00_CHANNEL == p_args->channel)
    {
        g_slave_event_flag = p_args->event;
    }
}
/***********************************************************************************************************************
* End of function sau_spi_slave_callback
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: active_transfer_callback
 *  Description  : Start the transfer when the Slave select pin signal of the Master MCU#2 transitions from LOW to HIGH.
 *  Arguments    : p_args       Pointer to the callback arguments.
 *  Return Value : None
 **********************************************************************************************************************/
void active_transfer_callback(external_irq_callback_args_t *p_args)
{
    if (DETECT_SS_SIGNAL_CHANNEL == p_args->channel)
    {
        /* Start the transfer when the Slave select pin signal of the Master MCU#2 transitions from LOW to HIGH */
        g_active_transfer = true;
    }
}
/***********************************************************************************************************************
* End of function active_transfer_callback
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sau_spi_slave_deinit
 *  Description  : This function de-initializes opened SAU SPI Slave module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void sau_spi_slave_deinit(void)
{
    /* De-inititalize the opened SAU SPI Slave module */
    if (MODULE_CLOSE != g_sau_spi_slave_ctrl.open)
    {
        if (FSP_SUCCESS != R_SAU_SPI_Close(&g_sau_spi_slave_ctrl))
        {
            APP_ERR_PRINT("** R_SAU_SPI_Close API failed **\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function sau_spi_slave_deinit
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: active_transfer_deinit
 *  Description  : This function de-initializes opened External IRQ module.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void active_transfer_deinit(void)
{
    /* De-inititalize the opened External IRQ module */
    if (MODULE_CLOSE != g_external_irq0_ctrl.open)
    {
        if (FSP_SUCCESS != R_ICU_ExternalIrqClose(&g_external_irq0_ctrl))
        {
            APP_ERR_PRINT("** R_ICU_ExternalIrqClose API failed **\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function active_transfer_deinit
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  Arguments    : err          Error status.
 *                 err_str      Error string.
 *  Return Value : None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err )
    {
        /* Print error information and error code */
        APP_ERR_PRINT(err_str);
        /* De-initialize the opened SAU SPI Slave module */
        sau_spi_slave_deinit();
        /* De-initialize the opened ADC module */
        adc_deinit();
        /* De-initialize the opened External IRQ module */
        active_transfer_deinit();
        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
 **********************************************************************************************************************/
