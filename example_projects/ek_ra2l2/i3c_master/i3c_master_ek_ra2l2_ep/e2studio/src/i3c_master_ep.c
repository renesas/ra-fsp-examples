/***********************************************************************************************************************
* File Name    : i3c_master_ep.c
* Description  : Contains macros data structures and functions used in i3c_master_ep.c.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "i3c_master_ep.h"

#ifdef I3C_HDR_DDR_SUPPORT
#include "i3c_master_hdr.h"
#endif /* I3C_HDR_DDR_SUPPORT */

/* Global variables */
static volatile uint32_t g_i3c_event_count[I3C_EVENT_SDA_WRITE_COMPLETE + ONE];
static volatile uint32_t g_i3c_event_status = RESET_VALUE;

/* Configure the information for the slave device */
static i3c_device_cfg_t master_device_cfg =
{
    /* This is the Static I3C / I2C Legacy address defined by the device manufacturer */
    .static_address  = I3C_MASTER_DEVICE_STATIC_ADDRESS,
    /* The dynamic address will be automatically updated when the master configures this device using CCC ENTDAA */
    .dynamic_address = I3C_MASTER_DEVICE_DYNAMIC_ADDRESS
};

/* I3C bus/device management */
static i3c_device_table_cfg_t       g_device_table_cfg;
static i3c_device_information_t     g_device_slave_info;
static uint32_t                     g_num_device_on_bus = RESET_VALUE;
static uint8_t                      g_ibi_target_address = RESET_VALUE;
static volatile bool                b_bus_initialized = false;
static volatile bool                b_target_hj_received = false;
static volatile bool                b_target_ibi_transfer_received = false;

/* Data buffers */
static uint8_t                      g_ibi_read_data[MAX_IBI_PAYLOAD_SIZE] BSP_ALIGN_VARIABLE(WORD_ALIGN);
uint8_t                             g_write_data[MAX_WRITE_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t                      g_read_data[2][MAX_READ_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
uint8_t                             * p_next = NULL;
uint8_t                             * p_last = NULL;
uint32_t                            g_data_transfer_size = RESET_VALUE;

/* Timer related timeout functions */
static uint32_t timeout_value_in_ms = RESET_VALUE;

/* Setup the command descriptor */
i3c_command_descriptor_t command_descriptor;

uint32_t      g_write_read_routine_count = RESET_VALUE;
static volatile bool b_process_timeout = false;

/* Private function declarations */
static fsp_err_t i3c_broadcast_ccc_send(void);
static fsp_err_t check_disp_i3c_slaveInfo(void);
static fsp_err_t master_write_read_verify(void);
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms);
static fsp_err_t hot_join_request_process(void);
static void i3c_deinit(void);
static void agt_deinit(void);

/**********************************************************************************************************************
 *  Function Name: i3c_master_entry
 *  Description  : This function is used to start the i3c_master example operation.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void i3c_master_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    unsigned char input_data[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint8_t converted_rtt_input = RESET_VALUE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);
    APP_PRINT(BANNER_INFO,EP_VERSION,version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch );
    APP_PRINT(EP_INFO);

    /* Initialize AGT driver */
    err = R_AGT_Open(&g_timeout_timer_ctrl, &g_timeout_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_AGT_Open API FAILED \r\n");
        APP_ERR_TRAP(err);
    }

    /* Initializes the I3C module */
    err = R_I3C_Open(&g_i3c0_ctrl, &g_i3c0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_Open API FAILED \r\n");
        /* De-initialize the opened AGT timer module */
        agt_deinit();
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nINFO: I3C Initialized successfully in master mode.\r\n");

    /* Set the device configuration for this device */
    err = R_I3C_DeviceCfgSet(&g_i3c0_ctrl, &master_device_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_DeviceCfgSet API FAILED \r\n");
        /* De-initialize the opened I3C and AGT timer module */
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Set the I3C devices information through device table entries */
    memset(&g_device_table_cfg, RESET_VALUE, sizeof(i3c_device_table_cfg_t));

    g_device_table_cfg.dynamic_address = (uint8_t)(I3C_SLAVE_DEVICE_DYNAMIC_ADDRESS_START);
    g_device_table_cfg.device_protocol = I3C_DEVICE_PROTOCOL_I3C;
    g_device_table_cfg.ibi_accept = true;
    g_device_table_cfg.ibi_payload = true;
    g_device_table_cfg.master_request_accept = false;

    err = R_I3C_MasterDeviceTableSet(&g_i3c0_ctrl, RESET_VALUE, &g_device_table_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_MasterDeviceTableSet API FAILED \r\n");
        /* De-initialize the opened I3C and AGT timer module */
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Enable I3C Mode */
    err = R_I3C_Enable(&g_i3c0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_Enable API FAILED \r\n");
        /* De-initialize the opened I3C and AGT timer module */
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Set the buffer for storing IBI data that is read from the slave */
    err = R_I3C_IbiRead(&g_i3c0_ctrl, g_ibi_read_data, MAX_IBI_PAYLOAD_SIZE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_IbiRead API FAILED \r\n");
        /* De-initialize the opened I3C and AGT timer module */
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Reset the buffer for storing data received during a read transfer */
    p_next = g_read_data[RESET_VALUE];

    /* Start assigning dynamic addresses to devices on the bus using the CCC ENTDAA command. */
    /* We have one slave device hence last argument is ONE */
    err = R_I3C_DynamicAddressAssignmentStart(&g_i3c0_ctrl, I3C_ADDRESS_ASSIGNMENT_MODE_ENTDAA, RESET_VALUE, ONE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_DynamicAddressAssignmentStart API FAILED \r\n");
        /* De-initialize the opened I3C and AGT timer module */
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Waiting for the bus initialization. */
    /* Hold up the application until the DAA is completed */
    i3c_app_event_notify(I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE, WAIT_TIME);
    if (b_bus_initialized && g_num_device_on_bus)
    {
        APP_PRINT ("\r\nINFO: Bus configuration is completed successfully.\r\n");
        APP_PRINT ("INFO: Number of I3C device on bus: %d **\r\n", g_num_device_on_bus);
    }
    else
    {
        APP_PRINT("INFO: Sending CCC broadcast signal for Dynamic address assignment\r\n");
        /* Sending broadcast signal */
        err = i3c_broadcast_ccc_send();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nERROR: i3c_broadcast_ccc_send function failed.\r\n");
            /* De-initialize the opened I3C and AGT timer module */
            i3c_deinit();
            agt_deinit();
            APP_ERR_TRAP(err);
        }
    }
    /* Print menu option */
    APP_PRINT(EP_FUNCTION_MENU);

    while (true)
    {
        /* Check for RTT input from the user */
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&input_data[RESET_VALUE], NULL_CHAR, BUFFER_SIZE_DOWN);
            converted_rtt_input = RESET_VALUE;

            /* Read RTT input from the user */
            APP_READ (input_data);
            converted_rtt_input = (uint8_t)atoi((char *)input_data);

            switch (converted_rtt_input)
            {
                case DISPLAY_I3C_SLAVE_INFO:
                {
                    /* Display slave info */
                    check_disp_i3c_slaveInfo();
                    break;
                }

                case MASTER_WRITE_READ:
                {
                    /* Perform master write read operation */
                    err = master_write_read_verify();
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nERROR: master_write_read_verify function failed.\r\n");
                        /* De-initialize the opened I3C and AGT timer module */
                        i3c_deinit();
                        agt_deinit();
                        APP_ERR_TRAP(err);
                    }
                    break;
                }

#ifdef I3C_HDR_DDR_SUPPORT
                case MASTER_WRITE_READ_HDR:
                {
                    if (I3C_HDR_DDR_MODE == g_device_slave_info.slave_info.bcr_b.hdr_capable)
                    {
                        /* Perform master write read operation in HDR-DDR */
                        err = master_write_read_hdr_verify();
                        if (FSP_SUCCESS != err)
                        {
                            APP_ERR_PRINT("\r\nERROR: master_write_read_verify function failed.\r\n");
                            /* De-initialize the opened I3C and AGT timer module */
                            i3c_deinit();
                            agt_deinit();
                            APP_ERR_TRAP(err);
                        }
                    }
                    else
                    {
                        APP_PRINT("\r\nThis slave has not been enabled or not supported I3C HDR-DDR mode\r\n");
                    }

                    break;
                }
#endif /* I3C_HDR_DDR_SUPPORT */

                default:
                {
                    APP_PRINT("Invalid Input\r\n");
                    break;
                }
            } /* Switch case end */
            APP_PRINT(EP_FUNCTION_MENU);
        }/* If APP_CHECK_DATA end */

        /* Wait for I3C events */
        uint32_t event_flag = i3c_app_event_notify((I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE |
                I3C_EVENT_FLAG_COMMAND_COMPLETE |
                I3C_EVENT_FLAG_WRITE_COMPLETE |
                I3C_EVENT_FLAG_READ_COMPLETE |
                I3C_EVENT_FLAG_IBI_READ_COMPLETE |
                I3C_EVENT_FLAG_IBI_READ_BUFFER_FULL |
                I3C_EVENT_FLAG_INTERNAL_ERROR), WAIT_TIME);

        /* Check if event is IBI read complete and hot-join request received */
        if ((event_flag & I3C_EVENT_FLAG_IBI_READ_COMPLETE) && (b_target_hj_received))
        {
            /* Perform hot join request process */
            err = hot_join_request_process();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nERROR: hot_join_request_process function failed.\r\n");
                /* De-initialize the opened I3C and AGT timer module */
                i3c_deinit();
                agt_deinit();
                APP_ERR_TRAP(err);
            }
        }

        /* Check if event is IBI read complete and IBI transfer is received */
        if ((event_flag & I3C_EVENT_FLAG_IBI_READ_COMPLETE) && (b_target_ibi_transfer_received))
        {
            APP_PRINT ("\r\nINFO: a slave IBI transfer is received.\r\n");
            APP_PRINT ("\r\nINFO: Target address:0x%02x, IBI Payload size: 0x%x\r\n", g_ibi_target_address,
                       g_data_transfer_size);
            b_target_ibi_transfer_received = false;
        }
    }
}
/**********************************************************************************************************************
* End of function i3c_master_entry
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: hot_join_request_process
 *  Description  : This function processes dynamic address assignment procedure, If a Hot-Join event is received.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t hot_join_request_process(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    APP_PRINT ("\r\nINFO: A hot Join event is received, Initiate DAA using CCC transmission.\r\n");

    /* If a Hot-Join event is received, then the master can initiate the dynamic address assignment procedure */
    err = R_I3C_DynamicAddressAssignmentStart(&g_i3c0_ctrl, I3C_ADDRESS_ASSIGNMENT_MODE_ENTDAA, RESET_VALUE, ONE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_DynamicAddressAssignmentStart API FAILED.\r\n");
        return err;
    }

    /* Wait for address assignment complete event */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        APP_ERR_PRINT("\r\nERROR: Requested event not received with in specified timeout.\r\n");
        err = FSP_ERR_TIMEOUT;
    }

    /* Reset hot joint event flag */
    b_target_hj_received = false;
    /* Update number of device as ONE */
    g_num_device_on_bus = ONE;
    APP_PRINT("\r\nINFO: DAA using CCC transmission completed. \r\nPlease check by pressing user "
              "input 1 (available in the menu option) for slave information\r\n");
    return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function hot_join_request_process
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_broadcast_ccc_send
 *  Description  : This function send a broadcast or direct command to slave devices on the bus.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t i3c_broadcast_ccc_send(void)
{
    fsp_err_t       err = FSP_SUCCESS;
    uint32_t        status = RESET_VALUE;

    /* Send the command RSTDAA */
    command_descriptor.command_code = I3C_CCC_BROADCAST_RSTDAA;
    /* Set a buffer for storing the data read by the command */
    command_descriptor.p_buffer = NULL;
    /* The length for a GETMWL command is 2 bytes */
    command_descriptor.length = RESET_VALUE;
    /* Terminate the transfer with a Repeated Start condition */
    command_descriptor.restart = false;
    /* The GETMWL command is a Direct Get Command */
    command_descriptor.rnw = false;

    /* Send a broadcast or direct command to slave devices on the bus */
    err = R_I3C_CommandSend(&g_i3c0_ctrl, &command_descriptor);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_I3C_CommandSend API FAILED.\r\n");
        return err;
    }

    /* Wait for command complete */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_COMMAND_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        APP_ERR_PRINT("\r\nERROR: Requested event not received with in specified timeout.\r\n");
        err = FSP_ERR_TIMEOUT;
    }

    APP_PRINT ("\r\nINFO: CCC Dynamic Address Assignment transfer completed successfully.\r\n");
    g_num_device_on_bus = RESET_VALUE;

    return err;
}
/**********************************************************************************************************************
* End of function i3c_broadcast_ccc_send
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: g_i3c0_callback
 *  Description  : This function is callback for i3c.
 *  Arguments    : p_args   Callback
 *  Return Value : None
 *********************************************************************************************************************/
void g_i3c0_callback(i3c_callback_args_t const *const p_args)
{
    /* Update the event in global array and this will be used in i3c_app_event_notify function */
    g_i3c_event_status = p_args->event_status;
    g_i3c_event_count[p_args->event]++;

    switch(p_args->event)
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
            b_bus_initialized = true;
            break;
        }

        case I3C_EVENT_READ_COMPLETE:
        {
           /* The number of bytes returns from the slave will be available in i3c_callback_args_t::transfer_size */
            g_data_transfer_size = p_args->transfer_size;
            set_next_read_buffer();
            break;
        }

        case I3C_EVENT_WRITE_COMPLETE:
        {
            /* The number of bytes writes to the slave will be available in i3c_callback_args_t::transfer_size */
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
                    b_target_ibi_transfer_received = true;
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
/**********************************************************************************************************************
* End of function g_i3c0_callback
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_app_event_notify
 *  Description  : This function starts the timer and wait for the event set in the i3c callback till specified timeout.
 *  Arguments    : set_event_flag_value  requested event flag
 *                 timeout               specified timeout
 *  Return Value : On successful operation, returns i3c event flag value.
 *********************************************************************************************************************/
uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout)
{
    fsp_err_t       err = FSP_SUCCESS;
    uint32_t        get_event_flag_value = RESET_VALUE;
    /* Reset the timeout flag */
    b_process_timeout = false;

    /* Start the timer */
    err = start_timeout_timer_with_defined_ms(timeout);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: start_timeout_timer_with_defined_ms function failed.\r\n");
        /* De-initialize the opened I3C and AGT timer module */
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Wait for the event set in the i3c callback till specified timeout */
    while (!b_process_timeout)
    {
        /* Process for all i3c events */
        for(uint8_t cnt = RESET_VALUE; cnt < (I3C_EVENT_INTERNAL_ERROR+ONE); cnt++)
        {
            /* Check for callback event */
            if (g_i3c_event_count[cnt] > RESET_VALUE)
            {
                /* Store the event in local variable */
                get_event_flag_value |= (uint32_t)(0x1 << cnt);
                g_i3c_event_count[cnt] -= ONE;
            }
        }

        /* Check for event received from i3c callback function is similar to event which user wants */
        get_event_flag_value = (set_event_flag_value & get_event_flag_value);
        if (get_event_flag_value)
        {
            g_i3c_event_status = RESET_VALUE;
            return get_event_flag_value;
        }
    }
    return 0;
}
/**********************************************************************************************************************
* End of function i3c_app_event_notify
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: set_next_read_buffer
 *  Description  : This function sets the next read buffer.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void set_next_read_buffer(void)
{
    p_last = p_next;
    p_next = ((p_next == g_read_data[RESET_VALUE]) ? g_read_data[ONE] : g_read_data[RESET_VALUE]);
}
/**********************************************************************************************************************
* End of function set_next_read_buffer
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: check_disp_i3c_slaveInfo
 *  Description  : This function checks the slave device is present on bus and display slave device information.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t  check_disp_i3c_slaveInfo(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for slave device existence on the bus */
    if ((RESET_VALUE == g_num_device_on_bus) && (RESET_VALUE == g_device_slave_info.dynamic_address))
    {
        APP_ERR_PRINT("\r\nERROR: No Slave device exists on I3C bus,\r\n"
                "Sending broadcast common command code to check for slave Hot Join requests\r\n"
                "Please re-check again\r\n");

        /* Sending broadcast CCC */
        err = i3c_broadcast_ccc_send();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nERROR: No Slave device exists on I3C bus, please confirm on "
                          "physical connection on bus.\r\n");
            return err;
        }
        APP_PRINT("\r\nINFO: Please re-check with menu option 1 to view dynamic address assigned "
                  "to slave before using menu option 2\r\n");
    }
    else
    {
        APP_PRINT ("\r\nINFO: number of I3C device on bus: %d **\r\n\n", g_num_device_on_bus);

        APP_PRINT("***********************************************\r\n");
        APP_PRINT("*                 I3C Slave Info              *\r\n");
        APP_PRINT("***********************************************\r\n");
        APP_PRINT("- Dynamic Address:   0x%02x\r\n", g_device_slave_info.dynamic_address);
        APP_PRINT("- BCR:               0x%02x\r\n", g_device_slave_info.slave_info.bcr);
        APP_PRINT("- DCR:               0x%02x\r\n", g_device_slave_info.slave_info.dcr);
        APP_PRINT("- PID:               0x%02x%02x%02x%02x%02x%02x\r\n",
                  g_device_slave_info.slave_info.pid[0],
                  g_device_slave_info.slave_info.pid[1],
                  g_device_slave_info.slave_info.pid[2],
                  g_device_slave_info.slave_info.pid[3],
                  g_device_slave_info.slave_info.pid[4],
                  g_device_slave_info.slave_info.pid[5]);
    }
    return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function check_disp_i3c_slaveInfo
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: master_write_read_verify
 *  Description  : This function performs the I3C master write and read operation.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t master_write_read_verify(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

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
       APP_ERR_PRINT("\r\nERROR: R_I3C_DeviceSelect API FAILED.\r\n");
       return err;
   }

   /* Perform write operation */
   err = R_I3C_Write(&g_i3c0_ctrl, g_write_data, sizeof(g_write_data), false);
   if (FSP_SUCCESS != err)
   {
       APP_ERR_PRINT("\r\nERROR: R_I3C_Write API FAILED.\r\n");
       return err;
   }

   /* Wait for write complete event */
   status = i3c_app_event_notify(I3C_EVENT_FLAG_WRITE_COMPLETE, WAIT_TIME);
   if (RESET_VALUE == status)
   {
       APP_ERR_PRINT("\r\nERROR: Requested event not received with in specified timeout.\r\n");
       return FSP_ERR_TIMEOUT;
   }

   /* Wait for slave device prepare buffer */
   R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MICROSECONDS);

   /* Start a read operation */
   err = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);
   if (FSP_SUCCESS != err)
   {
       APP_ERR_PRINT("\r\nERROR: R_I3C_Read API FAILED.\r\n");
       return err;
   }

   /* Wait for read complete event */
   status = i3c_app_event_notify(I3C_EVENT_FLAG_READ_COMPLETE, WAIT_TIME);
   if (RESET_VALUE == status)
   {
       APP_ERR_PRINT("\r\nERROR: Requested event not received with in specified timeout.\r\n");
       return FSP_ERR_TIMEOUT;
   }

   /* Compare read data with written data on slave */
   if (RESET_VALUE == memcmp(g_write_data, p_last, sizeof(g_write_data)))
   {
       APP_PRINT("\r\nINFO: Data written to I3C slave is read back and matching - SUCCESS\r\n");
       APP_PRINT("INFO: Data Transfer size 0x%x\r\n",g_data_transfer_size);
   }
   else
   {
       APP_ERR_PRINT("\r\nERROR: Data mismatch - FAILURE\r\n");
       return FSP_ERR_INVALID_DATA;
   }
   return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function master_write_read_verify
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_deinit
 *  Description  : This function closes opened I3C module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void i3c_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close I3C module */
    err = R_I3C_Close(&g_i3c0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* I3C Close failure message */
        APP_ERR_PRINT("\r\nERROR: R_I3C_Close API FAILED.\r\n");
    }
}
/**********************************************************************************************************************
* End of function i3c_deinit
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: g_timeout_timer_callback
 *  Description  : This function is callback for periodic mode timer and stops AGT0 timer in Periodic mode.
 *  Arguments    : p_args       Callback
 *  Return Value : None
 *********************************************************************************************************************/
void g_timeout_timer_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Check if specified timeout is zero */
    if (RESET_VALUE == --timeout_value_in_ms)
    {
        /* Set the timeout flag */
        b_process_timeout = true;
        /* Stop AGT timer */
        R_AGT_Stop(&g_timeout_timer_ctrl);
    }
}
/**********************************************************************************************************************
* End of function g_timeout_timer_callback
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: start_timeout_timer_with_defined_ms
 *  Description  : This function resets the counter value and start the AGT timer.
 *  Arguments    : timeout_ms      Specific timeout in ms
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update the specified timeout into a global variable and this will be checked in timer callback */
    timeout_value_in_ms = timeout_ms;
    /* Resets the counter value. */
    err = R_AGT_Reset(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_AGT_Reset API FAILED \r\n");
        return err;
    }

    /* Start the AGT timer */
    err = R_AGT_Start(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR: R_AGT_Start API FAILED \r\n");
    }
    return err;
}
/**********************************************************************************************************************
* End of function start_timeout_timer_with_defined_ms
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: agt_deinit
 *  Description  : This function closes opened AGT module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void agt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close AGT0 module */
    err = R_AGT_Close(&g_timeout_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT0 Close failure message */
        APP_ERR_PRINT("\r\nERROR: R_AGT_Close API FAILED.\r\n");
    }
}

/**********************************************************************************************************************
* End of function agt_deinit
**********************************************************************************************************************/
