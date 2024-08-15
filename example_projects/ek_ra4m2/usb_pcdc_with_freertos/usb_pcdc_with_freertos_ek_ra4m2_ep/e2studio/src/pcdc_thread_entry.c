/***********************************************************************************************************************
 * File Name    : pcdc_thread_entry.c
 * Description  : Contains macros and functions used in pcdc_thread_entry.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "pcdc_thread.h"
#include "common_utils.h"
#include "pcdc_freertos.h"

/* Error status flag */
static volatile bool g_err_flag = false;
/* Variable to store baud rate */
uint32_t g_baud_rate = RESET_VALUE ;
/* Buffer to store user data */
uint8_t user_data_buf[DATA_LEN] = {RESET_VALUE};
/* Variable to store size of data received from tera term */
volatile uint32_t g_terminal_data_size = RESET_VALUE ;

/* Pointers to store USB descriptors */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t * g_apl_string_table[];

const usb_descriptor_t g_usb_pcdc_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 g_apl_string_table,             /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR           /* Num entry String Descriptor */
};

typedef enum
{
    PERIPHERAL_NONE = 0,
    PERIPHERAL_USB,
    PERIPHERAL_UART,
}peripheral_t;

typedef struct
{
    uint32_t peripheral;
    union
    {
        uint32_t data_size;
    }u;
}queue_evt_t;

/* Variable to capture USB event. */
volatile bool g_tx_complete = true;
volatile usb_event_info_t usb_pcdc_event;

/* Flag to indicate USB resume/suspend status */
static bool  b_usb_attach = false;
/* Variable to store UART settings */
volatile usb_pcdc_linecoding_t g_line_coding;
usb_pcdc_ctrllinestate_t g_control_line_state =
{
 .bdtr = 0,
 .brts = 0,
};

/* Variables to store  Baud rate setting values */
baud_setting_t baud_setting;
bool       enable_bitrate_modulation = true;
uint32_t   error_rate_x_1000         = BAUD_ERROR_RATE;

uart_cfg_t g_uart_test_cfg;
sci_uart_extended_cfg_t sci_extend_cfg;
uint8_t usb_tx_buffer[512];

/* We have not woken a task at the start of the ISR. */
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

/*User defined functions */
void set_pcdc_line_coding(volatile usb_pcdc_linecoding_t *p_line_coding, const uart_cfg_t *p_uart_test_cfg);
void set_uart_line_coding_cfg(uart_cfg_t *p_uart_test_cfg, const volatile usb_pcdc_linecoding_t * p_line_coding);

/*******************************************************************************************************************//**
 *  @brief       Initialize line coding parameters based on UART configuration
 *  @param[in]   p_line_coding       Updates the line coding member values
                 p_uart_test_cfg     Pointer to store UART configuration properties
 *  @retval      None
 **********************************************************************************************************************/
 void set_pcdc_line_coding(volatile usb_pcdc_linecoding_t *p_line_coding, const uart_cfg_t *p_uart_test_cfg)
{
    /* Configure the line coding based on initial settings */
    if (g_uart_cfg.stop_bits == UART_STOP_BITS_1)
        p_line_coding->b_char_format = 0;
    else if (g_uart_cfg.stop_bits == UART_STOP_BITS_2)
        p_line_coding->b_char_format = 2;

    if (g_uart_cfg.parity == UART_PARITY_OFF)
        p_line_coding->b_parity_type = 0;
    else if (g_uart_cfg.parity == UART_PARITY_ODD)
        p_line_coding->b_parity_type = 1;
    else if (g_uart_cfg.parity == UART_PARITY_EVEN)
        p_line_coding->b_parity_type = 2;

    if (p_uart_test_cfg->data_bits == UART_DATA_BITS_8)
        p_line_coding->b_data_bits = 8;
    else if (p_uart_test_cfg->data_bits == UART_DATA_BITS_7)
        p_line_coding->b_data_bits = 7;
    else if (p_uart_test_cfg->data_bits == UART_DATA_BITS_9)
        p_line_coding->b_data_bits = 9;

    /* Ideally put the baud rate into p_line_coding;
     * but FSP does not have an API to calculate the baud-rate
     * based on the UART configuration values */
    ;
}

/*******************************************************************************************************************//**
 *  @brief       Initialize UART config values based on user input values through serial terminal
 *  @param[in]   p_uart_test_cfg   Pointer to store UART configuration properties   
                 p_line_coding     Updates the line coding member values
 *  @retval      None
 **********************************************************************************************************************/
 void set_uart_line_coding_cfg(uart_cfg_t *p_uart_test_cfg, const volatile usb_pcdc_linecoding_t * p_line_coding)
{
    /* Set number of parity bits */
    switch (p_line_coding->b_parity_type)
    {
        default:
            p_uart_test_cfg->parity = UART_PARITY_OFF;
            break;
        case 1:
            p_uart_test_cfg->parity = UART_PARITY_ODD;
            break;
        case 2:
            p_uart_test_cfg->parity = UART_PARITY_EVEN;
            break;
    }
    /* Set number of data bits */
    switch (p_line_coding->b_data_bits)
    {
        default:
        case 8:
            p_uart_test_cfg->data_bits = UART_DATA_BITS_8;
            break;
        case 7:
            p_uart_test_cfg->data_bits = UART_DATA_BITS_7;
            break;
        case 9:
            p_uart_test_cfg->data_bits = UART_DATA_BITS_9;
            break;
    }
    /* Set number of stop bits */
    switch (p_line_coding->b_char_format)
    {
        default:
            p_uart_test_cfg->stop_bits = UART_STOP_BITS_1;
            break;
        case 2:
            p_uart_test_cfg->stop_bits = UART_STOP_BITS_2;
            break;
    }
}

/* New Thread entry function */
/* pvParameters contains TaskHandle_t */
void pcdc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err              = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    BaseType_t err_queue       = pdFALSE;
    queue_evt_t q_instance;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open USB instance */
    {
        err = R_USB_Open (&g_basic_ctrl, &g_basic_cfg);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Open failed.\r\n");
            APP_ERR_TRAP(err);
        }
        APP_PRINT("\r\nUSB Opened successfully.\n\r");
    }

    /* Open the UART with initial configuration.*/
    {
        memcpy(&g_uart_test_cfg, &g_uart_cfg, sizeof(g_uart_test_cfg));
        memcpy(&sci_extend_cfg, g_uart_cfg.p_extend, sizeof(sci_extend_cfg));
        g_uart_test_cfg.p_extend = &sci_extend_cfg;
        err = R_SCI_UART_Open(&g_uart_ctrl, &g_uart_test_cfg);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n**  R_SCI_UART_Open API failed  **\r\n");
            APP_ERR_TRAP(err);
        }

        /* First time Synchronization of the line coding between UART & USB */
        set_pcdc_line_coding (&g_line_coding, &g_uart_test_cfg);

        /* Initialize the semaphores to allow one synchronization event to occur */
        {
            BaseType_t err_semaphore = xSemaphoreGive(g_uart_tx_mutex);
            if(pdTRUE != err_semaphore)
            {
                APP_ERR_PRINT("\r\n xSemaphoreGive on g_uart_tx_mutex Failed \r\n");
                APP_ERR_TRAP(err_semaphore);
            }

            err_semaphore = xSemaphoreGive(g_usb_tx_semaphore);
            if(pdTRUE != err_semaphore)
            {
                APP_ERR_PRINT("\r\n xSemaphoreGive on g_usb_tx_semaphore Failed \r\n");
                APP_ERR_TRAP(err_semaphore);
            }
        }
    }

    while (1)
    {
        /* Check if USB event is received */
        err_queue = xQueueReceive(g_event_queue, (void * const)&q_instance, (portMAX_DELAY));
        if(pdTRUE != err_queue)
        {
            APP_ERR_PRINT("\r\nNo USB Event received. Please check USB connection \r\n");
        }

        if (PERIPHERAL_UART == q_instance.peripheral)
        {
            /* Start sending the data */
            {
                QueueHandle_t * p_queue = (2 == g_uart_ctrl.data_bytes)  ? &g_usb_tx_x2_queue : &g_usb_tx_queue;
                UBaseType_t msg_waiting_count = uxQueueMessagesWaiting(*p_queue);
                volatile uint32_t rx_data_size = (2 == g_uart_ctrl.data_bytes) ? 2 : 1;

                if (0U < msg_waiting_count)
                {
                    /* Pull out as many item from queue as possible */
                    uint32_t unload_count = (msg_waiting_count < sizeof(usb_tx_buffer)) ?
                            msg_waiting_count : sizeof(usb_tx_buffer);

                    /* Wait for previous USB transfer to complete */
                    BaseType_t err_semaphore = xSemaphoreTake (g_usb_tx_semaphore, portMAX_DELAY);
                    if (pdTRUE == err_semaphore)
                    {
                        for(uint32_t itr = 0, idx = 0; itr < unload_count; itr++, idx+=rx_data_size)
                        {
                            if(pdTRUE != xQueueReceive(*p_queue, &usb_tx_buffer[idx], portMAX_DELAY))
                            {
                                APP_ERR_PRINT("\r\n Did not receive expected count of characters \r\n");
                                APP_ERR_TRAP(1);
                            }
                        }

                        /* Write data to host machine */
                        err = R_USB_Write (&g_basic_ctrl, &usb_tx_buffer[0], (uint32_t)unload_count*rx_data_size, USB_CLASS_PCDC);
                        if (FSP_SUCCESS != err)
                        {
                            APP_ERR_PRINT("\r\nR_USB_Write API failed.\r\n");
                            deinit_usb();
                            deinit_uart();
                            APP_ERR_TRAP(err);
                        }
                    }
                }

            }
            continue;
        }

        /* Check for usb event */
        if (PERIPHERAL_USB == q_instance.peripheral)
        {
            /* Send received data out to the UART */
            if ((true == b_usb_attach))
            {
                if(0U < q_instance.u.data_size)
                {
                    /* Wait till previously queued data is out completely */
                    {
                        BaseType_t err_semaphore = xSemaphoreTake( g_uart_tx_mutex, portMAX_DELAY );

                        if(pdTRUE != err_semaphore)
                        {
                            APP_ERR_PRINT("\r\nxSemaphoreTake on g_uart_tx_mutex Failed \r\n");
                            APP_ERR_TRAP(err_semaphore);
                        }
                    }

                    /* Write data to UART Tx pin */
                    err = R_SCI_UART_Write(&g_uart_ctrl, user_data_buf, q_instance.u.data_size);
                    if(FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Write API failed  **\r\n");
                        deinit_usb();
                        deinit_uart();
                        APP_ERR_TRAP(err);
                    }
                }
                else
                {
                    /* Buffer is physically transmitted since UART_EVENT_TX_COMPLETE was generated. */
                    /* Continue to read data from USB. */
                    /* The amount of data received will be known when USB_STATUS_READ_COMPLETE event occurs*/
                    err = R_USB_Read(&g_basic_ctrl, user_data_buf, DATA_LEN, USB_CLASS_PCDC);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\nR_USB_Read API failed.\r\n");
                        deinit_usb();
                        deinit_uart();
                        APP_ERR_TRAP(err);
                    }
                }
            }
            continue;
        }

        if(PERIPHERAL_NONE == q_instance.peripheral)
        {
            ;
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       Deinitialize initialized UART module
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_uart(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close module */
    err =  R_SCI_UART_Close (&g_uart_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Close API failed  ** \r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       Deinitialize initialized USB instance
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_usb(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close module */
    err = R_USB_Close (&g_basic_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n**  USB close API failed  ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       USB PCDC with freertos callback
 * @param[IN]   usb_event_info_t    usb event
 * @param[IN]   usb_hdl_t           task
 * @param[IN]   usb_onoff_t         state
 * @retval      None
 **********************************************************************************************************************/
void usb_pcdc_callback(usb_event_info_t * p_pcdc_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (task);
    FSP_PARAMETER_NOT_USED (state);

    queue_evt_t instance;

    switch (p_pcdc_event->event)
    {
        /* USB Read complete Class Request */
        case USB_STATUS_READ_COMPLETE:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_READ_COMPLETE \n");
            instance.peripheral = PERIPHERAL_USB;
            instance.u.data_size = p_pcdc_event->data_size;

            /* Send the event from queue */
            if (pdTRUE != (xQueueSend(g_event_queue, (const void *)&instance, (TickType_t)(RESET_VALUE))))
            {
                g_err_flag = true;
                APP_ERR_PRINT("\r\n xQueueSend on g_event_queue Failed \r\n");
                APP_ERR_TRAP(pdTRUE);
            }
        }
        break;

        /*  Write Complete Class Request */
        case USB_STATUS_WRITE_COMPLETE:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_WRITE_COMPLETE \n");
            BaseType_t err_semaphore = xSemaphoreGive(g_usb_tx_semaphore);
            if(pdTRUE != err_semaphore)
            {
                APP_ERR_PRINT("\r\n xSemaphoreGive on g_usb_tx_semaphore Failed \r\n");
                APP_ERR_TRAP(err_semaphore);
            }
            return;
        }
        break;

		/* Configured state class request */
        case USB_STATUS_CONFIGURED:
        {
            APP_PRINT("\r\nUSB Status Configured Successful\r\n");
            /* Read data from tera term */
            fsp_err_t err = R_USB_Read (&g_basic_ctrl, user_data_buf, DATA_LEN, USB_CLASS_PCDC);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read API failed.\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
            return;
        }
        break;

        /* Receive Class Request */
        case USB_STATUS_REQUEST:
        {
            /* Check for the specific CDC class request IDs */
            if (USB_PCDC_SET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST \nRequest_Type: USB_PCDC_SET_LINE_CODING \n");

                /* Get the class request.*/
                fsp_err_t err = R_USB_PeriControlDataGet (&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
                if(FSP_SUCCESS == err)
                {
                    /* Line Coding information read from the control pipe */
                    APP_PRINT ("\n*********SET***********\nbitrate = %d\nChar_Format = %d\nParity_Type = %d\ndata_Bit = %d\n***********************\n",\
                               g_line_coding.dw_dte_rate,
                               g_line_coding.b_char_format,
                               g_line_coding.b_parity_type,
                               g_line_coding.b_data_bits);
                    sci_extend_cfg.p_baud_setting = &baud_setting;
                    g_uart_test_cfg.p_extend =  &sci_extend_cfg;

                    /* Calculate the baud rate*/
                    g_baud_rate = g_line_coding.dw_dte_rate;

                    if(INVALID_SIZE < g_baud_rate)
                    {
						/* Calculate baud rate setting registers */
                        err = R_SCI_UART_BaudCalculate(g_baud_rate, enable_bitrate_modulation, error_rate_x_1000, &baud_setting);
                        if(FSP_SUCCESS != err)
                        {
                            APP_ERR_PRINT ("\r\n**  R_SCI_UART_BaudCalculate API failed  **\r\n");
                            deinit_usb();
                            deinit_uart();
                            APP_ERR_TRAP(err);
                        }
                    }

                    /* Set number of parity bits */
                    set_uart_line_coding_cfg (&g_uart_test_cfg, &g_line_coding);
                    /* Close module */
                    err =  R_SCI_UART_Close (&g_uart_ctrl);
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Close API failed  ** \r\n");
                    }

                    /* Open UART with changed UART settings */
                    err = R_SCI_UART_Open(&g_uart_ctrl, &g_uart_test_cfg);
                    if(FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT ("\r\n**  R_SCI_UART_Open API failed  **\r\n");
                        deinit_usb();
                        APP_ERR_TRAP(err);
                    }
                }
            }

            else if (USB_PCDC_GET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST \nRequest_Type: USB_PCDC_GET_LINE_CODING \n");
                /* Set the class request.*/
                fsp_err_t err = R_USB_PeriControlDataSet (&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
                if (FSP_SUCCESS == err)
                {
                    APP_PRINT ("\n*********GET***********\nbitrate = %d\nChar_Format = %d\nParity_Type = %d\ndata_Bit = %d\n***********************\n",\
                               g_line_coding.dw_dte_rate,
                               g_line_coding.b_char_format,
                               g_line_coding.b_parity_type,
                               g_line_coding.b_data_bits);
                }
                else
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed.\r\n");
                    deinit_usb();
                    deinit_uart();
                    APP_ERR_TRAP(err);
                }
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST \nRequest_Type: USB_PCDC_SET_CONTROL_LINE_STATE \n");
                /* Get the status of the control signals */
                fsp_err_t err = R_USB_PeriControlDataGet(&g_basic_ctrl,
                                                         (uint8_t*) &g_control_line_state,
                                                         sizeof(usb_pcdc_ctrllinestate_t));
                if (FSP_SUCCESS == err)
                {
                    if (FSP_SUCCESS == err)
                    {
                        g_control_line_state.bdtr = (unsigned char)((p_pcdc_event->setup.request_value >> 0) & 0x01);
                        g_control_line_state.brts = (unsigned char)((p_pcdc_event->setup.request_value >> 1) & 0x01);

                        /* Toggle the line state if the flow control pin is set to a value (other than SCI_UART_INVALID_16BIT_PARAM) */
                        if (SCI_UART_INVALID_16BIT_PARAM != g_uart_ctrl.flow_pin)
                        {
                            R_BSP_PinAccessEnable();
                            R_BSP_PinWrite(g_uart_ctrl.flow_pin,
                                    (g_control_line_state.brts == 0) ? BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH);
                            R_BSP_PinAccessDisable();
                        }
                        APP_PRINT("\n******Line State*******\nbdtr = %d\nbrts = %d\n***********************\n",
                                g_control_line_state.bdtr,
                                g_control_line_state.brts);
                    }
                }

                /* Set the usb status as ACK response.*/
                err = R_USB_PeriControlStatusSet (&g_basic_ctrl, USB_SETUP_STATUS_ACK);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed.\r\n");
                    deinit_usb();
                    deinit_uart();
                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                ;
            }
            return;
        }
        break;
        /* Complete Class Request */
        case USB_STATUS_REQUEST_COMPLETE:
        {
            if(USB_PCDC_SET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST_COMPLETE \nRequest_Type: USB_PCDC_SET_LINE_CODING \n");
            }
            else if (USB_PCDC_GET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST_COMPLETE \nRequest_Type: USB_PCDC_GET_LINE_CODING \n");
            }
            else
            {
                ;
            }
            return;
        }
        break;

        /* Detach, Suspend State Class requests */
        case USB_STATUS_DETACH:
            /* Stop all read/write transactions using R_USB_Stop */
        case USB_STATUS_SUSPEND:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_DETACH or USB_STATUS_SUSPEND\r\n");
            /* Reset the usb attached flag indicating usb is removed.*/
            b_usb_attach = false;
            memset (user_data_buf, RESET_VALUE, sizeof(user_data_buf));
            return;
        }
        /* Resume state */
        case USB_STATUS_RESUME:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_RESUME\r\n");
            /* set the usb attached flag*/
            b_usb_attach = true;
        }
        break;

        default:
            return;
            break;
    }
}


/*****************************************************************************************************************
 *  @brief      UART user callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            {
                queue_evt_t linstance;
                sci_uart_instance_ctrl_t const * const p_ctrl = p_args->p_context;

                linstance.peripheral = PERIPHERAL_UART;
                linstance.u.data_size = (2 == p_ctrl->data_bytes) ? 2 : 1;

                QueueHandle_t * p_queue = (2 == p_ctrl->data_bytes)  ? &g_usb_tx_x2_queue : &g_usb_tx_queue;
                /* Send the event from queue */
                if (pdTRUE != (xQueueSendFromISR(*p_queue, &p_args->data, NULL)))
                {
                    g_err_flag = true;
                    APP_ERR_PRINT("\r\n xQueueSend on g_event_queue Failed \r\n");
                    APP_ERR_TRAP(pdTRUE);
                }

                /* Send the event from queue */
                if (pdTRUE != (xQueueSendFromISR(g_event_queue, &linstance, NULL)))
                {
                    g_err_flag = true;
                    APP_ERR_PRINT("\r\n xQueueSend on g_event_queue Failed \r\n");
                    APP_ERR_TRAP(pdTRUE);
                }
            }
        }
        break;
		
        case UART_EVENT_TX_COMPLETE:
        {
            queue_evt_t linstance;
            BaseType_t err_semaphore = xSemaphoreGiveFromISR(g_uart_tx_mutex, NULL);
            if(pdTRUE != err_semaphore)
            {
                APP_ERR_PRINT("\r\n xSemaphoreGiveFromISR Failed \r\n");
                APP_ERR_TRAP(err_semaphore);
            }

            /* Indicate that all bytes were transferred over; and you're ready to listen again on USB. */
            linstance.peripheral = PERIPHERAL_USB;
            linstance.u.data_size = 0;

            /* Send the event from queue */
            if (pdTRUE != (xQueueSendFromISR(g_event_queue, &linstance, NULL)))
            {
                g_err_flag = true;
                APP_ERR_PRINT("\r\n xQueueSend on g_event_queue Failed \r\n");
                APP_ERR_TRAP(pdTRUE);
            }
        }
        break;

        case UART_EVENT_ERR_PARITY:
            APP_PRINT("\r\n !! UART_EVENT_ERR_PARITY \r\n");
            APP_ERR_TRAP(1);
            break;

        case UART_EVENT_ERR_FRAMING:
            APP_PRINT("\r\n !! UART_EVENT_ERR_FRAMING \r\n");
            APP_ERR_TRAP(1);
            break;

        case UART_EVENT_ERR_OVERFLOW:
            APP_PRINT("\r\n !! UART_EVENT_ERR_OVERFLOW \r\n");
            APP_ERR_TRAP(1);
            break;

        case UART_EVENT_BREAK_DETECT:
            APP_PRINT("\r\n !! UART_EVENT_BREAK_DETECT \r\n");
            APP_ERR_TRAP(1);
            break;

        default: /** Do Nothing */
            break;
    }
}
