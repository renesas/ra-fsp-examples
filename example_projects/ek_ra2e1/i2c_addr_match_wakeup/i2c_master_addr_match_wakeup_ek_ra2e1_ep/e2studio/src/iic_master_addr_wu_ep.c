/***********************************************************************************************************************
 * File Name    : iic_master_addr_wu_ep.c
 * Description  : Contains data structures and functions used in iic_master_addr_wu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"
#include "iic_master_addr_wu_ep.h"

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
volatile bool g_iic_rx_complete_flag  = false;
volatile bool g_iic_tx_complete_flag  = false;
volatile bool g_iic_abort_flag        = false;

uint8_t g_iic_data[BUFF_SIZE] = {RESET_VALUE};

/**********************************************************************************************************************
* Private function declarations
**********************************************************************************************************************/
static fsp_err_t iic_master_init(void);
static void iic_master_deinit(void);
static fsp_err_t iic_wait_transfer_end(void);
static fsp_err_t iic_master_write(void);
static fsp_err_t iic_master_read(void);
static fsp_err_t iic_master_wakeup_ep_main_process(void);
static fsp_err_t iic_master_operation(void);
static fsp_err_t set_slave_address(void);
static uint8_t get_user_input(void);

/***********************************************************************************************************************
 * Function Name: iic_master_wakeup_ep_entry
 * Description  : This function starts the IIC Master address match wakeup example operation.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_master_wakeup_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = iic_master_wakeup_ep_main_process();
    handle_error(err, "\r\niic_wakeup_ep_main_process failed!!!\r\n");
}
/***********************************************************************************************************************
* End of function iic_master_wakeup_ep_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_wakeup_ep_main_process
 * Description  : This function performs the main process of the example project.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_master_wakeup_ep_main_process(void)
{
    fsp_pack_version_t  version = {RESET_VALUE};
    fsp_err_t           err     = FSP_SUCCESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Turn OFF the error LED */
    if (g_bsp_leds.led_count > 1)
    {
        LED_TURN_OFF(ERR_LED);
    }

    /* Turn OFF the state LED */
    LED_TURN_OFF(STATE_LED);

#if (USE_SERIAL_TERM == 1)
    /* Initialize UART module first to print log to serial terminal */
    err = TERM_INIT;
    if (FSP_SUCCESS != err)
    {
        /* Turn ON the error LED to indicate TERM_INIT failed */
        if (g_bsp_leds.led_count > 1)
        {
            LED_TURN_ON(ERR_LED);
        }

        /* Error trap */
        ERROR_TRAP;
    }
#endif /* USE_SERIAL_TERM */

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    /* Print EP information and the board type */
    APP_PRINT(EP_INFO);
    APP_PRINT(BOARD_TYPE);

    /* Initialize the IIC Master module */
    err = iic_master_init();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\niic_master_init failed!!!\r\n");

    /* Turn ON the state LED */
    LED_TURN_ON(STATE_LED);

    while (true)
    {
        /* Print EP menu */
        APP_PRINT(EP_MENU);

        /* Perform IIC Master operation */
        err = iic_master_operation();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\niic_master_operation failed!!!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function iic_master_wakeup_ep_main_process
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_operation
 * Description  : This function performs the IIC Master operation.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_master_operation(void)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     user_input  = RESET_VALUE;

    /* Get the user input */
    user_input = get_user_input();
    switch (user_input)
    {
        case MASTER_READ:
        {
            /* Set the Slave address */
            err = set_slave_address();
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nset_slave_address failed!!!\r\n");

            /* Perform IIC Master read operation */
            err = iic_master_read();
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\niic_master_read failed!!!\r\n");
            break;
        }

        case MASTER_WRITE:
        {
            /* Set the Slave address */
            err = set_slave_address();
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nset_slave_address failed!!!\r\n");

            /* Perform IIC Master write operation */
            err = iic_master_write();
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\niic_master_write failed!!!\r\n");
            break;
        }
        default:
        {
            APP_PRINT("\r\n\r\nInvalid input!!! Please enter correct input as below\r\n");
            break;
        }
    }
    return err;
}
/***********************************************************************************************************************
* End of function iic_master_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_init
 * Description  : This function initializes the IIC Master module
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_master_init(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    /* Initialize IIC Master module */
    err = R_IIC_MASTER_Open(&g_i2c_master_ctrl, &g_i2c_master_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_IIC_MASTER_Open API failed!!!***\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function iic_master_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_deinit
 * Description  : This function de-initializes the IIC Master module
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void iic_master_deinit(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    /* De-initialize the IIC Master module if it had already opened */
    if (MODULE_CLOSE != g_i2c_master_ctrl.open)
    {
        err = R_IIC_MASTER_Close(&g_i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n***R_IIC_MASTER_Close API failed!!!***\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function iic_master_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wait_transfer_end
 * Description  : This function waits until the IIC operation had completed successfully.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_wait_transfer_end(void)
{
    fsp_err_t   err     = FSP_SUCCESS;
    uint32_t    timeout = RESET_VALUE;

    /* Set the timeout value for IIC transfer based on the message length */
    timeout = SAFETY_FACTOR * BUFF_SIZE * (BSP_DELAY_UNITS_SECONDS / IIC_BAUD_RATE);

    while (true)
    {
        if (g_iic_abort_flag)
        {
            APP_PRINT("\r\nSlave address mismatch detected!!!\r\n");

            /* Clear abort flag */
            g_iic_abort_flag = false;
            break;
        }
        else if ((g_iic_rx_complete_flag) || (g_iic_tx_complete_flag))
        {
            /* Delay to synchronize Master-Slave communication */
            R_BSP_SoftwareDelay(DELAY_TEN_MS, BSP_DELAY_UNITS_MILLISECONDS);
            break;
        }
        else
        {
            timeout--;
            if (RESET_VALUE == timeout)
            {
                return FSP_ERR_TIMEOUT;
            }

            R_BSP_SoftwareDelay(IIC_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function iic_wait_transfer_end
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_write
 * Description  : This function performs a master write operation
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_master_write(void)
{
    fsp_err_t err    = FSP_SUCCESS;

    /* Prepare g_iic_data buffer */
    memset(&g_iic_data[0], RESET_VALUE, sizeof(g_iic_data));
    memcpy(&g_iic_data[0], MASTER_MESSAGE, MASTER_MESSAGE_SIZE);

    /* Clear tx_complete flag */
    g_iic_tx_complete_flag = false;

    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, &g_iic_data[0], MASTER_MESSAGE_SIZE, false);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_IIC_MASTER_Write API failed!!!***");

    err = iic_wait_transfer_end();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n\r\nWrite the message to the Slave board failed!!!\r\n");

    if (g_iic_tx_complete_flag)
    {
        APP_PRINT("\r\n\r\nWrite the message to the Slave board was successful!\r\n");

        /* Clear tx_complete flag */
        g_iic_tx_complete_flag = false;
    }

    return err;
}
/***********************************************************************************************************************
* End of function iic_master_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_read
 * Description  : This function performs a master read operation
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_master_read(void)
{
    fsp_err_t err   = FSP_SUCCESS;

    /* Clean g_iic_data buffer */
    memset(&g_iic_data[0], RESET_VALUE, sizeof(g_iic_data));

    /* Clear rx_complete flag */
    g_iic_rx_complete_flag = false;

    err = R_IIC_MASTER_Read(&g_i2c_master_ctrl, &g_iic_data[0], EXPECTED_MESSAGE_SIZE, false);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_IIC_MASTER_Read API failed!!!***");

    err = iic_wait_transfer_end();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n\r\nRead the message from the Slave board failed!!!");

    if (g_iic_rx_complete_flag)
    {
        APP_PRINT("\r\n\r\nRead the message from the Slave board was successful!");
        if (RESET_VALUE != memcmp (&g_iic_data[0], EXPECTED_MESSAGE, EXPECTED_MESSAGE_SIZE))
        {
            err = FSP_ERR_TRANSFER_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nThe received data mismatches the expected data!!!\r\n");
        }
        else
        {
            APP_PRINT("\r\nThe received data matches the expected data!\r\n");
        }

        /* Clear rx_complete flag */
        g_iic_rx_complete_flag = false;
    }

    return err;
}
/***********************************************************************************************************************
* End of function iic_master_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: set_slave_address
 * Description  : This function gets the slave address from the user and the sets slave address and addressing mode of
 *                the slave board
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t set_slave_address(void)
{
    fsp_err_t   err             = FSP_SUCCESS;
    uint8_t     user_input      = RESET_VALUE;
    uint8_t     slave_address   = RESET_VALUE;

    while (RESET_VALUE == slave_address)
    {
        APP_PRINT(ADDR_MENU);
        user_input = get_user_input();
        switch (user_input)
        {
            case SLAVE_ADDR_1:
            {
                slave_address = CORRECT_SLAVE_ADDR;
                APP_PRINT("\r\nSelected Slave address: 0x%X \r\n", slave_address);
                break;
            }
            case SLAVE_ADDR_2:
            {
                slave_address = INCORRECT_SLAVE_ADDR;
                APP_PRINT("\r\nSelected Slave address: 0x%X \r\n", slave_address);
                break;
            }
            default:
            {
                APP_PRINT("\r\n\r\nInvalid input!!! Please enter correct input as below\r\n");
                break;
            }
        }
    }

    /* Set address and addressing mode of the slave board */
    err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_ctrl, slave_address, I2C_MASTER_ADDR_MODE_7BIT);
    APP_ERR_RET(FSP_SUCCESS != err, err, "***\r\nR_IIC_MASTER_SlaveAddressSet API failed!!!***\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function set_slave_address
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_user_input
 * Description  : This function is called to get user input from Terminal Application.
 * Arguments    : None
 * Return value : input_value   Return input value after receive enter key
 **********************************************************************************************************************/
static uint8_t get_user_input(void)
{
    uint8_t input_value = RESET_VALUE;
    char    user_input[TERM_BUFFER_SIZE + 1];

    /* Clean buffer */
    memset(&user_input[0], NULL_CHAR, sizeof(user_input));

    /* Wait until there is any user input */
    while (!APP_CHECK_DATA)
    {
        __NOP();
    }

    /* Read user input from the terminal */
    APP_READ(&user_input[0], TERM_BUFFER_SIZE);

    /* Convert to integer value */
    input_value = (uint8_t)atoi((char*) &user_input[0]);

    return input_value;
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_master_callback
 * Description  : This function performs IIC Master callback
 * Arguments    : p_args    Pointer to callback function
 * Return value : None
 **********************************************************************************************************************/
void iic_master_callback(i2c_master_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case I2C_MASTER_EVENT_RX_COMPLETE:
        {
            /* Set rx_complete flag */
            g_iic_rx_complete_flag = true;
            break;
        }
        case I2C_MASTER_EVENT_TX_COMPLETE:
        {
            /* Set tx_complete flag */
            g_iic_tx_complete_flag = true;
            break;
        }
        case I2C_MASTER_EVENT_ABORTED:
        {
            /* Set abort flag */
            g_iic_abort_flag = true;
            break;
        }
        default:
        {
            /* Do nothing */
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function iic_master_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error
 * Description  : This function handles error if any error occurred, closes opened IIC Master module and
 *                traps the error.
 * Arguments    : err       error status
 *                err_str   error string
 * Return Value : None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Turn ON the error LED and turn OFF the state LED when an error occurred */
        if (g_bsp_leds.led_count > 1)
        {
            LED_TURN_ON(ERR_LED);
        }
        LED_TURN_OFF(STATE_LED);

        /* Print the error message */
        APP_ERR_PRINT(err_str);

        /* De-initialize IIC Master module */
        iic_master_deinit();

        /* Return the error code and trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
