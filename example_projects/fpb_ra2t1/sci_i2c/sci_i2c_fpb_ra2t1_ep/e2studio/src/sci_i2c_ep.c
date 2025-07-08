/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.c
 * Description  : Contains macro and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "common_utils.h"
#include "sci_i2c_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup sci_i2c_ep
 * @{
 **********************************************************************************************************************/

/* Event flag to identify the master event */
static volatile master_transfer_mode_t g_master_RW  = MASTER_WRITE;
/* Slave transmit buffer */
static uint8_t g_slave_tx_buf[BUF_LEN];
/* Slave receive buffer */
static uint8_t g_slave_rx_buf[BUF_LEN];

/* Private Global Variables */

/* Capture callback event for slave and master modules */
static volatile i2c_master_event_t g_master_event = (i2c_master_event_t)RESET_VALUE;
static volatile i2c_slave_event_t g_slave_event  = (i2c_slave_event_t)RESET_VALUE;

/* Capture return value from slave read and write API */
static volatile fsp_err_t g_slave_api_ret_err = FSP_SUCCESS;

/* For on board LEDs */
extern bsp_leds_t g_bsp_leds;

/* Private functions */
static fsp_err_t sci_i2c_master_read(void);
static fsp_err_t sci_i2c_master_write(void);
static void toggle_led(void);

/*******************************************************************************************************************//**
 * @brief     Initialize SCI_I2C Master and IIC slave drivers.
 * @param[IN] None
 * @retval    FSP_SUCCESS       SCI_I2C_MASTER and IIC_SLAVE driver opened successfully.
 * @retval    err               Any Other Error code apart from FSP_SUCCESS like Unsuccessful Open.
 **********************************************************************************************************************/
fsp_err_t init_i2c_driver(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open SCI_I2C Master channel */
#if BSP_PERIPHERAL_SCI_B_PRESENT
    err = R_SCI_B_I2C_Open(&g_sci_i2c_master_ctrl, &g_sci_i2c_master_cfg);
#else
    err = R_SCI_I2C_Open(&g_sci_i2c_master_ctrl, &g_sci_i2c_master_cfg);
#endif
    if (FSP_SUCCESS != err)
    {
    	/* Display failure message in RTT */
        APP_ERR_PRINT ("** R_"SCI_TYPE"_I2C_Open API FAILED **\r\n");
        return err;
    }

    /* Open IIC Slave channel */
#if BSP_PERIPHERAL_IIC_B_PRESENT
    err = R_IIC_B_SLAVE_Open(&g_i2c_slave_ctrl, &g_i2c_slave_cfg);
#else
    err = R_IIC_SLAVE_Open(&g_i2c_slave_ctrl, &g_i2c_slave_cfg);
#endif
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_ERR_PRINT ("** R_"IIC_TYPE"_Open API FAILED **\r\n");

        /* IIC Slave open unsuccessful, close SCI_I2C Master module */
#if  BSP_PERIPHERAL_SCI_B_PRESENT
        if (FSP_SUCCESS != R_SCI_B_I2C_Close(&g_sci_i2c_master_ctrl))
        {
#else
        if (FSP_SUCCESS != R_SCI_I2C_Close(&g_sci_i2c_master_ctrl))
        {
#endif
            /* Display failure message in RTT */
            APP_ERR_PRINT ("** R_"SCI_TYPE"_I2C_Close API FAILED **\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      Performs Master write read operations and toggle LED on successful operation
 *              else turn ON LED on failure
 *              If SCI_I2C Master transaction failure occurs then it halts the application turning ON LED
 *              as sign of failure. Also displays failure messages in RTT.
 *  @param[IN]  None
 *  @retval     FSP_SUCCESS    On successfully SCI_I2C Mater and Slave Write/Read operation.
 *  @retval     err            Any Other Error code apart from FSP_SUCCES  Unsuccessful Write/Read operation.
 **********************************************************************************************************************/
fsp_err_t process_master_WriteRead(void)
{
    fsp_err_t      error = FSP_SUCCESS;

    switch(g_master_RW)
    {
        case MASTER_WRITE:
        {
            /* Change master event so that relevant operation can be performed following the current one */
            g_master_RW = MASTER_READ;

            /* Before beginning the operation turn OFF LED */
            set_led(LED_OFF);

            /* Master starts writing the data to slave */
            error = sci_i2c_master_write();
            if (FSP_SUCCESS != error)
            {
                /* Print RTT message */
                APP_ERR_PRINT ("** sci_i2c_master_write failed! **\r\n");
            }
            else
            {
                /* Toggle LED as sign of success */
                toggle_led();

                /* Print RTT message */
                APP_PRINT ("** sci_i2c_master_write is successful **\r\n");
            }

            break;
        }
        case MASTER_READ:
        {
            /* Change master event so that relevant operation can be performed following the current one */
            g_master_RW = MASTER_WRITE;

            /* Before beginning the operation turn OFF LED */
            set_led(LED_OFF);

            /* Master starts receiving data transmitted by slave */
            error = sci_i2c_master_read();
            if (FSP_SUCCESS != error)
            {
                /* Print RTT message */
                APP_ERR_PRINT ("** sci_i2c_master_read failed! **\r\n");
            }
            else
            {
                /* Toggle LED as sign of success */
                toggle_led();

                /* Print RTT message */
                APP_PRINT ("** sci_i2c_master_read is successful **\r\n");
            }

            break;
        }
        default:
            break;
    }

    return error;
}

/*******************************************************************************************************************//**
 *  @brief       Perform Master Read operation
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS               Master successfully read all data written by slave device.
 *  @retval      FSP_ERR_TRANSFER_ABORTED  callback event failure
 *  @retval      FSP_ERR_ABORTED           data mismatched occurred.
 *  @retval      FSP_ERR_TIMEOUT           In case of no callback event occurrence
 *  @retval      err                       API returned error if any
 **********************************************************************************************************************/
static fsp_err_t sci_i2c_master_read(void)
{
    fsp_err_t read_err           = FSP_SUCCESS;
    uint16_t read_time_out       = UINT16_MAX;
    uint8_t read_buffer[BUF_LEN] = {RESET_VALUE};

    /* Reset and update slave transmit buffer with data received by slave */
    memset(g_slave_tx_buf, RESET_VALUE, BUF_LEN);
    memcpy(g_slave_tx_buf, g_slave_rx_buf, BUF_LEN);

    /* Reset callback event */
    g_master_event = (i2c_master_event_t)RESET_VALUE;
    g_slave_event  = (i2c_slave_event_t)RESET_VALUE;

    /* Start SCI_I2C Master read. Master has to initiate the transfer */
#if BSP_PERIPHERAL_SCI_B_PRESENT
    read_err = R_SCI_B_I2C_Read(&g_sci_i2c_master_ctrl, read_buffer, BUF_LEN, false);
#else
    read_err = R_SCI_I2C_Read(&g_sci_i2c_master_ctrl, read_buffer, BUF_LEN, false);
#endif
    if (FSP_SUCCESS != read_err)
    {
        APP_ERR_PRINT("\r\n ** R_"SCI_TYPE"_I2C_Read API FAILED ** \r\n");
        return read_err;
    }

    /* Wait until slave write and master read process gets completed */
    while( (I2C_MASTER_EVENT_RX_COMPLETE != g_master_event) || (I2C_SLAVE_EVENT_TX_COMPLETE != g_slave_event) )
    {
        /* Check for aborted event */
        if ((I2C_SLAVE_EVENT_ABORTED == g_slave_event) || (I2C_MASTER_EVENT_ABORTED == g_master_event) )
        {
            APP_ERR_PRINT ("** EVENT_ABORTED received during Master read operation **\r\n");

            /* I2C transaction failure */
            return FSP_ERR_TRANSFER_ABORTED;
        }

        /* Handle error for slave write API return value g_slave_api_ret_err gets
         * updated only in case of error return from Slave write API */
        else if (FSP_SUCCESS != g_slave_api_ret_err)
        {
            read_err = g_slave_api_ret_err;

            /* Reset this with success again for further usage to capture in case of API failure */
            g_slave_api_ret_err = FSP_SUCCESS;

            return read_err;
        }

        else
        {
            /* Start checking for time out to avoid infinite loop */
            --read_time_out;

            /* Check for time elapse */
            if (RESET_VALUE == read_time_out)
            {
                /* We have reached to a scenario where I2C event not occurred */
                APP_ERR_PRINT (" ** No event received during Master read and Slave write operation **\r\r");

                /* No event received */
                return FSP_ERR_TIMEOUT;
            }
        }
    }

    /* Compare data received by master device with slave write buffer */
    if ( BUFF_EQUAL == memcmp(read_buffer, g_slave_tx_buf, BUF_LEN) )
    {
        read_err = FSP_SUCCESS;
    }
    else
    {
        read_err = FSP_ERR_ABORTED;
    }

    return read_err;
}

/*******************************************************************************************************************//**
 *  @brief      Perform Master Write operation
 *  @param[IN]  None
 *  @retval     FSP_SUCCESS               Master Writes and Slave receives data successfully.
 *  @retval     FSP_ERR_TRANSFER_ABORTED  callback event failure
 *  @retval     FSP_ERR_ABORTED           data mismatched occurred.
 *  @retval     FSP_ERR_TIMEOUT           In case of no callback event occurrence
 *  @retval     read_err                  API returned error if any
 **********************************************************************************************************************/
static fsp_err_t sci_i2c_master_write(void)
{
    fsp_err_t write_err           = FSP_SUCCESS;
    uint16_t write_time_out       = UINT16_MAX;

    /* Update master buffer and slave is recipient so clear slave RX buffer */
    uint8_t write_buffer[BUF_LEN] = {0x10, 0x20, 0x30, 0x40, 0x50};
    memset(g_slave_rx_buf, RESET_VALUE, BUF_LEN);

    /* Reset callback event */
    g_master_event = (i2c_master_event_t)RESET_VALUE;
    g_slave_event  = (i2c_slave_event_t)RESET_VALUE;

    /* Start SCI_I2C Master write operation */
#if BSP_PERIPHERAL_SCI_B_PRESENT
    write_err = R_SCI_B_I2C_Write(&g_sci_i2c_master_ctrl, write_buffer, BUF_LEN, false);
#else
    write_err = R_SCI_I2C_Write(&g_sci_i2c_master_ctrl, write_buffer, BUF_LEN, false);
#endif

    /* Handle error */
    if (FSP_SUCCESS != write_err)
    {
        APP_ERR_PRINT("\r\n ** R_"SCI_TYPE"_I2C_Write API FAILED ** \r\n");
        return write_err;
    }

    /* Wait until master write and slave read process gets completed */
    while ( (I2C_MASTER_EVENT_TX_COMPLETE != g_master_event) || (I2C_SLAVE_EVENT_RX_COMPLETE != g_slave_event) )
    {
        /* Check for aborted event */
        if ( (I2C_SLAVE_EVENT_ABORTED == g_slave_event) || (I2C_MASTER_EVENT_ABORTED == g_master_event) )
        {
            APP_ERR_PRINT ("** Error EVENT_ABORTED received during Master Write operation **\r\n");

            /* I2C transaction failure */
            return FSP_ERR_TRANSFER_ABORTED;

        }

        /* Handle error for slave read API return value g_slave_api_ret_err gets
         * updated only in case of error return from Slave read API */
        else if (FSP_SUCCESS != g_slave_api_ret_err)
        {
            write_err = g_slave_api_ret_err;

            /* Reset this with success again for further usage to capture in case of API failure */
            g_slave_api_ret_err = FSP_SUCCESS;

            return write_err;
        }

        else
        {
            /* Start checking for time out to avoid infinite loop */
            --write_time_out;

            /* Check for time elapse */
            if (RESET_VALUE == write_time_out)
            {
                /* We have reached to a scenario where I2C event not occurred */
                APP_ERR_PRINT (" ** No event received during Master Write and Slave read operation **\r\r");

                /* No event received */
                return FSP_ERR_TIMEOUT;
            }
        }
    }

    /* Compare master written data with slave read buffer */
    if ( BUFF_EQUAL == memcmp(g_slave_rx_buf, write_buffer, BUF_LEN) )
    {
        write_err = FSP_SUCCESS;
    }
    else
    {
        write_err = FSP_ERR_ABORTED;
    }

    return write_err;
}

/*******************************************************************************************************************//**
 *  @brief        The user defined master callback function.
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void sci_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_master_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief        The user defined slave callback function.
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void i2c_slave_callback(i2c_slave_callback_args_t * p_args)
{
    fsp_err_t err = FSP_SUCCESS;

    if (NULL != p_args)
    {
        switch(p_args->event)
        {
            case I2C_SLAVE_EVENT_RX_REQUEST:
            {
                /* Perform slave read operation */
#if BSP_PERIPHERAL_IIC_B_PRESENT
                err = R_IIC_B_SLAVE_Read(&g_i2c_slave_ctrl, g_slave_rx_buf, BUF_LEN);
#else
                err = R_IIC_SLAVE_Read(&g_i2c_slave_ctrl, g_slave_rx_buf, BUF_LEN);
#endif
                if(FSP_SUCCESS != err)
                {
                    /* Update return error here */
                    g_slave_api_ret_err = err;
                }

                break;
            }
            case I2C_SLAVE_EVENT_TX_REQUEST:
            {
                /* Perform slave write operation */
#if BSP_PERIPHERAL_IIC_B_PRESENT
                err = R_IIC_B_SLAVE_Write(&g_i2c_slave_ctrl, g_slave_tx_buf, BUF_LEN);
#else
                err = R_IIC_SLAVE_Write(&g_i2c_slave_ctrl, g_slave_tx_buf, BUF_LEN);
#endif
                if(FSP_SUCCESS != err)
                {
                    /* Update return error here */
                    g_slave_api_ret_err = err;
                }

                break;
            }
            default:
                /* Log the event to global variable for any other slave event */
                g_slave_event = p_args->event;
                break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     Toggle on board LED, which are connected and supported by BSP
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void toggle_led(void)
{
    /* Get LED information (pins) for this board */
    bsp_leds_t leds = g_bsp_leds;

    for(uint8_t cnt = RESET_VALUE; leds.led_count > cnt; cnt++)
    {
        R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], (bsp_io_level_t)LED_ON);
        R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

        R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], (bsp_io_level_t)LED_OFF);
    }
}

/*******************************************************************************************************************//**
 *  @brief       Turn on board LED ON or OFF.
 *  @param[IN]   b_value     LED_ON or LED_OFF
 *  @retval      None
 **********************************************************************************************************************/
void set_led(bool b_value)
{
    /* Get LED information (pins) for this board */
    bsp_leds_t leds = g_bsp_leds;

    for(uint8_t cnt = RESET_VALUE ; leds.led_count > cnt; cnt++)
    {
        R_IOPORT_PinWrite(g_ioport.p_ctrl, (bsp_io_port_pin_t)leds.p_leds[cnt], (bsp_io_level_t)b_value);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is called to do closing of SCI_I2C Master and IIC Slave module using its HAL level API.
 *            Close the SCI_I2C Master and IIC Slave module. Handle the error internally with proper RTT Message.
 *            Application handles the rest.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void deinit_i2c_driver(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened SCI_I2C Master module */
    if (MODULE_CLOSE != g_sci_i2c_master_ctrl.open)
    {
#if BSP_PERIPHERAL_SCI_B_PRESENT
        err = R_SCI_B_I2C_Close(&g_sci_i2c_master_ctrl);
#else
        err = R_SCI_I2C_Close(&g_sci_i2c_master_ctrl);
#endif
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(" ** R_"SCI_TYPE"_I2C_Close API FAILED ** \r\n");
        }
    }

    /* Close opened IIC Slave module */
    if (MODULE_CLOSE != g_i2c_slave_ctrl.open)
    {
#if BSP_PERIPHERAL_IIC_B_PRESENT
        err = R_IIC_B_SLAVE_Close(&g_i2c_slave_ctrl);
#else
        err = R_IIC_SLAVE_Close(&g_i2c_slave_ctrl);
#endif

        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(" ** R_"IIC_TYPE"_Close API FAILED ** \r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup sci_i2c_ep)
 **********************************************************************************************************************/
