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
static uint32_t trim_newline(char *str);

/* Wait counter for wait operation monitoring */
static volatile uint32_t g_wait_count = MAX_COUNT;

/* Event flags for Master*/
static volatile spi_event_t g_master_event_flag;        /* Master Transfer Event completion flag */ 

/* SPI module buffers for Master */
static uint32_t g_master_tx_buff[BUFF_LEN];             /* Master Transmit Buffer */
static uint32_t g_master_rx_buff[BUFF_LEN];             /* Master Receive Buffer */
static uint32_t g_master_rx_validate_buff[BUFF_LEN];    /* Master Receive Buffer to validate */

/*******************************************************************************************************************//**
 * @brief       This functions initializes SPI Master modules.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 Upon successful open of SPI module
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t spi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Open/Initialize SPI Master module */
    err = R_SPI_Open (&g_spi_master_ctrl, &g_spi_master_cfg);

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        /* SPI Master Failure message */
        APP_ERR_PRINT("** R_SPI_Open API for SPI Master failed **\r\n");
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

    /* Wait until master writes/reads complete */
    while(g_master_event_flag != SPI_EVENT_TRANSFER_COMPLETE)
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
        else if (g_master_event_flag == SPI_EVENT_TRANSFER_ABORTED)
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
    g_master_event_flag = (spi_event_t) RESET_VALUE;  /* Reseting master_event flag */ 
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
    uint32_t    num_bytes       = RESET_VALUE;      /* Number of bytes read by SEGGER real-time-terminal */
    uint32_t    spi_length      = RESET_VALUE;      /* Master SPI length */ 
    uint8_t     returned_size   = RESET_VALUE;      /* Number of bytes read from Slave */ 
    uint8_t     validate_result = RESET_VALUE;      /* Result of comparing transmitted and received message */
    uint8_t     padding_needed  = RESET_VALUE;      /* Padding needed for 4-byte alignment */

    /* Clean buffers */
    memset(&g_master_tx_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));
    memset(&g_master_rx_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));

    /* Input to Master buffer */
    APP_PRINT("\r\nEnter text input for Master buffer. Data size should less than 40 bytes\r\n");
    while (BYTES_RECEIVED_ZERO == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(g_master_tx_buff);
            /* Remove new line character */
            num_bytes = trim_newline ((char *) &g_master_tx_buff[0]);
            if(BYTES_RECEIVED_ZERO == num_bytes)
            {
                APP_PRINT("\r\nEnter text input for Master buffer. Data size should less than 40 bytes\r\n");
            }
        }
    }

    /* RTT Reads user input data 1 byte at a time. SPI transfers the data 4 bytes at a time
     * With the below logic, we will calculate how many length of data has to be transferred */
    if (num_bytes % BITS_TO_BYTES != RESET_VALUE)
    {
        /* Calculate padding needed for 4-byte alignment */
        padding_needed = BITS_TO_BYTES - (num_bytes % BITS_TO_BYTES);
        num_bytes += padding_needed;
    }

    spi_length = num_bytes/BITS_TO_BYTES;

    /* Master sends the message size to Slave */
    err = R_SPI_Write(&g_spi_master_ctrl, (uint8_t *)&spi_length, ONE_BYTE, SPI_BIT_WIDTH_8_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_Write() failed");
        return err;
    }

    /* Wait until Master Write() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Wait 1ms for the Slave device to complete Read and Write operations */
    R_BSP_SoftwareDelay(DELAY_ONE_MS, BSP_DELAY_UNITS_MILLISECONDS);

    /* Master reads the return size from Slave */
    err = R_SPI_Read(&g_spi_master_ctrl, (uint8_t *)&returned_size, ONE_BYTE, SPI_BIT_WIDTH_8_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_Read() failed");
        return err;
    }

    /* Wait until Master Read() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }
    /* Compare the message size with the return size */
    if (spi_length != returned_size)
    {
        err = FSP_ERR_INVALID_DATA;
        APP_PRINT("\r\nThe message size is not same with the return size\r\n");
        error_print();
        return err;
    }

    /* Display the message Master sent to Slave */
    APP_PRINT("\r\nMaster sends to Slave with the message: %s\r\n", g_master_tx_buff);

    /* Master sends the message to Slave */
    err = R_SPI_Write(&g_spi_master_ctrl, g_master_tx_buff, spi_length, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_Write() failed");
        return err;
    }

    /* Wait until Master Write() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Wait 1ms for the Slave device to complete Read and Write operations */
    R_BSP_SoftwareDelay(DELAY_ONE_MS, BSP_DELAY_UNITS_MILLISECONDS);

    /* Master receives message from Slave */
    err = R_SPI_Read(&g_spi_master_ctrl, g_master_rx_buff, spi_length, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_Read() failed");
        return err;
    }

    /* Wait until Master Read() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Compare the transmitted message with the received message */
    validate_result = (uint8_t) memcmp(g_master_tx_buff, g_master_rx_buff, num_bytes);

    /* Wait 1ms for the Slave device to complete Read and Write operations */
    R_BSP_SoftwareDelay(DELAY_ONE_MS, BSP_DELAY_UNITS_MILLISECONDS);

    /* Master sends the validate result to Slave */
    err = R_SPI_Write(&g_spi_master_ctrl, (uint8_t *)&validate_result, ONE_BYTE, SPI_BIT_WIDTH_8_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_Write() failed");
        return err;
    }

    /* Wait until Master Write() complete */
    err = wait_transfer_end();

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    if(BUFF_EQUAL == validate_result)
    {
        /* Display Master's received message on RTT */
        APP_PRINT("\r\nMaster receives from Slave with the message: %s\r\n", g_master_rx_buff);
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
 * @brief       This function demos R_SPI_WriteRead() for Master simultaneously.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                                 Upon successful Write and Read on Master
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful Write and Read on Master
 **********************************************************************************************************************/
fsp_err_t spi_write_read(void)
{
    fsp_err_t   err       = FSP_SUCCESS;        /* Error status */
    uint32_t    num_bytes = RESET_VALUE;        /* Number of bytes read by SEGGER real-time-terminal */

    /* Clean buffers */
    memset(&g_master_tx_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));
    memset(&g_master_rx_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));
    memset(&g_master_rx_validate_buff[0], NULL_CHAR, (BUFF_LEN * BITS_TO_BYTES));

    /* Input to Master buffer */
    APP_PRINT("\r\nEnter text input for Master buffer. Data size should less than 40 bytes\r\n");
    while (BYTES_RECEIVED_ZERO == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = APP_READ(g_master_tx_buff);
            if(BYTES_RECEIVED_ZERO == num_bytes)
            {
                APP_PRINT("\r\nNo Input\r\n");
            }
        }
    }

    /* Remove new line character */
    num_bytes -= 1U;

    /* Master sends message to Slave and receives message from Slave */
    err = R_SPI_WriteRead(&g_spi_master_ctrl, g_master_tx_buff, g_master_rx_buff,
                          BUFF_LEN, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_WriteRead() failed");
        return err;
    }

    /* Wait until Master WriteRead() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Wait 1ms for the Slave device to complete WriteRead operation */
    R_BSP_SoftwareDelay(DELAY_ONE_MS, BSP_DELAY_UNITS_MILLISECONDS);

    /* Display the received message from Slave */
    APP_PRINT("\r\nMaster received from Slave with the message: %s\r\n", g_master_rx_buff);

    /* Master sends message to Slave and receives message from Slave */
    err = R_SPI_WriteRead(&g_spi_master_ctrl, g_master_rx_buff, g_master_rx_validate_buff,
                          BUFF_LEN, SPI_BIT_WIDTH_32_BITS);

    /* Error handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_WriteRead() failed");
        return err;
    }

    /* Wait until Master WriteRead() complete */
    err = wait_transfer_end();

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation\r\n");
        return err;
    }

    /* Compare the transmitted message with the received message from Slave to validate */
    if(BUFF_EQUAL == memcmp(g_master_tx_buff, g_master_rx_validate_buff, num_bytes))
    {
        /* Display Master's received message on RTT */
        APP_PRINT("\r\nSlave received from Master with the message: %s\r\n", g_master_rx_validate_buff);
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
 * @brief       This function close SPI Master modules.
 * @param[IN]   None 
 * @retval      FSP_SUCCESS                                 SPI module closed successfully
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful close
 **********************************************************************************************************************/
fsp_err_t spi_exit_demo(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Close SPI Master module */
    err = R_SPI_Close(&g_spi_master_ctrl);

    /* Error Handle */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nMaster R_SPI_Close() failed");
        return err;
    }
    return err;
}

/**********************************************************************************************************************
 *  @brief      Removes trailing newline ('\n') and carriage return ('\r') characters from a string.
 *  @param[in]  str: str pointer.
 *  @retval     len: The length of the string after trimming.
 *********************************************************************************************************************/
uint32_t trim_newline(char *str)
{
    uint32_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
    return len;
}

/*******************************************************************************************************************//**
 * @brief       Master SPI callback function.
 * @param[in]   p_args
 * @retval      None
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

    /* Close SPI Master module */
    err = R_SPI_Close(&g_spi_master_ctrl);

    /* Error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_SPI_Close API for master failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup spi_ep)
 **********************************************************************************************************************/
