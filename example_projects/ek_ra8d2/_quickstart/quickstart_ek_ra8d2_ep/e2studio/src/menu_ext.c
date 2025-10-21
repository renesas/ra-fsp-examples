/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_ext
 * Version      : 1.0
 * Description  : Memory Demo application external memory read write test.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "hal_data.h"
#include "board_hw_cfg.h"
#include "r_console.h"
#include "menu_ext.h"

#include "jlink_console.h"
#include "board_cfg_switch.h"
#include "display_thread_entry.h"
#include "ospi_flash_issi_is25lx512.h"

#include "ospi_b_ep.h"

#define BLOCK_LIMIT_KB              (64)                        // 64 kB maximum block size for the test
#define GPT_CLOCK_MHZ               (200)                       // the GPT clock speed in MHz

#define WAITING_MESSAGE             ("Waiting Results")
#undef MODULE_NAME
#define MODULE_NAME                 "\r\n%d. OCTO-SPI SPEED DEMONSTRATION\r\n"

#define SUB_OPTIONS                 "\r\nCompares the write and read times to and from external Octo-SPI" \
                                    "\r\nflash memory\r\n" \
                                    "\r\n> Enter the text block size " \
                                    "\r\n(in multiples of 2 KB, max 64 KB) and press tab to continue : "

static uint32_t validate_user_input(char_t *p_input);

static char_t s_print_buffer[BUFFER_LINE_LENGTH];
static char_t s_block_sz_str[INPUT_BUFFER];

st_rgb565_text_block_page_t g_ext_page =
{
    13,                                         // number of lines required (including blanks)
    " OCTO-SPI SPEED DEMONSTRATION",            // title
    {
       {{PAGE_LEFT_EDGE, LINE(1)  + BLOCK(0)}, "INSTRUCTIONS"},
       {{PAGE_LEFT_EDGE, LINE(2)  + BLOCK(0)}, "Compares the write and read times to and from external Octo-SPI flash memory."},
       {{PAGE_LEFT_EDGE, LINE(3)  + BLOCK(0)}, " "},
       {{PAGE_LEFT_EDGE, LINE(5)  + BLOCK(0)}, "Operation/Flash"},
       {{PAGE_1ST_TAB,   LINE(5)  + BLOCK(0)}, "Octo-SPI"},
       {{PAGE_LEFT_EDGE, LINE(6)  + BLOCK(0)}, "___________________________________________"},
       {{PAGE_LEFT_EDGE, LINE(7)  + BLOCK(0)}, "Write"},
       {{PAGE_1ST_TAB,   LINE(7)  + BLOCK(0)}, ""},
       {{PAGE_LEFT_EDGE, LINE(8)  + BLOCK(0)}, "Read"},
       {{PAGE_1ST_TAB,   LINE(8)  + BLOCK(0)}, ""},
       {{PAGE_LEFT_EDGE, LINE(9)  + BLOCK(0)}, ""},

       {{PAGE_LEFT_EDGE, LINE(11) + BLOCK(0)}, "NOTE"},
       {{PAGE_LEFT_EDGE, LINE(12) + BLOCK(0)}, "Menu & Close icons Disabled"},
    }
};

/**********************************************************************************************************************
 * Function Name: ext_display_menu
 * Description  : Implements the external flash memory menu
 * Return Value : 0
 *********************************************************************************************************************/
test_fn ext_display_menu(void)
{
    bool_t   valid_block_size  = false;
    int32_t c = -1;

    uint32_t block_size_actual = 0;
    int32_t  block_sz_ndx      = 0;
    int32_t  block_sz_limit    = (INPUT_BUFFER - 2);    /* Allowing for TAB and End of Message */
    uint32_t value             = 0;

    char_t res_str[24];

    clear_console();

    sprintf(res_str, WAITING_MESSAGE);

    sprintf(g_ext_page.text_block[7].line, res_str);
    sprintf(g_ext_page.text_block[9].line, res_str);

    sprintf(res_str, " ");
    sprintf(g_ext_page.text_block[10].line, res_str);

    dsp_set_background(LCD_FULL_BG_EXTERNAL_MEMORY);

    sprintf(s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);
    print_to_console(SUB_OPTIONS);

        /* Keep trying to read a valid text block size
         * complete the loop in one of two ways:
     * [1] Valid block size is entered (2K boundary range 2-64K) followed by TAB
     * [2] Space Bar is pressed at any stage
     */

    while (!valid_block_size)
    {
        /* reset input state */
        c = -1;

        block_sz_ndx = 0;
        memset (&s_block_sz_str, 0, INPUT_BUFFER);

        while ((CONNECTION_ABORT_CRTL != c))
        {
            c = input_from_console();

            if ((block_sz_ndx < block_sz_limit) && (BACK_SPACE != c) && (BACK_SPACE_PUTTY != c) && (CARRAGE_RETURN != c)
                    && (MENU_ENTER_RESPONSE_CRTL != c))
            {
                /* cast to req type */
                s_block_sz_str[block_sz_ndx] = (char_t) c;
                block_sz_ndx++;
            }
            else if (((BACK_SPACE == c) || (BACK_SPACE_PUTTY == c)) && (block_sz_ndx > 0))
            {
                s_block_sz_str[block_sz_ndx - 1] = '\0';
                block_sz_ndx--;
                print_to_console("\b \b");
            }
            else if ((0 == block_sz_ndx) || (CARRAGE_RETURN == c))
            {
                __NOP();
            }
            else
            {
                /* maximum block size exceeded (4 digits / characters entered) */
                s_block_sz_str[block_sz_ndx] = MENU_ENTER_RESPONSE_CRTL;
                c                            = MENU_ENTER_RESPONSE_CRTL;
                break;
            }

            if (MENU_EXIT_CRTL == c)
            {
                /* abort the test */
                valid_block_size = true;
                block_size_actual = 0;
                break;
            }

            if ((MENU_ENTER_RESPONSE_CRTL == c) || (CARRAGE_RETURN == c))
            {
                break;
            }

            if ((CARRAGE_RETURN != c) && (BACK_SPACE != c))
            {
                sprintf (s_print_buffer, "%c", (char_t) c);
                print_to_console(s_print_buffer);
            }
        }

        /* if the input was terminated with a TAB then attempt to process it */
        if ((MENU_ENTER_RESPONSE_CRTL == c) || (CARRAGE_RETURN == c))
        {
            value = validate_user_input (&s_block_sz_str[0]);
        }

        vTaskDelay (10);

        if (value < 2)
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > BLOCK_LIMIT_KB) && (value < INVALID_MARKERS))
        {
            value = INVALID_BLOCK_SIZE;
        }

        if ((value > 0) && ((value % 2) != 0) && (value < INVALID_MARKERS))
        {
            value = INVALID_BLOCK_BOUNDARY;
        }

        switch (value)
        {
            case INVALID_CHARACTER:
            {
                print_to_console("\r\n> Invalid character in entry, enter the text block size specifying a 2K boundary (e.g. 24)"
                         " and press tab: ");
                break;
            }

            case INVALID_BLOCK_SIZE:
            {
                print_to_console("\r\n> Invalid size, enter the text block size (e.g. 24) and press tab: ");
                break;
            }

            case INVALID_BLOCK_BOUNDARY:
            {
                print_to_console("\r\n> Invalid boundary, enter the text block size specifying a 2K boundary (e.g. 24)"
                         " and press tab : ");
                break;
            }

            default:
            {
                valid_block_size = true;
            }
        }
    }

    block_size_actual = value;

    if ((MENU_ENTER_RESPONSE_CRTL == c) && (0 != block_size_actual))
    {
        fsp_err_t fsp_err = FSP_SUCCESS;

        uint32_t ospi_performance_write_result = 0;
        uint32_t ospi_performance_read_result = 0;
        uint32_t ospi_read_result = 0;
        uint32_t ospi_write_result = 0;

        sprintf (s_print_buffer, "\r\n\r\nGenerated a text block of %2lu KB in SRAM\r\n", block_size_actual);
        print_to_console(s_print_buffer);

        block_size_actual = block_size_actual * 1024;

        ospi_performance_test (block_size_actual, &ospi_performance_write_result, &ospi_performance_read_result);

        ospi_write_result =  ospi_performance_write_result / GPT_CLOCK_MHZ;
        ospi_read_result  =  ospi_performance_read_result / GPT_CLOCK_MHZ;

        /* Handle error */
        if (FSP_SUCCESS != fsp_err)
        {
            /* Fatal error */
            SYSTEM_ERROR_CFG_
        }

        print_to_console("\r\n-------------------------------------------------");
        print_to_console("\r\nOperation/Flash          Octo-SPI");
        print_to_console("\r\n-------------------------------------------------");

        sprintf (s_print_buffer, "\r\nWrite                    %6ld", ospi_write_result);
        print_to_console(s_print_buffer);

        sprintf (s_print_buffer, "\r\nRead                     %6ld", ospi_read_result);
        print_to_console(s_print_buffer);

        print_to_console("\r\n-------------------------------------------------");
        print_to_console("\r\nNote: Times are in microseconds");
        print_to_console(MENU_RETURN_INFO);

        sprintf(res_str, "%6ld us", ospi_write_result);
        sprintf(g_ext_page.text_block[7].line, res_str);
        sprintf(res_str, "%6ld us", ospi_read_result);
        sprintf(g_ext_page.text_block[9].line, res_str);

        sprintf(res_str, "Block Size %6ld Bytes", block_size_actual);
        sprintf(g_ext_page.text_block[10].line, res_str);
    }

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (0x00 == c))
        {
            break;
        }

        c = input_from_console ();
    }

    return (0);
}
/**********************************************************************************************************************
 End of function ext_display_menu
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: validate_user_input
 * Description  : parse the user input and convert to integer value
 * Argument     : p_input
 * Return Value : Integer value entered, or 0 if the menu exit character was entered
 *********************************************************************************************************************/
static uint32_t validate_user_input(char_t *p_input)
{
    uint32_t result = INVALID_CHARACTER;
    uint32_t value  = 0;
    uint32_t t      = 0;
    uint32_t c      = 0;

    while (true)
    {
        /* Cast to req type */
        c = (uint32_t) (*p_input);

        p_input++;

        if ((c >= '0') && (c <= '9'))
        {
            /* Cast as compiler will interpret result as int */
            value = (uint32_t) (value * 10);

            /* Cast to req type */
            t = (uint32_t) (c - '0');

            /* Cast as compiler will interpret result as int */
            value = (uint32_t) (value + t);
        }
        else
        {
            if (MENU_ENTER_RESPONSE_CRTL == c)
            {
                result = value;
            }

            if (MENU_EXIT_CRTL == c)
            {
                result = 0;
            }

            break;
        }
    }

    return (result);
}
/**********************************************************************************************************************
 End of function validate_user_input
 *********************************************************************************************************************/
