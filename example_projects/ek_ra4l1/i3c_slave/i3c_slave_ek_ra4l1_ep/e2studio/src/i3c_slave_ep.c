/***********************************************************************************************************************
* File Name    : i3c_slave_ep.c
* Description  : Contains functions definitions used in hal_entry.c.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "i3c_slave_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup i3c_slave_ep
 * @{
 **********************************************************************************************************************/

/* Global variables.*/
volatile uint32_t g_i3c_event_count[I3C_EVENT_SDA_WRITE_COMPLETE + ONE];
volatile uint32_t g_i3c_event_status = RESET_VALUE;
static volatile bool b_process_timeout = false;
static volatile bool         b_onboard_sw_pressed = false;

/* For on board LEDs */
extern bsp_leds_t g_bsp_leds;

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

/* Timer related timeout functions */
static uint32_t timeout_value_in_ms = RESET_VALUE;

/* Private function declarations.*/
static fsp_err_t i3c_slave_init(void);
static fsp_err_t i3c_slave_ops(void);
static fsp_err_t i3c_device_daa_participation(void);
static void set_next_read_buffer(void);
static uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timout);
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms);
static fsp_err_t icu_init(void);
static bool read_onboard_sw_status(void);
static void i3c_deinit(void);
static void icu_deinit(void);
static void agt_deinit(void);

/**********************************************************************************************************************
 *  Function Name: i3c_slave_entry
 *  Description  : This function is used to start the i3c_slave example operation.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void i3c_slave_entry(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
        fsp_err_t err = FSP_SUCCESS;
        fsp_pack_version_t version = {RESET_VALUE};

        /* Version get API for FLEX pack information */
        R_FSP_VersionGet(&version);
        APP_PRINT(BANNER_INFO,EP_VERSION,version.version_id_b.major, version.version_id_b.minor,
                  version.version_id_b.patch );
        APP_PRINT(EP_INFO);

        /* Initialize AGT driver */
        err = R_AGT_Open(&g_timeout_timer_ctrl, &g_timeout_timer_cfg);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\nERROR : R_AGT_Open API FAILED \r\n");
            APP_ERR_TRAP(err);
        }

        /* Initialize ICU driver */
        err = icu_init();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\nERROR : icu_init function failed.\r\n");
            /* De-initialize the opened AGT timer module.*/
            agt_deinit();
            APP_ERR_TRAP(err);
        }

        /* Initialize I3C slave device.*/
        err = i3c_slave_init();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\nERROR : i3c_slave_init function failed.\r\n");
            /* De-initialize the opened AGT timer and ICU modules.*/
            agt_deinit();
            icu_deinit();
            APP_ERR_TRAP(err);
        }

        while(true)
        {
            /* Perform I3C slave operation.*/
            err = i3c_slave_ops();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\nERROR : init_i3c_slave function failed.\r\n");
                /* De-initialize the opened AGT timer, I3C and ICU modules.*/
                agt_deinit();
                icu_deinit();
                i3c_deinit();
                APP_ERR_TRAP(err);
            }
        }
}
/**********************************************************************************************************************
* End of function i3c_slave_entry
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_slave_init
 *  Description  : This function initializes I3C driver as an I3C slave device.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t i3c_slave_init(void)
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
        /* De-initialize the opened I3C module.*/
        i3c_deinit();
        return err;
    }

    APP_PRINT("\r\nINFO : I3C device is ready and waiting for DAA.\r\n");

    /* Waiting for the bus initialization */
    err = i3c_device_daa_participation();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : i3c_device_daa_participation function failed.\r\n");
        /* De-initialize the opened I3C module.*/
        i3c_deinit();
        return err;
    }

    APP_PRINT ("\r\nINFO : Address assignment is completed, dynamic address: 0x%02x\r\n", g_slave_dynamic_address);
    if (g_bsp_leds.led_count == 3)
    {
        /* Toggle Led1.*/
        R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
    }
    else
    {
        /* Toggle both Led1 & Led2. */
        R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
        R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
        R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
    }

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
* End of function i3c_slave_init
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_slave_ops
 *  Description  : This function processes master request or send IBI to master.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t i3c_slave_ops(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint32_t  event_flag = RESET_VALUE;

    /* Wait for I3C events.*/
    event_flag = i3c_app_event_notify((I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE |
                          I3C_EVENT_FLAG_COMMAND_COMPLETE |
                          I3C_EVENT_FLAG_WRITE_COMPLETE |
                          I3C_EVENT_FLAG_READ_COMPLETE |
                          I3C_EVENT_FLAG_READ_BUFFER_FULL |
                          I3C_EVENT_FLAG_IBI_WRITE_COMPLETE |
                          I3C_EVENT_FLAG_INTERNAL_ERROR), WAIT_TIME);

    /* Check if flag is command complete and broadcast flag from master.*/
    if (I3C_EVENT_FLAG_COMMAND_COMPLETE & event_flag)
    {
        APP_PRINT ("\r\nINFO : received CCC: 0x%x\r\n", g_slave_received_ccc_code);
        if (g_data_transfer_size)
        {
            APP_PRINT ("INFO : received CCC payload size: 0x%x\r\n", g_data_transfer_size);
        }

        if (I3C_CCC_BROADCAST_RSTDAA == g_slave_received_ccc_code)
        {
            /* The current device dynamic address is reset by master */
            g_slave_dynamic_address = RESET_VALUE;
            APP_PRINT ("\r\nINFO : slave dynamic address is reset.\r\n");
            /* Process i3c device DAA block.*/
            status = i3c_device_daa_participation();
            if (FSP_SUCCESS != status)
            {
                APP_ERR_PRINT ("\r\nERROR : i3c_device_daa_participation function failed.\r\n");
                return FSP_ERR_INTERNAL;
            }
            APP_PRINT ("\r\nINFO : slave dynamic address is assigned.\r\n");
            if (g_bsp_leds.led_count == 3)
            {
                /* Toggle Led1.*/
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
            }
            else
            {
                /* Toggle both Led1 & Led2. */
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
            }
        }

#ifdef I3C_HDR_DDR_SUPPORT

        if (I3C_HDR_COMMAND_CODE_WRITE == g_slave_received_ccc_code)
        {
            set_next_read_buffer();
            /* Prepare write buffer */
            memcpy (&g_write_data[RESET_VALUE], p_last, g_data_transfer_size);
            status = R_I3C_Write (&g_i3c0_ctrl, g_write_data, g_data_transfer_size, false);
	        if (FSP_SUCCESS != status)
	        {
	            APP_ERR_PRINT("\r\nERROR : R_I3C_Write API failed.\r\n");
	            return status;
	        }

            APP_PRINT ("\r\nINFO : [HDR-DDR] Write complete, transfer size: 0x%x\r\n", g_data_transfer_size);
            if (g_bsp_leds.led_count == 3)
            {
                /* Toggle Led2.*/
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
            }
            else
            {
                /* Toggle Led1. */
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
            }
        }
        if (I3C_HDR_COMMAND_CODE_READ == g_slave_received_ccc_code)
        {
            status = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);
           	if (FSP_SUCCESS != status)
	        {
	            APP_ERR_PRINT("\r\nERROR : R_I3C_Read API failed.\r\n");
	            return status;
	        }

            APP_PRINT ("\r\nINFO : [HDR-DDR] Read complete, transfer size: 0x%x\r\n", g_data_transfer_size);
            if (g_bsp_leds.led_count == 3)
            {
                /* Toggle Led3.*/
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[2], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[2], (!led_status));
            }
            else
            {
                /* Toggle Led2. */
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
            }
        }
#endif /* I3C_HDR_DDR_SUPPORT */

    }

    /* Check if event is write complete.*/
    if (event_flag & I3C_EVENT_FLAG_WRITE_COMPLETE)
    {
        /* Note that the application may also call i3c_api_t::read or i3c_api_t::write from this event
         * In order to set the transfer buffers for the next transfer. */
        /* When a read buffer is full or a write operation (Master Read / Slave Write) is complete,
         * prepare a buffer for reading. */
        status = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);
        if (FSP_SUCCESS != status)
        {
            APP_ERR_PRINT ("\r\nERROR : R_I3C_Read API failed.\r\n");
            return status;
        }

        APP_PRINT ("\r\nINFO : Write complete, transfer size: 0x%x\r\n", g_data_transfer_size);
        if (g_bsp_leds.led_count == 3)
        {
            /* Toggle Led2.*/
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
        }
        else
        {
            /* Toggle Led1. */
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
        }
    }

    /* Check if event is read complete.*/
    if (event_flag & I3C_EVENT_FLAG_READ_COMPLETE)
    {
        /* Set the next read buffer.*/
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
        if (g_bsp_leds.led_count == 3)
        {
            /* Toggle Led3. */
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[2], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[2], (!led_status));
        }
        else
        {
            /* Toggle Led2. */
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
        }
    }

    /* Check if event is IBI write complete.*/
    if (event_flag & I3C_EVENT_FLAG_IBI_WRITE_COMPLETE)
    {
        APP_PRINT ("\r\nINFO : IBI Write complete, transfer size: 0x%x\r\n", g_data_transfer_size);
        if (g_bsp_leds.led_count == 3)
        {
            /* Toggle Led2.*/
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
        }
        else
        {
            /* Toggle Led1. */
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
        }
    }

    /* Check if event is read buffer full.*/
    if (event_flag & I3C_EVENT_FLAG_READ_BUFFER_FULL)
    {
        /* If there is no user provided read buffer, or if the user provided read buffer has been filled,
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

    /* Check if event is internal error.*/
    if (event_flag & I3C_EVENT_FLAG_INTERNAL_ERROR)
    {
        APP_ERR_PRINT ("\r\nERROR : I3C Internal Error occurred.\r\n");
        APP_PRINT ("\r\nINFO : transfer status : 0x%x\r\n", g_i3c_event_status);
        return FSP_ERR_INTERNAL;
    }

    /* Check if on-board switch is pressed.*/
    if (true == read_onboard_sw_status())
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
/**********************************************************************************************************************
* End of function i3c_slave_ops
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_deinit
 *  Description  : This function closes opened I3C module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
static void i3c_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close I3C module */
    err = R_I3C_Close(&g_i3c0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* I3C Close failure message */
        APP_ERR_PRINT("\r\nERROR : R_I3C_Close API FAILED.\r\n");
    }
}
/**********************************************************************************************************************
* End of function i3c_deinit
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: g_i3c0_callback
 *  Description  : This function is callback for i3c.
 *  Arguments    : p_args   Callback
 *  Return Value : None
 *********************************************************************************************************************/
void g_i3c0_callback(i3c_callback_args_t const *const p_args)
{
    /* Update the event in global array and this will be used in i3c_app_event_notify function.*/
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
            if (p_args->transfer_size)
            {
                g_data_transfer_size = p_args->transfer_size;
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
 *  Function Name: i3c_device_daa_participation
 *  Description  : This function waits for bus initialization and sends IBI hot join request to master.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t i3c_device_daa_participation(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint32_t  event_flag = RESET_VALUE;

    /* Waiting for the bus initialization */
    while(RESET_VALUE == g_slave_dynamic_address)
    {
        event_flag = i3c_app_event_notify(I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE, WAIT_TIME);
        if ((--g_wait_count == RESET_VALUE)&&(!(event_flag & I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE)))
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

/**********************************************************************************************************************
* End of function i3c_device_daa_participation
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_app_event_notify
 *  Description  : This function starts the timer and waits for the event set in the i3c callback
 *                 till specified timeout.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout)
{
    fsp_err_t       err = FSP_SUCCESS;
    uint32_t        get_event_flag_value = RESET_VALUE;
    /* Reset the timeout flag. */
    b_process_timeout = false;

    /* Start the timer.*/
    err = start_timeout_timer_with_defined_ms(timeout);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR : start_timeout_timer_with_defined_ms function failed.\r\n");
        /* De-initialize the opened I3C and AGT timer module.*/
        i3c_deinit();
        agt_deinit();
        APP_ERR_TRAP(err);
    }

    /* Wait for the event set in the i3c callback till specified timeout.*/
    while (!b_process_timeout)
    {
        /* Process for all i3c events.*/
        for(uint8_t cnt = RESET_VALUE; cnt < (I3C_EVENT_INTERNAL_ERROR+ONE); cnt++)
        {
            /* Check for callback event.*/
            if (g_i3c_event_count[cnt] > RESET_VALUE)
            {
                /* Store the event in local variable.*/
                get_event_flag_value |= (uint32_t)(0x1 << cnt);
                g_i3c_event_count[cnt] -= ONE;
            }
        }

        /* Check for event received from i3c callback function is similar to event which user wants.*/
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
static void set_next_read_buffer(void)
{
    p_last = p_next;
    p_next = ((p_next == g_read_data[RESET_VALUE]) ? g_read_data[ONE] : g_read_data[RESET_VALUE]);
}

/**********************************************************************************************************************
* End of function set_next_read_buffer
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: icu_init
 *  Description  : This function initializes and enables ICU module.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_sw2_ctrl, &g_external_irq_sw2_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_ICU_ExternalIrqOpen API FAILED.\r\n");
        return err;
    }

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_sw2_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_ICU_ExternalIrqEnable API FAILED.\r\n");
        /* De-initialize the opened ICU module.*/
        icu_deinit();
    }
    return err;
}

/**********************************************************************************************************************
* End of function icu_init
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: icu_deinit
 *  Description  : This function closes opened ICU module before the project ends up in an Error Trap.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
static void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_sw2_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        APP_ERR_PRINT("\r\nERROR : R_ICU_ExternalIrqClose API FAILED.\r\n");
    }
}

/**********************************************************************************************************************
* End of function icu_deinit
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: external_irq_callback
 *  Description  : User defined external irq callback.
 *  Arguments    : p_args          Callback
 *  Return Value : None
 *********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t *p_args)
{
    /* Check for the right interrupt*/
    if (USER_SW_IRQ_NUMBER == p_args->channel)
    {
        b_onboard_sw_pressed = true;
        R_IOPORT_PinRead(g_ioport.p_ctrl, USER_SW_PIN, &g_last_switch_status);
    }
}
/**********************************************************************************************************************
* End of function external_irq_callback
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: read_onboard_sw_status
 *  Description  : This function reads the on board switch status.
 *  Arguments    : None
 *  Return Value : True if switch is pressed else return false.
 *********************************************************************************************************************/
static bool read_onboard_sw_status(void)
{
    if (true == b_onboard_sw_pressed)
    {
        /* Reset the flag.*/
        b_onboard_sw_pressed = false;
        /* Read the current status of switch.*/
        R_IOPORT_PinRead(g_ioport.p_ctrl, USER_SW_PIN, &g_cur_switch_status);
        if ((g_cur_switch_status == BSP_IO_LEVEL_LOW) && (g_last_switch_status == BSP_IO_LEVEL_LOW))
        {
            return true;
        }
    }
    return false;
}
/**********************************************************************************************************************
* End of function read_onboard_sw_status
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: g_timeout_timer_callback
 *  Description  : This function is callback for periodic mode timer and stops AGT0 timer in Periodic mode.
 *  Arguments    : p_args          Callback
 *  Return Value : None
 *********************************************************************************************************************/
void g_timeout_timer_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Check if specified timeout is zero.*/
    if (RESET_VALUE == --timeout_value_in_ms)
    {
        /* Set the timeout flag.*/
        b_process_timeout = true;
        /* Stop AGT timer.*/
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

    /* Update the specified timeout into a global variable and this will be checked in timer callback.*/
    timeout_value_in_ms = timeout_ms;
    /* Resets the counter value.*/
    err = R_AGT_Reset(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_AGT_Reset API FAILED \r\n");
        return err;
    }

    /* Start the AGT timer.*/
    err = R_AGT_Start(&g_timeout_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nERROR : R_AGT_Start API FAILED \r\n");
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
static void agt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close AGT0 module */
    err = R_AGT_Close(&g_timeout_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT0 Close failure message */
        APP_ERR_PRINT("\r\nERROR : R_AGT_Close API FAILED.\r\n");
    }
}
/**********************************************************************************************************************
* End of function agt_deinit
**********************************************************************************************************************/
