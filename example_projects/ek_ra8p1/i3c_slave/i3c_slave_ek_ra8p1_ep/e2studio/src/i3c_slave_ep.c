/***********************************************************************************************************************
 * File Name    : i3c_slave_ep.c
 * Description  : Contains data structures and functions used in i3c_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "i3c_slave_ep.h"

/* Global variables */
volatile uint32_t g_i3c_event_status = RESET_VALUE;
volatile uint8_t g_slave_dynamic_address = RESET_VALUE;
volatile uint8_t g_slave_received_ccc_code = RESET_VALUE;
volatile uint32_t g_data_transfer_size = RESET_VALUE;
volatile bool g_b_ibi_write_sent = false;

/* Configure the information for the Slave device */
i3c_device_cfg_t g_slave_device_cfg =
{
    /* This is the Static I3C/I2C Legacy address defined by the device manufacturer */
    .static_address = I3C_SLAVE_DEVICE_STATIC_ADDRESS,

    /* The Dynamic Address will be automatically updated when the Master configures this device using CCC ENTDAA */
    .dynamic_address = RESET_VALUE
};

/* Transfer buffers — shared across multiple functions within this file */
static uint8_t s_ibi_write_data[MAX_IBI_PAYLOAD_SIZE] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t s_write_data[MAX_WRITE_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t s_read_data[2][MAX_READ_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
static uint8_t *p_next = NULL;
static uint8_t *p_last = NULL;

/* Switch status shared between ISR and read_onboard_sw_status */
static bsp_io_level_t s_last_switch_status;

/* Timer related timeout functions */
static uint32_t timeout_value_in_ms = RESET_VALUE;

/* Event count */
static volatile uint32_t s_i3c_event_count[I3C_EVENT_SDA_WRITE_COMPLETE + 1];

/* Callback flags — set in ISR context */
static volatile bool b_process_timeout = false;
static volatile bool b_onboard_sw_pressed = false;

/* Private function declarations */
static void set_next_read_buffer(void);
static fsp_err_t i3c_slave_read(void);
static fsp_err_t i3c_slave_write(void);
static fsp_err_t i3c_slave_ibi_write(i3c_ibi_type_t ibi_type, uint8_t *p_data, uint32_t length);
static uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout);
static fsp_err_t start_timeout_timer_with_defined_ms(uint32_t timeout_ms);
static bool read_onboard_sw_status(void);
static void led_indicate(uint8_t led_case);

/***********************************************************************************************************************
 * Function Name: i3c_slave_init.
 * Description  : This function initializes I3C module as an I3C Slave device, then waits for DAA.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t i3c_slave_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t pid_lower_32bits = RESET_VALUE;
    uint16_t pid_upper_16bits = RESET_VALUE;
    i3c_device_status_t slave_device_status = {.pending_interrupt = RESET_VALUE, .vendor_status = RESET_VALUE};

    memset(s_ibi_write_data, RESET_VALUE, sizeof(s_ibi_write_data));
    memset(s_write_data, RESET_VALUE, sizeof(s_write_data));

    /* Initialize the I3C module */
    err = R_I3C_Open(&g_i3c0_ctrl, &g_i3c0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the device configuration for this device */
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

    /* Set the device configuration for this device */
    err = R_I3C_DeviceCfgSet(&g_i3c0_ctrl, &g_slave_device_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable Slave mode */
    err = R_I3C_Enable(&g_i3c0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set the status returned to the Master in response to a GETSTATUS command */
    err = R_I3C_SlaveStatusSet(&g_i3c0_ctrl, slave_device_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_slave_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_slave_process_event.
 * Description  : This function processes Master request or sends IBI to Master.
 * Arguments    : p_event_flag              Pointer to store detected I3C event flags.
 * Return Value : FSP_SUCCESS               Processed successfully.
 *                Any other error codes     Failed to process.
 **********************************************************************************************************************/
fsp_err_t i3c_slave_process_event(uint32_t *p_event_flag)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t event_flag = RESET_VALUE;
    static uint32_t s_ibi_write_count = RESET_VALUE;

    g_b_ibi_write_sent = false;

    /* Wait for I3C events */
    event_flag = i3c_app_event_notify((I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE |
                                       I3C_EVENT_FLAG_COMMAND_COMPLETE |
                                       I3C_EVENT_FLAG_WRITE_COMPLETE |
                                       I3C_EVENT_FLAG_READ_COMPLETE |
                                       I3C_EVENT_FLAG_READ_BUFFER_FULL |
                                       I3C_EVENT_FLAG_IBI_WRITE_COMPLETE |
                                       I3C_EVENT_FLAG_INTERNAL_ERROR), WAIT_TIME);

    /* Check if flag is command complete and broadcast flag from Master */
    if (I3C_EVENT_FLAG_COMMAND_COMPLETE & event_flag)
    {
        if (I3C_CCC_BROADCAST_RSTDAA == g_slave_received_ccc_code)
        {
            /* The current device Dynamic Address is reset by Master */
            g_slave_dynamic_address = RESET_VALUE;

            /* Process I3C device DAA block */
            err = i3c_device_daa_participation();
            if (FSP_SUCCESS != err)
            {
                *p_event_flag = event_flag;
                return FSP_ERR_INTERNAL;
            }
        }

#ifdef I3C_HDR_DDR_SUPPORT
        if (I3C_HDR_COMMAND_CODE_WRITE == g_slave_received_ccc_code)
        {
            set_next_read_buffer();

            /* Prepare write buffer */
            memcpy(&s_write_data[RESET_VALUE], p_last, g_data_transfer_size);
            err = i3c_slave_write();
            if (FSP_SUCCESS != err)
            {
                *p_event_flag = event_flag;
                return err;
            }

            led_indicate(LED_WRITE);
        }
        if (I3C_HDR_COMMAND_CODE_READ == g_slave_received_ccc_code)
        {
            err = i3c_slave_read();
            if (FSP_SUCCESS != err)
            {
                *p_event_flag = event_flag;
                return err;
            }

            led_indicate(LED_READ);
        }
#endif /* I3C_HDR_DDR_SUPPORT */
    }

    /* Check if event is write complete */
    if (event_flag & I3C_EVENT_FLAG_WRITE_COMPLETE)
    {
        /* Note that the application may also call i3c_api_t::read or i3c_api_t::write from this event
         * In order to set the transfer buffers for the next transfer */
        /* When a read buffer is full or a write operation (Master Read/Slave Write) is complete,
         * prepare a buffer for reading */
        err = i3c_slave_read();
        if (FSP_SUCCESS != err)
        {
            *p_event_flag = event_flag;
            return err;
        }

        led_indicate(LED_WRITE);
    }

    /* Check if event is read complete */
    if (event_flag & I3C_EVENT_FLAG_READ_COMPLETE)
    {
        /* Set the next read buffer */
        set_next_read_buffer();

        /* Note that the application may also call i3c_api_t::read or i3c_api_t::write from this event
         * In order to set the transfer buffers for the next transfer */
        /* When a read operation (Master Write/Slave Read) is complete, prepare a buffer for writing */
        memcpy(&s_write_data[RESET_VALUE], p_last, g_data_transfer_size);
        err = i3c_slave_write();
        if (FSP_SUCCESS != err)
        {
            *p_event_flag = event_flag;
            return err;
        }

        led_indicate(LED_READ);
    }

    /* Check if event is IBI write complete */
    if (event_flag & I3C_EVENT_FLAG_IBI_WRITE_COMPLETE)
    {
        led_indicate(LED_WRITE);
    }

    /* Check if event is read buffer full */
    if (event_flag & I3C_EVENT_FLAG_READ_BUFFER_FULL)
    {
        /* If there is no user provided read buffer, or if the user provided read buffer has been filled,
         * the driver will notify the application that the buffer is full. The application may provide
         * a new read buffer by calling i3c_api_t::read. If no read buffer is provided, then any remaining bytes
         * in the transfer will be dropped. */
        err = i3c_slave_read();
        if (FSP_SUCCESS != err)
        {
            *p_event_flag = event_flag;
            return err;
        }
    }

    /* Check if event is internal error */
    if (event_flag & I3C_EVENT_FLAG_INTERNAL_ERROR)
    {
        *p_event_flag = event_flag;
        return FSP_ERR_INTERNAL;
    }

    /* Check if on-board switch is pressed */
    if (true == read_onboard_sw_status())
    {
        /* Set the value of the data to write */
        for (uint32_t cnt = RESET_VALUE; cnt < sizeof(s_ibi_write_data); cnt++)
        {
            s_ibi_write_data[cnt] = (uint8_t) (cnt + s_ibi_write_count) & UINT8_MAX;
        }
        s_ibi_write_count++;

        /* Initiate an IBI write operation */
        err = i3c_slave_ibi_write(I3C_IBI_TYPE_INTERRUPT, s_ibi_write_data, sizeof(s_ibi_write_data));
        if (FSP_SUCCESS != err)
        {
            *p_event_flag = event_flag;
            return err;
        }
        g_b_ibi_write_sent = true;
    }

    *p_event_flag = event_flag;
    return err;
}
/***********************************************************************************************************************
* End of function i3c_slave_process_event
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
 * Function Name: g_i3c0_callback.
 * Description  : This function is callback for I3C.
 * Arguments    : p_args                    Callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void g_i3c0_callback(i3c_callback_args_t const *const p_args)
{
    /* Update the event in global array and this will be used in i3c_app_event_notify function */
    g_i3c_event_status = p_args->event_status;
    s_i3c_event_count[p_args->event]++;

    switch (p_args->event)
    {
        case I3C_EVENT_ADDRESS_ASSIGNMENT_COMPLETE:
        {
            g_slave_dynamic_address = p_args->dynamic_address;
            break;
        }

        case I3C_EVENT_READ_COMPLETE:
        {
            /* The number of bytes read by the Slave will be available in i3c_callback_args_t::transfer_size */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_WRITE_COMPLETE:
        {
            /* The number of bytes written by the Slave will be available in i3c_callback_args_t::transfer_size */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_IBI_WRITE_COMPLETE:
        {
            /* Notify the application that the IBI write is complete */
            g_data_transfer_size = p_args->transfer_size;
            break;
        }

        case I3C_EVENT_COMMAND_COMPLETE:
        {
            /* The command code and transfer size will be available in p_args.
             * If the command code is a Broadcast or Direct Set, then data will
             * be stored in the read buffer provided by i3c_api_t::read.
             * If the command code is a Direct Get, then the data will be automatically
             * sent from device SFR */
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
/***********************************************************************************************************************
* End of function g_i3c0_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_device_daa_participation.
 * Description  : This function waits for bus initialization and sends IBI Hot-Join request to Master.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Bus initialized and sent IBI Hot-Join request successfully.
 *                Any other error codes     Failed to initialize bus or send IBI Hot-Join request.
 **********************************************************************************************************************/
fsp_err_t i3c_device_daa_participation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t event_flag = RESET_VALUE;
    static uint32_t s_wait_count = MAX_WAIT_TIME_BUS_INIT_10S;

    /* Wait for the bus initialization */
    while (RESET_VALUE == g_slave_dynamic_address)
    {
        event_flag = i3c_app_event_notify(I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE, WAIT_TIME);
        if ((--s_wait_count == RESET_VALUE) && (!(event_flag & I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE)))
        {
            /* Initiate an IBI Hot-Join request */
            err = i3c_slave_ibi_write(I3C_IBI_TYPE_HOT_JOIN, NULL, RESET_VALUE);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
            s_wait_count = MAX_WAIT_TIME_BUS_INIT_10S;
        }
    }

    led_indicate(LED_DAA);

    /* Set the buffer for storing data received during a read transfer */
    p_next = s_read_data[RESET_VALUE];

    /* Read the data from I3C bus */
    return i3c_slave_read();
}
/***********************************************************************************************************************
* End of function i3c_device_daa_participation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_app_event_notify.
 * Description  : This function starts the timer and waits for the event set in the I3C callback until
 *                specified timeout.
 * Arguments    : set_event_flag_value      Requested event flag.
 *                timeout                   Specified timeout in milliseconds.
 * Return Value : I3C event flag value      Received I3C event flag successfully.
 *                0                         Timeout expires before the event is received.
 **********************************************************************************************************************/
static uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout)
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

        /* Check for event received from I3C callback function is similar to event which user wants */
        get_event_flag_value = (set_event_flag_value & get_event_flag_value);
        if (get_event_flag_value)
        {
            g_i3c_event_status = RESET_VALUE;
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
static void set_next_read_buffer(void)
{
    p_last = p_next;
    p_next = ((p_next == s_read_data[RESET_VALUE]) ? s_read_data[1] : s_read_data[RESET_VALUE]);
}
/***********************************************************************************************************************
* End of function set_next_read_buffer
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_slave_read.
 * Description  : This function prepares the read buffer to receive data from the I3C Master.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Prepared successfully.
 *                Any other error codes     Failed to prepare.
 **********************************************************************************************************************/
static fsp_err_t i3c_slave_read(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_I3C_Read(&g_i3c0_ctrl, p_next, MAX_READ_DATA_LEN, false);

    return err;
}
/***********************************************************************************************************************
* End of function i3c_slave_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_slave_write.
 * Description  : This function prepares the write buffer to send data to the I3C Master.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Prepared successfully.
 *                Any other error codes     Failed to prepare.
 **********************************************************************************************************************/
static fsp_err_t i3c_slave_write(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_I3C_Write(&g_i3c0_ctrl, s_write_data, g_data_transfer_size, false);

    return err;
}
/***********************************************************************************************************************
* End of function i3c_slave_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_slave_ibi_write.
 * Description  : This function performs an IBI write to the I3C Master.
 * Arguments    : ibi_type                  IBI type (Interrupt or Hot-Join).
 *                p_data                    Pointer to IBI payload data.
 *                length                    Length of IBI payload in bytes.
 * Return Value : FSP_SUCCESS               Performed successfully.
 *                Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static fsp_err_t i3c_slave_ibi_write(i3c_ibi_type_t ibi_type, uint8_t *p_data, uint32_t length)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_I3C_IbiWrite(&g_i3c0_ctrl, ibi_type, p_data, length);

    return err;
}
/***********************************************************************************************************************
* End of function i3c_slave_ibi_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: icu_init.
 * Description  : This function initializes and enables the ICU module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_sw_ctrl, &g_external_irq_sw_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_sw_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function icu_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: icu_deinit.
 * Description  : This function closes the opened ICU module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void icu_deinit(void)
{
    if (g_external_irq_sw_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_sw_ctrl);
    }
}
/***********************************************************************************************************************
* End of function icu_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: external_irq_callback.
 * Description  : This function is a user defined external IRQ callback.
 * Arguments    : p_args                    Callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t *p_args)
{
    /* Check for the right interrupt */
    if (USER_SW_IRQ_NUMBER == p_args->channel)
    {
        b_onboard_sw_pressed = true;
        R_IOPORT_PinRead(g_ioport.p_ctrl, USER_SW_PIN, &s_last_switch_status);
    }
}
/***********************************************************************************************************************
* End of function external_irq_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: read_onboard_sw_status.
 * Description  : This function reads the on-board switch status.
 * Arguments    : None.
 * Return Value : True if switch is pressed else return false.
 **********************************************************************************************************************/
static bool read_onboard_sw_status(void)
{
    if (true == b_onboard_sw_pressed)
    {
        bsp_io_level_t cur_switch_status;

        /* Reset the flag */
        b_onboard_sw_pressed = false;

        /* Read the current status of switch */
        R_IOPORT_PinRead(g_ioport.p_ctrl, USER_SW_PIN, &cur_switch_status);
        if ((cur_switch_status == BSP_IO_LEVEL_LOW) && (s_last_switch_status == BSP_IO_LEVEL_LOW))
        {
            return true;
        }
    }
    return false;
}
/***********************************************************************************************************************
* End of function read_onboard_sw_status
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: led_indicate.
 * Description  : This function toggles the on-board LEDs based on the specified event case.
 * Arguments    : led_case                  LED pattern (1: DAA/init, 2: write, 3: read).
 * Return Value : None.
 **********************************************************************************************************************/
static void led_indicate(uint8_t led_case)
{
    uint8_t idx;
    bsp_io_level_t led_status;

    switch (led_case)
    {
        /* LED1 toggle on board with 3 LEDs, LED1 & LED2 toggle on board with only have 2 LEDs */
        case LED_DAA:
        {
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[0], (!led_status));
            if (g_bsp_leds.led_count != 3U)
            {
                R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], &led_status);
                R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[1], (!led_status));
            }
            break;
        }

        /* LED2 toggle on board with 3 LEDs, LED1 toggle on board with only have 2 LEDs */
        case LED_WRITE:
        {
            idx = (g_bsp_leds.led_count == 3U) ? 1U : 0U;
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[idx], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[idx], (!led_status));
            break;
        }

        /* LED3 toggle on board with 3 LEDs, LED2 toggle on board with only have 2 LEDs */
        case LED_READ:
        {
            idx = (g_bsp_leds.led_count == 3U) ? 2U : 1U;
            R_IOPORT_PinRead(g_ioport.p_ctrl, g_bsp_leds.p_leds[idx], &led_status);
            R_IOPORT_PinWrite(g_ioport.p_ctrl, g_bsp_leds.p_leds[idx], (!led_status));
            break;
        }

        default:
        {
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function led_indicate
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: g_timeout_timer_callback.
 * Description  : This function is callback for Periodic mode timer and stops AGT0 timer in Periodic mode.
 * Arguments    : p_args                    Callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
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
    timeout_value_in_ms = timeout_ms;

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
