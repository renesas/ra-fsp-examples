/***********************************************************************************************************************
 * File Name    : usb_hcdc_thread_entry.c
 * Version      : .
 * Description  : Handle USB HCDC (Host CDC) operations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_hcdc_thread.h"
#include "usb_hcdc_hmsc_app.h"

/**********************************************************************************************************************
 Private variables and functions
 *********************************************************************************************************************/
static usb_hcdc_linecoding_t g_com_parm;
static void set_line_coding(usb_instance_ctrl_t *p_ctrl, uint8_t device_address);
static void set_control_line_state(usb_instance_ctrl_t *p_ctrl, uint8_t device_address);
static void get_line_coding(usb_instance_ctrl_t *p_ctrl, uint8_t device_address);
static void usb_cdc_operation(void);
static void usb_cdc_write_operation(char * cdc_data, uint32_t size);
static uint8_t g_rcv_buf[CDC_READ_DATA_LEN] BSP_ALIGN_VARIABLE(4) = { RESET_VALUE };/* Receive buffer */
static uint8_t g_snd_buf[CDC_WRITE_DATA_LEN] BSP_ALIGN_VARIABLE(4) = { RESET_VALUE };
static usb_event_info_t *p_usb_cdc_event_info;
static bool g_cdc_request_complete = false;
static bool g_cdc_read_complete_flag = true;
static bool cdc_menu_display_enable = true;
volatile bool g_err_flag = false;

/**********************************************************************************************************************
 Public variables and functions
 *********************************************************************************************************************/
extern int8_t g_new_api_key_str[KEY_SIZE_IN_BYTES + 8];

/**********************************************************************************************************************
 * @brief     This function processes the data from the host to the device and prints the received data on
 *            the terminal.
 * @param[IN] pointer to pvParameters
 * @retval    None
 *********************************************************************************************************************/
void usb_hcdc_thread_entry(void *pvParameters)
{
    /* Suspend the USB HCDC thread and wait for resume from the USB common thread entry */
    vTaskSuspend (usb_hcdc_thread);

    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t err = FSP_SUCCESS;
    BaseType_t queue_item_status = pdFALSE;
    memset (&g_com_parm, RESET_VALUE, sizeof(g_com_parm));

    while (true)
    {
        /* Handle error if queue send fails*/
        if (true == g_err_flag)
        {
            handle_error (g_err_flag, "\r\nError in sending usb event through queue\r\n");
        }

        queue_item_status = xQueueReceive (g_usb_queue, &p_usb_cdc_event_info, NO_WAIT_TIME);

        /* Receive message from queue and analyze the received message*/
        if (pdTRUE == queue_item_status)
        {
            switch (p_usb_cdc_event_info->event)
            {
                case USB_STATUS_CONFIGURED:
                {
                    cdc_menu_display_enable = true;
                    g_cdc_request_complete = false;
                    /* Wait for the USB MSC (Mass Storage Class) command completion event if USB MSC device is connected */
                    xEventGroupWaitBits (g_usb_event_group, USB_MSC_CMD_COMPLETE, pdFALSE, pdFALSE, DELAY_200_TICK);

                    /* Set internal class identifier to USB_CLASS_INTERNAL_HCDC */
                    g_basic0_ctrl.type = (usb_class_t) USB_CLASS_INTERNAL_HCDC;

                    /* CDC Class request "SetLineCoding" */
                    set_line_coding (&g_basic0_ctrl, p_usb_cdc_event_info->device_address);
                    break;
                }

                case USB_STATUS_READ_COMPLETE:
                {
                    APP_PRINT("\r\nReceived data from the CDC device: " GREEN "'%.*s'" RESET "\r\n",
                              p_usb_cdc_event_info->data_size, g_rcv_buf);
                    g_cdc_read_complete_flag = true;
                    break;
                }
                case USB_STATUS_WRITE_COMPLETE:
                {
                    /* Clear the receive buffer */
                    memset (g_rcv_buf, RESET_VALUE, CDC_READ_DATA_LEN);

                    /* Start receive operation */
                    err = R_USB_Read (&g_basic0_ctrl, g_rcv_buf, CDC_READ_DATA_LEN,
                                      p_usb_cdc_event_info->device_address);

                    /* Handle Error */
                    if (FSP_SUCCESS != err)
                    {
                        handle_error (err, "**R_USB_Read API FAILED**");
                    }

                    break;
                }
                case USB_STATUS_REQUEST_COMPLETE:
                {
                    /* Check Complete request "SetLineCoding" */
                    if (USB_CDC_SET_LINE_CODING == (p_usb_cdc_event_info->setup.request_type & USB_BREQUEST))
                    {
                        /* Class notification "SerialState" receive start */
                        set_control_line_state (&g_basic0_ctrl, p_usb_cdc_event_info->device_address);
                    }
                    /* Check Complete request "SetControlLineState" */
                    else if (USB_CDC_SET_CONTROL_LINE_STATE == (p_usb_cdc_event_info->setup.request_type & USB_BREQUEST))
                    {
                        /* CDC Class request "SetLineCoding" */
                        get_line_coding (&g_basic0_ctrl, p_usb_cdc_event_info->device_address);
                        g_cdc_request_complete = true;
                    }
                    else
                    {
                        /* Not support request */
                    }

                    break;
                }
                case USB_STATUS_DETACH:
                {
                    /* Clear all usb event bits */
                    APP_PRINT("\r\n!! USB device is detached !!\r\n");
                    break;
                }

                default:
                {
                    /* No operation to do*/
                    break;
                }
            }
        }

        if (g_cdc_request_complete && g_cdc_read_complete_flag)
        {
            usb_cdc_operation ();
        }

        vTaskDelay (1);
    }

} /* End of function usb_hcdc_thread_entry() */

/**********************************************************************************************************************
 * @brief       This function handles the USB CDC operation.
 * @param[IN]   None.
 * @retval      None.
 *********************************************************************************************************************/
static void usb_cdc_operation(void)
{
    EventBits_t uxBits;
    uxBits = xEventGroupGetBits(g_usb_event_group);

    /* This is an example implementation of communication with the MSC device.
     * The handling in this section matches the expectation and communication
     * protocol of the usb_composite EP. The real-world application can have different use cases.
     * This section needs to adapt based on the MSC device connected. */
    if (uxBits & USB_COMPOSITE_DEVICE_ENABLE)
    {
        /* Display CDC main menu if not already displayed */
        if (cdc_menu_display_enable)
        {
            APP_PRINT(USB_HCDC_MAIN_MENU, USB_CDC_WRITE);
            cdc_menu_display_enable = false;
        }

        /* Check for user input for CDC operations */
        if (USB_CDC_WRITE == (int8_t) (g_new_api_key_str[INDEX_ZERO] - '0'))
        {

            APP_PRINT("\r\nPlease enter text data to write to the CDC device\r\n");

            /* Check if user has provided input */
            if (APP_GET_USER_INPUT())
            {
                /* Trim new line character and perform CDC write operation */
                g_cdc_read_complete_flag = false;
                trim_line_endings ((char*) g_new_api_key_str);
                usb_cdc_write_operation ((char*) g_new_api_key_str, CDC_WRITE_DATA_LEN);
                memset (g_new_api_key_str, NULL_CHAR, sizeof(g_new_api_key_str));
            }
        }
    }

    /* This is an example implementation of communication with the PCDC device.
     * The handling in this section matches the expectation and communication
     * protocol of the usb_pcdc EP. The real-world application can have different use cases.
     * This section needs to adapt based on the CDC device connected.*/
    else if ((uxBits & IS_CDC_DEVICE_CONNECTED))
    {
        APP_PRINT("\r\n!! The CDC device is connected !!");
        APP_PRINT("\r\nPlease enter text data to write to the CDC device\r\n");

        if (INPUT_STATUS_HAVE_INPUT == APP_GET_USER_INPUT())
        {
            char input_char = g_new_api_key_str[INDEX_ZERO];

            /* Check if the input character is valid ('1', '2', or Enter key) when testing with USB PCDC EP*/
            if (input_char == '1' || input_char == '2' || input_char == CARRIAGE_CHAR)
            {
                g_cdc_read_complete_flag = false;
                usb_cdc_write_operation ((char*) g_new_api_key_str, 1);
            }
            else
            {
                APP_PRINT("\r\nInvalid input. Please enter '1', '2', or press Enter.\r\n");
                g_cdc_read_complete_flag = true;
            }
        }
    }
} /* End of function usb_cdc_operation() */

/**********************************************************************************************************************
 * @brief       This function performs USB CDC write operation.
 * @param[IN]   None
 * @retval      None
 *********************************************************************************************************************/
static void usb_cdc_write_operation(char * cdc_data, uint32_t size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear the send buffer */
    memset (g_snd_buf, RESET_VALUE, CDC_WRITE_DATA_LEN);

    /* Copy data to the send buffer */
    memcpy (g_snd_buf, cdc_data, strlen(cdc_data));

    /* Execute writing CDC data to the CDC device */
    err = R_USB_Write (&g_basic0_ctrl, g_snd_buf, size, p_usb_cdc_event_info->device_address);
    if (FSP_SUCCESS != err)
    {
        handle_error (err, "\r\n**R_USB_Write API FAILED**\r\n");
    }
} /* End of function usb_cdc_write_operation() */

/**********************************************************************************************************************
 * @brief       This function initializes the setting of control line state information for host control transfer.
 * @param[IN]   p_ctrl         Pointer to control structure
 * @param[IN]   device_address         USB device address
 * @retval      None.
 *********************************************************************************************************************/
static void set_control_line_state(usb_instance_ctrl_t *p_ctrl, uint8_t device_address)
{
    usb_setup_t setup;
    fsp_err_t err = FSP_SUCCESS;
    uint8_t g_usb_dummy = RESET_VALUE; /* dummy variable to send */

    setup.request_type = SET_CONTROL_LINE_STATE; /* bRequestCode:SET_CONTROL_LINE_STATE, bmRequestType */
    setup.request_value = VALUE_ZERO; /* wValue:Zero */
    setup.request_index = VALUE_ZERO; /* wIndex:Interface */
    setup.request_length = VALUE_ZERO; /* wLength:Zero */

    err = R_USB_HostControlTransfer (p_ctrl, &setup, &g_usb_dummy, device_address);
    if (FSP_SUCCESS != err)
    {
        handle_error (err, "**R_USB_HOSTControlTransfer API 1 FAILED**");
    }
} /* End of function set_control_line_state */

/**********************************************************************************************************************
 * @brief       This function initializes the setting of line coding information for host control transfer.
 * @param[IN]   device_address  USB device address
 * @retval      None.
 *********************************************************************************************************************/
static void set_line_coding(usb_instance_ctrl_t *p_ctrl, uint8_t device_address)
{
    usb_setup_t setup;
    fsp_err_t err = FSP_SUCCESS;
    g_com_parm.dwdte_rate = (uint32_t) USB_HCDC_SPEED_115200;
    g_com_parm.bdata_bits = USB_HCDC_DATA_BIT_8;
    g_com_parm.bchar_format = USB_HCDC_STOP_BIT_1;
    g_com_parm.bparity_type = USB_HCDC_PARITY_BIT_NONE;

    setup.request_type = SET_LINE_CODING; /* bRequestCode:SET_LINE_CODING, bmRequestType */
    setup.request_value = VALUE_ZERO; /* wValue:Zero */
    setup.request_index = VALUE_ZERO; /* wIndex:Interface */
    setup.request_length = LINE_CODING_LENGTH; /* Data:Line Coding Structure */

    /* Request Control transfer */
    err = R_USB_HostControlTransfer (p_ctrl, &setup, (uint8_t*) &g_com_parm, device_address);
    if (FSP_SUCCESS != err)
    {
        handle_error (err, "**R_USB_HostControlTransfer API 2 FAILED**");
    }
} /* End of function set_line_coding */

/**********************************************************************************************************************
 * @brief       This function initializes the retrieval of line coding information for host control transfer.
 * @param[IN]   p_ctrl                pointer to control structure
 * @param[IN]   device_address         USB device address
 * @retval      None.
 *********************************************************************************************************************/
static void get_line_coding(usb_instance_ctrl_t *p_ctrl, uint8_t device_address)
{
    usb_setup_t setup;
    fsp_err_t err = FSP_SUCCESS;
    setup.request_type = GET_LINE_CODING; /* bRequestCode:GET_LINE_CODING, bmRequestType */
    setup.request_value = VALUE_ZERO; /* wValue:Zero */
    setup.request_index = VALUE_ZERO; /* wIndex:Interface */
    setup.request_length = LINE_CODING_LENGTH; /* Data:Line Coding Structure */

    /* Request Control transfer */
    err = R_USB_HostControlTransfer (p_ctrl, &setup, (uint8_t*) &g_com_parm, device_address);
    if (FSP_SUCCESS != err)
    {
        handle_error (err, "**R_USB_HostControlTransfer API 3 FAILED**");
    }
} /* End of function get_line_coding */

/**********************************************************************************************************************
 * @brief       This function is callback for FreeRTOS+HCDC and triggers when USB event occurs from the device.
 * @param[IN]   p_event_info    Triggered event
 * @param[IN]   cur_task        current task handle
 * @param[IN]   usb_state       State of USB
 * @retval      None.
 *********************************************************************************************************************/
void usb_app_hcdc_callback(usb_event_info_t *p_event_info, usb_hdl_t cur_task, usb_onoff_t usb_state)
{
    FSP_PARAMETER_NOT_USED(cur_task);
    FSP_PARAMETER_NOT_USED(usb_state);


    if (USB_STATUS_DETACH == p_event_info->event)
    {
        g_cdc_request_complete = false;
        xEventGroupClearBits (g_usb_event_group, ALL_BIT);
    }

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_usb_queue, &p_event_info, (TickType_t) (NO_WAIT_TIME))))
    {
        g_err_flag = true;
    }
} /* End of function usb_app_hcdc_callback() */

/**********************************************************************************************************************
 * End  Of File
 *********************************************************************************************************************/
