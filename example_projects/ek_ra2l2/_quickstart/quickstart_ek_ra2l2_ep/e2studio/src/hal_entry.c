/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : hal_entry.c
 * Version      : 1.00
 * Description  : hal_entry.c of EK-RA2L2 Quick Start Example Project
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"
#include "board_info.h"
#include "common_init.h"
#include "common_utils.h"
#include "r_typedefs.h"
#include "qsep_setting.h"

FSP_CPP_HEADER
void R_BSP_WarmStart (bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/**********************************************************************************************************************
 * Function Name: hal_entry
 * Description  : hal_entry function of EK-RA2L2 Quick Start Example Project
 * Argument     : none
 * Return Value : none
 *********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t                    err;
    usb_event_info_t             event_info;
    usb_status_t                 usb_event;
    uint8_t                      g_buf[USB_EP_PACKET_SIZE] = {'\0'};
    usb_pcdc_linecoding_t        line_coding;


    if (common_init () != FSP_SUCCESS)
    {
        SYSTEM_ERROR;
    }

    /* Open USB instance */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        SYSTEM_ERROR;
    }

    while (true)
    {

        /* Obtain USB related events */
        err = R_USB_EventGet(&event_info, &usb_event);
        if (FSP_SUCCESS != err)
        {
            SYSTEM_ERROR;
        }

        switch (usb_event)
        {
            case USB_STATUS_CONFIGURED:
            {
                err = R_USB_Read(&g_basic0_ctrl, g_buf, USB_EP_PACKET_SIZE, USB_CLASS_PCDC);
                if (FSP_SUCCESS != err)
                {
                    SYSTEM_ERROR;
                }

                break;
            }
            case USB_STATUS_READ_COMPLETE:
            {
                err = R_USB_Read(&g_basic0_ctrl, g_buf, USB_EP_PACKET_SIZE, USB_CLASS_PCDC);
                if (FSP_SUCCESS != err)
                {
                    SYSTEM_ERROR;
                }

                err = update_console(g_buf);
                if (FSP_SUCCESS != err)
                {
                    SYSTEM_ERROR;
                }

                break;
            }
            case USB_STATUS_REQUEST :
            {
                /* Check for the specific CDC class request IDs */
                if (USB_PCDC_SET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
                {
                    /* Conversion from adress value to pointer to 8-bit int */
                    err =  R_USB_PeriControlDataGet(&g_basic0_ctrl, ((uint8_t *)&line_coding), LINE_CODING_LENGTH);
                    if (FSP_SUCCESS != err)
                    {
                        SYSTEM_ERROR;
                    }
                }
                else if (USB_PCDC_GET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
                {
                    /* Conversion from adress value to pointer to 8-bit int */
                    err =  R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t *)&line_coding, LINE_CODING_LENGTH);
                    if (FSP_SUCCESS != err)
                    {
                        SYSTEM_ERROR;
                    }
                }
                else if (USB_PCDC_SET_CONTROL_LINE_STATE == (event_info.setup.request_type & USB_BREQUEST))
                {
                    err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
                    if (FSP_SUCCESS != err)
                    {
                        SYSTEM_ERROR;
                    }

                }
                else
                {
                    /* none */
                }

                break;
            }
            case USB_STATUS_DETACH:
            case USB_STATUS_SUSPEND:
            case USB_STATUS_RESUME:
                break;
            default:
            {
                break;
            }
        }
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}
/**********************************************************************************************************************
 End of function hal_entry
 *********************************************************************************************************************/


/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
