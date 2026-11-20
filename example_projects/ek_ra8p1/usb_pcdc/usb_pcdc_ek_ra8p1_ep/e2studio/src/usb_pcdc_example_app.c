/***********************************************************************************************************************
 * File Name    : usb_pcdc_example_app.c
 * Description  : Example application layer for the USB PCDC EP.
 *                - All printing via the USB PCDC (the terminal is the COM port of the USB Serial Device).
 *                - hal_entry.c calls a single function: usb_pcdc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "common_init.h"
#include "usb_pcdc_ep.h"
#include "usb_pcdc_example_app.h"

/* Private variable */
static char s_kit_info[USB_EP_PACKET_SIZE] = {'\0'};

/* Private functions */
static void handle_error(fsp_err_t err);
static void handle_usb_command(uint8_t user_input);
static void process_kit_info(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PCDC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_pcdc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    usb_event_info_t event_info = {0};
    usb_status_t usb_event = (usb_status_t)RESET_VALUE;

    /* Initialize the GPT, ICU, and ADC modules */
    err = common_init();
    handle_error(err);

    /* Open the USB instance */
    err = usb_init();
    handle_error(err);

    while (true)
    {
        /* Obtain the USB related events */
        err = usb_event_get(&event_info, &usb_event);
        handle_error(err);

        /* USB event received by usb_event_get */
        switch (usb_event)
        {
            case USB_STATUS_CONFIGURED:
            {
                err = usb_read();
                handle_error(err);

                g_b_usb_write_complete = true;
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                if (g_b_usb_attach)
                {
                    err = usb_read();
                    handle_error(err);
                }
                break;
            }

            /* Receive the class request */
            case USB_STATUS_REQUEST:
            {
                err = usb_class_request(&event_info);
                handle_error(err);
                break;
            }

            case USB_STATUS_WRITE_COMPLETE:
            {
                /* The USB is ready for the new write operations */
                g_b_usb_write_complete = true;
                break;
            }

            case USB_STATUS_DETACH:
            case USB_STATUS_SUSPEND:
            {
                g_b_usb_attach = false;
                memset(g_read_buf, RESET_VALUE, sizeof(g_read_buf));
                break;
            }

            case USB_STATUS_RESUME:
            {
                g_b_usb_attach = true;
                break;
            }

            default:
            {
                /* Do nothing */
                break;
            }
        }

        /* Check if the USB is ready for writing */
        if (g_b_usb_write_complete)
        {
            /* Handle the user input */
            handle_usb_command(g_read_buf[0]);
            memset(g_read_buf, RESET_VALUE, sizeof(g_read_buf));
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by turning on the red LED and entering a trap state.
 * @param[in] err                       Error code.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err)
{
    if (FSP_SUCCESS != err)
    {
        /* Turn on the red LED to indicate a fatal error */
        TURN_RED_ON;
        __BKPT(0);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs the action of the command entered from the terminal.
 * @param[in] user_input                The input received from the user.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_usb_command(uint8_t user_input)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Switch case evaluation of the user input */
    switch (user_input)
    {
        case KIT_INFO:
        {
            process_kit_info();
            break;
        }

        case NEXT_STEPS:
        {
            err = usb_write(NEXT_STEPS_TEXT);
            handle_error(err);
            break;
        }

        case CARRIAGE_RETURN:
        {
            /* Print the welcome text to the terminal */
            err = usb_write(WELCOME_TEXT);
            handle_error(err);
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function prints the kit information to the terminal.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_kit_info(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t adc_data = RESET_VALUE;
    float mcu_temp_f = 0.0f;
    float mcu_temp_c = 0.0f;
    size_t index = 0;

    /* Read the die temperature */
    err = adc_temperature_get(&adc_data);
    handle_error(err);

    /* Convert the ADC temperature to Fahrenheit */
    mcu_temp_f = ADCTEMP_AS_F(adc_data);

    /* Convert the ADC temperature to Celsius */
    mcu_temp_c = ADCTEMP_AS_C(adc_data);

    /* Clear the kit info buffer before updating the data */
    memset(s_kit_info, '\0', sizeof(s_kit_info));

    /* Update the predefined text in the buffer */
    memcpy(s_kit_info, KIT_INFO_PRIMARY_TEXT, sizeof(KIT_INFO_PRIMARY_TEXT));
    index = strlen(s_kit_info);

    /* Check for the current LED blinking frequency */
    if (BLINK_FREQ_1HZ == g_curr_led_freq)
    {
        snprintf(&s_kit_info[index], sizeof(s_kit_info) - index, LED_FREQ_TEXT "\t%d", 1);
    }
    else if (BLINK_FREQ_5HZ == g_curr_led_freq)
    {
        snprintf(&s_kit_info[index], sizeof(s_kit_info) - index, LED_FREQ_TEXT "\t%d", 5);
    }
    else if (BLINK_FREQ_10HZ == g_curr_led_freq)
    {
        snprintf(&s_kit_info[index], sizeof(s_kit_info) - index, LED_FREQ_TEXT "\t%d", 10);
    }
    else
    {
        /* Do nothing */
    }

    /* Append the MCU die temperature to the buffer */
    index = strlen(s_kit_info);
    snprintf(&s_kit_info[index], sizeof(s_kit_info) - index, MCU_TEMP_TEXT "\t\t%.02f/%.02f", mcu_temp_f,
             mcu_temp_c);

    /* Append the menu text to the buffer */
    index = strlen(s_kit_info);
    snprintf(&s_kit_info[index], sizeof(s_kit_info) - index, "\r\n" MENU_TEXT);

    /* Print the kit information to the terminal */
    err = usb_write(s_kit_info);
    handle_error(err);
}
