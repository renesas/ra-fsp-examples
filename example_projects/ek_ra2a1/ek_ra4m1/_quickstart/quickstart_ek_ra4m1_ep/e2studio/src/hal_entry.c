/***********************************************************************************************************************
 * File Name    : hal_entry.c
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "hal_entry.h"
#include "common_init.h"

/* Function declaration */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Global variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];
extern int g_curr_led_freq;

const usb_descriptor_t usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 g_apl_string_table,             /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

usb_status_t            usb_event;
usb_setup_t             usb_setup;

/* Banner Info */
char p_welcome[200] = {
                       "\r\n Welcome to Quick Start BLINKY example project for "
                       KIT_NAME_MACRO
                       "!"
                       "\r\n Press 1 for Kit Information or 2 for Next Steps.\r\n"
};

/* Next steps */
char nextsteps[USB_EP_PACKET_SIZE] = {
                       "\r\n 2. NEXT STEPS \r\n"
                       "\r\nVisit the following URLs to learn about the kit "
                       "and the RA family of MCUs, download tools "
                       "and documentation, and get support:\r\n"
                       "\r\n a) "
                       KIT_NAME_MACRO
                       " resources: \t"
                       KIT_LANDING_URL
                       "\r\n b) RA product information:  \t"
                       PRODUCT_INFO_URL
                       "\r\n c) RA product support forum: \t"
                       PRODUCT_SUPPORT_URL
                       "\r\n d) Renesas support: \t\t"
                       RENESAS_SUPPORT_URL
                       "\r\n\r\n Press 1 for Kit Information or 2 for Next Steps.\r\n"
};

char kitinfo[USB_EP_PACKET_SIZE] = {'\0'};

const char *p_mcu_temp = "\r\n d) MCU Die temperature (F/C):  ";
const char *p_led_freq = "\r\n c) Current blinking frequency (Hz): ";
const char *p_kit_menu_ret = "\r\n Press 1 for Kit Information or 2 for Next Steps.\r\n";

uint8_t g_usb_module_number = 0x00;
usb_class_t g_usb_class_type    = 0x00;

/* Private functions */
static fsp_err_t check_for_write_complete(void);
static fsp_err_t print_to_console(char *p_data);
static void process_kit_info(void);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err                           = FSP_SUCCESS;
    uint8_t g_buf[READ_BUF_SIZE]            = {0};
    static usb_pcdc_linecoding_t g_line_coding;

    /* Initialize GPT, ICU, ADC modules */
    if(FSP_SUCCESS != common_init())
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_TRAP(1);
    }

    /* Open USB instance */
    err = R_USB_Open (&g_basic0_ctrl, &g_basic0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_TRAP(err);
    }

    /* Get USB class type */
    err = R_USB_ClassTypeGet (&g_basic0_ctrl, &g_usb_class_type);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_TRAP(1);

    }

    /* Get module number */
    err = R_USB_ModuleNumberGet(&g_basic0_ctrl, &g_usb_module_number);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_TRAP(1);
    }

    while (true)
    {
        /* Obtain USB related events */
        err = R_USB_EventGet (&g_basic0_ctrl, &usb_event);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Turn ON RED LED to indicate fatal error */
            TURN_RED_ON
            APP_ERR_TRAP(err);
        }

        /* USB event received by R_USB_EventGet */
        switch (usb_event)
        {
            case USB_STATUS_CONFIGURED:
            {
                err = R_USB_Read (&g_basic0_ctrl, g_buf, READ_BUF_SIZE, (uint8_t)g_usb_class_type);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Turn ON RED LED to indicate fatal error */
                    TURN_RED_ON
                    APP_ERR_TRAP(err);
                }
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                err = R_USB_Read (&g_basic0_ctrl, g_buf, 1, (uint8_t)g_usb_class_type);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Turn ON RED LED to indicate fatal error */
                    TURN_RED_ON
                    APP_ERR_TRAP(err);
                }

                /* Switch case evaluation of user input */
                switch (g_buf[0])
                {
                    case KIT_INFO:
                    {
                        process_kit_info();
                        break;
                    }
                    case NEXT_STEPS:
                    {
                        err = print_to_console(nextsteps);
                        if (FSP_SUCCESS != err)
                        {
                            /* Turn ON RED LED to indicate fatal error */
                            TURN_RED_ON
                            APP_ERR_TRAP(err);
                        }
                        break;
                    }

                    case CARRIAGE_RETURN:
                    {
                        /* Print banner info to console */
                        err = print_to_console(p_welcome);
                        if (FSP_SUCCESS != err)
                        {
                            /* Turn ON RED LED to indicate fatal error */
                            TURN_RED_ON
                            APP_ERR_TRAP(err);
                        }
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
                break;
            }

            case USB_STATUS_REQUEST : /* Receive Class Request */
            {
                R_USB_SetupGet(&g_basic0_ctrl, &usb_setup);

                /* Check for the specific CDC class request IDs */
                if (USB_PCDC_SET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err =  R_USB_PeriControlDataGet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH );
                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Turn ON RED LED to indicate fatal error */
                        TURN_RED_ON
                        APP_ERR_TRAP(err);
                    }
                }
                else if (USB_PCDC_GET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err =  R_USB_PeriControlDataSet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH );
                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Turn ON RED LED to indicate fatal error */
                        TURN_RED_ON
                        APP_ERR_TRAP(err);
                    }
                }
                else if (USB_PCDC_SET_CONTROL_LINE_STATE == (usb_setup.request_type & USB_BREQUEST))
                {
                    err = R_USB_PeriControlStatusSet (&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        /* Turn ON RED LED to indicate fatal error */
                        TURN_RED_ON
                        APP_ERR_TRAP(err);
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
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */
        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*****************************************************************************************************************
 *  @brief      Prints the message to console
 *  @param[in]  p_msg contains address of buffer to be printed
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS, Write is unsuccessful
 ****************************************************************************************************************/
static fsp_err_t print_to_console(char *p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t len = ((uint32_t)strlen(p_data));

    err = R_USB_Write (&g_basic0_ctrl, (uint8_t*)p_data, len, (uint8_t)g_usb_class_type);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = check_for_write_complete();
    if (FSP_SUCCESS != err)
    {
        /* Did not get the event hence returning error */
        return FSP_ERR_USB_FAILED;
    }
    return err;
}

/*****************************************************************************************************************
 *  @brief      Check for write completion
 *  @param[in]  None
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS
 ****************************************************************************************************************/
static fsp_err_t check_for_write_complete(void)
{
    usb_status_t usb_write_event = USB_STATUS_NONE;
    int32_t timeout_count = UINT16_MAX;
    fsp_err_t err = FSP_SUCCESS;

    do
    {
        err = R_USB_EventGet (&g_basic0_ctrl, &usb_write_event);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        --timeout_count;

        if (0 > timeout_count)
        {
            timeout_count = 0;
            err = (fsp_err_t)USB_STATUS_NONE;
            break;
        }
    }while(USB_STATUS_WRITE_COMPLETE != usb_write_event);

    return err;
}

/*****************************************************************************************************************
 *  @brief      Process kit information
 *  @param[in]  None
 *  @retval     None
 ****************************************************************************************************************/
static void process_kit_info(void)
{
    uint16_t buffer_index_count = 0x0000;
    uint16_t adc_data                    = 0;
    float mcu_temp_f                     = 0;
    float mcu_temp_c                     = 0;
    fsp_err_t err = FSP_SUCCESS;

    /* Read die temperature */
    err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        print_to_console ("** R_ADC_Read API failed ** \r\n");
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_TRAP(err);
    }

    /* Conversion ADC temperature in Fahrenheit */
    mcu_temp_f = ADCTEMP_AS_F(adc_data);
    /* Conversion ADC temperature in celsius */
    mcu_temp_c = ADCTEMP_AS_C(adc_data);

    /* clear kit info buffer before updating data */
    memset(kitinfo, '\0', 511);

    /* update  predefined text in the buffer */
    memcpy(kitinfo, (char *)KIT_INFO_PRIMARY_TEXT, strlen((char *)KIT_INFO_PRIMARY_TEXT) );

    /* calculate current data filled length */
    buffer_index_count = ((uint16_t)(strlen(kitinfo)));

    /* Check for current led frequency */
    if (BLINK_FREQ_1HZ == g_curr_led_freq)
    {
        sprintf((char*)&kitinfo[buffer_index_count],"%s\t%d",p_led_freq,1);
    }
    else if (BLINK_FREQ_5HZ == g_curr_led_freq)
    {
        sprintf((char*)&kitinfo[buffer_index_count],"%s\t%d",p_led_freq,5);
    }
    else if (BLINK_FREQ_10HZ == g_curr_led_freq)
    {
        sprintf((char*)&kitinfo[buffer_index_count],"%s\t%d",p_led_freq,10);
    }
    else
    {
        /* Do Nothing */
    }

    buffer_index_count = 0U;

    /* kit_processing_data is filled with led frequency details */
    buffer_index_count =  ((uint16_t)(strlen(kitinfo))) ;

    /* appends the data from current buffer_index_count */
    sprintf((char *)&kitinfo[buffer_index_count],
            "%s\t%.02f/%.02f",p_mcu_temp,mcu_temp_f,mcu_temp_c);

    buffer_index_count  = 0U;

    /* update index count */
    buffer_index_count = ((uint16_t) (strlen(kitinfo)));

    /* update index count */
    sprintf((char*)&kitinfo[buffer_index_count],"\r\n%s",p_kit_menu_ret);

    /* Print kit menu to console */
    err = print_to_console(kitinfo);
    /* Handle error*/
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_TRAP(err);
    }

}
/*******************************************************************************************************************//**
 * @} (end addtogroup hal_entry)
 **********************************************************************************************************************/

