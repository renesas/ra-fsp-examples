/***********************************************************************************************************************
 * File Name    : usb_console_main.c
 * Description  : Entry function.
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "usb_console_main.h"
#include "usb_console.h"
#include "common_init.h"

#include "common_utils.h"
#include "portable.h"

#define USB_CONSOLE       (1)
#define ENABLE_USB_WRITE  (1)
#define USB_QUEUE_SIZE    (20)
#define USB_QUEUE_TIMEOUT (10)


/* Global variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t * gp_apl_string_table[];

const usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
    g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
    g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
    gp_apl_string_table,             /* Pointer to the string descriptor table */
    NUM_STRING_DESCRIPTOR
};

bool g_usb_configured = false;

usb_class_t g_usb_class_type         = 0x00;
usb_onoff_t g_state                  = USB_OFF;
int16_t g_console_connection_timeout = 100;

uint8_t g_buf[READ_BUF_SIZE]     = {0};


/* Private functions */
static fsp_err_t check_for_write_complete (void);

static int8_t           s_response[READ_BUF_SIZE]     = {0};
static usb_event_info_t s_usb_event_list[20]          = {0};

static bool             s_newmsg                      = false;
static uint8_t          s_usb_event_pindex            = 0;
static usb_event_info_t s_apl_event_info;
static usb_setup_t      s_usb_setup;

/**********************************************************************************************************************
 * Function Name: rtos_callback
 * Description  : .
 * Arguments    : p_event_info
 *              : hdl
 *              : state
 * Return Value : .
 *********************************************************************************************************************/
void rtos_callback(usb_event_info_t* p_event_info, usb_hdl_t hdl, usb_onoff_t state)
{
    /* Void unused args */
    FSP_PARAMETER_NOT_USED(hdl);
    BaseType_t xHigherPriorityTaskWoken;

    memcpy(&s_usb_event_list[s_usb_event_pindex], p_event_info, sizeof(usb_event_info_t));

    g_state = state;
    s_newmsg = true;

    if (xQueueIsQueueFullFromISR(g_usb_transaction_queue) == pdFALSE)
    {
        xQueueSendToBackFromISR(g_usb_transaction_queue, &s_usb_event_pindex, &xHigherPriorityTaskWoken);

        /* set next available index, cast to maintain sign integrity */
        s_usb_event_pindex = (uint8_t)((s_usb_event_pindex + 1) % USB_QUEUE_SIZE);
    }
    else
    {
        /* Fatal error - usb message queue is full, this should not occur unless there has been a fatal USB error */
        SYSTEM_ERROR
    }

    /* Switch context if necessary. */
    if (xHigherPriorityTaskWoken)
    {
        taskYIELD ();
    }
}
/**********************************************************************************************************************
 End of function rtos_callback
 *********************************************************************************************************************/


/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used. 
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: usb_console_main
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void usb_console_main(void)
{
    static usb_pcdc_linecoding_t s_line_coding;
    static uint8_t               s_usb_event_cindex = 0;
    fsp_err_t                    err                = FSP_SUCCESS;
    int8_t                       set_console_mode   = 2;

    /* Initialize GPT, ICU, ADC modules */
    if (common_init () != FSP_SUCCESS)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    /* Open USB instance */
    err = R_USB_Open (&g_basic0_ctrl, &g_basic0_cfg);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    g_usb_class_type    = USB_CLASS_PCDC;

    while (true)
    {
        while (true)
        {
            /* Block here till a message is added to the queue by rtos_callback_isr() */
            if (xQueueReceive(g_usb_transaction_queue, &(s_usb_event_cindex), USB_QUEUE_TIMEOUT) == pdPASS)
            {
                memcpy(&s_apl_event_info, &s_usb_event_list[s_usb_event_cindex], sizeof(usb_event_info_t));

                break;
            }
        }

        s_newmsg = false;

        if (true == set_console_mode)
        {
            g_console_connection_timeout = 100;
        }

        switch (s_apl_event_info.event)
        {
            case USB_STATUS_REQUEST_COMPLETE:
            case USB_STATUS_CONFIGURED:
            {
                if (false == set_console_mode)
                {
                    g_console_connection_timeout = -1;
                }

                err = R_USB_Read(&s_apl_event_info, g_buf, READ_BUF_SIZE, g_usb_class_type);

                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Fatal error */
                    SYSTEM_ERROR
                }
                break;
            }

            case USB_STATUS_WRITE_COMPLETE:
            {
                if (false == set_console_mode)
                {
                    g_console_connection_timeout = -1;
                }

                err = R_USB_Read (&s_apl_event_info, g_buf, READ_BUF_SIZE, g_usb_class_type);

                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Fatal error */
                    SYSTEM_ERROR
                }

                EventBits_t uxBits;

                uxBits = xEventGroupGetBits(g_update_console_event);

                if ((uxBits & (STATUS_WRITE_COMPLETE)) != (STATUS_WRITE_COMPLETE))
                {
                    xEventGroupSetBits(g_update_console_event, STATUS_WRITE_COMPLETE);
                }
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                err = R_USB_Read (&s_apl_event_info, g_buf, 1, g_usb_class_type);

                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Fatal error */
                    SYSTEM_ERROR
                }
                memcpy(&s_response , &g_buf, READ_BUF_SIZE);
                memset(&g_buf, 0, READ_BUF_SIZE);
                break;
            }

            case USB_STATUS_REQUEST : /* Receive Class Request */
            {
                if (false == set_console_mode)
                {
                    g_console_connection_timeout = 0;
                }

                R_USB_SetupGet(&s_apl_event_info, &s_usb_setup);

                /* Check for the specific CDC class request IDs */
                if (USB_PCDC_SET_LINE_CODING == (s_usb_setup.request_type & USB_BREQUEST))
                {
                    /* Cast parameter 2 of R_USB_PeriControlDataGet */
                    err =  R_USB_PeriControlDataGet (&s_apl_event_info, (uint8_t *) &s_line_coding, LINE_CODING_LENGTH);

                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Fatal error */
                        SYSTEM_ERROR
                    }
                }
                else if (USB_PCDC_GET_LINE_CODING == (s_usb_setup.request_type & USB_BREQUEST))
                {
                    /* Cast parameter 2 of R_USB_PeriControlDataSet */
                    err =  R_USB_PeriControlDataSet (&s_apl_event_info, (uint8_t *) &s_line_coding, LINE_CODING_LENGTH);

                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Fatal error */
                        SYSTEM_ERROR
                    }
                }
                else if (USB_PCDC_SET_CONTROL_LINE_STATE == (s_usb_setup.request_type & USB_BREQUEST))
                {
                    err = R_USB_PeriControlStatusSet (&s_apl_event_info, USB_SETUP_STATUS_ACK);

                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Fatal error */
                        SYSTEM_ERROR
                    }
                }
                else
                {
                    /* none */
                    ;
                }
                break;
            }

            case USB_STATUS_DETACH:
                g_usb_configured = false;
                break;

            case USB_STATUS_SUSPEND:
                g_usb_configured = false;
                break;

            case USB_STATUS_RESUME:
                break;


            default:
            {
                break;
            }
        }

        if ((USB_STATUS_REQUEST_COMPLETE == s_apl_event_info.event) && (set_console_mode > 0))
        {
            set_console_mode--;
            if (0 == set_console_mode)
            {
                g_console_connection_timeout = -1;
                g_usb_configured = true;
                set_console_mode = false;
            }
        }

    }
}
/**********************************************************************************************************************
 End of function usb_console_main
 *********************************************************************************************************************/


#ifdef USB_CONSOLE
/*****************************************************************************************************************
 *  @brief      Prints the message to console
 *  @param[in]  p_msg contains address of buffer to be printed
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS, Write is unsuccessful
 ****************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: print_to_console
 *********************************************************************************************************************/
fsp_err_t print_to_console(uint8_t * p_data)
{
    /* Cast to enforce read only input to strlen() */
    uint32_t len = strlen((const char *)p_data);
    fsp_err_t err = FSP_SUCCESS;

    if (true == g_usb_configured)
    {
#if   ENABLE_USB_WRITE

        /* Enforce that data as read only */
        err = R_USB_Write (&g_basic0_ctrl, (uint8_t const * const)p_data, len, g_usb_class_type);
#endif

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            return err;
        }

#if   ENABLE_USB_WRITE
        err = check_for_write_complete();
        vTaskDelay(10);

        if (FSP_SUCCESS != err)
        {
            /* Did not get the event hence returning error */
            return FSP_ERR_USB_FAILED;
        }
#endif
    }
    return err;
}
/**********************************************************************************************************************
 End of function print_to_console
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  @brief      Accepts the message from the console
 *  @retval     Next Console character
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: input_from_console
 *********************************************************************************************************************/
int8_t input_from_console(void)
{
    int8_t ret = 0;
    bool retry = true;

    while (retry)
    {
        if (true == g_usb_configured)
        {
            /* Cast to enforce read only input to strlen() */
            if (strlen((const char *)&s_response) > 0)
            {
                uint8_t response_remain[READ_BUF_SIZE]     = {0};
                ret = s_response[0];
                memcpy(&response_remain, &s_response[1], READ_BUF_SIZE-1);
                memcpy(&s_response, &response_remain, READ_BUF_SIZE);

                retry = false;
            }
            else
            {
                vTaskDelay(10);
            }
        }
        else
        {
            retry = false;
        }
    }
    return (ret);
}
/**********************************************************************************************************************
 End of function input_from_console
 *********************************************************************************************************************/

#endif /* USB_CONSOLE */


/**********************************************************************************************************************
 *  @brief      Check for write completion
 *  @param[in]  None
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: check_for_write_complete
 *********************************************************************************************************************/
static fsp_err_t check_for_write_complete(void)
{
    static int32_t s_timeout_count;
    static fsp_err_t s_err;
    static EventBits_t s_uxbits;

    s_timeout_count = UINT16_MAX;
    s_err = FSP_SUCCESS;
    do
    {
        while (( --s_timeout_count) > 0)
        {
            s_uxbits = xEventGroupWaitBits(g_update_console_event,  STATUS_WRITE_COMPLETE, pdTRUE, pdTRUE, 1);

            if ((s_uxbits & (STATUS_WRITE_COMPLETE)) == (STATUS_WRITE_COMPLETE))
            {
                s_apl_event_info.event = USB_STATUS_WRITE_COMPLETE;
                break;
            }
        }
        s_timeout_count = 0;

        s_err = USB_STATUS_NONE;
    } while (USB_STATUS_WRITE_COMPLETE != s_apl_event_info.event);

    return s_err;
}
/**********************************************************************************************************************
 End of function check_for_write_complete
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup hal_entry)
 **********************************************************************************************************************/

