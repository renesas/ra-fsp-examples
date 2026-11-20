/***********************************************************************************************************************
 * File Name    : i3c_master_ep.c
 * Description  : Contains data structures and functions used in i3c_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "i3c_master_ep.h"

#ifdef I3C_HDR_DDR_SUPPORT
#include "i3c_master_hdr.h"
#endif /* I3C_HDR_DDR_SUPPORT */

/* Global variables */
static volatile uint32_t s_i3c_event_count[I3C_EVENT_SDA_WRITE_COMPLETE + 1];
static volatile uint32_t s_i3c_event_status = RESET_VALUE;

/* Configure the information for the Slave device */
static i3c_device_cfg_t master_device_cfg =
{
    /* This is the Static I3C/I2C Legacy address defined by the device manufacturer */
    .static_address = I3C_MASTER_DEVICE_STATIC_ADDRESS,

    /* The dynamic address will be automatically updated when the Master configures this device using CCC ENTDAA */
    .dynamic_address = I3C_MASTER_DEVICE_DYNAMIC_ADDRESS
};

/* I3C bus/device management */
static i3c_device_table_cfg_t s_device_table_cfg;
i3c_device_information_t g_device_slave_info;
uint32_t g_num_device_on_bus = RESET_VALUE;
uint8_t g_ibi_target_address = RESET_VALUE;
volatile bool g_b_bus_initialized = false;
volatile bool g_b_ibi_transfer_received = false;
volatile bool g_b_hot_join_done = false;

/* Callback flags — set in ISR context */
static volatile bool b_target_hj_received = false;
static volatile bool b_process_timeout = false;

/* Data buffers */
static uint8_t s_ibi_read_data[MAX_IBI_PAYLOAD_SIZE] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t s_read_data[2][MAX_READ_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
uint8_t g_write_data[MAX_WRITE_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
uint8_t *g_p_next = NULL;
uint8_t *g_p_last = NULL;
uint32_t g_data_transfer_size = RESET_VALUE;

/* Timer related timeout function */
static uint32_t s_timeout_value_in_ms = RESET_VALUE;

/* Setup the command descriptor */
i3c_command_descriptor_t command_descriptor;

uint32_t g_write_read_routine_count = RESET_VALUE;

/* Private function declarations */
static fsp_err_t i3c_write_sdr(void);
static fsp_err_t i3c_read_sdr(void);
static fsp_err_t i3c_dynamic_address_assign(void);
static fsp_err_t hot_join_request_process(void);
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms);

/***********************************************************************************************************************
 * Function Name: i3c_master_init.
 * Description  : This function initializes the I3C module as an I3C Master device, configures the bus, and
 *                waits for DAA completion. Sends a broadcast CCC if DAA does not complete in time.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t i3c_master_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the I3C module */
    err = R_I3C_Open(&g_i3c0_ctrl, &g_i3c0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the device configuration for this device */
    err = R_I3C_DeviceCfgSet(&g_i3c0_ctrl, &master_device_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the I3C device information through device table entries */
    memset(&s_device_table_cfg, RESET_VALUE, sizeof(i3c_device_table_cfg_t));

    s_device_table_cfg.dynamic_address = (uint8_t) (I3C_SLAVE_DEVICE_DYNAMIC_ADDRESS_START);
    s_device_table_cfg.device_protocol = I3C_DEVICE_PROTOCOL_I3C;
    s_device_table_cfg.ibi_accept = true;
    s_device_table_cfg.ibi_payload = true;
    s_device_table_cfg.master_request_accept = false;

    err = R_I3C_MasterDeviceTableSet(&g_i3c0_ctrl, RESET_VALUE, &s_device_table_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable I3C mode */
    err = R_I3C_Enable(&g_i3c0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the buffer for storing IBI data that is read from the Slave */
    err = R_I3C_IbiRead(&g_i3c0_ctrl, s_ibi_read_data, MAX_IBI_PAYLOAD_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset the buffer for storing data received during a read transfer */
    g_p_next = s_read_data[RESET_VALUE];

    /* Start assigning Dynamic Addresses to devices on the bus using the CCC ENTDAA command */
    err = i3c_dynamic_address_assign();
    if (FSP_SUCCESS != err && FSP_ERR_TIMEOUT != err)
    {
        return err;
    }

    if (!g_b_bus_initialized || (RESET_VALUE == g_num_device_on_bus))
    {
        /* Send broadcast signal for Dynamic Address Assignment */
        err = i3c_ccc_reset_address_send();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_master_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_master_process_event.
 * Description  : This function waits for I3C events and handles Hot-Join requests.
 * Arguments    : p_event_flag              Pointer to store detected I3C event flags.
 * Return Value : FSP_SUCCESS               Processed successfully.
 *                Any other error codes     Failed to process.
 **********************************************************************************************************************/
fsp_err_t i3c_master_process_event(uint32_t *p_event_flag)
{
    fsp_err_t err = FSP_SUCCESS;

    g_b_hot_join_done = false;

    /* Wait for I3C events */
    *p_event_flag = i3c_app_event_notify((I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE |
                                          I3C_EVENT_FLAG_COMMAND_COMPLETE |
                                          I3C_EVENT_FLAG_WRITE_COMPLETE |
                                          I3C_EVENT_FLAG_READ_COMPLETE |
                                          I3C_EVENT_FLAG_IBI_READ_COMPLETE |
                                          I3C_EVENT_FLAG_IBI_READ_BUFFER_FULL |
                                          I3C_EVENT_FLAG_INTERNAL_ERROR), WAIT_TIME);

    /* Check if event is IBI read complete and Hot-Join request received */
    if ((*p_event_flag & I3C_EVENT_FLAG_IBI_READ_COMPLETE) && b_target_hj_received)
    {
        err = hot_join_request_process();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        g_b_hot_join_done = true;
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_master_process_event
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_dynamic_address_assign.
 * Description  : This function starts Dynamic Address Assignment for Slave devices on bus.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Assigned successfully.
 *                Any other error codes     Failed to assign.
 **********************************************************************************************************************/
static fsp_err_t i3c_dynamic_address_assign(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    /* Start Dynamic Address Assignment using CCC ENTDAA command */
    err = R_I3C_DynamicAddressAssignmentStart(&g_i3c0_ctrl, I3C_ADDRESS_ASSIGNMENT_MODE_ENTDAA, RESET_VALUE, 1);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for address assignment complete event */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        return FSP_ERR_TIMEOUT;
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_dynamic_address_assign
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: hot_join_request_process.
 * Description  : This function processes Dynamic Address Assignment procedure when a Hot-Join event is received.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Processed successfully.
 *                Any other error codes     Failed to process.
 **********************************************************************************************************************/
static fsp_err_t hot_join_request_process(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* If a Hot-Join event is received, then the Master can initiate the Dynamic Address Assignment procedure */
    err = i3c_dynamic_address_assign();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset Hot-Join event flag */
    b_target_hj_received = false;

    /* Update number of device as 1 */
    g_num_device_on_bus = 1;

    return err;
}
/***********************************************************************************************************************
* End of function hot_join_request_process
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_ccc_reset_address_send.
 * Description  : This function sends a broadcast to reset Dynamic Address of all Slave devices on the bus.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Sent successfully.
 *                Any other error codes     Failed to send.
 **********************************************************************************************************************/
fsp_err_t i3c_ccc_reset_address_send(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    /* Set the command RSTDAA */
    command_descriptor.command_code = I3C_CCC_BROADCAST_RSTDAA;

    /* Set a buffer for storing the data read by the command */
    command_descriptor.p_buffer = NULL;

    /* RSTDAA has no payload, length is zero */
    command_descriptor.length = RESET_VALUE;

    /* Terminate the transfer with a Repeated Start condition */
    command_descriptor.restart = false;

    /* RSTDAA is a broadcast write command */
    command_descriptor.rnw = false;

    /* Send a broadcast or direct command to Slave devices on the bus */
    err = R_I3C_CommandSend(&g_i3c0_ctrl, &command_descriptor);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for command complete */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_COMMAND_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        return FSP_ERR_TIMEOUT;
    }

    g_num_device_on_bus = RESET_VALUE;

    return err;
}
/***********************************************************************************************************************
* End of function i3c_ccc_reset_address_send
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: g_i3c0_callback.
 * Description  : This function is callback for I3C.
 * Arguments    : p_args                    Callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void g_i3c0_callback(i3c_callback_args_t const *const p_args)
{
    /* Update the event in global array and this will be used in i3c_app_event_notify function */
    s_i3c_event_status = p_args->event_status;
    s_i3c_event_count[p_args->event]++;

    switch (p_args->event)
    {
        case I3C_EVENT_ENTDAA_ADDRESS_PHASE:
        {
            /* The device PID, DCR, and BCR registers will be available in i3c_callback_args_t::p_slave_info */
            g_device_slave_info.dynamic_address = (uint8_t)(I3C_SLAVE_DEVICE_DYNAMIC_ADDRESS_START);
            memcpy(g_device_slave_info.slave_info.pid, p_args->p_slave_info->pid, sizeof(p_args->p_slave_info->pid));
            g_device_slave_info.slave_info.bcr = p_args->p_slave_info->bcr;
            g_device_slave_info.slave_info.dcr = p_args->p_slave_info->dcr;
            break;
        }

        case I3C_EVENT_ADDRESS_ASSIGNMENT_COMPLETE:
        {
            /* Set the bus initialized flag */
            g_b_bus_initialized = true;
            break;
        }

        case I3C_EVENT_READ_COMPLETE:
        {
            /* The number of bytes returns from the Slave will be available in i3c_callback_args_t::transfer_size */
            g_data_transfer_size = p_args->transfer_size;
            set_next_read_buffer();
            break;
        }

        case I3C_EVENT_WRITE_COMPLETE:
        {
            /* The number of bytes writes to the Slave will be available in i3c_callback_args_t::transfer_size */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_COMMAND_COMPLETE:
        {
            /* The command code and transfer size will be available in p_args.
             * If the command code is a Broadcast or Direct Set, then data will
             * be stored in the read buffer provided by i3c_api_t::read.
             * If the command code is a Direct Get, then the data will be automatically
             * sent from device SFR. */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_IBI_READ_COMPLETE:
        {
            /* When an IBI is completed, the transfer_size, ibi_type, and ibi_address will be available in p_args */
            switch (p_args->ibi_type)
            {
                case I3C_IBI_TYPE_INTERRUPT:
                {
                    /* Notify the application that an IBI was read */
                    g_b_ibi_transfer_received = true;
                    g_data_transfer_size = p_args->transfer_size;
                    g_ibi_target_address = p_args->ibi_address;
                    break;
                }

                case I3C_IBI_TYPE_HOT_JOIN:
                {
                    b_target_hj_received = true;
                    break;
                }

                default:
                {
                    break;
                }
            }
            break;
        }

        default:
        {
           break;
        }
    }
}
/***********************************************************************************************************************
* End of function g_i3c0_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_app_event_notify.
 * Description  : This function starts the timer and waits for the event set in the I3C callback until the
 *                specified timeout.
 * Arguments    : set_event_flag_value      Requested event flag.
 *                timeout                   Specified timeout in milliseconds.
 * Return Value : I3C event flag value      Received I3C event flag successfully.
 *                0                         Timeout expires before the event is received.
 **********************************************************************************************************************/
uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t get_event_flag_value = RESET_VALUE;

    /* Reset the timeout flag */
    b_process_timeout = false;

    /* Start the timer */
    err = start_timeout_timer_with_defined_ms(timeout);
    if (FSP_SUCCESS != err)
    {
        return 0;
    }

    /* Wait for the event set in the I3C callback till specified timeout */
    while (!b_process_timeout)
    {
        /* Process for all I3C events */
        for (uint8_t cnt = RESET_VALUE; cnt < (I3C_EVENT_SDA_WRITE_COMPLETE+1); cnt++)
        {
            /* Check for callback event */
            if (s_i3c_event_count[cnt] > RESET_VALUE)
            {
                /* Store the event in local variable */
                get_event_flag_value |= (uint32_t)(0x1 << cnt);
                s_i3c_event_count[cnt] -= 1;
            }
        }

        /* Check for event received from I3C callback function is similar to event which the user wants */
        get_event_flag_value = (set_event_flag_value & get_event_flag_value);
        if (get_event_flag_value)
        {
            s_i3c_event_status = RESET_VALUE;
            return get_event_flag_value;
        }
    }
    return 0;
}
/***********************************************************************************************************************
* End of function i3c_app_event_notify
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: set_next_read_buffer.
 * Description  : This function sets the next read buffer.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void set_next_read_buffer(void)
{
    g_p_last = g_p_next;
    g_p_next = ((g_p_next == s_read_data[RESET_VALUE]) ? s_read_data[1] : s_read_data[RESET_VALUE]);
}
/***********************************************************************************************************************
* End of function set_next_read_buffer
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_write_sdr.
 * Description  : This function performs I3C write operation in SDR mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t i3c_write_sdr(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    /* Perform write operation in SDR mode */
    err = R_I3C_Write(&g_i3c0_ctrl, g_write_data, sizeof(g_write_data), false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for write complete event */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_WRITE_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        return FSP_ERR_TIMEOUT;
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_write_sdr
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_read_sdr.
 * Description  : This function performs I3C read operation in SDR mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
static fsp_err_t i3c_read_sdr(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    /* Perform read operation in SDR mode */
    err = R_I3C_Read(&g_i3c0_ctrl, g_p_next, MAX_READ_DATA_LEN, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for read complete event */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_READ_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        return FSP_ERR_TIMEOUT;
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_read_sdr
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: master_write_read_verify.
 * Description  : This function performs the I3C Master write and read operations in SDR Mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Verified successfully.
 *                Any other error codes     Failed to verify.
 **********************************************************************************************************************/
fsp_err_t master_write_read_verify(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update buffer */
    for (uint32_t cnt = RESET_VALUE; cnt < sizeof(g_write_data); cnt++)
    {
        g_write_data[cnt] = (uint8_t) (cnt + g_write_read_routine_count) & UINT8_MAX;
    }
    g_write_read_routine_count++;

    /* Switch to SDR mode */
    err = R_I3C_DeviceSelect(&g_i3c0_ctrl, 0, I3C_BITRATE_MODE_I3C_SDR0_STDBR);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Perform write operation */
    err = i3c_write_sdr();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for Slave device prepare buffer */
    R_BSP_SoftwareDelay(DELAY_FOR_SLAVE_PREPARE, BSP_DELAY_UNITS_MICROSECONDS);

    /* Start a read operation */
    err = i3c_read_sdr();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Compare read data with written data on Slave */
    if (RESET_VALUE != memcmp(g_write_data, g_p_last, sizeof(g_write_data)))
    {
        return FSP_ERR_INVALID_DATA;
    }

    return err;
}
/***********************************************************************************************************************
* End of function master_write_read_verify
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_deinit.
 * Description  : This function closes the opened I3C module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void i3c_deinit(void)
{
    if (g_i3c0_ctrl.open)
    {
        R_I3C_Close(&g_i3c0_ctrl);
    }
}
/***********************************************************************************************************************
* End of function i3c_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: g_timeout_timer_callback.
 * Description  : This function is callback for periodic mode timer and stops AGT0 timer in Periodic mode.
 * Arguments    : p_args                    Callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void g_timeout_timer_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Check if specified timeout is zero */
    if (RESET_VALUE == --s_timeout_value_in_ms)
    {
        /* Set the timeout flag */
        b_process_timeout = true;

        /* Stop AGT timer */
        R_AGT_Stop(&g_timeout_timer_ctrl);
    }
}
/***********************************************************************************************************************
* End of function g_timeout_timer_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: start_timeout_timer_with_defined_ms.
 * Description  : This function resets the counter value and starts the AGT timer.
 * Arguments    : timeout_ms                Specific timeout in millisecond.
 * Return Value : FSP_SUCCESS               Reset and started successfully.
 *                Any other error codes     Failed to reset or start.
 **********************************************************************************************************************/
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update the specified timeout into a global variable and this will be checked in timer callback */
    s_timeout_value_in_ms = timeout_ms;

    /* Reset the counter value */
    err = R_AGT_Reset(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start the AGT timer */
    err = R_AGT_Start(&g_timeout_timer_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function start_timeout_timer_with_defined_ms
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: agt_init.
 * Description  : This function initializes the AGT timer module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t agt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the AGT module */
    err = R_AGT_Open(&g_timeout_timer_ctrl, &g_timeout_timer_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function agt_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: agt_deinit.
 * Description  : This function closes the opened AGT module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void agt_deinit(void)
{
    if (g_timeout_timer_ctrl.open)
    {
        R_AGT_Close(&g_timeout_timer_ctrl);
    }
}
/***********************************************************************************************************************
* End of function agt_deinit
***********************************************************************************************************************/
