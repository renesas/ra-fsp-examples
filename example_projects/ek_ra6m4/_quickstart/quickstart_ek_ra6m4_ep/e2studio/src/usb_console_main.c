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
#include "board_cfg.h"

#include "common_utils.h"
#include "portable.h"

/* Function declaration */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Global variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t * g_apl_string_table[];

/* Private functions */
static fsp_err_t check_for_write_complete (void);

const usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
    g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
    g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
    g_apl_string_table,             /* Pointer to the string descriptor table */
    NUM_STRING_DESCRIPTOR
};

extern fsp_err_t print_to_console(char * p_data);
extern int input_from_console(void);

usb_status_t            usb_event;
usb_setup_t             usb_setup;
static uint8_t          s_response[READ_BUF_SIZE]     = {0};
bool                    b_usb_configured = false;

char kitinfo[USB_EP_PACKET_SIZE] = {'\0'};

uint8_t g_usb_module_number = 0x00;
usb_class_t g_usb_class_type    = 0x00;

usb_event_info_t s_usb_event_info;
usb_onoff_t g_state = USB_OFF;
bool s_newmsg = false;

int    g_console_connection_timeout = 100;

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
    FSP_PARAMETER_NOT_USED(hdl);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBits;

    memcpy(&s_usb_event_info, p_event_info, sizeof(usb_event_info_t));
    g_state = state;
    s_newmsg = true;

    uxBits = xEventGroupGetBitsFromISR(g_update_console_event);

    if ((uxBits & (STATUS_USB_EVENT)) != (STATUS_USB_EVENT))
    {
        xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_USB_EVENT,
                                            &xHigherPriorityTaskWoken);

        /* Was the message posted successfully? */
        if (pdFAIL != xResult)
        {
            /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
            switch should be requested.  The macro used is port specific and will
            be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
            the documentation page for the port being used. */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
/**********************************************************************************************************************
 End of function rtos_callback
 *********************************************************************************************************************/


uint8_t g_buf[READ_BUF_SIZE]     = {0};

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
    static usb_pcdc_linecoding_t g_line_coding;
    static fsp_err_t err = FSP_SUCCESS;
    int    set_console_mode = 2;

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

    /* Get USB class type */
    err = R_USB_ClassTypeGet (&g_basic0_ctrl, &g_usb_class_type);
    if (FSP_SUCCESS != err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    /* Get module number */
    err = R_USB_ModuleNumberGet(&g_basic0_ctrl, &g_usb_module_number);
    if (FSP_SUCCESS != err)
    {
        /* Fatal error */
        SYSTEM_ERROR
    }

    while (true)
    {
        EventBits_t uxBits;

        while (true)
        {
            uxBits = xEventGroupWaitBits(g_update_console_event,  STATUS_USB_EVENT, pdTRUE, pdTRUE, 1);

            if ((uxBits & (STATUS_USB_EVENT)) == (STATUS_USB_EVENT))
            {
                break;
            }
        }
        s_newmsg = false;

        memcpy(&g_basic0_ctrl, &s_usb_event_info, sizeof(usb_event_info_t));

        if (true == set_console_mode)
        {
            g_console_connection_timeout = 100;
        }

        switch (g_basic0_ctrl.event)
        {
            case USB_STATUS_REQUEST_COMPLETE:
            case USB_STATUS_CONFIGURED:
            {
                // TURN_GREEN_OFF
                if(set_console_mode == false)
                {
                    g_console_connection_timeout = -1;
                }

                err = R_USB_Read (&g_basic0_ctrl, g_buf, READ_BUF_SIZE, (uint8_t)g_usb_class_type);

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
                // TURN_GREEN_OFF
                if(set_console_mode == false)
                {
                    g_console_connection_timeout = -1;
                }

                err = R_USB_Read (&g_basic0_ctrl, g_buf, READ_BUF_SIZE, (uint8_t)g_usb_class_type);

                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Fatal error */
                    SYSTEM_ERROR
                }

                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                BaseType_t xResult = pdFAIL;

                uxBits = xEventGroupGetBitsFromISR(g_update_console_event);

                if ( ( uxBits & ( STATUS_WRITE_COMPLETE ) ) != ( STATUS_WRITE_COMPLETE ) )
                {
                    xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_WRITE_COMPLETE,
                                                        &xHigherPriorityTaskWoken);

                    /* Was the message posted successfully? */
                    if(xResult != pdFAIL)
                    {
                        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
                        switch should be requested.  The macro used is port specific and will
                        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
                        the documentation page for the port being used. */
                        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                    }
                }
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                err = R_USB_Read (&g_basic0_ctrl, g_buf, 1, (uint8_t)g_usb_class_type);

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

                R_USB_SetupGet(&g_basic0_ctrl, &usb_setup);

                /* Check for the specific CDC class request IDs */
                if (USB_PCDC_SET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err =  R_USB_PeriControlDataGet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);

                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Fatal error */
                        SYSTEM_ERROR
                    }
                }
                else if (USB_PCDC_GET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err =  R_USB_PeriControlDataSet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);

                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Fatal error */
                        SYSTEM_ERROR
                    }
                }
                else if (USB_PCDC_SET_CONTROL_LINE_STATE == (usb_setup.request_type & USB_BREQUEST))
                {
                    err = R_USB_PeriControlStatusSet (&g_basic0_ctrl, USB_SETUP_STATUS_ACK);

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
                b_usb_configured = false;
                break;

            case USB_STATUS_SUSPEND:
                b_usb_configured = false;
                break;

            case USB_STATUS_RESUME:
                break;


            default:
            {
                break;
            }
        }

        if ((USB_STATUS_REQUEST_COMPLETE == g_basic0_ctrl.event) && (set_console_mode > 0))
        {
            set_console_mode--;
            if (0 == set_console_mode)
            {
                g_console_connection_timeout = -1;
                b_usb_configured = true;
                set_console_mode = false;
            }
        }

    }
}
/**********************************************************************************************************************
 End of function usb_console_main
 *********************************************************************************************************************/

#define ENABLE_USB_WRITE (1)

/*****************************************************************************************************************
 *  @brief      Prints the message to console
 *  @param[in]  p_msg contains address of buffer to be printed
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS, Write is unsuccessful
 ****************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: print_to_console
 *********************************************************************************************************************/
fsp_err_t print_to_console(char *p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t len = ((uint32_t)strlen(p_data));

    if (true == b_usb_configured)
    {
#if   ENABLE_USB_WRITE
        err = R_USB_Write (&g_basic0_ctrl, (uint8_t*)p_data, len, (uint8_t)g_usb_class_type);
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
 *  @param[in]  p_msg contains address of buffer to be filled
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS, Write is unsuccessful
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: input_from_console
 *********************************************************************************************************************/
int input_from_console(void)
{
    int ret = 0;
    bool retry = true;

    while (retry)
    {
        if (true == b_usb_configured)
        {
            if (((uint32_t)strlen((const char *)s_response)) != 0)
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
    static int32_t timeout_count;
    static fsp_err_t err;
    static EventBits_t uxBits;

    timeout_count = UINT16_MAX;
    err = FSP_SUCCESS;
    do
    {
        while (( --timeout_count) > 0)
        {
            uxBits = xEventGroupWaitBits(g_update_console_event,  STATUS_WRITE_COMPLETE, pdTRUE, pdTRUE, 1);

            if ( ( uxBits & ( STATUS_WRITE_COMPLETE ) ) == ( STATUS_WRITE_COMPLETE ) )
            {
                break;
            }
        }
        timeout_count = 0;

        err = (fsp_err_t)USB_STATUS_NONE;

        break;
    } while (USB_STATUS_WRITE_COMPLETE != g_basic0_ctrl.event);

    return err;
}
/**********************************************************************************************************************
 End of function check_for_write_complete
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end addtogroup hal_entry)
 **********************************************************************************************************************/

