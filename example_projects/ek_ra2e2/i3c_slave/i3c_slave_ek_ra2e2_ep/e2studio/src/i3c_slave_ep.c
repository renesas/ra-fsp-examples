/***********************************************************************************************************************
 * File Name    : i3c_slave_ep.c
 * Description  : Contains functions definitions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "i3c_slave_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup i3c_slave_ep
 * @{
 **********************************************************************************************************************/

/* Global variables.*/
volatile uint32_t g_i3c_event_count[I3C_EVENT_INTERNAL_ERROR + ONE];
volatile uint32_t g_i3c_event_status = RESET_VALUE;
static volatile bool b_process_timeout = false;
static volatile bool         b_onboard_sw_pressed = false;

/* Configure the information for the slave device. */
static i3c_device_cfg_t g_slave_device_cfg =
{
    /* This is the Static I3C / I2C Legacy address defined by the device manufacturer. */
    .static_address  = I3C_SLAVE_DEVICE_STATIC_ADDRESS,
    /* The dynamic address will be automatically updated when the master configures this device using CCC ENTDAA. */
    .dynamic_address = RESET_VALUE
};

static i3c_device_status_t g_slave_device_status =
{
     .pending_interrupt = RESET_VALUE,
     .vendor_status = RESET_VALUE
};

static uint8_t            g_ibi_write_data[MAX_IBI_PAYLOAD_SIZE] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t            g_write_data[MAX_WRITE_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t            g_read_data[2][MAX_READ_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t          * p_next = NULL;
static uint8_t          * p_last = NULL;

static uint8_t            g_slave_received_ccc_code = RESET_VALUE;
static uint8_t            g_slave_dynamic_address = RESET_VALUE;
static uint32_t           g_data_transfer_size = RESET_VALUE;
static uint32_t           g_wait_count = MAX_WAIT_TIME_BUS_INIT_10S;
static uint32_t           g_ibi_write_count = RESET_VALUE;
static bsp_io_level_t     g_last_switch_status;
static bsp_io_level_t     g_cur_switch_status;
static bsp_io_level_t     led_status;

/* private function declarations.*/
static fsp_err_t i3c_device_daa_participation(void);
static void set_next_read_buffer(void);
static uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timout);
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms);

/*******************************************************************************************************************//**
 * @brief     Initializes I3C driver as an I3C slave device.
 * @param[IN] None
 * @retval    FSP_SUCCESS       I3C driver is opened and configured successfully.
 * @retval    err               Any Other Error code apart from FSP_SUCCESS like Unsuccessful Open.
 **********************************************************************************************************************/
fsp_err_t i3c_slave_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t  pid_lower_32bits = RESET_VALUE;
    uint16_t  pid_upper_16bits = RESET_VALUE;

    memset(g_ibi_write_data, RESET_VALUE, sizeof(g_ibi_write_data));
    memset(g_write_data, RESET_VALUE, sizeof(g_write_data));

    /* Initializes the I3C module. */
    err = R_I3C_Open(&g_i3c0_ctrl, &g_i3c0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_I3C_Open API FAILED \r\n");
        return err;
    }
    APP_PRINT("\r\nINFO : I3C Initialized successfully in slave mode.\r\n");


    /* Set the device configuration for this device. */
    g_slave_device_cfg.slave_info.bcr = BCR_SETTING;
    g_slave_device_cfg.slave_info.dcr = DCR_SETTING;
    pid_lower_32bits = (uint32_t)(((uint32_t)(PID_VENDOR_PART_ID << PID_PART_ID_POS) & PID_PART_ID_MSK) |
            ((uint32_t)(PID_VENDOR_INSTANCE_ID << PID_INSTANCE_ID_POS) & PID_INSTANCE_ID_MSK) |
            ((uint32_t)(PID_VENDOR_DEF << PID_VENDOR_DEF_POS) & PID_VENDOR_DEF_MSK));
    pid_upper_16bits = (uint16_t) (((uint16_t)(PID_TYPE_SELECTION) & PID_TYPE_MSK) |
            ((uint16_t)(PID_MANUFACTURER) & PID_MANUFACTURER_MSK));

    g_slave_device_cfg.slave_info.pid[0] = (uint8_t)(pid_upper_16bits >> 8);
    g_slave_device_cfg.slave_info.pid[1] = (uint8_t)(pid_upper_16bits >> 0);
    g_slave_device_cfg.slave_info.pid[2] = (uint8_t)(pid_lower_32bits >> 24);
    g_slave_device_cfg.slave_info.pid[3] = (uint8_t)(pid_lower_32bits >> 16);
    g_slave_device_cfg.slave_info.pid[4] = (uint8_t)(pid_lower_32bits >> 8);
    g_slave_device_cfg.slave_info.pid[5] = (uint8_t)(pid_lower_32bits >> 0);

    APP_PRINT("I3C device Information:\r\n");
    APP_PRINT(" - Static Address:0x%02x\r\n", g_slave_device_cfg.static_address);
    APP_PRINT(" - BCR:           0x%02x\r\n", g_slave_device_cfg.slave_info.bcr);
    APP_PRINT(" - DCR:           0x%02x\r\n", g_slave_device_cfg.slave_info.dcr);
    APP_PRINT(" - PID:           0x%02x%02x%02x%02x%02x%02x\r\n",
            g_slave_device_cfg.slave_info.pid[0],
            g_slave_device_cfg.slave_info.pid[1],
            g_slave_device_cfg.slave_info.pid[2],
            g_slave_device_cfg.slave_info.pid[3],
            g_slave_device_cfg.slave_info.pid[4],
            g_slave_device_cfg.slave_info.pid[5]);

    /* Set the device configuration for this device. */
    err = R_I3C_DeviceCfgSet(&g_i3c0_ctrl, &g_slave_device_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_I3C_DeviceCfgSet API FAILED \r\n");
        /* de-initialize the opened I3C module.*/
        i3c_deinit();
        return err;
    }

    /* Enable Slave Mode. */
    err = R_I3C_Enable(&g_i3c0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_I3C_Enable API FAILED \r\n");
        /* de-initialize the opened I3C module.*/
        i3c_deinit();
        return err;
    }

    /* Set the status returned to the master in response to a GETSTATUS command */
    err = R_I3C_SlaveStatusSet(&g_i3c0_ctrl, g_slave_device_status);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_I3C_SlaveStatusSet API FAILED \r\n");
        /* de-initialize the opened I3C module.*/
        i3c_deinit();
        return err;
    }

    APP_PRINT("\r\nINFO : I3C device is ready and waiting for DAA.\r\n");

    /* waiting for the bus initialization */
    err = i3c_device_daa_participation();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : i3c_device_daa_participation function failed.\r\n");
        /* de-initialize the opened I3C module.*/
        i3c_deinit();
        return err;
    }

    APP_PRINT ("\r\nINFO : Address assignment is completed, dynamic address: 0x%02x\r\n", g_slave_dynamic_address);
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This functions processes master request or send IBI to master.
 * @param[IN] None
 * @retval    FSP_SUCCESS       Master request processed successfully.
 * @retval    err               Any Other Error code apart from FSP_SUCCESS like Unsuccessful IBI write.
 **********************************************************************************************************************/
fsp_err_t i3c_slave_ops(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint32_t  event_flag = RESET_VALUE;

    /* wait for I3C events.*/
    event_flag = i3c_app_event_notify((I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE |
                          I3C_EVENT_FLAG_COMMAND_COMPLETE |
                          I3C_EVENT_FLAG_WRITE_COMPLETE |
                          I3C_EVENT_FLAG_READ_COMPLETE |
                          I3C_EVENT_FLAG_READ_BUFFER_FULL |
                          I3C_EVENT_FLAG_IBI_WRITE_COMPLETE |
                          I3C_EVENT_FLAG_INTERNAL_ERROR), WAIT_TIME);

    /* check if flag is command complete and broadcast flag from master.*/
    if(I3C_EVENT_FLAG_COMMAND_COMPLETE & event_flag)
    {
        APP_PRINT ("\r\nINFO : received CCC: 0x%x\r\n", g_slave_received_ccc_code);
        if(g_data_transfer_size)
        {
            APP_PRINT ("INFO : received CCC payload size: 0x%x\r\n", g_data_transfer_size);
        }

        if(I3C_CCC_BROADCAST_RSTDAA == g_slave_received_ccc_code)
        {
            /* the current device dynamic address is reset by master */
            g_slave_dynamic_address = RESET_VALUE;
            APP_PRINT ("\r\nINFO : slave dynamic address is reset.\r\n");
            /* process i3c device DAA block.*/
            status = i3c_device_daa_participation();
            if (FSP_SUCCESS != status)
            {
                APP_ERR_PRINT ("\r\nERROR : i3c_device_daa_participation function failed.\r\n");
                return FSP_ERR_INTERNAL;
            }
            APP_PRINT ("\r\nINFO : slave dynamic address is assigned.\r\n");
            /* toggle the blue led.*/
            R_IOPORT_PinRead(g_ioport.p_ctrl, LED1_BLUE, &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, LED1_BLUE, (!led_status));
        }
    }

    /* check if event is write complete.*/
    if(event_flag & I3C_EVENT_FLAG_WRITE_COMPLETE)
    {
        /* Note that the application may also call i3c_api_t::read or i3c_api_t::write from this event
         * In order to set the transfer buffers for the next transfer. */
        /* When a read buffer is full or a write operation (Master Read / Slave Write) is complete, prepare a buffer for reading. */
        status = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);
        if (FSP_SUCCESS != status)
        {
            APP_ERR_PRINT ("\r\nERROR : R_I3C_Read API failed.\r\n");
            return status;
        }

        APP_PRINT ("\r\nINFO : Write complete, transfer size: 0x%x\r\n", g_data_transfer_size);
        /* toggle the green led.*/
        R_IOPORT_PinRead(g_ioport.p_ctrl, LED2_GREEN, &led_status);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, LED2_GREEN, (!led_status));
    }

    /* check if event is read complete.*/
    if(event_flag & I3C_EVENT_FLAG_READ_COMPLETE)
    {
        /* set the next read buffer.*/
        set_next_read_buffer();
       /* Note that the application may also call i3c_api_t::read or i3c_api_t::write from this event
        * In order to set the transfer buffers for the next transfer. */
       /* When a read operation (Master Write / Slave Read) is complete, prepare a buffer for writing. */
        memcpy(&g_write_data[RESET_VALUE], p_last, g_data_transfer_size);
        status = R_I3C_Write(&g_i3c0_ctrl, g_write_data, g_data_transfer_size, false);
        if (FSP_SUCCESS != status)
        {
            APP_ERR_PRINT ("\r\nERROR : R_I3C_Write API failed.\r\n");
            return status;
        }

        APP_PRINT ("\r\nINFO : Read complete, transfer size: 0x%x\r\n", g_data_transfer_size);
        /* toggle the red led.*/
        R_IOPORT_PinRead(g_ioport.p_ctrl, LED3_RED_ARDUINO_D2_PMOD2_RESET, &led_status);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, LED3_RED_ARDUINO_D2_PMOD2_RESET, (!led_status));
    }

    /* check if event is IBI write complete.*/
    if(event_flag & I3C_EVENT_FLAG_IBI_WRITE_COMPLETE)
    {
        APP_PRINT ("\r\nINFO : IBI Write complete, transfer size: 0x%x\r\n", g_data_transfer_size);
        /* toggle the green led.*/
        R_IOPORT_PinRead(g_ioport.p_ctrl, LED2_GREEN, &led_status);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, LED2_GREEN, (!led_status));
    }

    /* check if event is read buffer full.*/
    if(event_flag & I3C_EVENT_FLAG_READ_BUFFER_FULL)
    {
        /*If there is no user provided read buffer, or if the user provided read buffer has been filled,
         * the driver will notify the application that the buffer is full. The application may provide
         * a new read buffer by calling i3c_api_t::read. If no read buffer is provided, then any remaining bytes
         * in the transfer will be dropped.*/
        status = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);
        if (FSP_SUCCESS != status)
        {
            APP_ERR_PRINT ("\r\nERROR : R_I3C_Read API failed.\r\n");
            return status;
        }
    }

    /* check if event is internal error.*/
    if(event_flag & I3C_EVENT_FLAG_INTERNAL_ERROR)
    {
        APP_ERR_PRINT ("\r\nERROR : I3C Internal Error occurred.\r\n");
        APP_PRINT ("\r\nINFO : transfer status : 0x%x\r\n", g_i3c_event_status);
        return FSP_ERR_INTERNAL;
    }

    /* check if on-board switch is pressed.*/
    if(true == read_onboard_sw_status())
    {
        /* Notify that user pushbutton is pressed */
        APP_PRINT("\r\nINFO : User Pushbutton Pressed.\r\n");
        APP_PRINT("INFO : Initiate an IBI transfer request.\r\n");

        /* Set the value of the data to write. */
        for (uint32_t cnt = RESET_VALUE; cnt < sizeof(g_ibi_write_data); cnt++)
        {
            g_ibi_write_data[cnt] = (uint8_t) (cnt + g_ibi_write_count) & UINT8_MAX;
        }
        g_ibi_write_count++;

        /* Initiate an IBI write operation.*/
        status = R_I3C_IbiWrite(&g_i3c0_ctrl, I3C_IBI_TYPE_INTERRUPT, g_ibi_write_data, sizeof(g_ibi_write_data));
        if (FSP_SUCCESS != status)
        {
            APP_ERR_PRINT ("\r\nERROR : R_I3C_IbiWrite API FAILED.\r\n");
            return status;
        }
        APP_PRINT("\r\nINFO : IBI transfer request initiated successfully.\r\n");
    }
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief This function closes opened I3C module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void i3c_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close I3C module */
    err = R_I3C_Close(&g_i3c0_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* I3C Close failure message */
        APP_ERR_PRINT("\r\nERROR : R_I3C_Close API FAILED.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief This function is callback for i3c.
 *
 * @param[in] p_args
 **********************************************************************************************************************/
void g_i3c0_callback(i3c_callback_args_t const *const p_args)
{
    /* update the event in global array and this will be used in i3c_app_event_notify function.*/
    g_i3c_event_status = p_args->event_status;
    g_i3c_event_count[p_args->event]++;

    switch(p_args->event)
    {
        case I3C_EVENT_ADDRESS_ASSIGNMENT_COMPLETE:
        {
            g_slave_dynamic_address = p_args->dynamic_address;
            break;
        }

        case I3C_EVENT_READ_COMPLETE:
        {
           /* The number of bytes read by the slave will be available in i3c_callback_args_t::transfer_size. */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_WRITE_COMPLETE:
        {
           /* The number of bytes written by the slave will be available in i3c_callback_args_t::transfer_size. */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_IBI_WRITE_COMPLETE:
        {
            /* Notify the application that the IBI write is complete. */
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
            g_slave_received_ccc_code = p_args->command_code;
            if(p_args->transfer_size)
            {
                g_data_transfer_size = p_args->transfer_size;
                set_next_read_buffer();
            }
            break;
        }

        default:
        {
           break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This functions waits for bus initialization and sends IBI hot join request to master.
 * @param[IN] None
 * @retval    FSP_SUCCESS       hot join request processed successfully.
 * @retval    err               Any Other Error code apart from FSP_SUCCESS like Unsuccessful IBI write.
 **********************************************************************************************************************/
static fsp_err_t i3c_device_daa_participation(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint32_t  event_flag = RESET_VALUE;

    /* waiting for the bus initialization */
    while(RESET_VALUE == g_slave_dynamic_address)
    {
        event_flag = i3c_app_event_notify(I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE, WAIT_TIME);
        if((--g_wait_count == RESET_VALUE)&&(!(event_flag & I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE)))
        {
            APP_PRINT ("\r\nINFO : Request Hot-Join IBI\r\n");
            /* Initiate an IBI write operation.*/
            status = R_I3C_IbiWrite(&g_i3c0_ctrl, I3C_IBI_TYPE_HOT_JOIN, NULL, RESET_VALUE);
            if (FSP_SUCCESS != status)
            {
                APP_ERR_PRINT ("\r\nERROR : Request I3C_IBI_TYPE_HOT_JOIN FAILED \r\n");
                return status;
            }
            g_wait_count = MAX_WAIT_TIME_BUS_INIT_10S;
        }
    }
    /* Set the buffer for storing data received during a read transfer. */
    p_next = g_read_data[RESET_VALUE];

    /* Read the data from I3C bus.*/
    status = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);
    if (FSP_SUCCESS != status)
    {
        APP_ERR_PRINT ("\r\nERROR : R_I3C_Read API FAILED \r\n");
        return status;
    }
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief This function starts the timer and wait for the event set in the i3c callback till specified timeout.
 * @param[IN]   set_event_flag_value  requested event flag
 * @param[IN]   timeout               specified timeout
 * @retval      on successful operation, returns i3c event flag value.
 **********************************************************************************************************************/
static uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout)
{
    fsp_err_t       err = FSP_SUCCESS;
    uint32_t        get_event_flag_value = RESET_VALUE;
    /* Reset the timeout flag. */
    b_process_timeout = false;

    /* start the timer.*/
    err = start_timeout_timer_with_defined_ms(timeout);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR : start_timeout_timer_with_defined_ms function failed.\r\n");
        /* de-initialize the opened I3C and AGT timer module.*/
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* wait for the event set in the i3c callback till specified timeout.*/
    while (!b_process_timeout)
    {
        /* process for all i3c events.*/
        for(uint8_t cnt = RESET_VALUE; cnt < (I3C_EVENT_INTERNAL_ERROR+ONE); cnt++)
        {
            /* check for callback event.*/
            if(g_i3c_event_count[cnt] > RESET_VALUE)
            {
                /* store the event in local variable.*/
                get_event_flag_value |= (uint32_t)(0x1 << cnt);
                g_i3c_event_count[cnt] -= ONE;
            }
        }

        /* check for event received from i3c callback function is similar to event which user wants.*/
        get_event_flag_value = (set_event_flag_value & get_event_flag_value);
        if(get_event_flag_value)
        {
            g_i3c_event_status = RESET_VALUE;
            return get_event_flag_value;
        }
    }
    return 0;
}

/*******************************************************************************************************************//**
 * @brief This function sets the next read buffer.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void set_next_read_buffer(void)
{
    p_last = p_next;
    p_next = ((p_next == g_read_data[RESET_VALUE]) ? g_read_data[ONE] : g_read_data[RESET_VALUE]);
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes and enables ICU module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_sw1_ctrl, &g_external_irq_sw1_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_ICU_ExternalIrqOpen API FAILED.\r\n");
        return err;
    }

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_sw1_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_ICU_ExternalIrqEnable API FAILED.\r\n");
        /* de-initialize the opened ICU module.*/
        icu_deinit();
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened ICU module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_sw1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        APP_ERR_PRINT("\r\nERROR : R_ICU_ExternalIrqClose API FAILED.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief      User defined external irq callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t *p_args)
{
    /* check for the right interrupt*/
    if(USER_SW_IRQ_NUMBER == p_args->channel)
    {
        b_onboard_sw_pressed = true;
        R_IOPORT_PinRead(g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, &g_last_switch_status);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function reads the on board switch status.
 * @param[IN]   None
 * @retval      True if switch is pressed else return false.
 **********************************************************************************************************************/
bool read_onboard_sw_status(void)
{
    if(true == b_onboard_sw_pressed)
    {
        /* Reset the flag.*/
        b_onboard_sw_pressed = false;
        /* Read the current status of switch.*/
        R_IOPORT_PinRead(g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, &g_cur_switch_status);
        if((g_cur_switch_status == BSP_IO_LEVEL_LOW) && (g_last_switch_status == BSP_IO_LEVEL_LOW))
        {
            return true;
        }
    }
    return false;
}

/* timer related functions */
static uint32_t timeout_value_in_ms = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief This function is callback for periodic mode timer and stops AGT0 timer in Periodic mode.
 *
 * @param[in] (timer_callback_args_t *) p_args
 **********************************************************************************************************************/
void g_timeout_timer_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* check if specified timeout is zero.*/
    if(RESET_VALUE == --timeout_value_in_ms)
    {
        /* set the timeout flag.*/
        b_process_timeout = true;
        /* stop AGT timer.*/
        R_AGT_Stop(&g_timeout_timer_ctrl);
    }
}

/*******************************************************************************************************************//**************
 * @brief       This function Resets the counter value and start the AGT timer.
 * @param[IN]   timeout_ms
 * @retval      FSP_SUCCESS or Any Other Error code apart from FSP_SUCCESS upon unsuccessful start_timeout_timer_with_defined_ms.
 ***********************************************************************************************************************************/
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms)
{
    fsp_err_t err = FSP_SUCCESS;

    /* update the specified timeout into a global variable and this will be checked in timer callback.*/
    timeout_value_in_ms = timeout_ms;
    /* Resets the counter value.*/
    err = R_AGT_Reset(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_AGT_Reset API FAILED \r\n");
        return err;
    }

    /* start the AGT timer.*/
    err = R_AGT_Start(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_AGT_Start API FAILED \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function closes opened AGT module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void agt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close AGT0 module */
    err = R_AGT_Close(&g_timeout_timer_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT0 Close failure message */
        APP_ERR_PRINT("\r\nERROR : R_AGT_Close API FAILED.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup i3c_slave_ep)
 **********************************************************************************************************************/
