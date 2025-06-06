/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_utils.c
 * Version      : 1.00
 * Description  : common utility functions for EK-RA2L2 Quick Start Example Project
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include "board_info.h"
#include "hal_data.h"
#include "r_typedefs.h"
#include "qsep_setting.h"
#include "strings.h"
#include "common_utils.h"

/******************************************************************************
Imported global variables
******************************************************************************/
extern uint8_t   g_apl_device[];
extern uint8_t   g_apl_configuration[];
extern uint8_t   g_apl_hs_configuration[];
extern uint8_t   g_apl_qualifier_descriptor[];
extern uint8_t * g_apl_string_table[]; // @suppress("9.1l Global (API or Non-API) pointer variable prefix")

extern st_board_status_t g_board_status;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,                   /* Pointer to the device descriptor */
    g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
    g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
    g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
    g_apl_string_table,             /* Pointer to the string descriptor table */
    NUM_STRING_DESCRIPTOR
};

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: convert_temperature_to_char
 * Description  : Function to convert the int temperature data to char and store it in a array
 * Argument     : char *str, int32_t temp
 * Return Value : offset index
 *********************************************************************************************************************/
static uint8_t convert_temperature_to_char(char_t *str, int32_t temp)
{
    uint8_t offset = 0;

    if (0 > temp)
    {
        temp  *= (-1);
        str[5] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[4] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[2] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[1] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/

        if ('0' != str[1])
        {
            str[0] = '-';
        }
        else
        {
            offset++;
            str[1] = '-';
        }
    }

    else
    {
        str[5] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[4] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[2] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[1] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/
        temp  /= 10;
        str[0] = (char)((temp % 10) + '0');     /*Conversion from 32-bit int to 8-bit char*/

        if ('0' == str[0])
        {
            offset++;
        }

        if (('0' == str[0]) && ('0' == str[1]))
        {
            offset++;
        }
    }

    return offset;
}
/**********************************************************************************************************************
 End of function convert_temperature_to_char
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_temperature
 * Description  : Function to convert the read ADC value to temperature and store it in a char array
 * Argument     : char
 * Return Value : Pointer to temperature data storage destination and offset index
 *********************************************************************************************************************/
static fsp_err_t get_temperature(char_t *str_f, char_t *str_c, uint8_t *offset)
{
    int32_t             temp_f;
    int32_t             temp_c;
    uint16_t            adc_data = 0;
    fsp_err_t           err      = FSP_SUCCESS;
    adc_info_t          adc_info = {};
    float32_t           v1;
    float32_t           vs;

    R_ADC_InfoGet(&g_adc_ctrl, &adc_info);
    if (0xFFFF == adc_info.calibration_data)
    {
        return FSP_ERR_UNSUPPORTED;
    }

    err = R_ADC_Read(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Conversion from 32-bit unsigned int to 32-bit float */
    v1 = (PWR_V) * ((float32_t)adc_info.calibration_data\
            / powf(2.0, ADC_PRECISION));

    /* Conversion from 32-bit int to 32-bit float */
    vs = PWR_V * ((float32_t)adc_data / powf(2.0, ADC_PRECISION));

    /* Conversion from 32-bit float to 32-bit int */
    temp_c = (int32_t)((((vs - v1) / ((float32_t)adc_info.slope_microvolts\
            / 1000000.0)) + T_J) * 100);

    /* Conversion from 32-bit float to 32-bit int */
    temp_f = (int32_t)((((((vs - v1) / ((float32_t)adc_info.slope_microvolts / 1000000.0)) + T_J) * 1.8) + 32) * 100);

    offset[FAHRENHEIT] = convert_temperature_to_char(str_f, temp_f);
    offset[CELSIUS]    = convert_temperature_to_char(str_c, temp_c);

    return err;
}
/**********************************************************************************************************************
 End of function get_temperature
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_for_write_complete
 * Description  : Function to check if data transmission is complete
 * Argument     : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t check_for_write_complete(void)
{
    usb_status_t          usb_write_event = USB_STATUS_NONE;
    fsp_err_t             err             = FSP_SUCCESS;
    int32_t               timeout_count   = UINT16_MAX;
    usb_event_info_t      event_info      = {0};
    usb_pcdc_linecoding_t g_line_coding;

    do
    {
        err = R_USB_EventGet(&event_info, &usb_write_event);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (USB_STATUS_REQUEST == usb_write_event)
        {
            /* Check for the specific CDC class request IDs */
            if (USB_PCDC_SET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
            {
                /* Conversion from adress value to pointer to 8-bit int */
                err =  R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
            }
            else if (USB_PCDC_GET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
            {
                /* Conversion from adress value to pointer to 8-bit int */
                err =  R_USB_PeriControlDataSet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (event_info.setup.request_type & USB_BREQUEST))
            {
                err = R_USB_PeriControlStatusSet (&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
            }
            else
            {
                /*Do nothing*/
            }
        }
        else
        {
            /*Do nothing*/
        }

        --timeout_count;

        if (0 > timeout_count)
        {
            timeout_count = 0;
            err           = (fsp_err_t)USB_STATUS_NONE;     /* Conversin from enum e_usb status to fsp_err_t*/
            break;
        }
    }while (USB_STATUS_WRITE_COMPLETE != usb_write_event);

    return err;
}
/**********************************************************************************************************************
 End of function check_for_write_complete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_to_console
 * Description  : Function to output strings to PC terminal
 * Argument     : *p_data (string to output)
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t print_to_console(char *p_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t  len = ((uint32_t)strlen(p_data));  /*Cast size_t to uint32_t*/

    err = R_USB_Write (&g_basic0_ctrl, (uint8_t*)p_data, len, USB_CLASS_PCDC); /*Cast char pointer to uint8_t pointer */

    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = check_for_write_complete();
    if (FSP_SUCCESS != err)
    {
        return FSP_ERR_USB_FAILED;
    }
    return err;
}
/**********************************************************************************************************************
 End of function print_to_console
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: clear_screen
 * Description  : Function to clear PC terminal
 * Argument     : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t clear_screen(void)
{
    char_t               packet[USB_EP_PACKET_SIZE] = {'\0'};
    fsp_err_t            err                        = FSP_SUCCESS;
    const char_t * const p_clear_screen             = "\x1b[2J";
    const char_t * const p_cursor_home              = "\x1b[H";

    sprintf(packet, "%s%s", p_cursor_home, p_clear_screen);

    err = print_to_console(packet);

    return err;

}
/**********************************************************************************************************************
 End of function clear_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_kit_info
 * Description  : Function to output kit info to PC terminal
 * Argument     : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t print_kit_info(void)
{
    char_t                      mcu_temp_f[]               = "000.00";
    char_t                      mcu_temp_c[]               = "000.00";
    uint8_t                     offset[2]                  = {0, 0};
    fsp_err_t                   err                        = FSP_SUCCESS;
    static       char_t         packet[USB_EP_PACKET_SIZE] = {'\0'};
    static const char_t * const p_green_fg                 = "\x1b[32m";
    static const char_t * const p_white_fg                 = "\x1b[97m";
    const bsp_unique_id_t     * p_uid                      = R_BSP_UniqueIdGet();

    err = print_to_console((char *)STR_KITINFO);                    /*Cast Macro value to char pointer*/
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*Cast Macro value to char pointer*/
    sprintf(packet, "%s\t%s%08lX-%08lX-%08lX-%08lX%s", (char *)STR_MCU_ID, p_green_fg, p_uid->unique_id_words[0], \
            p_uid->unique_id_words[1], p_uid->unique_id_words[2], p_uid->unique_id_words[3], p_white_fg);
    err = print_to_console(packet);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    memset(packet, '\0', USB_EP_PACKET_SIZE);

    err = get_temperature(mcu_temp_f, mcu_temp_c, offset);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /*Cast Macro value to char pointer*/
    sprintf(packet, "%s\t%s%s/%s%s", (char *)STR_MCU_TEMP, p_green_fg, \
            mcu_temp_f + offset[FAHRENHEIT], mcu_temp_c + offset[CELSIUS], p_white_fg);
    err = print_to_console(packet);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    memset(packet, '\0', USB_EP_PACKET_SIZE);

    if (0 == g_board_status.led_frequency)
    {
        /*Cast Macro value to char pointer*/
        sprintf(packet, "%s%s\t%d%s", (char *)STR_LED_FREQ, p_green_fg, 1, p_white_fg);
    }
    else if (1 == g_board_status.led_frequency)
    {
        /*Cast Macro value to char pointer*/
        sprintf(packet, "%s%s\t%d%s", (char *)STR_LED_FREQ, p_green_fg, 5, p_white_fg);
    }
    else if (2 == g_board_status.led_frequency)
    {
        /*Cast Macro value to char pointer*/
        sprintf(packet, "%s%s\t%d%s", (char *)STR_LED_FREQ, p_green_fg, 10, p_white_fg);
    }
    else
    {
        /*Do nothing*/
    }
    err = print_to_console(packet);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    memset(packet, '\0', USB_EP_PACKET_SIZE);
    if (0 == g_board_status.led_intensity)
    {
        /*Cast Macro value to char pointer*/
        sprintf(packet, "%s%s\t%d%s", (char *)STR_LED_INTENSE, p_green_fg, 10, p_white_fg);
    }
    else if (1 == g_board_status.led_intensity)
    {
        /*Cast Macro value to char pointer*/
        sprintf(packet, "%s%s\t%d%s", (char *)STR_LED_INTENSE, p_green_fg, 50, p_white_fg);
    }
    else if (2 == g_board_status.led_intensity)
    {
        /*Cast Macro value to char pointer*/
        sprintf(packet, "%s%s\t%d%s", (char *)STR_LED_INTENSE, p_green_fg, 90, p_white_fg);
    }
    else
    {
        /*Do nothing*/
    }
    err = print_to_console(packet);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    err = print_to_console((char*)STR_RETURN);      /*Cast Macro value to char pointer*/
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function print_kit_info
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_welcome
 * Description  : Function to output welcome messages to PC terminal
 * Argument     : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t print_welcome(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = print_to_console((char *)STR_WELCOME);        /*Cast Macro value to char pointer*/
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function print_welcome
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_nextsteps
 * Description  : Function to output next step menu to PC terminal
 * Argument     : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t print_nextsteps(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = print_to_console((char *)STR_NEXTSTEPS);      /*Cast Macro value to char pointer*/
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}
/**********************************************************************************************************************
 End of function print_nextsteps
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_console
 * Description  : Function to update PC console depending on the argument
 * Argument     : g_buf
 * Return Value : error code
 *********************************************************************************************************************/
fsp_err_t update_console(uint8_t * g_buf)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (g_buf[0])
    {
        case WELCOME:
            err = clear_screen();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            err = print_welcome();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            g_board_status.cur_menu = WELCOME;

            break;

        case NEXT_STEPS:
            if (WELCOME == g_board_status.cur_menu)
            {
                err = clear_screen();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                err = print_nextsteps();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                g_board_status.cur_menu = NEXT_STEPS;
            }

            break;

        case KIT_INFO:
            if (WELCOME == g_board_status.cur_menu)
            {
                err = clear_screen();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                err = print_kit_info();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }
                g_board_status.cur_menu = KIT_INFO;
            }
            break;

        default:
            break;
    }

    return err;
}

/**********************************************************************************************************************
 End of function update_console
 *********************************************************************************************************************/





