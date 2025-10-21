/***********************************************************************************************************************
 * File Name    : usb_composite_thread_entry.c
 * Description  : Contains entry function of USB Composite.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_composite_thread.h"
#include "common_utils.h"
#include "usb_composite.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_composite_ep
 * @{
 **********************************************************************************************************************/

uint8_t g_buf[DATA_LEN] = {RESET_VALUE};
/* Variable to capture USB event */
static volatile usb_event_info_t * p_usb_event = NULL;
static volatile bool g_err_flag = false;
static bool b_usb_attach = false;
static bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;
static usb_pcdc_linecoding_t g_line_coding;

/* Board's user LED */
extern bsp_leds_t g_bsp_leds;

/* Global variables related to custom block media */
extern const rm_block_media_api_t g_rm_block_media_on_user_media;

extern void r_usb_pmsc_block_media_event_callback(rm_block_media_callback_args_t *p_args);

const rm_block_media_cfg_t g_rm_block_media_cfg =
{.p_extend = NULL, .p_callback = r_usb_pmsc_block_media_event_callback, .p_context = NULL};

rm_block_media_instance_t g_rm_block_media =
{.p_api = &g_rm_block_media_on_user_media, .p_ctrl = NULL, .p_cfg = &g_rm_block_media_cfg};

/*******************************************************************************************************************//**
 * @brief       Entry function of USB Composite thread.
 * @param[in]   Pointer to pvParameters.
 * @retval      None.
 **********************************************************************************************************************/
void usb_composite_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t err              = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    BaseType_t err_queue       = pdFALSE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);
    /* Read the LED pin level */
    R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], &pin_level);

    /* Open USB instance */
    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open API failed\r\n");
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nUSB opened successfully\r\n");

    while (true)
    {
        /* Handle error if queue send fails */
        if (true == g_err_flag)
        {
            handle_error(FSP_ERR_ABORTED, "\r\nError in sending USB event through queue\r\n");
        }

        /* Receive message from queue */
        err_queue = xQueueReceive(g_event_queue, &p_usb_event,(portMAX_DELAY));
        if (pdTRUE != err_queue)
        {
            handle_error(FSP_ERR_ABORTED, "\r\nError in receiving event through queue\r\n");
        }

        /* Process USB events */
        err = process_usb_events();
        handle_error(err, "\r\nprocess_usb_events failed\r\n");

        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function processes USB events.
 * @param[in]   None.
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t process_usb_events(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* USB event received */
    switch (p_usb_event->event)
    {
        case USB_STATUS_CONFIGURED: /* Configured State */
        {
            APP_PRINT("USB configured successfully\r\n");
            /* Read data from Tera Term */
            err = R_USB_Read(&g_basic_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read API failed\r\n");
            }
            break;
        }

        case USB_STATUS_WRITE_COMPLETE: /* Write complete state */
        {
            APP_PRINT("\nUSB status: USB_STATUS_WRITE_COMPLETE\r\n");
            if (b_usb_attach)
            {
                /* Read data from Tera Term */
                err = R_USB_Read(&g_basic_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_Read API failed\r\n");
                }
            }
            else
            {
                /* Do nothing as USB is removed and not connected yet */
            }
            break;
        }

        case USB_STATUS_READ_COMPLETE: /* Read complete state */
        {
            APP_PRINT("\nUSB status: USB_STATUS_READ_COMPLETE\r\n");
            if (b_usb_attach)
            {
                /* Write back the read data from MCU to Tera Term */
                err = R_USB_Write(&g_basic_ctrl, g_buf, p_usb_event->data_size, USB_CLASS_PCDC);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_Write API failed\r\n");
                }
            }
            else
            {
                /* Do nothing as USB is removed and not connected yet */
            }
            break;
        }

        case USB_STATUS_REQUEST: /* Receive class request */
        {
            /* Perform USB status request operation */
            err = usb_status_request();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nusb_status_request failed\r\n");
            }
            break;
        }

        case USB_STATUS_REQUEST_COMPLETE: /* Request complete state */
        {
            if (USB_PCDC_SET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB status: USB_STATUS_REQUEST_COMPLETE\r\nRequest_Type: USB_PCDC_SET_LINE_CODING\r\n");
            }
            else if (USB_PCDC_GET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB status: USB_STATUS_REQUEST_COMPLETE\r\nRequest_Type: USB_PCDC_GET_LINE_CODING\r\n");
            }
            else
            {
                /* Do nothing */
            }
            break;
        }
        case USB_STATUS_DETACH:
        case USB_STATUS_SUSPEND:
        {
            APP_PRINT("\nUSB status: USB_STATUS_DETACH & USB_STATUS_SUSPEND\r\n");
            /* Reset the USB attached flag as indicating USB is removed */
            b_usb_attach = false;
            memset(g_buf, RESET_VALUE, sizeof(g_buf));
            break;
        }
        case USB_STATUS_RESUME:
        {
            APP_PRINT("\nUSB status: USB_STATUS_RESUME\r\n");
            /* Set the USB attached flag */
            b_usb_attach = true;
            break;
        }
        default:
        {
            break;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function processes USB status request.
 * @param[in]   None.
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t usb_status_request(void)
{
    fsp_err_t err  = FSP_SUCCESS;

    /* Check for the specific CDC class request IDs */
    if (USB_PCDC_SET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        APP_PRINT("\nUSB status: USB_STATUS_REQUEST\r\nRequest_Type: USB_PCDC_SET_LINE_CODING\r\n");

        /* Get the class request */
        err = R_USB_PeriControlDataGet(&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet API failed.\r\n");
        }
        else
        {
            APP_PRINT("\nBitrate = %d\nChar_Format = %d\nParity_Type = %d\nData_Bit = %d\n",\
                       g_line_coding.dw_dte_rate, g_line_coding.b_char_format, g_line_coding.b_parity_type,\
                       g_line_coding.b_data_bits);
        }
    }
    else if (USB_PCDC_GET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        APP_PRINT("\nUSB status: USB_STATUS_REQUEST\r\nRequest_Type: USB_PCDC_GET_LINE_CODING\r\n");

        /* Set the class request */
        err = R_USB_PeriControlDataSet(&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet API failed\r\n");
        }
        else
        {
            APP_PRINT("\nBitrate = %d\nChar_Format = %d\nParity_Type = %d\nData_Bit = %d\n",\
                      g_line_coding.dw_dte_rate, g_line_coding.b_char_format, g_line_coding.b_parity_type,\
                      g_line_coding.b_data_bits);
        }
    }
    else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        APP_PRINT("\nUSB status: USB_STATUS_REQUEST\r\nRequest_Type: USB_PCDC_SET_CONTROL_LINE_STATE\r\n");

        /* Toggle on-board LED when the host sets or changes the control line state */
        pin_level ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], pin_level);

        /* Delay for the user to observe the LED state change */
        R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

        /* Set the USB status as ACK response */
        err = R_USB_PeriControlStatusSet(&g_basic_ctrl, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet API failed\r\n");
        }
    }
    else
    {
        /* Do nothing */
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for FreeRTOS+Composite.
 * @param[in]   usb_event_info_t  *p_event_info.
 * @param[in]   usb_hdl_t         handler.
 * @param[in]   usb_onoff_t       on_off.
 * @retval      None.
 **********************************************************************************************************************/
void usb_composite_callback(usb_event_info_t * p_event_info, usb_hdl_t handler, usb_onoff_t on_off)
{

    FSP_PARAMETER_NOT_USED(handler);
    FSP_PARAMETER_NOT_USED(on_off);

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_event_queue, (const void *)&p_event_info, (TickType_t)(RESET_VALUE))))
    {
        g_err_flag = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function closes the USB module, prints and traps error.
 *  @param[in]   status    Error status.
 *  @param[in]   err_str   Error string.
 *  @retval      None.
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_USB_Close(&g_basic_ctrl))
        {
            APP_ERR_PRINT("\r\n** R_USB_Close API failed **\r\n ");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_composite_ep)
 **********************************************************************************************************************/
