/***********************************************************************************************************************
 * File Name    : spi_ep.c
 * Description  : Contains data structures and functions used in spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "spi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup spi_ep
 * @{
 **********************************************************************************************************************/
/*
 * Private function declarations
 */
static void error_print(void);
static fsp_err_t wait_transfer_end(void);

/* Wait counter for wait operation monitoring */
static volatile uint32_t g_wait_count = MAX_COUNT;

/* Event flags for Slave */
static volatile spi_event_t g_slave_event_flag;         /* Slave Transfer Event completion flag */ 

/* SPI module buffers for Slave */
static uint32_t g_slave_rx_buff[BUFF_LEN];              /* Slave Receive Buffer */ 
static uint32_t g_slave_rx_validate_buff[BUFF_LEN];     /* Slave Receive Buffer to validate */

/*******************************************************************************************************************//**
 * @brief       This functions initializes SPI Slave modules.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 Upon successful open of SPI module
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t spi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Open/Initialize SPI Slave module */
    err = R_SPI_Open (&g_spi_slave_ctrl, &g_spi_slave_cfg);

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        /* SPI Slave Failure message */
        APP_ERR_PRINT("** R_SPI_Open API for SPI Slave failed **\r\n");
        return err;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function checks occurrence of data transfer complete event until timeout occurs and handles errors.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 Upon successful transfer data of SPI module
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful transfer
 **********************************************************************************************************************/
fsp_err_t wait_transfer_end(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */
    g_wait_count = MAX_COUNT;

    /* Wait until slave writes/reads complete */
    while(g_slave_event_flag != SPI_EVENT_TRANSFER_COMPLETE)
    {
        /* Time out operation if SPI operation fails to complete */
        g_wait_count--;
        if (MIN_COUNT >= g_wait_count)
        {
            /* Return time out error if SPI operation fails to complete */
            err = FSP_ERR_TIMEOUT;
            APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
            error_print();
            return err;
        }
        else if (g_slave_event_flag == SPI_EVENT_TRANSFER_ABORTED)
        {
            err = FSP_ERR_TRANSFER_ABORTED;
            APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
            error_print();
            return err;
        }
        else
        {
            /* Do nothing */
        }
        R_BSP_SoftwareDelay(DELAY_ONE_US , BSP_DELAY_UNITS_MICROSECONDS);
    }
    g_slave_event_flag = (spi_event_t) RESET_VALUE;     /* Reseting slave_event flag */ 
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function demos both R_SPI_Write() and R_SPI_Read() individually.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 Upon successful SPI Write and SPI Read
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful Write and Read
 **********************************************************************************************************************/
fsp_err_t spi_write_and_read(void)
{
    fsp_err_t   err             = FSP_SUCCESS;      /* Error status */
    uint8_t     received_size   = RESET_VALUE;      /* Number of bytes receive from Master */
    uint8_t     validate_result = RESET_VALUE;      /* Result of comparing transmitted and received message */
    
    /* Clean buffer */
    memset(&g_slave_rx_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));

    APP_PRINT("\r\nWaiting for the transmitted message from the Master\r\n");

    /* Slave receives the message size from Master */
    err = R_SPI_Read(&g_spi_slave_ctrl, (uint8_t *)&received_size, ONE_BYTE, SPI_BIT_WIDTH_8_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_Read() failed");
        return err;
    }

    /* Wait until Slave Read() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Slave sends the received size to Master */
    err = R_SPI_Write(&g_spi_slave_ctrl, (uint8_t *)&received_size, ONE_BYTE, SPI_BIT_WIDTH_8_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_Write() failed");
        return err;
    }

    /* Wait until Slave Write() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Slave receives message from Master */
    err = R_SPI_Read(&g_spi_slave_ctrl, g_slave_rx_buff, received_size, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_Read() failed");
        return err;
    }

    /* Wait until Slave Read() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Slave sends the received message to Master */
    err = R_SPI_Write(&g_spi_slave_ctrl, g_slave_rx_buff, received_size, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_Write() failed");
        return err;
    }

    /* Wait until Slave Write() complete */
    err = wait_transfer_end();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Slave receives the validate result from Master */
    err = R_SPI_Read(&g_spi_slave_ctrl,(uint8_t *)&validate_result, ONE_BYTE, SPI_BIT_WIDTH_8_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_Read() failed");
        return err;
    }

    /* Wait until Slave Read() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }
    
    if (BUFF_EQUAL == validate_result)
    {
        /* Display the received message from Master */
        APP_PRINT("\r\nMaster sends to Slave with the message: %s\r\n", g_slave_rx_buff);
    }
    else
    {
        /* Return Parity error in case of data mismatch */
        err = FSP_ERR_SPI_PARITY;
        APP_PRINT("\r\nSlave received a message that does not match the one transmitted by Master\r\n");
        error_print();
        return err;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function demos R_SPI_WriteRead() for Slave simultaneously.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 Upon successful Write and Read on Slave
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful Write and Read on Slave
 **********************************************************************************************************************/
fsp_err_t spi_write_read(void)
{
    fsp_err_t   err                  = FSP_SUCCESS;      /* Error status */
    uint8_t     slave_message_buff[] = SLAVE_MESSAGE;    /* Slave message Buffer */

    /* Clean buffers */
    memset(&g_slave_rx_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));
    memset(&g_slave_rx_validate_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));

    APP_PRINT("\r\nWaiting for the transmitted message from the Master\r\n");

    /* Slave sends the fixed-content message to Master and receives the message from Master */
    err = R_SPI_WriteRead(&g_spi_slave_ctrl, slave_message_buff, g_slave_rx_buff, BUFF_LEN, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_WriteRead() failed");
        return err;
    }

    /* Wait until Slave WriteRead() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Display the received message from Master */
    APP_PRINT("\r\nSlave received from Master with the message: %s\r\n", g_slave_rx_buff);

    /* Slave sends the message to Master and receives the message from Master */
    err = R_SPI_WriteRead(&g_spi_slave_ctrl, g_slave_rx_buff, g_slave_rx_validate_buff,
                          BUFF_LEN, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_WriteRead() failed");
        return err;
    }

    /* Wait until Slave WriteRead() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Compare the fixed-content message with the received message from Master to validate */
    if(BUFF_EQUAL == memcmp(SLAVE_MESSAGE, g_slave_rx_validate_buff, SLAVE_MESSAGE_SIZE))
    {
        /* Display Slave's message on RTT */
        APP_PRINT("\r\nMaster received from Slave with the message: %s\r\n", g_slave_rx_validate_buff);
    }
    else
    {
        /* Return Parity error in case of data mismatch */
        err = FSP_ERR_SPI_PARITY;
        APP_PRINT("\r\nMaster received a message that does not match the one transmitted by Slave\r\n");
        error_print();
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function close SPI Slave modules.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 SPI module closed successfully
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful close
 **********************************************************************************************************************/
fsp_err_t spi_exit_demo(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Close SPI Slave module */
    err = R_SPI_Close(&g_spi_slave_ctrl);
    
	/* Error Handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSlave R_SPI_Close() failed");
        return err;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief      Slave SPI callback function.
 * @param[in]  p_args
 * @retval     None
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

/*******************************************************************************************************************//**
 * @brief       This function prints the error message.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void error_print(void)
{
    APP_PRINT("\r\nError in configuration or connection\r\n");
    APP_PRINT("\r\nReset the MCU...\r\n");
}

/*******************************************************************************************************************//**
 * @brief       This function closes the opened SPI module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void spi_clean_up(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Close SPI Slave module */
    err = R_SPI_Close(&g_spi_slave_ctrl);

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_SPI_Close API for slave failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup spi_ep)
 **********************************************************************************************************************/
