/***********************************************************************************************************************
 * File Name    : qspi_blockmedia_thread_entry.c
 * Description  : Contains entry function of qspi_blockmedia_usb_composite.
 ***********************************************************************************************************************/
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
#include "qspi_blockmedia_thread.h"
#include "common_utils.h"
#include "qspi_blockmedia.h"
#include "setup_qspi.h"

/*******************************************************************************************************************//**
 * @addtogroup qspi_blockmedia_usb_composite_ep
 * @{
 **********************************************************************************************************************/

uint8_t g_buf[DATA_LEN] = {RESET_VALUE};
/* Variable to capture USB event. */
static volatile usb_event_info_t   * p_usb_event = NULL ;
static volatile bool g_err_flag = false;
static bool  b_usb_attach = false;
static bsp_io_level_t pin_level   = BSP_IO_LEVEL_HIGH;
static usb_pcdc_linecoding_t g_line_coding;

/* Board's user LED */
extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * @brief     Entry function of qspi_blockmedia thread.
 * @param[IN] pointer to  pvParameters
 * @retval    None
 **********************************************************************************************************************/
void qspi_blockmedia_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
     fsp_err_t err              = FSP_SUCCESS;
     fsp_pack_version_t version = { RESET_VALUE };
     BaseType_t err_queue       = pdFALSE;

     /* version get API for FLEX pack information */
     R_FSP_VersionGet(&version);

     /* Example Project information printed on the Console */
     APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
     APP_PRINT(EP_INFO);

     /* QSPI initialization  */
     err = setup_qspi(&g_qspi_ctrl, &g_qspi_cfg);
     if(FSP_SUCCESS != err)
     {
         APP_ERR_PRINT("\r\nsetup_qspi failed.\r\n");
         APP_ERR_TRAP(err);
     }

     /* Open USB instance */
     err = R_USB_Open (&g_basic_ctrl, &g_basic_cfg);
     if (FSP_SUCCESS != err)
     {
         APP_ERR_PRINT("\r\nR_USB_Open failed.\r\n");
         APP_ERR_TRAP(err);
     }
     APP_PRINT("\r\nUSB Opened successfully.\n\r");

     while (true)
     {
         /* Read the LED pin level.*/
         R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], &pin_level);
         /* Handle error if queue send fails*/
         if (true == g_err_flag)
         {
             handle_error(FSP_ERR_ABORTED, "\r\nError in sending usb event through queue\r\n");
         }

         /* Receive message from queue */
         err_queue = xQueueReceive(g_event_queue, &p_usb_event,(portMAX_DELAY));
         if(pdTRUE != err_queue)
         {
             handle_error(FSP_ERR_ABORTED, "\r\nError in receiving event through queue\r\n");
         }

         /* process USB events */
         err = process_usb_events();
         handle_error(err, "\r\nprocess_usb_events failed.\r\n");

         vTaskDelay (1);
     }
}

/*******************************************************************************************************************//**
 * @brief     Function processes USB events.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t process_usb_events(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* USB event received */
    switch (p_usb_event->event)
    {
        case USB_STATUS_CONFIGURED: /* Configured State */
        {
            APP_PRINT("USB Configured Successfully\r\n");
            /* Read data from Tera-term */
            err = R_USB_Read (&g_basic_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read API failed.\r\n");
            }
            break;
        }

        case USB_STATUS_WRITE_COMPLETE: /* Write Complete State */
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_WRITE_COMPLETE\r\n");
            if(b_usb_attach)
            {
                /* Read data from Tera-term */
                err = R_USB_Read (&g_basic_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_Read API failed.\r\n");
                }
            }
            else
            {
                /* Do Nothing as USB is removed and not connected yet.*/
            }
            break;
        }

        case USB_STATUS_READ_COMPLETE: /* Read Complete State */
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_READ_COMPLETE\r\n");
            if(b_usb_attach)
            {
                /* Write back the read data from MCU to Tera-term.*/
                err = R_USB_Write (&g_basic_ctrl, g_buf, p_usb_event->data_size, USB_CLASS_PCDC);
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_Write API failed.\r\n");
                }
            }
            else
            {
                /* Do Nothing as USB is removed and not connected yet.*/
            }
            break;
        }

        case USB_STATUS_REQUEST: /* Receive Class Request */
        {
            /* Perform USB status request operation.*/
            err = usb_status_request();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nusb_status_request failed.\r\n");
            }
            break;
        }

        case USB_STATUS_REQUEST_COMPLETE:  /* Request Complete State */
        {
            if(USB_PCDC_SET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST_COMPLETE \nRequest_Type: USB_PCDC_SET_LINE_CODING \n");
            }
            else if (USB_PCDC_GET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
            {
                APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST_COMPLETE \nRequest_Type: USB_PCDC_GET_LINE_CODING \n");
            }
            else
            {
                /* Do Nothing.*/
            }
            break;
        }

        case USB_STATUS_DETACH:
        case USB_STATUS_SUSPEND:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_DETACH & USB_STATUS_SUSPEND\r\n");
            /* Reset the USB attached flag as indicating usb is removed.*/
            b_usb_attach = false;
            memset (g_buf, RESET_VALUE, sizeof(g_buf));
            break;
        }
        case USB_STATUS_RESUME:
        {
            APP_PRINT("\nUSB STATUS : USB_STATUS_RESUME\r\n");
            /* set the USB attached flag*/
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
 * @brief     Function processes USB status request.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t usb_status_request(void)
{
    fsp_err_t err  = FSP_SUCCESS;

    /* Check for the specific CDC class request IDs */
    if (USB_PCDC_SET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST \nRequest_Type: USB_PCDC_SET_LINE_CODING \n");

        /* Get the class request.*/
        err = R_USB_PeriControlDataGet (&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet failed.\r\n");
        }
        else
        {
            APP_PRINT ("\nbitrate = %d\nChar_Format = %d\nParity_Type = %d\ndata_Bit = %d\n",\
                       g_line_coding.dw_dte_rate, g_line_coding.b_char_format, g_line_coding.b_parity_type, g_line_coding.b_data_bits);
        }
    }
    else if (USB_PCDC_GET_LINE_CODING == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST \nRequest_Type: USB_PCDC_GET_LINE_CODING \n");

        /* Set the class request.*/
        err = R_USB_PeriControlDataSet (&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed.\r\n");
        }
        else
        {
            APP_PRINT ("\nbitrate = %d\nChar_Format = %d\nParity_Type = %d\ndata_Bit = %d\n",\
                            g_line_coding.dw_dte_rate, g_line_coding.b_char_format, g_line_coding.b_parity_type, g_line_coding.b_data_bits);
        }
    }
    else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_usb_event->setup.request_type & USB_BREQUEST))
    {
        APP_PRINT("\nUSB STATUS : USB_STATUS_REQUEST \nRequest_Type: USB_PCDC_SET_CONTROL_LINE_STATE \n");
        /* Start the software timer */
        xTimerStart(g_timer, RESET_VALUE);
        /* Set the USB status as ACK response.*/
        err = R_USB_PeriControlStatusSet (&g_basic_ctrl, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed.\r\n");
        }
    }
    else
    {
        /* Do Nothing.*/
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for FreeRTOS+Composite.
 * @param[IN]   usb_event_info_t  *p_event_info
 * @param[IN]   usb_hdl_t         handler
 * @param[IN]   usb_onoff_t       on_off
 * @retval      None.
 ***********************************************************************************************************************/
void usb_composite_callback(usb_event_info_t * p_event_info, usb_hdl_t handler, usb_onoff_t on_off)
{

    FSP_PARAMETER_NOT_USED (handler);
    FSP_PARAMETER_NOT_USED (on_off);

    /* Send event received to queue */
    if (pdTRUE != (xQueueSend(g_event_queue, (const void *)&p_event_info, (TickType_t)(RESET_VALUE))))
    {
        g_err_flag = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Closes the USB module , Print and traps error.
 *  @param[IN]   status    error status
 *  @param[IN]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_USB_Close(&g_basic_ctrl))
        {
            APP_ERR_PRINT ("\r\n** R_USB_Close API Failed ** \r\n ");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for software timer.
 * @param[IN]   TimerHandle_t  xTimer
 * @retval      None.
 ***********************************************************************************************************************/
void g_timer_callback(TimerHandle_t xTimer)
{
    FSP_PARAMETER_NOT_USED (xTimer);
    /* Toggle the state of LED */
    pin_level = !(pin_level);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], pin_level);
}

/*******************************************************************************************************************//**
 * @} (end addtogroup qspi_blockmedia_usb_composite_ep)
 **********************************************************************************************************************/
