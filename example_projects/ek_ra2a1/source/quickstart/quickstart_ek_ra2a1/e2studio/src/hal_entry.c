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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include <stdio.h>
#include "hal_entry.h"
#include "common_init.h"

/* Function declaration */
void R_BSP_WarmStart(bsp_warm_start_event_t event);
extern uint16_t adc_reading(void);

/* Global variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];
extern int g_curr_led_freq;
extern usb_cfg_t g_basic0_cfg;

static const usb_descriptor_t usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 g_apl_string_table,             /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

usb_instance_ctrl_t     usb_ctrl;
usb_instance_transfer_t usb_trans;
usb_status_t            usb_evt;
usb_setup_t             usb_setup;

volatile usb_pcdc_ctrllinestate_t g_control_line_state = {
    .bdtr = 0,
    .brts = 0,
};

/* Banner Info */
char *p_welcome[WELCOME_BANNER_MAX] = {
                                             "\r\n Welcome to Quick Start BLINKY example project for ",
                                             KIT_NAME_MACRO,
                                             "!",
                                             "\r\n Press 1 for Kit Information or 2 for Next Steps.\r\n"
};


/* Kit Info Sub Menu Options */
char kitinfo[KIT_INFO_MAX][USB_EP_PACKET_SIZE] = {
                                       "\r\n 1. KIT INFORMATION \r\n\r\n a) Kit name: \t\t\t\t",
                                       KIT_NAME_MACRO,
                                       "\r\n b) Kit ordering part number: \t\t",
                                       KIT_PARTNUM_MACRO,
};

/* Next steps */
char *p_nextsteps[NEXT_STEPS_MAX] = {
                                           "\r\n 2. NEXT STEPS \r\n",
                                           "\r\nVisit the following URLs to learn about the kit ",
                                           "and the RA family of MCUs, download tools ",
                                           "and documentation, and get support:\r\n",
                                           "\r\n a) ",
                                           KIT_NAME_MACRO,
                                           " resources: \t",
                                           KIT_LANDING_URL,
                                           "\r\n b) RA product information:  \t",
                                           PRODUCT_INFO_URL,
                                           "\r\n c) RA product support forum: \t",
                                           PRODUCT_SUPPORT_URL,
                                           "\r\n d) Renesas support: \t\t",
                                           RENESAS_SUPPORT_URL,
                                           "\r\n\r\n Press 1 for Kit Information or 2 for Next Steps.\r\n"
};


const char *p_mcu_temp = "\r\n c) MCU Die temperature (F/C): ";
const char *p_led_freq = "\r\n d) Current blinking frequency (Hz): ";
const char *p_kit_menu_ret = "\r\n Press 1 for Kit Information or 2 for Next Steps.\r\n";

 /*****************************************************************************************************************
 *  @brief      Prints the message to console
 *  @param[in]  p_msg contains address of buffer to be printed
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS, Write is unsuccessful
 ****************************************************************************************************************/
static fsp_err_t print_to_console(char *p_msg)
{
    fsp_err_t err = FSP_SUCCESS;
    unsigned int cnt = 0, num = 0;
    char str[64] = {0};

    usb_trans.module_number = USB_CFG_USE_USBIP;
    usb_trans.type = USB_CLASS_PCDC;

    /* USB PCDC End point size is set to 128bytes. If we
     * see an input string > 128bytes, we chunk it up and
     * print it.
     */
    do
    {
        cnt = MIN (strlen(p_msg) - num, sizeof(str) - 8);
        memcpy (str, &p_msg[num], cnt);
        str[cnt] = '\0';

        /* Write to the serial port */
        err = R_USB_Write (&usb_ctrl, (uint8_t*)str, (uint32_t)(USB_EP_PACKET_SIZE - 8), &usb_trans);

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        num += cnt;

    }while (num < strlen(p_msg));

    return err;
}

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    uint16_t adc_data                            = 0;
    float mcu_temp_f                             = 0;
    float mcu_temp_c                             = 0;
    fsp_err_t err                                = FSP_SUCCESS;
    uint8_t g_buf[USB_EP_PACKET_SIZE]            = {0};
    volatile int8_t g_write_complete             = 0;
    static int menu_cnt                          = 0;
    static volatile uint32_t kit_menu_pending    = 0;
    static volatile uint32_t ns_menu_pending     = 0;
    static volatile uint32_t banner_info_pending = 0;
    static usb_pcdc_linecoding_t g_line_coding;

    /* Initialize GPT, IO port, ADC modules */
    if(FSP_SUCCESS != common_init())
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON

        APP_ERR_TRAP(1);
    }

    /* Initialize USB */
    g_basic0_cfg.p_usb_reg = (usb_descriptor_t  *)&usb_descriptor;

    /* Open USB instance */
    usb_trans.module_number = USB_CFG_USE_USBIP;
    usb_trans.type = USB_CLASS_PCDC;
    err = R_USB_Open (&usb_ctrl, &g_basic0_cfg, &usb_trans);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON

        APP_ERR_TRAP(err);
    }

    while (true)
    {
        /* Obtain USB related events */
        err = R_USB_EventGet (&usb_ctrl, &usb_evt);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Turn ON RED LED to indicate fatal error */
            TURN_RED_ON

            APP_ERR_TRAP(err);
        }
        /* USB event received by R_USB_EventGet */
        switch (usb_evt)
        {
            case USB_STATUS_CONFIGURED:
            {
                usb_trans.type = USB_CLASS_PCDC;
                /* USB data read */
                err = R_USB_Read (&usb_ctrl, g_buf, USB_EP_PACKET_SIZE, &usb_trans);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* Turn ON RED LED to indicate fatal error */
                    TURN_RED_ON

                    APP_ERR_TRAP(err);
                }
                break;
            }

            case USB_STATUS_WRITE_COMPLETE:
            {
                /* Set a flag on write complete */
                g_write_complete = 1;
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                usb_trans.type = USB_CLASS_PCDC;
                /* Data read through serial terminal  */
                err = R_USB_Read (&usb_ctrl, g_buf, 1, &usb_trans);
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
                        menu_cnt = 0;
                        /* Read die temperature */
                        adc_data = adc_reading();
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

                        /* Check for current led frequency */
                        if (BLINK_FREQ_1HZ == g_curr_led_freq)
                        {
                            sprintf((char*)kitinfo[5],"%s\t%d",p_led_freq,1);
                        }
                        else if (BLINK_FREQ_5HZ == g_curr_led_freq)
                        {
                            sprintf((char*)kitinfo[5],"%s\t%d",p_led_freq,5);
                        }
                        else if (BLINK_FREQ_10HZ == g_curr_led_freq)
                        {
                            sprintf((char*)kitinfo[5],"%s\t%d",p_led_freq,10);
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        sprintf((char*)kitinfo[4],"%s\t\t%.02f/%.02f",p_mcu_temp,mcu_temp_f,mcu_temp_c);


                        sprintf((char*)kitinfo[6],"\r\n%s",p_kit_menu_ret);

                        /* Print kit menu to console */
                        err = print_to_console(kitinfo[0]);
                        /* Handle error*/
                        if (FSP_SUCCESS != err)
                        {
                            /* Turn ON RED LED to indicate fatal error */
                            TURN_RED_ON

                            APP_ERR_TRAP(err);
                        }

                        kit_menu_pending = 1;

                        break;
                    }

                    case NEXT_STEPS:
                    {
                        menu_cnt = 0;
                        /* Prints next step menu to console */
                        err = print_to_console(p_nextsteps[menu_cnt]);
                        if (FSP_SUCCESS != err)
                        {
                            /* Turn ON RED LED to indicate fatal error */
                            TURN_RED_ON

                            APP_ERR_TRAP(err);
                        }

                        ns_menu_pending = 1;
                        break;
                    }

                    case CARRIAGE_RETURN:
                    {
                        /* Print banner info to console */
                        err = print_to_console(p_welcome[0]);
                        if (FSP_SUCCESS != err)
                        {
                            /* Turn ON RED LED to indicate fatal error */
                            TURN_RED_ON

                            APP_ERR_TRAP(err);
                        }

                        banner_info_pending = 1;

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
                R_USB_SetupGet(&usb_ctrl, &usb_setup);
                usb_trans.type = USB_CLASS_REQUEST;
                usb_trans.setup = usb_setup;

                /* Check for the specific CDC class request IDs */
                if (USB_PCDC_SET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err = R_USB_Read (&usb_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH, &usb_trans);
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
                    err = R_USB_Write (&usb_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH, &usb_trans);
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
                    err = R_USB_Read(&usb_ctrl, (uint8_t *) &g_control_line_state, sizeof(g_control_line_state), &usb_trans);
                    if (FSP_SUCCESS == err)
                    {
                        g_control_line_state.bdtr = (unsigned char)((usb_setup.request_value >> 0) & 0x01);
                        g_control_line_state.brts = (unsigned char)((usb_setup.request_value >> 1) & 0x01);
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

        /* Check for write complete and pending banner info */
        if (g_write_complete && banner_info_pending)
        {
            /* Reset the flag */
            g_write_complete = 0;
            /* Check for menu count to be less than maximum defined banner */
            if (menu_cnt < (WELCOME_BANNER_MAX - 1))
            {
                err = print_to_console(p_welcome[++menu_cnt]);
                if (FSP_SUCCESS != err)
                {
                    /* Turn ON RED LED to indicate fatal error */
                    TURN_RED_ON

                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                /* Resets the flags */
                banner_info_pending = 0;
                menu_cnt = 0;
            }
        }

        /* Check for write complete and pending kit menu */
        if (g_write_complete && kit_menu_pending)
        {
            g_write_complete = 0;
            /* Check for menu count to be less than maximum defined kit_menu */
            if (menu_cnt < (KIT_INFO_MAX - 1))
            {
                /* Print kit menu accordingly*/
                err = print_to_console(kitinfo[++menu_cnt]);
                if (FSP_SUCCESS != err)
                {
                    /* Turn ON RED LED to indicate fatal error */
                    TURN_RED_ON

                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                /* Resets the flag */
                kit_menu_pending = 0;
                menu_cnt = 0;
            }
        }

        /* Check for write complete and pending next step menu */
        if (g_write_complete && ns_menu_pending)
        {
            g_write_complete = 0;
            /* Check for the menu count to be less than maximum defined ns_menu */
            if (menu_cnt < (NEXT_STEPS_MAX - 1))
            {
                /* Print next step menu accordingly */
                err = print_to_console(p_nextsteps[++menu_cnt]);
                if (FSP_SUCCESS != err)
                {
                    /* Turn ON RED LED to indicate fatal error */
                    TURN_RED_ON

                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                /* Resets the flag */
                ns_menu_pending = 0;
                menu_cnt = 0;
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


/*******************************************************************************************************************//**
 * @} (end addtogroup hal_entry)
 **********************************************************************************************************************/

