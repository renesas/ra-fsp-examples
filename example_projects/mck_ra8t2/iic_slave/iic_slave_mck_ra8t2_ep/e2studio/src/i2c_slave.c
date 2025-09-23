/***********************************************************************************************************************
 * File Name    : i2c_slave.c
 * Description  : Contains data structures and functions used in i2c_slave.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "i2c_slave.h"

/*******************************************************************************************************************//**
 * @addtogroup r_iic_slave_ep
 * @{
 **********************************************************************************************************************/

/* Private global variables */

/* Enumerators to identify Slave event to be processed */
typedef enum slave
{
    SLAVE_READ  = 1U,
    SLAVE_WRITE = 2U,
    SLAVE_NO_RW = 3U
}slave_transfer_mode_t;

/* Slave transmit buffer */
static uint8_t g_slave_tx_buf[BUF_LEN];
/* Slave receive buffer */
static uint8_t g_slave_rx_buf[BUF_LEN];
/* Master buffer for both read and write */
static uint8_t g_master_buf[BUF_LEN];

/* Capture callback event for Slave and Master module */
static volatile i2c_master_event_t g_master_event = (i2c_master_event_t)RESET_VALUE;
static volatile i2c_slave_event_t  g_slave_event  = (i2c_slave_event_t)RESET_VALUE;

/* Capture return value from Slave read and write API */
static volatile fsp_err_t g_slave_api_ret_err = FSP_SUCCESS;

/* Event flag to identify the Slave event */
static volatile slave_transfer_mode_t g_slave_RW  = SLAVE_READ;

/* For on board LEDs */
extern bsp_leds_t g_bsp_leds;

/* Private functions */
static fsp_err_t iic_slave_write(void);
static fsp_err_t iic_slave_read(void);
static void toggle_led(void);

/*******************************************************************************************************************//**
 * @brief     Initialize IIC Master and Slave driver.
 * @param[IN] None
 * @retval    FSP_SUCCESS       R_IIC_Master and Slave driver opened successfully.
 * @retval    err               Any other error code apart from FSP_SUCCESS like unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t init_i2C_driver(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open I2C Master channel */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_MASTER_Open(&g_iica_master_ctrl,&g_iica_master_cfg);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_MASTER_Open(&g_i2c_master_ctrl,&g_i2c_master_cfg);
#else
    err = R_IIC_MASTER_Open(&g_i2c_master_ctrl,&g_i2c_master_cfg);
#endif
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("R_"IIC_TYPE"_MASTER_Open API failed\r\n");
        return err;
    }

    /* Open I2C Slave channel */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_SLAVE_Open(&g_iica_slave_ctrl, g_iica_slave.p_cfg);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_SLAVE_Open(&g_i2c_slave_ctrl, g_i2c_slave.p_cfg);
#else
    err = R_IIC_SLAVE_Open(&g_i2c_slave_ctrl, g_i2c_slave.p_cfg);
#endif
    if (FSP_SUCCESS != err)
    {
        /* Display failure message in RTT */
        APP_ERR_PRINT ("R_"IIC_TYPE"_SLAVE_Open API failed\r\n");

        /* Slave open unsuccessful closing master module */
#if (BSP_PERIPHERAL_IICA_PRESENT)
        if (FSP_SUCCESS !=  R_IICA_MASTER_Close(&g_iica_master_ctrl))
#elif defined (BOARD_RA6T2_MCK)
        if (FSP_SUCCESS !=  R_IIC_B_MASTER_Close(&g_i2c_master_ctrl))
#else
    	if (FSP_SUCCESS !=  R_IIC_MASTER_Close(&g_i2c_master_ctrl))
#endif
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_"IIC_TYPE"_MASTER_Close API failed\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      Performs Slave write read operations and toggle LED on successful operation
 *              else Turn LED ON on failure.
 *              If I2C Slave transaction failure occurs then it halts the application turning LED ON
 *              as sign of failure. Also displays failure messages in RTT.
 *  @retval     FSP_SUCCESS    On successfully R_IIC_Mater and Slave write/read operation.
 *  @retval     err            Any other error code apart from FSP_SUCCESS unsuccessful write/read operation.
 **********************************************************************************************************************/
fsp_err_t process_slave_WriteRead(void)
{
    fsp_err_t      error            = FSP_SUCCESS;

    switch (g_slave_RW)
    {
        case SLAVE_READ:
        {
            /* Before beginning the operation turn OFF LED */
            set_led(LED_OFF);

            /* Slave starts reading the data */
            error = iic_slave_read();
            if (FSP_SUCCESS != error)
            {
                /* Turn ON LED */
                set_led(LED_ON);

                /* Print RTT message */
                APP_ERR_PRINT ("** Slave read operation failed! **\r\n");
            }
            else
            {
                /* Toggle LED as sign of success */
                toggle_led();

                /* Print RTT message */
                APP_PRINT ("** Slave read operation is successful **\r\n");
            }

            /* Set variable value to perform slave write operation */
            g_slave_RW = SLAVE_WRITE;

            break;
        }
        case SLAVE_WRITE:
        {
            /* Before beginning the operation turn OFF LED */
            set_led(LED_OFF);

            /* Slave starts transmission of data */
            error = iic_slave_write();
            if (FSP_SUCCESS != error)
            {
                /* Turn ON LED */
                set_led(LED_ON);

                /* Print RTT message */
                APP_ERR_PRINT ("** Slave write operation failed! **\r\n");
            }
            else
            {
                /* Toggle LED as sign of success */
                toggle_led();

                /* Print RTT message */
                APP_PRINT ("** Slave write operation is successful **\r\n");
            }

            /* Set variable value to perform slave read operation */
            g_slave_RW = SLAVE_READ;

            break;
        }
        default:
            break;
    }

    return error;
}

/*******************************************************************************************************************//**
 *  @brief       Perform slave write operation.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS               Slave successfully write all data to Master.
 *  @retval      FSP_ERR_TRANSFER_ABORTED  Callback event failure.
 *  @retval      FSP_ERR_ABORTED           Data mismatched occurred.
 *  @retval      FSP_ERR_TIMEOUT           In case of no callback event occurrence.
 *  @retval      write_err                 API returned error if any.
**********************************************************************************************************************/
static fsp_err_t iic_slave_write(void)
{
    fsp_err_t write_err           = FSP_SUCCESS;
    uint16_t write_time_out       = UINT16_MAX;
    uint8_t write_buffer[BUF_LEN] = {0x01, 0x02, 0x03, 0x04, 0x05};

    /* Update Slave transmit buffer and Master is recipient so clear Master buffer */
    memset(g_master_buf, RESET_VALUE, BUF_LEN);
    memcpy(g_slave_tx_buf, write_buffer, BUF_LEN);

    /* Resetting callback event */
    g_master_event = (i2c_master_event_t)RESET_VALUE;
    g_slave_event  = (i2c_slave_event_t)RESET_VALUE;

    /* Start Master read. Master has to initiate the transfer */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    write_err = R_IICA_MASTER_Read(&g_iica_master_ctrl, g_master_buf, BUF_LEN, false);
#elif defined (BOARD_RA6T2_MCK)
    write_err = R_IIC_B_MASTER_Read(&g_i2c_master_ctrl, g_master_buf, BUF_LEN, false);
#else
    write_err = R_IIC_MASTER_Read(&g_i2c_master_ctrl, g_master_buf, BUF_LEN, false);
#endif
    if (FSP_SUCCESS != write_err)
    {
        APP_ERR_PRINT("\r\n ** R_"IIC_TYPE"_MASTER_Read API failed **\r\n");
        return write_err;
    }

    /* Wait until Slave write and Master read process gets completed */
    while ( (I2C_SLAVE_EVENT_TX_COMPLETE != g_slave_event) || (I2C_MASTER_EVENT_RX_COMPLETE != g_master_event) )
    {
        /* Check for aborted event */
        if ( (I2C_MASTER_EVENT_ABORTED == g_master_event) || (I2C_SLAVE_EVENT_ABORTED == g_slave_event) )
        {
            	APP_ERR_PRINT ("** EVENT_ABORTED received during Slave write operation **\r\n");

                /* I2C transaction failure */
                return FSP_ERR_TRANSFER_ABORTED;
        }
        /* Handle error for slave write API return value g_slave_api_ret_err gets
         * updated only in case of error return from slave write API */
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
            	APP_ERR_PRINT ("** No event received during slave write operation **\r\r");

            	/* No event received */
            	return FSP_ERR_TIMEOUT;
            }
        }
    }

    /* Compare slave write buffer with data received by master device */
    if (BUFF_EQUAL == memcmp(g_slave_tx_buf,g_master_buf, BUF_LEN))
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
 *  @brief      Perform slave read operation.
 *  @param[in]  None
 *  @retval     FSP_SUCCESS               Slave successfully received all data issued by Master.
 *  @retval     FSP_ERR_TRANSFER_ABORTED  Callback event failure.
 *  @retval     FSP_ERR_ABORTED           Data mismatched occurred.
 *  @retval     FSP_ERR_TIMEOUT           In case of no callback event occurrence.
 *  @retval     read_err                  API returned error if any.
 **********************************************************************************************************************/
static fsp_err_t iic_slave_read(void)
{
    fsp_err_t read_err           = FSP_SUCCESS;
    uint16_t read_time_out       = UINT16_MAX;
    uint8_t read_buffer[BUF_LEN] = {0x10, 0x20, 0x30, 0x40, 0x50};

    /* Update Master buffer and Slave is recipient so clear Slave RX buffer */
    memset(g_slave_rx_buf, RESET_VALUE, BUF_LEN);
    memcpy(g_master_buf, read_buffer, BUF_LEN);

    /* Resetting callback event */
    g_master_event = (i2c_master_event_t)RESET_VALUE;
    g_slave_event  = (i2c_slave_event_t)RESET_VALUE;

    /* Master write to Slave */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    read_err = R_IICA_MASTER_Write(&g_iica_master_ctrl, g_master_buf, BUF_LEN, false);
#elif defined (BOARD_RA6T2_MCK)
    read_err = R_IIC_B_MASTER_Write(&g_i2c_master_ctrl, g_master_buf, BUF_LEN, false);
#else
    read_err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, g_master_buf, BUF_LEN, false);
#endif
    /* Handle error */
    if (FSP_SUCCESS != read_err)
    {
        APP_ERR_PRINT("\r\n** R_"IIC_TYPE"_MASTER_Write API failed **\r\n");
        return read_err;
    }

    /* Wait until slave read and master write process gets completed */
    while ((I2C_SLAVE_EVENT_RX_COMPLETE != g_slave_event) || (I2C_MASTER_EVENT_TX_COMPLETE != g_master_event))
    {
    	 /* Check for aborted event */
        if ( (I2C_MASTER_EVENT_ABORTED == g_master_event) || (I2C_SLAVE_EVENT_ABORTED == g_slave_event) )
        {
           	APP_ERR_PRINT ("** Error EVENT_ABORTED received during Slave read operation **\r\n");

            /* I2C transaction failure */
            return FSP_ERR_TRANSFER_ABORTED;

        }
        /* Handle error for slave read API return value g_slave_api_ret_err gets
         * updated only in case of error return from Slave read API */
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
            	APP_ERR_PRINT ("** No event received during slave read operation **\r\r");

            	/* No event received */
            	return FSP_ERR_TIMEOUT;
            }
        }
    }

    /* Compare Slave read buffer with data written by Master device */
    if (BUFF_EQUAL == memcmp(g_slave_rx_buf, g_master_buf, BUF_LEN))
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
 *  @brief        The user defined Master callback function.
 *  @param[in]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        g_master_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief        The user defined Slave callback function.
 *  @param[in]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void i2c_slave_callback(i2c_slave_callback_args_t * p_args)
{
    fsp_err_t err = FSP_SUCCESS;

    if (NULL != p_args)
    {
        switch (p_args->event)
        {
            case I2C_SLAVE_EVENT_RX_REQUEST:
            {
            	/* Perform Slave read operation */
#if (BSP_PERIPHERAL_IICA_PRESENT)
                err = R_IICA_SLAVE_Read(&g_iica_slave_ctrl, g_slave_rx_buf, BUF_LEN);
#elif defined (BOARD_RA6T2_MCK)
                err = R_IIC_B_SLAVE_Read(&g_i2c_slave_ctrl, g_slave_rx_buf, BUF_LEN);
#else
                err = R_IIC_SLAVE_Read(&g_i2c_slave_ctrl, g_slave_rx_buf, BUF_LEN);
#endif
                if (FSP_SUCCESS != err)
                {
                	/* Update return error here */
                	g_slave_api_ret_err = err;
                }

                break;
            }
            case I2C_SLAVE_EVENT_TX_REQUEST:
            {
                /* Perform Slave write operation */
#if (BSP_PERIPHERAL_IICA_PRESENT)
                err = R_IICA_SLAVE_Write(&g_iica_slave_ctrl, g_slave_tx_buf, BUF_LEN);
#elif defined (BOARD_RA6T2_MCK)
                err = R_IIC_B_SLAVE_Write(&g_i2c_slave_ctrl, g_slave_tx_buf, BUF_LEN);
#else
                err = R_IIC_SLAVE_Write(&g_i2c_slave_ctrl, g_slave_tx_buf, BUF_LEN);
#endif
                if (FSP_SUCCESS != err)
                {
                	/* Update return error here */
                	g_slave_api_ret_err = err;
                }

                break;
            }
            default:

                /* Log the event to global variable for any other Slave event */
                g_slave_event = p_args->event;
                break;
        }
    }
 }

/*******************************************************************************************************************//**
 * @brief     Toggle on board LED, which are connected and supported by BSP.
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
static void toggle_led(void)
{
    /* Get LED information (pins) for this board */
    bsp_leds_t leds = g_bsp_leds;

    for (uint8_t cnt = RESET_VALUE; leds.led_count > cnt; cnt++)
    {
        R_IOPORT_PinWrite(g_ioport.p_ctrl,(bsp_io_port_pin_t)leds.p_leds[cnt], LED_ON);
        R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

        R_IOPORT_PinWrite(g_ioport.p_ctrl,(bsp_io_port_pin_t)leds.p_leds[cnt], LED_OFF);
    }
}

/*******************************************************************************************************************//**
 *  @brief       Turn on board LED ON or OFF.
 *  @param[in]   led_state     LED_ON or LED_OFF.
 *  @retval      None
 **********************************************************************************************************************/
void set_led(bsp_io_level_t led_state)
{
    /* Get LED information (pins) for this board */
    bsp_leds_t leds = g_bsp_leds;

    for (uint8_t cnt = RESET_VALUE; leds.led_count > cnt; cnt++)
    {
       R_IOPORT_PinWrite(g_ioport.p_ctrl,(bsp_io_port_pin_t)leds.p_leds[cnt], led_state);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is called to do closing of external I2C Master and Slave module using its HAL level API.
 *            Close the IIC Master and Slave module. Handle the Error internally with Proper RTT Message.
 *            Application handles the rest.
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
void deinit_i2c_driver(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened IIC Master module */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_MASTER_Close(&g_iica_master_ctrl);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_MASTER_Close(&g_i2c_master_ctrl);
#else
    err = R_IIC_MASTER_Close(&g_i2c_master_ctrl);
#endif
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_"IIC_TYPE"_MASTER_Close API failed **\r\n");
    }

    /* Close opened IIC Slave module */
#if (BSP_PERIPHERAL_IICA_PRESENT)
    err = R_IICA_SLAVE_Close(&g_iica_slave_ctrl);
#elif defined (BOARD_RA6T2_MCK)
    err = R_IIC_B_SLAVE_Close(&g_i2c_slave_ctrl);
#else
    err = R_IIC_SLAVE_Close(&g_i2c_slave_ctrl);
#endif
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_"IIC_TYPE"_Slave_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iic_slave_ep)
 **********************************************************************************************************************/
